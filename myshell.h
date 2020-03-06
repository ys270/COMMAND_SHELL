//include<sys / types.h>
//#include <sys/wait.h>
//#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "environment.h"
#include "variable.h"
using namespace std;
class MyShell {
 private:
  string command;
  variable v;
  Environment_Settings e;
  vector<string> environment;

 public:
  void type_prompt();
  void read_command();
  vector<string> pause_command(/*string command*/);
  int find_dollar(string & s);
  vector<string> pause_dollar(string & str);
  vector<string> replace_dollar();
  void handle_command();
  void delete_privatefields();
};
