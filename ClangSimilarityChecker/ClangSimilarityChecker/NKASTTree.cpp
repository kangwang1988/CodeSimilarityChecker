//
//  NKASTTree.cpp
//  CodeSimilarityChecker
//
//  Created by KyleWong on 15/02/2017.
//  Copyright © 2017 KyleWong. All rights reserved.
//

#include "NKASTTree.hpp"
#include "CodeSimilarityCheckerUtil.hpp"
#include "CodeSimilarityChecker.h"

NKASTTree::NKASTTree(){
    this->value = NULL;
    this->children = NULL;
    this->sibling = NULL;
}

NKASTTree::~NKASTTree(){
    delete this->value;
    delete this->children;
    delete this->sibling;
}

bool NKASTTree::insertValueWithParent(void *value,void *parent){
    if(!value || !parent)
        return false;
    //Root is parent.
    if(this->value == parent){
        NKASTTree *newNode = new NKASTTree();
        newNode->value = value;
        if(!this->children){
            this->children = newNode;
        }
        else{
            NKASTTree *treeNode = this->children;
            while(treeNode && treeNode->value!=newNode->value && treeNode->sibling != NULL){
                treeNode = treeNode->sibling;
            }
            if(!treeNode->sibling && treeNode->value!=newNode->value){
                treeNode->sibling = newNode;
            }
        }
        return true;
    }
    bool found = false;
    //Search Sibling
    NKASTTree *treeNode = this->sibling;
    while(treeNode && !found){
        found = treeNode->insertValueWithParent(value, parent);
        treeNode = treeNode->sibling;
    }
    //Search Children
    treeNode = this->children;
    while(treeNode && !found){
        found = treeNode->insertValueWithParent(value, parent);
        treeNode = treeNode->sibling;
    }
    return found;
}

void NKASTTree::optimize(){
}

void NKASTTree::uniformNames(){
}

template <typename NodeT> vector<NodeT *> NKASTTree::filteredNodesOfType(){
    return vector<NodeT*>();
}

int NKASTTree::hash(){
    return 0;
}

bool NKASTTree::isEqualToASTTree(NKASTTree *astTree){
    return this->hash()==astTree->hash();
}

void NKASTTree::dumpWithDepth(int depth){
    string s(depth,' ');
    cout<<"|"<<string(depth,' ')<<(depth%2?"|":"`")<<astNodeDescription(this->value);
    NKASTTree *treeNode = this->children;
    while(treeNode){
        treeNode->dumpWithDepth(depth+1);
        treeNode=treeNode->sibling;
    }
}
