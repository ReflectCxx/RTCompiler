
#include <map>
#include <vector>
#include <string>

#include "Constants.h"
#include "StringUtils.h"
#include "ReflectionMeta.h"

namespace clmirror 
{
	std::string ReflectionMeta::toMethodIdentifierSyntax() const
	{
		std::vector<std::string> typenames = splitQualifiedName(fnRecord);

		std::string syntaxStr = "\nnamespace type {";
		for (const auto& typeStr : typenames) {
			syntaxStr.append("\nnamespace " + typeStr + " {");
		}

		syntaxStr.append("\nnamespace method {")
			     .append("\n    inline constexpr std::string_view ")
			     .append(fnName)
			     .append(" = \"")
			     .append(fnName)
			     .append("\";\n}");

		for (const auto& typeStr : typenames) {
			syntaxStr.append("}");
		}
		syntaxStr.append("}");
		return syntaxStr;
	}


	std::string ReflectionMeta::toRecordIdentifierSyntax() const
	{
		std::vector<std::string> typenames = splitQualifiedName(fnRecord);

		std::string syntaxStr = "\nnamespace type {";
		for (const auto& typeStr : typenames) {
			syntaxStr.append("\nnamespace " + typeStr + " {");
		}

		syntaxStr.append("\n    inline constexpr std::string_view ")
			     .append("id")
			     .append(" = \"")
			     .append(fnRecord)
			     .append("\";\n");

		for (const auto& typeStr : typenames) {
			syntaxStr.append("}");
		}
		syntaxStr.append("}");

		return syntaxStr;
	}
}