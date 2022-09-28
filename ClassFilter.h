#ifndef _CLASS_FILTER_
#define _CLASS_FILTER_

#include "Error.h"
#include <utility>
#include <vector>
#include <map>

using namespace std;

// This class works as a pass between the parser and the type checker.
// The class does two jobs:
// 1) checks for cyclic inheritence.
// 2) uses topological sort to get the write order for 
//    building symbol table. e.g, class A extends class B
//    so class B must be in the table before class A.

class ClassFilter {
public:
  ClassFilter() = default;
  ClassFilter(vector<pair<string, string>> ed,
              vector<pair<string, string>> ord) : edges(ed), OrderEdges(ord) {}
  void detect();
  vector<string> getOrderList();
private:
  void buildGraph();
  void buildGraphDep();
  void dfss(int v);
  void dfs(int v);
  void clear();
  ErrorHandling err;
  vector<vector<int>>graph;
  vector<int>vis;
  vector<pair<string, string>> edges;
  vector<pair<string, string>> OrderEdges;
  map<string, int> nameToNum;
  map<int, string> numToName;
  vector<string> ordString;
  vector<int> ordInt;
};

#endif
