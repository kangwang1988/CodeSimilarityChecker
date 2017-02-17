//
//  NKASTTree.hpp
//  CodeSimilarityChecker
//
//  Created by KyleWong on 15/02/2017.
//  Copyright © 2017 KyleWong. All rights reserved.
//

#ifndef NKASTTree_hpp
#define NKASTTree_hpp

#include <string>
#include <vector>
#include <iostream>
#include <typeinfo>

#include "clang/AST/AST.h"

using namespace std;
using namespace clang;

class NKASTTree{
private:
    int depth;
    map<int,map<string,string>> *pUniformedVarMap;
    string uniformedVarName(string varName);
    string valueDescription(bool showAddress);
    int countVarCnt();
public:
    void *value;
    NKASTTree *children;
    NKASTTree *sibling;
    NKASTTree();
    ~NKASTTree();
    void setDepth(int depth);
    void setPUniformedVarMap(map<int,map<string,string>> *pUVM);
    NKASTTree* insertValueWithParent(void *pv,void *pparent);
    void optimize();
    void uniform();
    string fingerprint();
    void dump();
    template <typename NodeT,typename BaseNodeT> void filterASTTreesOfType(vector<NKASTTree *> &astTreeVec){
        if(isa<NodeT>((BaseNodeT*)value)){
            astTreeVec.push_back(this);
            return;
        }
        NKASTTree *treeNode = children;
        while(treeNode){
            treeNode->filterASTTreesOfType<NodeT,BaseNodeT>(astTreeVec);
            treeNode=treeNode->sibling;
        }
    }
};
#endif /* NKASTTree_hpp */
