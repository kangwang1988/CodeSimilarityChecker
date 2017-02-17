//
//  NKASTTree.cpp
//  CodeSimilarityChecker
//
//  Created by KyleWong on 15/02/2017.
//  Copyright © 2017 KyleWong. All rights reserved.
//

#include "NKASTTree.hpp"
#include "CodeSimilarityCheckerUtil.hpp"
#include <sstream>
#include "clang/AST/AST.h"
using namespace clang;

NKASTTree::NKASTTree(){
    value = NULL;
    children = NULL;
    sibling = NULL;
    pUniformedVarMap = NULL;
    depth = 0;
}

NKASTTree::~NKASTTree(){
//    delete value;
//    delete children;
//    delete sibling;
    value = NULL;
    children = NULL;
    sibling = NULL;
}

void NKASTTree::setDepth(int d){
    depth = d;
    NKASTTree *treeNode = children;
    while(treeNode){
        treeNode->setDepth(depth+1);
        treeNode=treeNode->sibling;
    }
}

void NKASTTree::setPUniformedVarMap(map<int,map<string,string>> *pUVM){
    pUniformedVarMap = pUVM;
    NKASTTree *treeNode = children;
    while(treeNode){
        treeNode->setPUniformedVarMap(pUniformedVarMap);
        treeNode=treeNode->sibling;
    }
}

NKASTTree* NKASTTree::insertValueWithParent(void *pv,void *pparent){
    if(!pv || !pparent)
        return NULL;
    //Root is parent.
    NKASTTree *newNode = NULL;
    if(value == pparent){
        newNode = new NKASTTree();
        newNode->value = pv;
        newNode->setDepth(depth+1);
        newNode->setPUniformedVarMap(pUniformedVarMap);
        if(!children){
            children = newNode;
        }
        else{
            NKASTTree *treeNode = children;
            while(treeNode && treeNode->value!=newNode->value && treeNode->sibling != NULL){
                treeNode = treeNode->sibling;
            }
            if(!treeNode->sibling && treeNode->value!=newNode->value){
                treeNode->sibling = newNode;
            }
        }
        return newNode;
    }
    bool found = false;
    //Search Sibling
    NKASTTree *treeNode = sibling;
    while(treeNode && !newNode){
        newNode = treeNode->insertValueWithParent(pv, pparent);
        treeNode = treeNode->sibling;
    }
    //Search Children
    treeNode = children;
    while(treeNode && !newNode){
        newNode = treeNode->insertValueWithParent(pv, pparent);
        treeNode = treeNode->sibling;
    }
    return newNode;
}

int NKASTTree::countVarCnt(){
    if(!pUniformedVarMap)
        return 0;
    int varCnt = 0;
    for(map<int,map<string,string>>::iterator it = pUniformedVarMap->begin();it != pUniformedVarMap->end();it++)
        varCnt+=it->second.size();
    return varCnt;
}

void NKASTTree::optimize(){
    NKASTTree *curNode = children,*preNode = NULL,*preDeclStmtNode = NULL;
    while(curNode){
        bool isCurNodeDeclStmt = isa<DeclStmt>((Stmt*)curNode->value);
        if(!isCurNodeDeclStmt){
            preDeclStmtNode = NULL;
        }
        else{
            if(!preDeclStmtNode){
                preDeclStmtNode = curNode;
            }
            else{
                NKASTTree *newNode = preDeclStmtNode->insertValueWithParent(curNode->children?curNode->children->value:NULL, preDeclStmtNode->value);
                newNode->children = curNode->children->children;
                newNode->sibling = curNode->children->sibling;
                preNode->sibling = curNode->sibling;
                delete curNode;
                curNode = preNode;
            }
        }
        preNode = curNode;
        curNode=curNode->sibling;
    }
    //Recursive optimize.
    NKASTTree *treeNode = children;
    while(treeNode){
        treeNode->optimize();
        treeNode=treeNode->sibling;
    }
}

void NKASTTree::uniform(){
    map<string,string> *pDepthMap;
    if(!pUniformedVarMap)
        return;
    if(pUniformedVarMap->find(depth)==pUniformedVarMap->end()){
        pDepthMap = new map<string,string>();
        (*pUniformedVarMap)[depth]=*pDepthMap;
    }
    pDepthMap = &(*pUniformedVarMap)[depth];
    if(isa<ObjCMethodDecl>((Decl *)value)){
        stringstream ss;
        ss<<"v"<<countVarCnt();
        ObjCMethodDecl *methodDecl = (ObjCMethodDecl* )value;
        string varOriName = methodDecl->getNameAsString(),varMappedName = ss.str();
        (*pDepthMap)[varOriName]=varMappedName;
    }
    else if(isa<ParmVarDecl>((Decl *)value)){
        stringstream ss;
        ss<<"v"<<countVarCnt();
        ParmVarDecl *pVarDecl = (ParmVarDecl* )value;
        string varOriName = pVarDecl->getNameAsString(),varMappedName = ss.str();
        (*pDepthMap)[varOriName]=varMappedName;
    }
    else if(isa<FunctionDecl>((Decl *)value)){
        stringstream ss;
        ss<<"v"<<countVarCnt();
        FunctionDecl *functionDecl = (FunctionDecl* )value;
        string varOriName = functionDecl->getNameAsString(),varMappedName = ss.str();
        (*pDepthMap)[varOriName]=varMappedName;
    }
    else if(isa<IntegerLiteral>((Stmt *)value)){
    }
    else if(isa<VarDecl>((Decl *)value)){
        stringstream ss;
        ss<<"v"<<countVarCnt();
        VarDecl *varDecl = (VarDecl* )value;
        string varOriName = varDecl->getNameAsString(),varMappedName = ss.str();
        (*pDepthMap)[varOriName]=varMappedName;
    }
    NKASTTree *treeNode = children;
    while(treeNode){
        treeNode->uniform();
        treeNode=treeNode->sibling;
    }
}

