#include "variable.h"

#include <string.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
string variable::find_key(
    string key) {  //find key in the map to replace legal string after $
  map<string, string>::iterator iter;
  iter = variables.find(key);
  string res = "";
  if (iter != variables.end()) {
    res = iter->second;
  }
  if (res == "") {
    cerr << key << " not found!" << endl;
  }
  return res;
}

void variable::add_element(string key, string value) {  //use in case set
  map<string, string>::iterator iter;
  iter = variables.find(key);
  if (iter != variables.end()) {
    variables.erase(iter);
    variables.insert(pair<string, string>(key, value));
  }
  else {
    variables.insert(pair<string, string>(key, value));
  }
}

void variable::rev_element(string key) {  //use in case rev
  map<string, string>::iterator iter;
  iter = variables.find(key);
  if (iter != variables.end()) {
    reverse(iter->second.begin(), iter->second.end());
  }
  else {
    cerr << "key not find!" << endl;
  }
}

string variable::export_element(string key) {  //use in case export
  map<string, string>::iterator iter;
  iter = variables.find(key);
  string p = "";
  if (iter != variables.end()) {
    p = key + "=" + iter->second;
  }
  else {
    cerr << "key not find!" << endl;
  }
  return p;
}

void variable::delete_all() {  //delete private field in case of exit or cin=eof
  variables.clear();
}
