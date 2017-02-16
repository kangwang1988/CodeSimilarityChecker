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
using namespace std;

class NKASTTree{
public:
    void *value;
    NKASTTree *children;
    NKASTTree *sibling;
    NKASTTree();
    ~NKASTTree();
    bool insertValueWithParent(void *value,void *parent);
    void optimize();
    void uniformNames();
    template <typename NodeT> vector<NodeT *> filteredNodesOfType();
    int hash();
    bool isEqualToASTTree(NKASTTree *astTree);
    void dumpWithDepth(int depth);
};
#endif /* NKASTTree_hpp */
