#include "myshell.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
void MyShell::type_prompt() {  //print standarf format: ffosh:xxx$
  char * pathname = getcwd(NULL, 0);
  printf("ffosh:%s$ ", pathname);
  free(pathname);
}

void MyShell::read_command() {  //read command from cin
  std::getline(cin, command);
}

vector<string> MyShell::
    pause_command() {  //split the command into few arguments, taking care of '\' and '"'
  char * input = const_cast<char *>(command.c_str());
  char c;
  int i = 0;
  string str;
  vector<string> vec;
  int counter = 0;
  while (input[i] == ' ') {  //omit all the white space in front of the first argument
    i++;
  }
  if (input[i] == '\0') {  //case: command only has white space
    vec.push_back("");
    return vec;
  }
  while (input[i] != '\0') {
    if (input[i] == '"') {  //case:the fist character is '"'
      counter++;
      while (input[++i] != '\0') {
        if (input[i] == '\\') {  //encounter '\'
          c = input[++i];
          str.push_back(c);
        }
        else {
          if (input[i] == '"') {  //the corresponding '"' is met
            counter++;
            vec.push_back(str);
            str = "";
            i = i + 1;
            while (input[i] == ' ') {  //omit white space until we reach the next argument
              ++i;
            }
            break;
          }
          else {
            str.push_back(input[i]);
          }
        }
      }
    }
    else {                     //case:the first character of the argument is not "
      if (input[i] == '\\') {  //encounter '\'
        i++;
      }
      str.push_back(
          input[i]);  //push the fist character is not white space and not '\' into str
      while (input[i] != '\0' && input[++i] != '\0') {
        if (input[i] == '"') {  //encounter '"' inside the argument
          counter++;
          if (str != "") {
            vec.push_back(str);
            str = "";  //clear str
          }
          while (input[++i] != '\0') {
            if (input[i] == '\\') {  //encounter '\'
              c = input[++i];
            }
            else {
              if (input[i] == '"') {  //the corresponding '"' is met
                counter++;
                vec.push_back(str);
                str = "";
                while (input[i + 1] == ' ') {  //omit white space until the next argument
                  i++;
                }
                break;
              }
              else {
                str.push_back(input[i]);
              }
            }
          }
        }
        else if (input[i] == ' ') {  //encounter white space outside ""
          vec.push_back(str);
          str = "";
          while (input[i] == ' ') {
            i++;
          }
          break;
        }
        else {
          if (input[i] == '\\') {  // encounter '\'
            i = i + 1;
          }
          str.push_back(input[i]);
        }
      }
    }
  }
  if (counter % 2 != 0) {  //having unclosed quotation in the arguments
    cerr << "unclosed quotation" << endl;
    command.clear();
    command.shrink_to_fit();
    vec.clear();
    str.clear();
    vec.shrink_to_fit();
    str.shrink_to_fit();
  }
  if (command.size() == 0 || str != "") {
    vec.push_back(str);
  }
  return vec;
}

int MyShell::find_dollar(
    string & s) {  //return $'s index in the string, if there is no $,return -1
  int pos = -1;
  for (size_t i = 0; i < s.size(); i++) {
    if (s[i] == '$') {
      pos = i;
      break;
    }
  }
  return pos;
}

vector<string> MyShell::pause_dollar(string & str) {  //split the str by '$'
  vector<string> res;
  stringstream input(str);
  string temp;
  while (getline(input, temp, '$')) {
    res.push_back(temp);
  }
  return res;
}

vector<string> MyShell::
    replace_dollar() {  //if there's $ in the argument, repalce it with the value in map
  read_command();
  vector<string> input = pause_command();
  for (size_t i = 0; i < input.size(); i++) {
    int pos = find_dollar(input[i]);
    if (pos == -1) {
      continue;
    }  //case: no $ in the argument,continue to the next argument
    string str1 = input[i].substr(0, pos);
    string str2 = input[i].substr(pos);
    vector<string> res = pause_dollar(str2);
    str2 = "";
    for (size_t j = 1; j < res.size(); j++) {
      size_t k = 0;
      while (res[j][k] == '_' || isdigit(res[j][k]) || isalpha(res[j][k])) {
        k++;
      }  //read legal characters to find key
      string key = res[j].substr(0, k);
      string value = v.find_key(key);  //find the value in map
      str2 = str2 + value + res[j].substr(k);
    }
    input[i] = str1 + str2;  //replace the legal string after $
  }
  return input;
}

