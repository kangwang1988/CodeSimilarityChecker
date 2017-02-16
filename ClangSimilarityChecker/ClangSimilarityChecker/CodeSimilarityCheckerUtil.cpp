//
//  CodeSimilarityCheckerUtil.cpp
//
//  Created by KyleWong on 11/10/2016.
//
//

#include "CodeSimilarityCheckerUtil.hpp"
#include <streambuf>

string gSrcRootPath = "";
string filename;

string astNodeDescription(void *astNode){
    stringstream ss;
    if(isa<TranslationUnitDecl>((Decl *)astNode)){
        TranslationUnitDecl *tud = (TranslationUnitDecl *)astNode;
        ss<<"-TranslationUnitDecl "<<tud<<endl;
    }
    else if(isa<ObjCInterfaceDecl>((Decl *)astNode)){
        ObjCInterfaceDecl *ocid = (ObjCInterfaceDecl *)astNode;
        ss<<"-ObjCInterfaceDecl "<<ocid<<endl;
    }
    else if(isa<ObjCMethodDecl>((Decl *)astNode)){
        ObjCMethodDecl *methodDecl = (ObjCMethodDecl *)astNode;
        if(methodDecl->hasBody()){
            ss<<"-ObjCMethodDecl "<<methodDecl<<" - "<<methodDecl->getNameAsString()<<" '"<<methodDecl->getReturnType().getAsString()<<"' "<<endl;
        }
    }
    else if(isa<ImplicitParamDecl>((Decl *)astNode)){
        ImplicitParamDecl *paramDecl = (ImplicitParamDecl *)astNode;
        ss<<"-ImplicitParamDecl "<<paramDecl<<" implicit "<<paramDecl->getNameAsString()<<" '"<<paramDecl->getType().getAsString()<<"' "<<endl;
    }
    else if(isa<ParmVarDecl>((Decl *)astNode)){
        ParmVarDecl *pVarDecl = (ParmVarDecl *)astNode;
        ss<<"-ParmVarDecl "<<pVarDecl<<" "<<pVarDecl->getNameAsString()<<" '"<<pVarDecl->getType().getAsString()<<"' "<<endl;
    }
    else if(isa<VarDecl>((Decl *)astNode)){
        VarDecl *varDecl = (VarDecl *)astNode;
        ss<<"-VarDecl "<<varDecl<<" "<<varDecl->getNameAsString()<<" '"<<varDecl->getType().getAsString()<<"' "<<endl;
    }
    else if(isa<ObjCStringLiteral>((Stmt *)astNode)){
        ObjCStringLiteral *literal = (ObjCStringLiteral *)astNode;
        ss<<"-ObjCStringLiteral "<<literal<<" '"<<literal->getType().getAsString()<<"' "<<endl;
    }
    else if(isa<StringLiteral>((Stmt *)astNode)){
        StringLiteral *literal = (StringLiteral *)astNode;
        ss<<"-StringLiteral "<<literal<<" '"<<literal->getType().getAsString()<<" '"<<"\""<<literal->getString().str()<<"\""<<endl;
    }
    else if(isa<IntegerLiteral>((Stmt *)astNode)){
        IntegerLiteral *literal = (IntegerLiteral *)astNode;
        ss<<"-IntegerLiteral "<<literal<<" '"<<literal->getType().getAsString()<<" '"<<"\""<<*(literal->getValue().getRawData())<<"\""<<endl;
    }
    else if(isa<FloatingLiteral>((Stmt *)astNode)){
        FloatingLiteral *literal = (FloatingLiteral *)astNode;
        ss<<"-FloatingLiteral "<<literal<<" '"<<literal->getType().getAsString()<<" '"<<"\""<<literal->getValue().convertToFloat()<<"\""<<endl;
    }
    else if(isa<CallExpr>((Stmt *)astNode)){
        CallExpr *callExpr = (CallExpr *)astNode;
        ss<<"-CallExpr "<<callExpr<<" '"<<callExpr->getType().getAsString()<<"' "<<endl;
    }
    else if(isa<DeclRefExpr>((Stmt *)astNode)){
        DeclRefExpr *refExpr = (DeclRefExpr *)astNode;
        ss<<"-DeclRefExpr "<<refExpr<<" '"<<refExpr->getNameInfo().getAsString()<<"'  '"<<refExpr->getType().getAsString()<<"'"<<endl;
    }
    else if(isa<ImplicitCastExpr>((Stmt *)astNode)){
        ImplicitCastExpr *castExpr = (ImplicitCastExpr *)astNode;
        ss<<"-ImplicitCastExpr "<<castExpr<<" "<<castExpr->getCastKindName()<<" '"<<castExpr->getType().getAsString()<<"' "<<endl;
    }
    else if(isa<ObjCMessageExpr>((Stmt *)astNode)){
        ObjCMessageExpr *objcExpr = (ObjCMessageExpr *)astNode;
        string returnType = "";
        if(objcExpr->getMethodDecl())
            returnType = string(" '")+objcExpr->getMethodDecl()->getReturnType().getAsString()+string(" '");
        ss<<"-ObjCMessageExpr "<<objcExpr<<returnType<<" selector = "<<objcExpr->getSelector().getAsString()<<" class = '"<<objcExpr->getReceiverType().getAsString()<<"'"<<endl;
        
    }
    else if(isa<CompoundStmt>((Stmt *)astNode)){
        CompoundStmt *compStmt = (CompoundStmt *)astNode;
        ss<<"-CompoundStmt "<<compStmt<<endl;
    }
    else if(isa<DeclStmt>((Stmt *)astNode)){
        DeclStmt *declStmt = (DeclStmt *)astNode;
        ss<<"-DeclStmt  "<<declStmt<<endl;
    }
    else if(isa<BinaryOperator>((Stmt *)astNode)){
        BinaryOperator *bo = (BinaryOperator *)astNode;
        ss<<"-BinaryOperator  "<<bo->getOpcodeStr().str()<<endl;
    }
    else if(isa<IfStmt>((Stmt *)astNode)){
        IfStmt *ifs = (IfStmt *)astNode;
        ss<<"-IfStmt  "<<endl;
    }
    else if(isa<ReturnStmt>((Stmt *)astNode)){
        ReturnStmt *rs = (ReturnStmt *)astNode;
        ss<<"-ReturnStmt  "<<endl;
    }
    else if(isa<ObjCImplDecl>((Decl *)astNode)){
        ObjCImplDecl *implDecl = (ObjCImplDecl *)astNode;
        ss<<"-ObjCImplementation "<<implDecl<<" '"<<implDecl->getNameAsString()<<"'"<<endl;
    }
    else if(isa<FunctionDecl>((Decl *)astNode)){
        FunctionDecl *functionDecl = (FunctionDecl *)astNode;
        if(functionDecl->hasBody()){
            ss<<"-FunctionDecl "<<functionDecl<<" - "<<functionDecl->getNameAsString()<<" '"<<functionDecl->getType().getAsString()<<"' "<<endl;
        }
    }
    return ss.str();
}
