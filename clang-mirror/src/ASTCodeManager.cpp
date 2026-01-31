#include <iostream>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <unordered_set>

#include "Logger.h"
#include "ASTMeta.h"
#include "ASTCodeManager.h"
#include "ASTCodeGenerator.h"

namespace clmirror 
{
    ASTCodeManager::ASTCodeManager() 
    { }

    ASTCodeManager::~ASTCodeManager() 
    { 
        for (auto& itr : m_codeGens) {
            delete itr.second;
        }
    }

    ASTCodeManager& ASTCodeManager::instance()
    {
        static ASTCodeManager instance;
        return instance;
    }

    void ASTCodeManager::dumpMetadataIds(std::fstream& pOut)
    {
        pOut << "\n#pragma once"
                "\n#include <string_view>\n"
                "\nnamespace rtcl {\n";
        for (const auto& itr : m_codeGens) {
            printFreeFunctionIds(itr.second->getFreeFunctionsMap(), pOut);
        }
        for (const auto& itr : m_codeGens) {
            printRecordTypeIds(itr.second->getRecordsMap(), pOut);
        }
        pOut << "\n}";
    }

    void ASTCodeManager::dumpRegistrationDecls(std::fstream& pOut)
    {
        pOut << "\n#pragma once"
                "\n#include <vector>\n"
                "\nnamespace rtl { class Function; }\n"
                "\nnamespace rtcl {\n"
                "\nnamespace " + std::string(NS_REGISTRATION) + " {"
                "\n    " + std::string(DECL_INIT_REGIS) + "\n}\n";

        for (const auto& itr : m_codeGens) {
            printRegistrationDecls(itr.second->getRecordsMap(), pOut);
        }
        pOut << "\n}";
    }

    void ASTCodeManager::printFreeFunctionIds(const RtlFunctionsMap& pFunctionsMap, std::fstream& pOut)
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


    void ASTCodeManager::printRegistrationDecls(const RtlRecordsMap& pRecodsMap, std::fstream& pOut)
    {
        for (const auto& itr : pRecodsMap) {

            std::unordered_set<std::string> seen;
            const auto& methodMap = itr.second.methods;
            const auto& fnMeta = methodMap.begin()->second;

            pOut << fnMeta.toRegistrationDeclSyntax() << "\n";
        }
    }


    void ASTCodeManager::printRecordTypeIds(const RtlRecordsMap& pRecodsMap, std::fstream& pOut)
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


    ASTCodeGenerator* ASTCodeManager::getCodeGenerator(const std::string& pSrcFile, bool pCreate /*= false*/)
    {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);

        if (pCreate) {
            auto codegen = [&]()-> ASTCodeGenerator*
            {
                const auto& itr = m_codeGens.find(pSrcFile);
                if (itr == m_codeGens.end()) 
                {
                    auto codegen = new ASTCodeGenerator(pSrcFile);
                    m_codeGens.insert(std::make_pair(pSrcFile, codegen));
                    return codegen;
                }
                else {
                    auto& codegen = itr->second;
                    return codegen;
                }
            }();
            return codegen;
        }
        else {
            const auto& itr = m_codeGens.find(pSrcFile);
            return (itr != m_codeGens.end() ? itr->second : nullptr);
        }
    }


    void ASTCodeManager::dumpRegistrations(const std::string& pSrcFile, std::size_t pIndex)
    {
        auto codegen = getCodeGenerator(pSrcFile);
        if (codegen != nullptr) 
        {
            std::string filePath = std::filesystem::current_path().string() + "/";
            filePath.append(std::string(FILE_REG_PREFIX) + std::to_string(pIndex) + ".cpp");

            std::fstream fout(filePath, std::ios::out);
            if (!fout.is_open()) {
                Logger::outException("Error opening file for writing!");
                return;
            }

            fout << "\n"
                    "\n#include \"" << std::string(FILE_REG_IDS) << "\""
                    "\n#include \"" << std::string(FILE_REG_DECLS) << "\"";

            fout.flush();
            fout.close();

            if (fout.fail() || fout.bad()) {
                Logger::outException("Error closing file:" + std::string(FILE_REG_IDS));
                return;
            }
            Logger::out("generated file : " + filePath);
        }
    }


    void ASTCodeManager::dumpReflectionIds()
    {
        {
            const std::string fileStr = std::filesystem::current_path().string() + "/" + std::string(FILE_REG_IDS);
            std::fstream fout(fileStr, std::ios::out);
            if (!fout.is_open()) {
                Logger::outException("Error opening file for writing!");
                return;
            }

            dumpMetadataIds(fout);
            fout.flush();
            fout.close();

            if (fout.fail() || fout.bad()) {
                Logger::outException("Error closing file:" + std::string(FILE_REG_IDS));
                return;
            }
            Logger::out("generated file : " + fileStr);
        } {
            const std::string fileStr = std::filesystem::current_path().string() + "/" + std::string(FILE_REG_DECLS);
            std::fstream fout(fileStr, std::ios::out);
            if (!fout.is_open()) {
                Logger::outException("Error opening file for writing!");
                return;
            }

            dumpRegistrationDecls(fout);
            fout.flush();
            fout.close();

            if (fout.fail() || fout.bad()) {
                Logger::outException("Error closing file:" + std::string(FILE_REG_IDS));
                return;
            }
            Logger::out("generated file : " + fileStr);
        }
        Logger::out("Number of reflectable entities generated: " + std::to_string(m_codeGens.size()));
    }
}
