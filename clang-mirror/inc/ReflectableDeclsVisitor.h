#pragma once

#include <map>
#include "clang/AST/RecursiveASTVisitor.h"

namespace clmirror {
    class RtlCodeGenerator;
}

namespace clmirror {

    class ReflectableDeclsVisitor : public clang::RecursiveASTVisitor<ReflectableDeclsVisitor>
    {
        const std::string m_srcFile;
    public:

        ReflectableDeclsVisitor(const std::string& pSrcFile);

        bool VisitFunctionDecl(clang::FunctionDecl* pFuncDecl);
    };
}