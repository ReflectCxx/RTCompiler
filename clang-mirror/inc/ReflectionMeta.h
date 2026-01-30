
#include <map>
#include <vector>
#include <string>
#include <unordered_map>

#include "Constants.h"

namespace clmirror 
{
	struct ReflectionMeta 
	{
		MetaKind fnType;
		
		std::string fnHeader;
		std::string fnSource;

		std::string fnName;
		std::string fnRecord;
		std::vector<std::string> fnArgs;

		std::string toMethodIdentifierSyntax() const;

		std::string toRecordIdentifierSyntax() const;
	};

	struct UserType {

		using MemberFnsMap = std::unordered_multimap<std::string, clmirror::ReflectionMeta>;

		std::string typeStr;
		MemberFnsMap methods;
	};
}