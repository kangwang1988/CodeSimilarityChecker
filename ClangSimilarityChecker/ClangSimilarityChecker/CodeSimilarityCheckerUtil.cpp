//
//  CodeSimilarityCheckerUtil.cpp
//
//  Created by KyleWong on 11/10/2016.
//
//

#include "CodeSimilarityCheckerUtil.hpp"
#include <sys/stat.h>
#include <sstream>
using namespace std;

string gSrcRootPath = "";
string filename;

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

bool file_exists(const string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}