string NKASTTree::uniformedVarName(string varName){
    if(!pUniformedVarMap)
        return varName;
    for(int d=depth;d>=0;d--){
        map<string,string> m = (*pUniformedVarMap)[d];
        if(m.find(varName)!=m.end())
            return m[varName];
    }
    return varName;
}

string NKASTTree::fingerprint(){
    stringstream ss;
    ss<<valueDescription(false);
    NKASTTree *treeNode = children;
    while(treeNode){
        ss<<" |#| "<<treeNode->fingerprint();
        treeNode=treeNode->sibling;
    }
    return ss.str();
}

void NKASTTree::dump(){
    string s(depth,' ');
    cout<<"|"<<string(depth,' ')<<(depth%2?"|":"`")<<valueDescription(true)<<endl;
    NKASTTree *treeNode = children;
    while(treeNode){
        treeNode->dump();
        treeNode=treeNode->sibling;
    }
}

string NKASTTree::valueDescription(bool showAddress){
    void *astNode = value;
    stringstream ss;
    if(isa<TranslationUnitDecl>((Decl *)astNode)){
        TranslationUnitDecl *tud = (TranslationUnitDecl *)astNode;
        ss<<"-TranslationUnitDecl ";
        if(showAddress)
            ss<<tud;
    }
    else if(isa<ObjCInterfaceDecl>((Decl *)astNode)){
        ObjCInterfaceDecl *ocid = (ObjCInterfaceDecl *)astNode;
        ss<<"-ObjCInterfaceDecl ";
        if(showAddress)
            ss<<ocid;
    }
    else if(isa<ObjCMethodDecl>((Decl *)astNode)){
        ObjCMethodDecl *methodDecl = (ObjCMethodDecl *)astNode;
        if(methodDecl->hasBody()){
            ss<<"-ObjCMethodDecl ";
            if(showAddress)
                ss<<methodDecl;
            ss<<" - "<<uniformedVarName(methodDecl->getNameAsString())<<" "<<methodDecl->getReturnType().getAsString();
            if(methodDecl->param_size()>0){
                ss<<" '";
                for(ObjCMethodDecl::param_iterator pi = methodDecl->param_begin();pi!=methodDecl->param_end();pi++){
                    ParmVarDecl *pvd = *pi;
                    ss<<pvd->getType().getAsString();
                    if(pi != methodDecl->param_end()-1)
                        ss<<",";
                }
                ss<<"'";
            }
        }
    }
    else if(isa<ParmVarDecl>((Decl *)astNode)){
        ParmVarDecl *paramVarDecl = (ParmVarDecl *)astNode;
        ss<<"-ParmVarDecl ";
        if(showAddress)
            ss<<paramVarDecl;
        ss<<" "<<uniformedVarName(paramVarDecl->getNameAsString())<<" '"<<paramVarDecl->getType().getAsString()<<"' ";
    }
    else if(isa<ObjCImplDecl>((Decl *)astNode)){
        ObjCImplDecl *implDecl = (ObjCImplDecl *)astNode;
        ss<<"-ObjCImplementation ";
        if(showAddress)
            ss<<implDecl;
        ss<<" '"<<implDecl->getNameAsString()<<"'";
    }
    else if(isa<FunctionDecl>((Decl *)astNode)){
        FunctionDecl *functionDecl = (FunctionDecl *)astNode;
        if(functionDecl->hasBody()){
            ss<<"-FunctionDecl ";
            if(showAddress)
                ss<<functionDecl;
            ss<<" - "<<uniformedVarName(functionDecl->getNameAsString())<<" "<<functionDecl->getReturnType().getAsString();
            if(functionDecl->param_size()>0){
                ss<<" '";
                for(FunctionDecl::param_iterator pi = functionDecl->param_begin();pi!=functionDecl->param_end();pi++){
                    ParmVarDecl *pvd = *pi;
                    ss<<pvd->getType().getAsString();
                    if(pi != functionDecl->param_end()-1)
                        ss<<",";
                }
                ss<<"'";
            }
        }
    }
    else if(isa<StringLiteral>((Stmt *)astNode)){
        StringLiteral *literal = (StringLiteral *)astNode;
        ss<<"-StringLiteral ";
        if(showAddress)
            ss<<literal;
        ss<<" '"<<literal->getType().getAsString()<<" '"<<"\""<<literal->getString().str()<<"\"";
    }
    else if(isa<IntegerLiteral>((Stmt *)astNode)){
        IntegerLiteral *literal = (IntegerLiteral *)astNode;
        ss<<"-IntegerLiteral ";
        if(showAddress)
            ss<<literal;
        ss<<" '"<<literal->getType().getAsString()<<" '"<<"\""<<*(literal->getValue().getRawData())<<"\"";
    }
    else if(isa<FloatingLiteral>((Stmt *)astNode)){
        FloatingLiteral *literal = (FloatingLiteral *)astNode;
        ss<<"-FloatingLiteral ";
        if(showAddress)
            ss<<literal;
        ss<<" '"<<literal->getType().getAsString()<<" '"<<"\""<<literal->getValue().convertToFloat()<<"\"";
    }
    else if(isa<ImplicitParamDecl>((VarDecl *)astNode)){
        ImplicitParamDecl *paramDecl = (ImplicitParamDecl *)astNode;
        ss<<"-ImplicitParamDecl ";
        if(showAddress)
            ss<<paramDecl;
        ss<<" implicit "<<paramDecl->getNameAsString()<<" '"<<paramDecl->getType().getAsString()<<"' ";
    }
    else if(isa<VarDecl>((Decl *)astNode)){
        VarDecl *varDecl = (VarDecl *)astNode;
        ss<<"-VarDecl ";
        if(showAddress)
            ss<<varDecl;
        ss<<" "<<uniformedVarName(varDecl->getNameAsString())<<" '"<<varDecl->getType().getAsString()<<"' ";
    }
    else if(isa<CallExpr>((Stmt *)astNode)){
        CallExpr *callExpr = (CallExpr *)astNode;
        ss<<"-CallExpr ";
        if(showAddress)
            ss<<callExpr;
        ss<<" '"<<callExpr->getType().getAsString()<<"' ";
    }
    else if(isa<DeclRefExpr>((Stmt *)astNode)){
        DeclRefExpr *refExpr = (DeclRefExpr *)astNode;
        ss<<"-DeclRefExpr ";
        if(showAddress)
            ss<<refExpr;
        ss<<" '"<<uniformedVarName(refExpr->getNameInfo().getAsString())<<"'  '"<<refExpr->getType().getAsString()<<"'";
    }
    else if(isa<ImplicitCastExpr>((Stmt *)astNode)){
        ImplicitCastExpr *castExpr = (ImplicitCastExpr *)astNode;
        ss<<"-ImplicitCastExpr ";
        if(showAddress)
            ss<<castExpr;
        ss<<" "<<castExpr->getCastKindName()<<" '"<<castExpr->getType().getAsString()<<"' ";
    }
    else if(isa<ObjCMessageExpr>((Stmt *)astNode)){
        ObjCMessageExpr *objcExpr = (ObjCMessageExpr *)astNode;
        string returnType = "";
        if(objcExpr->getMethodDecl())
            returnType = string(" '")+objcExpr->getMethodDecl()->getReturnType().getAsString()+string(" '");
        ss<<"-ObjCMessageExpr ";
        if(showAddress)
            ss<<objcExpr;
        ss<<returnType<<" selector = "<<objcExpr->getSelector().getAsString()<<" class = '"<<objcExpr->getReceiverType().getAsString()<<"'";
        
    }
    else if(isa<ObjCStringLiteral>((Stmt *)astNode)){
        ObjCStringLiteral *literal = (ObjCStringLiteral *)astNode;
        ss<<"-ObjCStringLiteral ";
        if(showAddress)
            ss<<literal;
        ss<<" '"<<literal->getType().getAsString()<<"' ";
    }
    else if(isa<CompoundStmt>((Stmt *)astNode)){
        CompoundStmt *compStmt = (CompoundStmt *)astNode;
        ss<<"-CompoundStmt ";
        if(showAddress)
            ss<<compStmt;
    }
    else if(isa<DeclStmt>((Stmt *)astNode)){
        DeclStmt *declStmt = (DeclStmt *)astNode;
        ss<<"-DeclStmt  ";
        if(showAddress)
            ss<<declStmt;
    }
    else if(isa<BinaryOperator>((Stmt *)astNode)){
        BinaryOperator *bo = (BinaryOperator *)astNode;
        ss<<"-BinaryOperator  ";
        if(showAddress)
            ss<<bo;
        ss<<bo->getOpcodeStr().str();
    }
    else if(isa<IfStmt>((Stmt *)astNode)){
        IfStmt *ifs = (IfStmt *)astNode;
        ss<<"-IfStmt  ";
        if(showAddress)
            ss<<ifs;
    }
    else if(isa<ReturnStmt>((Stmt *)astNode)){
        ReturnStmt *rs = (ReturnStmt *)astNode;
        ss<<"-ReturnStmt  ";
        if(showAddress)
            ss<<rs;
    }
    return ss.str();
}
