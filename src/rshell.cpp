#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <boost/tokenizer.hpp>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;
using namespace boost;
//http://www.boost.org/doc/libs/1_33_1/libs/tokenizer/char_separator.htm

int main() {
/*
  vector<char *> commandVector;

  // do a push_back for the command, then each of the arguments
  commandVector.push_back("ls");
  commandVector.push_back("..");

  // push NULL to the end of the vector (execvp expects NULL as last element)
  commandVector.push_back(NULL);

  // pass the vector's internal array to execvp
  char **command = &commandVector[0];

  int status = execvp(command[0], command);
  return 0;
*/
 vector<char *> vec;
 string temp;
 char_delimiters_separator<char> sep(";");
 string str;
 pid_t pid;
 bool failed, andFlag, orFlag;
 int status;
 char ** ptr;
 while(1) {
  failed = false;
  andFlag = false;
  orFlag = false;
  cout << "$ ";
  getline(cin, temp);
  tokenizer<char_delimiters_separator<char> >tkn(temp, sep);
  for(tokenizer<>::iterator beg= tkn.begin(); beg!=tkn.end(); ++beg)
  {
   if((!beg->compare("exit")) && vec.empty()) {
    return 1;
   }
   if((!beg->compare(";")))
   {
    if(!failed) {
     vec.push_back(NULL);
     ptr = &vec[0];
     pid = fork();
     if(pid == -1) {
      return -1;
     }
     if(pid == 0) {  
      if(execvp(ptr[0], ptr) == -1)
      {
         perror("Command Failed");
      }
      exit(EXIT_SUCCESS);
     }
     (void)waitpid(pid, &status, 0);
    }
    else {
     failed = false;
     vec.clear();
     continue;
    }
     vec.clear();
     failed = false;
     continue;
   }
   if((!beg->compare("&")))
   {
     if(!andFlag) {
      andFlag = 1;
      continue;
     }
     else{
      if(!failed) {   
       vec.push_back(NULL);
       ptr = &vec[0];
       pid = fork();
       if(pid == -1) {
        return -1;
       }
       if(pid == 0) {  
       if(execvp(ptr[0], ptr) == -1)
       {
         perror("Command Failed");
         exit(EXIT_FAILURE);
         //tell parent it failed
       }
       exit(EXIT_SUCCESS);
       //tell parent it succeeded
      }
      (void)waitpid(pid, &status, 0);
      }
      else {
       failed = false;
       vec.clear();
       continue;
      }
      vec.clear();
      andFlag = 0;
      if(WEXITSTATUS(status) == EXIT_SUCCESS) {
       failed = false;
      }
      else {
       failed = true;
      }
      continue;
     }
   }
   andFlag = false;
   if((!beg->compare("|")))
   {
     if(!orFlag) {
      orFlag = 1;
      continue;
     }
     else{
      if(!failed) {   
       vec.push_back(NULL);
       ptr = &vec[0];
       pid = fork();
       if(pid == -1) {
        return -1;
       }
       if(pid == 0) {  
       if(execvp(ptr[0], ptr) == -1)
       {
         perror("Command Failed");
         exit(EXIT_FAILURE);
       }
       exit(EXIT_SUCCESS);
      }
      (void)waitpid(pid, &status, 0);
      }
      else{
       failed = false;
       vec.clear();
       continue;
      }
      vec.clear();
      orFlag = 0;
      if(WEXITSTATUS(status) == EXIT_SUCCESS) {
       failed = true;
      }
      else {
       failed = false;
      }
      continue;
     }
   }
  orFlag = false;
  str = *beg;
  char * writable = new char[str.size() + 1];
  copy(str.begin(), str.end(), writable);
  writable[str.size()] = '\0';
  vec.push_back(writable);
  }
  if(!vec.empty()) {
  if(!failed) {
     vec.push_back(NULL);
     ptr = &vec[0];
     pid = fork();
     if(pid == -1) {
      return -1;
     }
     if(pid == 0) {  
      if(execvp(ptr[0], ptr) == -1)
      {
         perror("Command Failed");
      }
      exit(EXIT_SUCCESS);
     }
     (void)waitpid(pid, &status, 0);
    }
    else {
     failed = false;
     vec.clear();
     continue;
    }
     vec.clear();
     failed = false;
     continue;
 }
 }
}