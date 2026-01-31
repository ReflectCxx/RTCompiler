#include <iostream>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <unordered_set>

#include "Logger.h"
#include "ReflectionMeta.h"
#include "RtlCodeManager.h"
#include "RtlCodeGenerator.h"

namespace clmirror 
{
    RtlCodeManager::RtlCodeManager() 
    { }

    RtlCodeManager::~RtlCodeManager() 
    { }

    RtlCodeManager& RtlCodeManager::instance()
    {
        static RtlCodeManager instance;
        return instance;
    }

    void RtlCodeManager::dumpMetadataIds(std::fstream& pOut)
    {
        pOut << "\n#pragma once"
                "\n#include <string_view>\n"
                "\nnamespace rtcl {\n";
        for (const auto& itr : m_codeGens) {
            printFreeFunctionIds(itr.second.getFreeFunctionsMap(), pOut);
        }
        for (const auto& itr : m_codeGens) {
            printRecordTypeIds(itr.second.getRecordsMap(), pOut);
        }
        pOut << "\n}";
    }

    void RtlCodeManager::dumpRegistrationDecls(std::fstream& pOut)
    {
        pOut << "\n#pragma once"
                "\n#include <vector>\n"
                "\nnamespace rtl { class Function; }\n"
                "\nnamespace rtcl {\n"
                "\nnamespace " + std::string(NS_REGISTRATION) + " {"
                "\n    " + std::string(DECL_INIT_REGIS) + "\n}\n";

        for (const auto& itr : m_codeGens) {
            printRegistrationDecls(itr.second.getRecordsMap(), pOut);
        }
        pOut << "\n}";
    }

    void RtlCodeManager::printFreeFunctionIds(const RtlFunctionsMap& pFunctionsMap, std::fstream& pOut)
    {
        std::unordered_set<std::string> seen;
        for (auto it = pFunctionsMap.begin(); it != pFunctionsMap.end(); ++it)
        {
            const std::string& key = it->first;
            if (!seen.insert(key).second) {
                continue;
            }
            pOut << it->second.toFunctionIdentifierSyntax() << "\n";
        }
    }


    void RtlCodeManager::printRegistrationDecls(const RtlRecordsMap& pRecodsMap, std::fstream& pOut)
    {
        for (const auto& itr : pRecodsMap) {

            std::unordered_set<std::string> seen;
            const auto& methodMap = itr.second.methods;
            const auto& fnMeta = methodMap.begin()->second;

            pOut << fnMeta.toRegistrationDeclSyntax() << "\n";
        }
    }


    void RtlCodeManager::printRecordTypeIds(const RtlRecordsMap& pRecodsMap, std::fstream& pOut)
    {
        for (const auto& itr : pRecodsMap) {

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


    RtlCodeGenerator& RtlCodeManager::getCodeGenerator(const std::string& pSrcFile)
    {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);

        auto& codegen = [&]()-> RtlCodeGenerator&
        {
            const auto& itr = m_codeGens.find(pSrcFile);
            if (itr == m_codeGens.end()) {
                auto& codegen = m_codeGens.emplace(pSrcFile, RtlCodeGenerator(pSrcFile)).first->second;
                return codegen;
            }
            else {
                auto& codegen = itr->second;
                return codegen;
            }
        }();
        return codegen;
    }


    void RtlCodeManager::dumpReflectionIds()
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
        Logger::out("Number of reflectable entities generated: " + std::to_string(m_codeGens.size()));
    }
}
