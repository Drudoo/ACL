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
int checkUsername(string username);
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
vector<string> getUserGroup(string username);

map<string, vector<string> > permissions;
void setPermissions(string filename);
void writeFile(string filename, string text);
void readFile(string filename);
bool canWrite(string userPermissions);
bool canRead(string userPermissions);
string getPermissions(string filename, string username);
void editPermissions(string filename, string newPermissions, string uORg);
void replacePermissions(string filename, string newPermissions, string uORg);
void log(string text);
