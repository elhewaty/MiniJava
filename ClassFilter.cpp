#include "ClassFilter.h"
#include <algorithm>

void ClassFilter::buildGraph() {
  // First give values to simple classes and derived classes
  int cntr = 1;
  // If a parent class doesn't exist terminate
  map<string, bool> exist;
  for(auto ed: edges) {
    string from = ed.first;
    if(nameToNum.count(from)) {
      cerr << BOLDRED << "Error:" << RESET << " redefinition of class "
      << from << "." << endl;
      err.error();
    }
    nameToNum[from] = cntr;
    numToName[cntr] = from;
    exist[from] = 1;
    cntr++;
  }
  graph.resize(cntr);
  for(auto ed: edges) {
    string to = ed.second;
    // Simple class
    if(to == "") {
      continue;
    }
    if(exist.count(to) == 0) {
      cerr << BOLDRED << "Error:" << RESET << " class " << to
      << " doesn't exist, so it can not be extended." << endl;
      err.error();
      err.failure();
    }
    string from = ed.first;
    graph[nameToNum[from]].push_back(nameToNum[to]);
  }
  if(err.hasErrors()) {
    err.failure();
  }
  vis.resize(graph.size());
}

void ClassFilter::dfs(int v) {
  // We will use graph coloring to detect cylces
  vis[v] = 1;
  for(auto u : graph[v]) {
    if(vis[u] == 0) {
      dfs(u);
    } else if(vis[u] == 1) {
      cerr << BOLDRED << "Error:" << RESET << " Cyclic inheritence "
      << "involving class " << numToName[u] << "." << endl;
      err.error();
    }
  }
  vis[v] = 2;
}

void ClassFilter::dfss(int v) {
  vis[v] = 1;
  for(auto u : graph[v]) {
    if(vis[u] == 0) {
      dfss(u);
    }
  }
  ordInt.push_back(v);
}


void ClassFilter::detect() {
  clear();
  buildGraph();
  for(int i = 1; i < graph.size(); i++) {
    if(!vis[i]) {
      dfs(i);
    }
  }
  if(err.hasErrors()) {
    err.failure();
  } 
}

void ClassFilter::clear() {
  graph.clear();
  vis.clear();
  edges.clear();
  nameToNum.clear();
  numToName.clear();
  ordString.clear();
  ordInt.clear();
}


void ClassFilter::buildGraphDep() {
  int cntr = 1;
  map<pair<string, string>, bool> used;
  for(auto ed: OrderEdges) {
    if(ed.second == "" || ed.first == "" || used.count({ed.first, ed.second})) {
      continue;
    }
    string from = ed.first;
    string to = ed.second;
    if(from == to) {
      continue;
    }
    nameToNum[from] = cntr;
    numToName[cntr] = from;
    cntr++;
    nameToNum[to] = cntr;
    numToName[cntr] = to;
    cntr++;
    used[{from, to}] = true;
  }
  graph.resize(cntr);
  for(auto ed: used) {
    string from = ed.first.first;
    string to = ed.first.second;
    if(from == "" || to == "") {
      continue;
    }
    graph[nameToNum[from]].push_back(nameToNum[to]);
  }
  if(err.hasErrors()) {
    err.failure();
  }
  vis.resize(graph.size());
}

vector<string> ClassFilter::getOrderList() {
  clear();
  buildGraphDep();
  for(int i = 1; i < graph.size(); i++) {
    if(graph[i].size() == 0) {
      continue;
    }
    if(!vis[i]) {
      dfss(i);
    }
  }
  reverse(ordInt.begin(), ordInt.end());
  for(auto u : ordInt) {
    ordString.push_back(numToName[u]);
  }
  for(auto u : OrderEdges) {
    string from = u.first;
    string to = u.second;
    if(!nameToNum.count(from)) {
      ordString.push_back(from);
    }
    if(!nameToNum.count(to)) {
      ordString.push_back(to);
    }
  }
  return ordString;
}

