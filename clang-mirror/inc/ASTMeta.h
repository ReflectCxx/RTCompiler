
#pragma once

#include <map>
#include <vector>
#include <string>
#include <unordered_map>

#include "Constants.h"

namespace clmirror
{
    struct ASTMetaFn
    {
        MetaKind m_metaKind;
		
        std::string m_header;
        std::string m_record;
        std::string m_function;
		
        std::vector<std::string> m_argTypes;
        
        std::string toRegistrationDeclSyntax() const;
        std::string toRecordIdentifierSyntax() const;
        std::string toMethodIdentifierSyntax() const;
        std::string toFunctionIdentifierSyntax() const;
    };

    struct ASTMetaType
    {
        using MemberFnsMap = std::unordered_multimap<std::string, clmirror::ASTMetaFn>;
        
        std::string typeStr;
        MemberFnsMap methods;
	};
}