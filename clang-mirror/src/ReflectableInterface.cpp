#include <iostream>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <unordered_set>

#include "Logger.h"
#include "ReflectionMeta.h"
#include "ReflectableInterface.h"


namespace {

	std::mutex g_mutex;
}


namespace clmirror 
{
	ReflectableInterface::ReflectableInterface() {

	}


	ReflectableInterface::~ReflectableInterface() {

	}


	void ReflectableInterface::addReflectionMetaAsRecord(const ReflectionMeta& pReflMeta)
	{
		auto& userType = [&]()-> UserType&
		{
			const auto& itr = m_metaTypes.find(pReflMeta.fnRecord);
			if (itr == m_metaTypes.end()) 
			{
				auto& userType = m_metaTypes.emplace(pReflMeta.fnRecord,
					UserType{
						.typeStr = pReflMeta.fnRecord,
						.methods = UserType::MemberFnsMap()
					}).first->second;
				return userType;
			}
			else {
				auto& userType = itr->second;
				return userType;
			}
		}();
		userType.methods.emplace(pReflMeta.fnName, pReflMeta);
	}


	ReflectableInterface& ReflectableInterface::Instance()
	{
		static ReflectableInterface instance;
		return instance;
	}


	void ReflectableInterface::addFunctionSignature(MetaKind pMetaKind, const std::string& pSrcFile, 
											        const std::string& pHeaderFile, const std::string& pRecord,
													const std::string& pFunctionName, const std::vector<std::string>& pParmTypes)
	{
		std::lock_guard<std::mutex> lock(g_mutex);

		const auto& srcItr = m_functionSignatureMap.find(pSrcFile);
		if (srcItr == m_functionSignatureMap.end()) 
		{
			FuncSignature funcSigMap;
			FuncHeaderMap funcHeaderMap;
			funcSigMap.emplace(pFunctionName, pParmTypes);
			funcHeaderMap.emplace(pHeaderFile, funcSigMap);
			m_functionSignatureMap.emplace(pSrcFile, funcHeaderMap);
		}
		else 
		{
			auto& funcHeaderMap = srcItr->second;
			const auto& headerItr = funcHeaderMap.find(pHeaderFile);
			if (headerItr == funcHeaderMap.end()) {
				FuncSignature signatureMap;
				signatureMap.emplace(pFunctionName, pParmTypes);
				funcHeaderMap.emplace(pHeaderFile, signatureMap);
			}
			else {
				headerItr->second.emplace(pFunctionName, pParmTypes);
			}
		}

		if (pMetaKind == MetaKind::MemberFnConst ||
			pMetaKind == MetaKind::MemberFnNonConst ||
			pMetaKind == MetaKind::MemberFnStatic) {

			addReflectionMetaAsRecord( ReflectionMeta{
					.fnType = pMetaKind,
					.fnHeader = pHeaderFile,
					.fnSource = pSrcFile,
					.fnName = pFunctionName,
					.fnRecord = pRecord,
					.fnArgs = pParmTypes
			});
		}
		else {
			m_metaFns.push_back( ReflectionMeta{
					.fnType = pMetaKind,
					.fnHeader = pHeaderFile,
					.fnSource = pSrcFile,
					.fnName = pFunctionName,
					.fnRecord = pRecord,
					.fnArgs = pParmTypes
			});
		}
	}


	void ReflectableInterface::dump()
	{
		const std::string fileStr = std::filesystem::current_path().string() + "/" + CL_REFLECT_INTERFACE;
		std::fstream fout(fileStr, std::ios::out);

		if (!fout.is_open()) {
			Logger::outException("Error opening file for writing!");
			return;
		}
		
		fout << "\n"
			"\n#pragma once"
			"\n#include <string_view>\n\n"
			"\nnamespace rtcl {\n";

		for (const auto& itr : m_metaTypes) {

			const auto& methodMap = itr.second.methods;
			const auto& fnMeta = methodMap.begin()->second;
			fout << fnMeta.toRecordIdentifierSyntax();
			fout << "\n";

			std::unordered_set<std::string> seen;

			for (auto it = methodMap.begin(); it != methodMap.end(); ++it)
			{
				const std::string& key = it->first;
				if (!seen.insert(key).second)
					continue;

				fout << it->second.toMethodIdentifierSyntax() << "\n";

				//auto [first, last] = methodMap.equal_range(key);
			}
			fout << "\n";
		}
		fout << "\n}";

		fout.flush();
		fout.close();
		if (fout.fail() || fout.bad()) {
			Logger::outException("Error closing file:" + std::string(CL_REFLECT_INTERFACE));
			return;
		}

		Logger::out("Number of reflectable types generated: " + std::to_string(m_metaTypes.size()));
		//Logger::out("Number of headerfiles shortlisted, containing reflectable functions declarations: " + std::to_string(m_functionSignatureMap.size()));
		Logger::out("Reflection interface file generated : " + fileStr);
	}
}
