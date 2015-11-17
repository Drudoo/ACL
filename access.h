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
fstream myAudit;
string whosLoggedIn;
bool wrongUsername, wrongPassword;
bool isFirstRun = true;
int groupCount = 0;
int numberOfUsers = 0;
bool groupExists = false;
int checkSetup(vector<string> instructions);
int checkCommand(string username);
int checkPassword(string password);
void login(string username, string password);
void logout();
bool createUser(string username, string password, int count);
bool createGroup(string groupname, int count);
bool isAdmin(string username);
bool isOwner(string filename, string username);
bool addToGroup(string username, string groupname, int count);
bool groupExist(string groupname);
bool userExist(string username);
bool fileExist(string filename);
void createFile(string filename);
bool strncasecmp(string s1, string s2);
vector<string> getUserGroup(string username);

bool contains(string s1, string s2);

map<string, vector<string> > permissions;
map<string, vector<string> > usergroups;
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
