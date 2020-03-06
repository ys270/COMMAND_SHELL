#include "environment.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

string Environment_Settings::get_path() {  //get path from PATH
  char * temp_ptr = getenv("PATH");
  string path(temp_ptr);
  return path;
}
string Environment_Settings::add_newenvp() {  //return string: "ECE551PATH=....."
  string path = get_path();
  string ECE551PATH = "ECE551PATH=";
  ECE551PATH += path;
  return ECE551PATH;
}

vector<string> Environment_Settings::pause_path(
    string & path) {  //split the path by ':', return a vector<string>
  vector<string> result;
  stringstream input(path);
  string temp;
  while (getline(input, temp, ':')) {
    result.push_back(temp);
  }
  temp.clear();
  temp.shrink_to_fit();
  return result;
}
vector<string> Environment_Settings::make_full_path(
    string & s,
    string &
        input) {  // make all possible full path to search for command, return a vector<string>
  vector<string> s_pause = pause_path(s);
  vector<string> full_path;
  for (size_t i = 0; i < s_pause.size(); i++) {
    full_path.push_back(s_pause[i] + "/" + input);
  }
  return full_path;
}
