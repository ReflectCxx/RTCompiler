#include "clang/AST/RecursiveASTVisitor.h"


namespace clmirror {

    class ParmVarDeclsVisitor : public clang::RecursiveASTVisitor<ParmVarDeclsVisitor>
    {
    public:

        bool VisitTypedefType(clang::TypedefType* pTypeDefType);
    };
}