#pragma once

#include <map>
#include "clang/AST/RecursiveASTVisitor.h"

namespace clmirror {

    class ReflectableDeclsVisitor : public clang::RecursiveASTVisitor<ReflectableDeclsVisitor>
    {
        const std::string& m_currentSrcFile;
        std::vector<std::string>& m_unreflectedFunctions;

    public:

        ReflectableDeclsVisitor(const std::string& pCurrentSrcFile, std::vector<std::string>& pUnreflectedFunctions);

        bool VisitFunctionDecl(clang::FunctionDecl* pFuncDecl);
    };
}