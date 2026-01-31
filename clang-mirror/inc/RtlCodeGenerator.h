#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "ReflectionMeta.h"

namespace clmirror 
{
	class RtlCodeManager;
}

namespace clmirror
{
	class RtlCodeGenerator 
	{
		const std::string m_srcFile;
		mutable RtlRecordsMap m_recordsMap;
		mutable RtlFunctionsMap m_freeFnsMap;
		mutable std::unordered_set<std::string> m_incFiles;

		RtlCodeGenerator(const std::string& pSrcFile);

		void addRtlRecord(const RtlFunction& pFnMeta) const;

	public:

		RtlCodeGenerator(RtlCodeGenerator&&) = default;
		RtlCodeGenerator(const RtlCodeGenerator&) = default;
		RtlCodeGenerator& operator=(RtlCodeGenerator&&) = delete;
		RtlCodeGenerator& operator=(const RtlCodeGenerator&) = delete;

		GETTER_CREF(std::string, SrcFile, m_srcFile)
		GETTER_CREF(RtlRecordsMap, RecordsMap, m_recordsMap)
		GETTER_CREF(RtlFunctionsMap, FreeFunctionsMap, m_freeFnsMap)

		void addFunction(MetaKind pMetaKind, const std::string& pHeaderFile, const std::string& pRecord,
						 const std::string& pFnName, const std::vector<std::string>& pParamTypes) const;

		friend RtlCodeManager;
	};
}