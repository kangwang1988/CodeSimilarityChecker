//
//  CodeSimilarityChecker.h
//  CodeSimilarityChecker
//
//  Created by KyleWong on 15/02/2017.
//  Copyright © 2017 KyleWong. All rights reserved.
//

#ifndef CodeSimilarityChecker_h
#define CodeSimilarityChecker_h

#include<iostream>
#include<sstream>
#include<typeinfo>
#include<string>
#include<regex>
#include <string>
#include <fstream>
#include <sys/stat.h>

#include "clang/Driver/Options.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/RecursiveASTVisitor.h"

class NKASTTree;

using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

extern string gSrcRootPath;

namespace CodeSimilarityChecker
{
    class CodeSimilarityCheckerASTVisitor : public RecursiveASTVisitor<CodeSimilarityCheckerASTVisitor>
    {
    private:
        ASTContext *context;
    public:
        // List of visited nodes during traversal.
        NKASTTree *astTree;
        void setContext(ASTContext &context);
        bool VisitDecl(Decl *decl);
        bool VisitExpr(Expr *e);
        bool VisitStmt(Stmt *s);
        bool shouldTraversePostOrder();
        template <typename NodeT> void * getParentNode(NodeT *aASTNode);
    };
    class CodeSimilarityCheckerASTConsumer : public ASTConsumer
    {
    private:
        CodeSimilarityCheckerASTVisitor visitor;
        void HandleTranslationUnit(ASTContext &context);
    };
    
    class CodeSimilarityCheckerASTAction : public PluginASTAction
    {
    public:
        unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler,llvm::StringRef InFile);
        bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string>& args);
    };
}

#endif /* CodeSimilarityChecker_h */
