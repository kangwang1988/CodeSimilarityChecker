#include<regex>
#include <string>
#include <fstream>

#include "CodeSimilarityChecker.h"
#include "CodeSimilarityCheckerUtil.hpp"

using namespace std;
extern vector<string> getSourcesByProject(string prjPath);

namespace CodeSimilarityChecker
{
    void CodeSimilarityCheckerASTVisitor::setContext(ASTContext &context)
    {
        this->context = &context;
    }
    template <typename NodeT> void * CodeSimilarityCheckerASTVisitor::getParentNode(NodeT *aASTNode)
    {
        const auto &parents = context->getParents(*aASTNode);
        void *parentNode = NULL;
        if(!(parents.empty()))
        {
            parentNode = (void *)parents[0].template get<Decl>();
            if(!parentNode)
                parentNode = (void *)parents[0].template get<Expr>();
            if(!parentNode)
                parentNode = (void *)parents[0].template get<Stmt>();
        }
        return parentNode;
    }
    
    bool CodeSimilarityCheckerASTVisitor::VisitDecl(Decl *decl) {
        string filename = context->getSourceManager().getFilename(decl->getSourceRange().getBegin()).str();
        if(filename.find(gSrcRootPath)==string::npos)
            return true;
        void *parentNode = getParentNode<Decl>(decl);
        this->astTree->insertValueWithParent(decl,parentNode);
        return true;
    }
    
    bool CodeSimilarityCheckerASTVisitor::VisitExpr(Expr *e){
        string filename = context->getSourceManager().getFilename(e->getSourceRange().getBegin()).str();
        if(filename.find(gSrcRootPath)==string::npos)
            return true;
        void *parentNode = getParentNode<Expr>(e);
        this->astTree->insertValueWithParent(e,parentNode);
        return true;
    }
    
    bool CodeSimilarityCheckerASTVisitor::VisitStmt(Stmt *s){
        string filename = context->getSourceManager().getFilename(s->getSourceRange().getBegin()).str();
        if(filename.find(gSrcRootPath)==string::npos)
            return true;
//        string str(context->getSourceManager().getCharacterData(s->getSourceRange().getBegin()),s->getSourceRange().getEnd().getRawEncoding()-s->getSourceRange().getBegin().getRawEncoding()+1);
        void *parentNode = getParentNode<Stmt>(s);
        this->astTree->insertValueWithParent(s,parentNode);
        return true;
    }
    
    bool CodeSimilarityCheckerASTVisitor::shouldTraversePostOrder(){
        return false;
    }
    
    void CodeSimilarityCheckerASTConsumer::HandleTranslationUnit(ASTContext &context){
        visitor.setContext(context);
        visitor.TraverseDecl(context.getTranslationUnitDecl());
    }
    
    unique_ptr<ASTConsumer> CodeSimilarityCheckerASTAction::CreateASTConsumer(CompilerInstance &Compiler,llvm::StringRef InFile)
    {
        return unique_ptr<CodeSimilarityCheckerASTConsumer>(new CodeSimilarityCheckerASTConsumer);
    }
    
    bool CodeSimilarityCheckerASTAction::ParseArgs(const CompilerInstance &CI, const std::vector<std::string>& args) {
        return true;
    }
}


static llvm::cl::OptionCategory MyToolCategory("Demo Options");

int main(int argc, const char **argv) {
    // parse the command-line args passed to your code
    CommonOptionsParser op(argc, argv, MyToolCategory);
    // create a new Clang Tool instance (a LibTooling environment)
    string wspprjFile = op.getSourcePathList().at(0);
    size_t npos = wspprjFile.rfind("/");
    if(npos==string::npos){
        return 1;
    }
    string directory = wspprjFile.substr(0,npos),wspprjFilename = wspprjFile.substr(npos+1,wspprjFile.length()-npos);
    
    vector<string> projsVec;
    //Parse Workspace
    if(wspprjFilename.find(".xcworkspace") == wspprjFilename.length()-string(".xcworkspace").length()){
        ifstream ifs(directory+"/"+wspprjFilename+"/contents.xcworkspacedata");
        string content((istreambuf_iterator<char>(ifs)),(istreambuf_iterator<char>()    ) );
        vector<string> lineVec = split(content,'\n');
        
        regex prjRegex("^[\\s]+location = \"group:[\\w\/]+[\\.]xcodeproj\">$");
        for(string line : lineVec){
            smatch sm;
            regex_match(line,sm,prjRegex);
            for (std::smatch::iterator it = sm.begin(); it!=sm.end(); ++it) {
                string projFile = *it;
                string preFlagStr("group:");
                size_t pos1 = projFile.find(preFlagStr),pos2 = projFile.find("\">");
                if(pos1!=string::npos && pos2!=string::npos){
                    projsVec.push_back(projFile.substr(pos1+preFlagStr.length(),pos2-pos1-preFlagStr.length()));
                }
            }
        }
    }
    else if(wspprjFilename.find(".xcodeproj") == wspprjFilename.length()-string(".xcodeproj").length()){
        projsVec.push_back(wspprjFilename);
    }
    
    vector<string> sourceFiles;
    //Parse Project
    for(string projFileRelPath : projsVec){
        string pbxprojFile = directory+"/"+projFileRelPath;
        if(!file_exists(pbxprojFile))
            continue;
        vector<string> srcFilesVec = getSourcesByProject(pbxprojFile);
        sourceFiles.insert(sourceFiles.end(),srcFilesVec.begin(),srcFilesVec.end());
    }
    gSrcRootPath = directory;
    
    ClangTool tool(op.getCompilations(),sourceFiles);
    // LibTooling to collect all method implementations.
    vector<unique_ptr<ASTUnit>> astVec;
    int result =  tool.buildASTs(astVec);
    vector<NKASTTree *> astTreeVec;
    for(vector<unique_ptr<ASTUnit>>::iterator it = astVec.begin();it!=astVec.end();it++){
        ASTUnit *astUnit= (*it).get();
        auto tud = astUnit->getASTContext().getTranslationUnitDecl();
        // We traverse the translation unit and store all visited nodes.
        CodeSimilarityChecker::CodeSimilarityCheckerASTVisitor visitor;
        visitor.setContext(astUnit->getASTContext());
        NKASTTree *treeNode = new NKASTTree();
        treeNode->value = tud;
        visitor.astTree = treeNode;
        astTreeVec.push_back(treeNode);
        visitor.TraverseDecl(tud);
    }
    for(NKASTTree *treeNode : astTreeVec){
        treeNode->optimize();
        treeNode->uniformNames();
        treeNode->dumpWithDepth(0);
    }
    //Lib
    return result;
}
