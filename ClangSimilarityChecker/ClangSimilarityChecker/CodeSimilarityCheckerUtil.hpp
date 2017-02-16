//
//  CodeSimilarityCheckerUtil.hpp
//
//  Created by KyleWong on 14/10/2016.
//
//

#ifndef CodeSimilarityCheckerUtil_hpp
#define CodeSimilarityCheckerUtil_hpp
#include "clang/AST/AST.h"
#include <string>
#include<algorithm>
#include <functional>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <sys/stat.h>

#include <sstream>

#define kAppMainEntryClass  "UIApplication"
#define kAppMainEntrySelector  "main"

using namespace std;
using namespace clang;

string astNodeDescription(void *astNode);

static vector<string> split(const string &s, char delim) {
    vector<string> elems;
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

inline bool file_exists(const string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}
#endif /* CodeSimilarityCheckerUtil_hpp */
