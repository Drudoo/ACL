#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <sstream>
#include <map>
using namespace std;

struct Users {
	string username;
	vector<string> groups;
	string password;
};

string arg1, arg2, arg3, arg4;
string u, p, g;
Users *listUsers = new Users[10];
bool isLoggedIn;
fstream myAccounts;
string whosLoggedIn;
bool wrongUsername, wrongPassword;
bool isFirstRun = true;
int groupCount = 0;
int numberOfUsers = 0;
bool groupExists = false;
int checkSetup(vector<string> instructions);
int checkUsername(string username);
int checkPassword(string password);
void login(string username, string password);
void logout();
bool createUser(string username, string password, int count);
bool createGroup(string groupname, int count);
bool isAdmin(string username);
bool addToGroup(string username, string groupname, int count);
bool groupExist(string groupname);
bool userExist(string username);
void createFile(string filename);
vector<string> getUserGroup(string username, int count);

map<string, vector<string> > permissions;
void setPermissions(string filename);
void writeFile(string filename, string text);
void readFile(string filename);