void MyShell::handle_command() {  //handle the command
  pid_t cpid, w;
  int wstatus;
  vector<char *>
      env;  //a temp vector for converting private member environmnet's type to char**
  string env_path_name;
  string env_value;
  int env_index = 0;
  while (environ[env_index] != NULL) {  //put all environ into the environment field
    string str(environ[env_index]);
    env_index++;
    environment.push_back(str);
    str.clear();
  }
  environment.push_back(e.add_newenvp());  //add ece551path into environmnet
  while (true) {
    if (cin.eof()) {  //case: CONTROL+D, need to free all things
      env_path_name.clear();
      env_path_name.shrink_to_fit();
      env_value.clear();
      env_value.shrink_to_fit();
      env.clear();
      delete_privatefields();
      env.shrink_to_fit();
      cout << "process finsh!" << endl;
      exit(EXIT_SUCCESS);
    }
    type_prompt();  //print out "ffosh...$ "
    vector<string> vec = replace_dollar();
    int len = vec.size();
    char ** newargv = new char *[len + 1];  //make space for newargv
    if (vec[0] == "exit") {                 //case:exit, need to free all things
      vec.clear();
      vec.shrink_to_fit();
      env_path_name.clear();
      env_path_name.shrink_to_fit();
      env_value.clear();
      env_value.shrink_to_fit();
      env.clear();
      delete_privatefields();
      env.shrink_to_fit();
      delete[] newargv;
      exit(EXIT_SUCCESS);
    }
    else if (vec[0] == "") {  //case:\n
      delete[] newargv;
      continue;
    }
    else if (vec[0] == "set") {  //case:set
      delete[] newargv;
      if (vec.size() < 3) {  //need at least three arguments
        cerr << "wrong input format!" << endl;
        continue;
      }
      size_t i;
      for (i = 0; i < vec[1].size(); i++) {  //the key_name need to be legal
        if (!(isalnum(vec[1][i]) || vec[1][i] == '_')) {
          cerr << "wrong key format!" << endl;
          break;
        }
      }
      if (i != vec[1].size()) {
        continue;
      }
      string set_value;
      size_t sec_blank = 0;
      int white_space = 0;
      for (sec_blank = 0; sec_blank < command.size(); sec_blank++) {
        if (command[sec_blank] == ' ') {
          white_space++;
        }
        if (white_space == 2) {  //find the second white space in the command
          break;
        }
      }
      set_value = command.substr(
          sec_blank +
          1);  //put all the following string into the key's corresponding value
      int p = -1;
      if ((p = set_value.find(vec[1])) != -1 || command[0] != 's') {
        cerr << "You should only type in one white space between set and key name,and no "
                "white space before set!"
             << endl;
        continue;
      }  //if the input format of set is mot met, should print this message
      v.add_element(vec[1], set_value);
      set_value.clear();
    }
    else if (vec[0] == "rev") {  //case:rev
      delete[] newargv;
      if (vec.size() != 2) {
        cerr << "wrong input format!" << endl;
        continue;
      }
      size_t i;
      for (i = 0; i < vec[1].size(); i++) {
        if (!(isalnum(vec[1][i]) || vec[1][i] == '_')) {
          cerr << "wrong key format!" << endl;
          break;
        }
      }
      if (i != vec[1].size()) {
        continue;
      }
      v.rev_element(vec[1]);
    }
    else if (vec[0] == "export") {  //case:export
      delete[] newargv;
      if (vec.size() != 2) {
        cerr << "wrong input format!" << endl;
        continue;
      }
      size_t ii;
      for (ii = 0; ii < vec[1].size(); ii++) {
        if (!(isalnum(vec[1][ii]) || vec[1][ii] == '_')) {
          cerr << "wrong key format!" << endl;
          break;
        }
      }
      if (ii != vec[1].size()) {
        continue;
      }
      string env_add = v.export_element(vec[1]);
      size_t i;
      for (i = 1; i < environment.size(); i++) {
        size_t pos = environment[i].find("=");
        env_path_name = environment[i].substr(0, pos);
        if (vec[1] ==
            env_path_name) {  //if find the exist environment path name, just change the existing path
          environment[i] = env_add;
          break;
        }
      }
      if (i == environment.size()) {  //if not find, add another path
        environment.push_back(env_add);
      }
    }
    else if (vec[0] == "cd") {  //case:cd
      if (vec.size() == 1) {    //cd without other argument
        chdir("/home/ys270");
        delete[] newargv;
      }
      else {  //cd with other argument
        int a = chdir(vec[1].c_str());
        if (a == -1) {
          perror("chdir");
          delete_privatefields();
        }
        delete[] newargv;
      }
    }
    else {  //normal cases
      cpid = fork();
      if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
      }
      else if (cpid == 0) {  //child process
        if (vec[0][0] == '/' || (vec[0][0] == '.' && vec[0][1] == '/') ||
            (vec[0][0] == '.' && vec[0][1] == '/' &&
             vec[0][2] == '/')) {  //if the user type in absolute path
          char * filename = const_cast<char *>(vec[0].c_str());
          for (int i = 0; i < len; i++) {
            newargv[i] = const_cast<char *>(vec[i].c_str());
          }
          newargv[len] = NULL;
          char * envp[] = {NULL};
          execve(filename, newargv, envp);
          cerr << "execve error" << endl;
          delete[] newargv;
          command.clear();
          vec.clear();
          vec.shrink_to_fit();
          exit(EXIT_FAILURE);
        }
        else {  //if the user did not type in the full path
          for (size_t i = 0; i < environment.size(); i++) {
            env.push_back(const_cast<char *>(environment[i].c_str()));
          }  //cast environment into env for type change
          env.push_back(NULL);
          char ** envp = env.data();  //envp's type is char** and can be put into execve
          size_t ece551_i;
          for (ece551_i = 0; ece551_i < environment.size(); ece551_i++) {
            int pos = -1;
            pos = environment[ece551_i].find("ECE551PATH");
            if (pos != -1) {
              break;
            }
          }
          int pos = environment[ece551_i].find("=");
          env_value = environment[ece551_i].substr(pos + 1);
          vector<string> input = e.make_full_path(
              env_value,
              vec[0]);  //search if there is matching path for the command
          for (size_t i = 0; i < input.size(); i++) {
            char * filename = const_cast<char *>((input[i]).c_str());
            newargv[0] = filename;
            for (int i = 1; i < len; i++) {
              newargv[i] = const_cast<char *>(vec[i].c_str());
            }
            newargv[len] = NULL;
            execve(filename,
                   newargv,
                   envp);  //if any of the path matches,the program can execve
          }
          delete[] newargv;
          cerr << "Command " << vec[0] << " not found"
               << endl;  //if the command is not find
          command.clear();
          vec.clear();
          vec.shrink_to_fit();
          exit(EXIT_FAILURE);
        }
      }
      else {  //parent process
        do {
          delete[] newargv;
          w = waitpid(-1, &wstatus, WUNTRACED | WCONTINUED);
          if (w == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
          }
          if (WIFEXITED(wstatus)) {
            if (WEXITSTATUS(wstatus) == 0) {
              printf("Program was successful\n");
            }
            else {
              printf("Program failed with code %d\n", WEXITSTATUS(wstatus));
            }
          }
          else if (WIFSIGNALED(wstatus)) {
            printf("Terminated by signal %d\n", WTERMSIG(wstatus));
          }
        } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
      }
    }
  }
}

void MyShell::
    delete_privatefields() {  //when exit or encouter eof, need to clear private fields
  command.clear();
  command.shrink_to_fit();
  v.delete_all();
  environment.clear();
  environment.shrink_to_fit();
}
