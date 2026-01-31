#include <iostream>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <unordered_set>

#include "Logger.h"
#include "ReflectionMeta.h"
#include "RTLCodeManager.h"


namespace clmirror 
{
    RTLCodeManager::RTLCodeManager() 
	{ }

    RTLCodeManager::~RTLCodeManager() 
	{ }

    RTLCodeManager& RTLCodeManager::Instance()
    {
        static RTLCodeManager instance;
        return instance;
    }

    void RTLCodeManager::dumpMetadataIds(std::fstream& pOut)
    {
        pOut << "\n#pragma once"
                "\n#include <string_view>\n"
                "\nnamespace rtcl {\n";
        printFreeFunctionIds(pOut);
        printRecordTypeIds(pOut);
        pOut << "\n}";
    }

    void RTLCodeManager::dumpRegistrationDecls(std::fstream& pOut)
	{
       pOut << "\n#pragma once"
               "\n#include <vector>\n"
               "\nnamespace rtl { class Function; }\n"
               "\nnamespace rtcl {\n";
        printRegistrationDecls(pOut);
        pOut << "\n}";
    }

    void RTLCodeManager::printFreeFunctionIds(std::fstream& pOut)
    {
        std::unordered_set<std::string> seen;
        for (auto it = m_metaFns.begin(); it != m_metaFns.end(); ++it)
        {
            const std::string& key = it->first;
            if (!seen.insert(key).second) {
                continue;
            }
            pOut << it->second.toFunctionIdentifierSyntax() << "\n";
        }
    }


    void RTLCodeManager::printRegistrationDecls(std::fstream& pOut)
    {
        for (const auto& itr : m_metaTypes) {

            std::unordered_set<std::string> seen;
            const auto& methodMap = itr.second.methods;
            const auto& fnMeta = methodMap.begin()->second;

            pOut << fnMeta.toRegistrationDeclSyntax() << "\n";
        }
    }


    void RTLCodeManager::printRecordTypeIds(std::fstream& pOut)
    {
        for (const auto& itr : m_metaTypes) {

            std::unordered_set<std::string> seen;
            const auto& methodMap = itr.second.methods;
            const auto& fnMeta = methodMap.begin()->second;

            pOut << fnMeta.toRecordIdentifierSyntax() << "\n";
            for (auto it = methodMap.begin(); it != methodMap.end(); ++it)
            {
                const std::string& key = it->first;
                if (!seen.insert(key).second) {
                    continue;
                }
                pOut << it->second.toMethodIdentifierSyntax() << "\n";
            }
            pOut << "\n";
        }
    }


    void RTLCodeManager::addReflectionMetaAsRecord(const ReflectionMeta& pReflMeta)
    {
        auto& userType = [&]()-> UserType&
        {
            const auto& itr = m_metaTypes.find(pReflMeta.m_record);
            if (itr == m_metaTypes.end()) 
            {
                auto& userType = m_metaTypes.emplace(pReflMeta.m_record,
                    UserType{
                        .typeStr = pReflMeta.m_record,
                        .methods = UserType::MemberFnsMap()
                    }).first->second;
                return userType;
            }
            else {
                auto& userType = itr->second;
                return userType;
            }
        }();
        userType.methods.emplace(pReflMeta.m_function, pReflMeta);
    }


    void RTLCodeManager::addFunctionSignature(MetaKind pMetaKind, const std::string& pSrcFile,
                                              const std::string& pHeaderFile, const std::string& pRecord,
                                              const std::string& pFunctionName, const std::vector<std::string>& pParmTypes)
    {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);

        if (pMetaKind == MetaKind::NonMemberFn)
        {
            m_metaFns.emplace(pFunctionName, (ReflectionMeta{
                    .m_metaKind = pMetaKind,
                    .m_header = pHeaderFile,
                    .m_source = pSrcFile,
                    .m_record = pRecord,
                    .m_function = pFunctionName,
                    .m_argTypes = pParmTypes
            }));
        }
        else if (pMetaKind != MetaKind::None)
        {
            addReflectionMetaAsRecord(ReflectionMeta{
                    .m_metaKind = pMetaKind,
                    .m_header = pHeaderFile,
                    .m_source = pSrcFile,
                    .m_record = pRecord,
                    .m_function = pFunctionName,
                    .m_argTypes = pParmTypes
            });
        }
    }


    void RTLCodeManager::generateRegistrationCode()
    {
        {
            const std::string fileStr = std::filesystem::current_path().string() + "/" + std::string(META_ID_HEADER);
            std::fstream fout(fileStr, std::ios::out);
            if (!fout.is_open()) {
                Logger::outException("Error opening file for writing!");
                return;
            }

            dumpMetadataIds(fout);
            fout.flush();
            fout.close();

            if (fout.fail() || fout.bad()) {
                Logger::outException("Error closing file:" + std::string(META_ID_HEADER));
                return;
            }
            Logger::out("generated file : " + fileStr);
        } {
            const std::string fileStr = std::filesystem::current_path().string() + "/" + std::string(REGISTRATION_HEADER);
            std::fstream fout(fileStr, std::ios::out);
            if (!fout.is_open()) {
                Logger::outException("Error opening file for writing!");
                return;
            }

            dumpRegistrationDecls(fout);
            fout.flush();
            fout.close();

            if (fout.fail() || fout.bad()) {
                Logger::outException("Error closing file:" + std::string(META_ID_HEADER));
                return;
            }
            Logger::out("generated file : " + fileStr);
        }
        Logger::out("Number of reflectable types generated: " + std::to_string(m_metaTypes.size()));
    }
}
