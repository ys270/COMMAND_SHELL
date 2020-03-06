#include <string.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
class variable {
  map<string, string> variables;  //the map to save things we set(format:<key,value>)
 public:
  string find_key(string key);                 //to find if the key is in the map
  void add_element(string key, string value);  //use in case set
  void rev_element(string key);                //use in case rev
  string export_element(string key);           //use in case export
  void
  delete_all();  //need to explicitly clear the private field in case of exit or cin=eof
};
