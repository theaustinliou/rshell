#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <boost/tokenizer.hpp>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

using namespace std;
using namespace boost;

int main() {
 vector<char *> vec;
 string temp;
 char_delimiters_separator<char> sep(";");
 string str;
 struct stat buffer;
 pid_t pid;
 bool failed, andFlag, orFlag, hasTrue, andFailed;
 char statFlag = 'e';
 int status;
 int testFlag;
 char ** ptr;
 char* currdir;
 char* nextdir;
 char buf[1024];
 while(1) {
 //these flags are used to determine if the next command should run, and if we see && or || respectively
 failed = false;
 andFlag = false;
 orFlag = false;
 //We introduce these two to keep track of the boolean value in parentheses
 hasTrue = false;
 andFailed = false;
 getcwd(buf, sizeof(buf));
 cout << buf << " $ ";
 //cout << getenv("PWD") << " $ ";
 getline(cin, temp);
 tokenizer<char_delimiters_separator<char> >tkn(temp, sep);
 //for each token
 for(tokenizer<>::iterator beg = tkn.begin(); beg != tkn.end(); ++beg) {
  statFlag = 'e';
  testFlag = 0;
  //if exit, then quit
  if((!beg->compare("exit")) && vec.empty()) {
    return 1;
  }
  if((!beg->compare("cd")) && vec.empty()) {
   char writable3 [100];
    int size = 0;
    int i = 0;
    ++beg;
    while((beg != tkn.end()) && (*beg != ";") && (*beg != "|") && (*beg != "&") && (*beg != ")")) {
     i++;

     str = *beg;
     for(unsigned int i = 0; i < str.size(); i++){
      writable3[i+size] = str[i];
     }
     size += str.size();
     ++beg;
   }
   writable3[size] = '\0';
    if(!strcmp(writable3, "-")) {
      nextdir = getenv("OLDPWD");
    }
    else if(!strcmp(writable3,"")) {
      nextdir = getenv("HOME");
     break;
    }
    else {
     nextdir = writable3;
    }
    getcwd(buf, sizeof(buf));
    currdir = buf;
    if(chdir(nextdir) == -1) {
     cout << "ERROR: couldn't change directory" << endl;
     break;
    }
    setenv("PWD", nextdir, 1);
    setenv("OLDPWD", currdir, 1);
    break;
   }
  //if we encounter (, then begin counting precedence
  if((!beg->compare("(")) && vec.empty()) {
   if(failed == true) {
    ++beg;
    while(*beg != ")") {
     ++beg;
    }
    ++beg;
    if(beg == tkn.end()) {
     break;
    }
    if(*beg == ";") {
     failed = false;
    }
    else if(*beg == "|") {
     ++beg;
     if(*beg == "|") {
      failed = false;
     }
    }
    else if(*beg == "&") {
     ++beg;
     if(*beg == "&") {
      failed = true;
     }
    }
    continue;
   }
   hasTrue = false;
   andFailed = false;
   continue;
  }
  //if we encounter ), finish counting precedence
  if((!beg->compare(")")) && vec.empty()) {
   ++beg;
   if(beg == tkn.end()) {
     break;
    }
    if(*beg == ";") {
     if(hasTrue && !andFailed) {
      failed = false;
     }
     else {
      failed = true;
     }
    }
    else if(*beg == "|") {
     ++beg;
     if(*beg == "|") {
      if(hasTrue && !andFailed) {
       failed = true;
      }
      else {
       failed = false;
      }
     }
    }
    else if(*beg == "&") {
     ++beg;
     if(*beg == "&") {
      if(hasTrue && !andFailed) {
       failed = false;
      }
      else {
       failed = true;
      }
     }
    }
    continue;
   }
  //here we run the test command
  if((!beg->compare("test")) && vec.empty()) {
   ++beg;
   if(!beg->compare("-")) {
    ++beg;
    if(!beg->compare("f")) {
     statFlag = 'f';
    }
    if(!beg->compare("d")) {
     statFlag = 'd';
    }
   ++beg;
   }
   
   char writable2 [100];
   int size = 0;
   while((beg != tkn.end()) && (*beg != ";") && (*beg != "|") && (*beg != "&")) {
   str = *beg;
   for(unsigned int i = 0; i < str.size(); i++){
    writable2[i+size] = str[i];
   }
   
   size += str.size();
   ++beg;
   }
   writable2[size] = '\0';
   status = stat(writable2, &buffer);

   if(failed == true) {
    failed = false;
    if(beg == tkn.end()) {
    break;
   }
    continue;
   }
   if(beg == tkn.end()) {
    testFlag = 4;
   }
   else if(*beg == ";") {
    testFlag = 0;
   }
   else if(*beg == "|") {
    ++beg;
    if(*beg == "|") {
     testFlag = 1;
    }  
   }
   else if(*beg == "&") {
    ++beg;
    if(*beg == "&") {
     testFlag = 2;
    }  
   }
   //if the file was not found
   if(status == -1) {
    cout << "(False)" << endl;
    if((testFlag == 0) || (testFlag == 1)) {
     failed = false;
    }
    else {
     failed = true;
     andFailed = true;
    }
    if(testFlag == 4) {
     break;
    }
    continue;
   }
   if(statFlag == 'e') {
    cout << "(True)" << endl;
    if((testFlag == 0) || (testFlag == 2)) {
     failed = false;
    }
    else {
     failed = true;
    }
    if(testFlag == 4) {
     break;
    }
    hasTrue = true;
    continue;
   }
   if(statFlag == 'f') {
    if(S_ISREG(buffer.st_mode) != 0) {
     cout << "(True)" << endl;
     if((testFlag == 0) || (testFlag == 2)) {
     failed = false;
     }
     else {
      failed = true;
     }
     hasTrue = true;
    }
    else {
     cout << "(False)" << endl;
     if((testFlag == 0) || (testFlag == 1)) {
     failed = false;
     }
     else {
      failed = true;
      andFailed = true;
     }
    }
    if(testFlag == 4) {
     break;
    }
    continue;
   }
   if(statFlag == 'd') {
    if(S_ISDIR(buffer.st_mode) != 0) {
     cout << "(True)" << endl;
     if((testFlag == 0) || (testFlag == 2)) {
     failed = false;
     }
     else {
      failed = true;
     }
     hasTrue = true;
    }
    else {
     cout << "(False)" << endl;
     if((testFlag == 0) || (testFlag == 1)) {
     failed = false;
     }
     else {
      failed = true;
      andFailed = true;
     }
    }
    if(testFlag == 4) {
     break;
    }
    continue;    
   }
  }
  //same as test
  if((!beg->compare("[")) && vec.empty()) {
   ++beg;
   if(!beg->compare("-")) {
    ++beg;
    if(!beg->compare("f")) {
     statFlag = 'f';
    }
    if(!beg->compare("d")) {
     statFlag = 'd';
    }
   ++beg;
   }
   
   char writable2 [100];
   int size = 0;
   while(*beg != "]") {
   str = *beg;
   for(unsigned int i = 0; i < str.size(); i++){
    writable2[i+size] = str[i];
   }
   size += str.size();
   ++beg;
   }
   ++beg;
   writable2[size] = '\0';
   status = stat(writable2, &buffer);
   
   if(failed == true) {
    failed = false;
    if(beg == tkn.end()) {
    break;
   }
    continue;
   }
   if(beg == tkn.end()) {
    testFlag = 4;
   }
   else if(*beg == ";") {
    testFlag = 0;
   }
   else if(*beg == "|") {
    ++beg;
    if(*beg == "|") {
     testFlag = 1;
    }  
   }
   else if(*beg == "&") {
    ++beg;
    if(*beg == "&") {
     testFlag = 2;
    }  
   }
   if(status == -1) {
    cout << "(False)" << endl;
    if((testFlag == 0) || (testFlag == 1)) {
     failed = false;
    }
    else {
     failed = true;
     andFailed = true;
    }
    if(testFlag == 4) {
     break;
    }
    continue;
   }
   if(statFlag == 'e') {
    cout << "(True)" << endl;
    if((testFlag == 0) || (testFlag == 2)) {
     failed = false;
    }
    else {
     failed = true;
    }
    if(testFlag == 4) {
     break;
    }
    hasTrue = true;
    continue;
   }
   if(statFlag == 'f') {
    if(S_ISREG(buffer.st_mode) != 0) {
     cout << "(True)" << endl;
     if((testFlag == 0) || (testFlag == 2)) {
     failed = false;
     }
     else {
      failed = true;
     }
     hasTrue = true;
    }
    else {
     cout << "(False)" << endl;
     if((testFlag == 0) || (testFlag == 1)) {
     failed = false;
     }
     else {
      failed = true;
      andFailed = true;
     }
    }
    if(testFlag == 4) {
     break;
    }
    continue;
   }
   if(statFlag == 'd') {
    if(S_ISDIR(buffer.st_mode) != 0) {
     cout << "(True)" << endl;
     if((testFlag == 0) || (testFlag == 2)) {
     failed = false;
     }
     else {
      failed = true;
     }
     hasTrue = true;
    }
    else {
     cout << "(False)" << endl;
     if((testFlag == 0) || (testFlag == 1)) {
     failed = false;
     }
     else {
      failed = true;
      andFailed = true;
     }
    }
    if(testFlag == 4) {
     break;
    }
    continue;    
   }
  }
   if((!beg->compare(";"))) {
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
   //if we encounter &
   if((!beg->compare("&"))) {
     if(!andFlag) {
      andFlag = 1;
      continue;
     }
     else {
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
       hasTrue = true;
      }
      else {
       failed = true;
       andFailed = true;
      }
      continue;
     }
   }
   andFlag = false;
   //if we encounter ||
   if((!beg->compare("|")))
   {
     if(!orFlag) {
      orFlag = 1;
      continue;
     }
     else {
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
      else {
       failed = false;
       vec.clear();
       continue;
      }
      vec.clear();
      orFlag = 0;
      if(WEXITSTATUS(status) == EXIT_SUCCESS) {
       failed = true;
       hasTrue = true;
      }
      else {
       failed = false;
      }
      continue;
     }
   }
  orFlag = false;
  //add the string to the command vector
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