#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Environment_Settings {
 public:
  string get_path();                         //get path from PATH to initialize ECE551PATH
  string add_newenvp();                      //return string:"ECE551PATH=......"
  vector<string> pause_path(string & path);  //split path by ':'
  vector<string> make_full_path(
      string & s,
      string & input);  //make all possible path to search for command
};
