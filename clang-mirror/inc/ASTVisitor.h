#pragma once

#include <map>
#include "clang/AST/RecursiveASTVisitor.h"

namespace clmirror {
    class ASTCodeGenerator;
}

namespace clmirror {

    class ASTVisitor : public clang::RecursiveASTVisitor<ASTVisitor>
    {
        const std::string m_srcFile;
    public:

        ASTVisitor(const std::string& pSrcFile);

        bool VisitFunctionDecl(clang::FunctionDecl* pFuncDecl);
    };
}