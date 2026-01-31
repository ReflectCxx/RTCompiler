#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace clmirror {
	struct UserType;
	struct ReflectionMeta;
}

namespace clmirror 
{
	class RTLCodeManager
	{
		std::unordered_map<std::string, UserType> m_metaTypes;
		std::unordered_multimap<std::string, ReflectionMeta> m_metaFns;

		RTLCodeManager();
		~RTLCodeManager();

		void printRecordTypeIds(std::fstream& pOut);

		void printFreeFunctionIds(std::fstream& pOut);

		void printRegistrationDecls(std::fstream& pOut);

		void addReflectionMetaAsRecord(const ReflectionMeta& pReflMeta);

		void dumpMetadataIds(std::fstream& pOut);

		void dumpRegistrationDecls(std::fstream& pOut);

	public:

		RTLCodeManager(const RTLCodeManager&) = delete;
		RTLCodeManager& operator=(const RTLCodeManager&) = delete;

		static RTLCodeManager& Instance();

		void addFunctionSignature(MetaKind pMetaKind, const std::string& pSrcFile,
								  const std::string& pHeaderFile, const std::string& pRecord,
								  const std::string& pFunctionName, const std::vector<std::string>& pParmTypes);
		void generateRegistrationCode();
	};
}