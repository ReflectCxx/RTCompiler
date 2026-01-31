#pragma once

#include <string>
#include <unordered_map>

namespace clmirror {
	class RtlCodeGenerator;
}

namespace clmirror 
{
	class RtlCodeManager
	{
		std::unordered_map<std::string, RtlCodeGenerator> m_codeGens;

		void dumpMetadataIds(std::fstream& pOut);
		void dumpRegistrationDecls(std::fstream& pOut);

		static void printRecordTypeIds(const RtlRecordsMap& pRecodsMap, std::fstream& pOut);
		static void printFreeFunctionIds(const RtlFunctionsMap& pFunctionsMap, std::fstream& pOut);
		static void printRegistrationDecls(const RtlRecordsMap& pRecodsMap, std::fstream& pOut);

		RtlCodeManager();
		~RtlCodeManager();

	public:

		RtlCodeManager(RtlCodeManager&&) = delete;
		RtlCodeManager(const RtlCodeManager&) = delete;
		RtlCodeManager& operator=(RtlCodeManager&&) = delete;
		RtlCodeManager& operator=(const RtlCodeManager&) = delete;
		
		const RtlCodeGenerator& getCodeGenerator(const std::string& pSrcFile);

		static RtlCodeManager& Instance();

		void generateRegistrationCode();
	};
}