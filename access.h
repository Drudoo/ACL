//
// access.h
//
// The MIT License (MIT)
//
// Copyright (c) 2015 Frederik Vanggaard Berentsen
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//  Created by Frederik Vanggaard Berentsen on 15/09/15.
//
//
// CS 465 Introduction to Computer Security: Programming Assignment #2
// Due on December 2nd, 2015
// Frederik Berentsen
// Login: xxxxxxxxxxx
// ID: xxxxxxxxxxx
// Email: xxxxxxxxxxx@mix.wvu.edu

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <sstream>
#include <map>
#include <stdlib.h> //for fun bash command.

using namespace std;

string arg1, arg2, arg3, arg4;
string u, p, g;
bool isLoggedIn;
fstream myAccounts;
fstream myAudit;
string whosLoggedIn;
bool wrongUsername, wrongPassword;
bool isFirstRun = true;
bool groupExists = false;
int checkSetup(vector<string> instructions);
int checkCommand(string username);
int checkPassword(string password);
void login(string username, string password);
void logout();
bool createUser(string username, string password);
bool createGroup(string groupname);
bool isAdmin(string username);
bool isOwner(string filename, string username);
bool addToGroup(string username, string groupname);
bool groupExist(string groupname);
bool userExist(string username);
bool fileExist(string filename);
void createFile(string filename);
bool strncasecmp(string s1, string s2);

vector<string> getUserGroup(string username);
vector<string> instructions; //for loading instructions from file.
bool canContinue(string answer);

bool contains(string s1, string s2);

map<string, vector<string> > permissions;
map<string, vector<string> > usergroups;
map<string, string> UAC;
map<string, string> userMap;

void setUAC(string username, string permissions);
string getUAC(string username);
string getUACString(string username);
void setPermissions(string filename);
void writeFile(string filename, string text);
void readFile(string filename);
void executeFile(string filename);
bool canWrite(string userPermissions);
bool canRead(string userPermissions);
bool canExecute(string userPermissions);
string getPermissions(string filename, string username);
void editPermissions(string filename, string newPermissions, string uORg);
void replacePermissions(string filename, string newPermissions, string uORg);
void denyPermissions(string filename, string newPermissions);
void log(string text);

void programExecute(string filename);
void programCreate(string filename);
void programRead(string filename);
void programWrite(string filename, string text);

bool isRestrictedName(string filename);
