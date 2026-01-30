#pragma once

#include <map>
#include <vector>
#include <string>


namespace clmirror {
	struct UserType;
	struct ReflectionMeta;
}

namespace clmirror 
{
	class ReflectableInterface
	{
		using FuncSignature = std::multimap<std::string, std::vector<std::string> >;
		using FuncHeaderMap = std::map<std::string, FuncSignature>;
		std::map<std::string, FuncHeaderMap> m_functionSignatureMap;

		std::vector<ReflectionMeta> m_metaFns;
		std::unordered_map<std::string, UserType> m_metaTypes;

		ReflectableInterface();
		~ReflectableInterface();

		void addReflectionMetaAsRecord(const ReflectionMeta& pReflMeta);

	public:

		ReflectableInterface(const ReflectableInterface&) = delete;
		ReflectableInterface& operator=(const ReflectableInterface&) = delete;

		static ReflectableInterface& Instance();

		void addFunctionSignature(MetaKind pMetaKind, const std::string& pSrcFile,
								  const std::string& pHeaderFile, const std::string& pRecord,
								  const std::string& pFunctionName, const std::vector<std::string>& pParmTypes);
		void dump();
	};
}