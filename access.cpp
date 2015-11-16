#include "access.h"
#include <typeinfo> //just for debug
vector< vector<string> > groups(10, vector<string>(1)); //Only supports 10 user groups. Variable is global, so we don't have to pass it all the time.


int main(int argc, char const *argv[]) {


	//JUST FOR DEBUG. PLEASE DELETE!

	permissions["Filename "].push_back("Owner");
	permissions["Filename "].push_back("Permissions");

	//-------------------------------!!!!

	vector<string> instructions; //for loading instructions from the file.
	whosLoggedIn = "admin"; //admin is logged in from the start to create the default groups and admin user.
	if (argc < 2) { //if we don't have a filename, give an error and exit.
		cout << "Error: Please specify a filename.\nUse the format './access filename'" << endl;
		return 1;
	} else {
		ifstream myInput (argv[1]);
		string line;
		if (myInput.is_open()) {
			while (getline(myInput,line)){
				instructions.push_back(line);
			}
			myInput.close();
		} else {
			cout << "Error: Cannot open file: " << argv[1] << "\nExiting...\n" << endl;
			return 1;
		}
	}

	myAccounts.open("accounts.txt", ios_base::out | ios_base::in);  // will not create file

	if (myAccounts.is_open()) {
		remove("accounts.txt");
	}
	myAccounts.open ("accounts.txt");
	myAccounts.close();

	myAudit.open("audit.txt", ios_base::out | ios_base::in);
	if (myAudit.is_open()) {
		remove("audit.txt");
	}
	myAudit.open("audit.txt", ios_base::app);
	myAudit.close();

	if (checkSetup(instructions)!=0) return 1;

	for (size_t i = 0; i < instructions.size(); i++) {
		istringstream in(instructions[i]);
		in >> arg1;
		//cout << arg1 << " " << arg2 << endl;
		if (arg1 == "net") {
			in >> arg2;
			if (arg2 == "group") {
				u = "";
				in >> g >> u;
				if(u=="") {
					if (createGroup(g, numberOfUsers)) {
						groupCount++;
					} else {
						cout << "Error: only an Administrator may issue net group command" << endl;
					}
				} else {
					if (addToGroup(u, g, numberOfUsers)) {
						cout << "User " << u << " added to group " << g << endl;
						myAudit.open("audit.txt", ios_base::app);
						myAudit << "User " << u << " added to group " << g << endl;
						myAudit.close();
					}
				}
			}

			if (arg2 == "user") {
				in >> u >> p;
				if(createUser(u, p, numberOfUsers)) {
					numberOfUsers++;
				}
			}
		}

		if (arg1 == "login") {
			in >> u >> p;
			if(isLoggedIn) {
				cout << "Login failed: simultaneous login not permitted" << endl;
				myAudit.open("audit.txt", ios_base::app);
				myAudit << "Login failed: simultaneous login not permitted" << endl;
				myAudit.close();
			} else {
				login(u,p);
			}
		}
		if (arg1 == "logout") {
			logout();
		}

		if (arg1 == "create") {
			in >> arg2;
			createFile(arg2);
		}
		if (arg1 == "write") {
			in >> arg2;
			string text;
			getline(in, text);
			text.erase(0,1);
			writeFile(arg2, text);
		}
		if (arg1 == "xcacls") {
			string filename, uORg, newPermissions;
			in >> filename >> arg2;
			if (arg2 == "/E") {
				in >> uORg >> newPermissions;
				//cout << ">> " << filename << " " << arg2 << " " << uORg << " " << newPermissions << endl;
				editPermissions(filename, newPermissions, uORg);
			} else {
				in >> uORg;
				//cout << ">> " << filename << " " << arg2 << " " << uORg << endl;
				replacePermissions(filename,newPermissions, uORg);


			}
		}

		if (arg1 == "read") {
			string filename;
			in >> filename;
			readFile(filename);
		}

	}

	cout << "_________TEST-OUTPUT__STRUCT_______" << endl;
	for (size_t i = 0; i < numberOfUsers; i++) {
		cout << listUsers[i].username << " ";
		for (size_t j = 0; j < listUsers[i].groups.size(); j++) {
			cout << listUsers[i].groups[j] << " ";
		}
		cout << endl;
	}

	//vector<string> testGroups = getUserGroup("admin",count);

	cout << "_________TEST-OUTPUT__VECTOR_______" << endl;
	for (size_t i = 0; i < groupCount; i++) {
		if (groups[i].size() < groups[i+1].size()) {
			for (size_t j = 0; j < groups[i].size(); j++) {
				cout << groups[i][j] << " "; // "(" << i << ","<< j<< ") "
			}
		} else {
			for (size_t j = 0; j < groups[i].size(); j++) {
				cout << groups[i][j] << " "; //  "(" << i << ","<< j<< ") "
			}
		}

		cout << endl;
	}

	cout << "_________TEST-OUTPUT_______________" << endl;
	for (size_t i = 0; i < numberOfUsers; i++) {
		cout << listUsers[i].username << endl;
	}

	cout << "_________TEST-OUTPUT__MAP__________" << endl;
	for(auto ii=permissions.begin(); ii!=permissions.end(); ++ii){
		cout << (*ii).first << ": ";
		vector <string> inVect = (*ii).second;
		for (unsigned j=0; j<inVect.size(); j++){
			cout << inVect[j] << " ";
		}
		cout << endl;
	}

	cout << "_________TEST-OUTPUT__Files________" << endl;
	string line;

	myAccounts.open("file1.txt");
	while (getline(myAccounts,line)) {
		cout << line << endl;
	}
	myAccounts.close();
	myAccounts.open("file2.txt");
	while (getline(myAccounts,line)) {
		cout << line << endl;
	}
	myAccounts.close();



	return 0;
}

void editPermissions(string filename, string newPermissions, string uORg) {
	if (isLoggedIn) {
		if (fileExist(filename)) {
			if (isOwner(filename,whosLoggedIn) || isAdmin(whosLoggedIn)) {
				permissions[filename].push_back(newPermissions);
				cout << "The ACL for " << filename << " appended by " << whosLoggedIn << " to include " << newPermissions << endl;
				myAudit.open("audit.txt", ios_base::app);
				myAudit << "The ACL for " << filename << " appended by " << whosLoggedIn << " to include " << newPermissions << endl;
				myAudit.close();
			} else {
				cout << "Error with xcacls: Only file owner or member of Administrators group may run command" << endl;
				myAudit.open("audit.txt", ios_base::app);
				myAudit << "Error with xcacls: Only file owner or member of Administrators group may run command" << endl;
				myAudit.close();
			}
		} else {
			cout << "Error: File " << filename << " does not exist" << endl;
			myAudit.open("audit.txt", ios_base::app);
			myAudit << "Error: File " << filename << " does not exist" << endl;
			myAudit.close();
		}
	} else {
		cout << "Error: Please login to change permissions file" << endl;
		myAudit.open("audit.txt", ios_base::app);
		myAudit << "Error: Please login to change permissions file" << endl;
		myAudit.close();
	}
}


void replacePermissions(string filename, string newPermissions, string uORg) {
	if (isLoggedIn) {
		if (fileExist(filename)) {
			if (isOwner(filename,whosLoggedIn) || isAdmin(whosLoggedIn)) {

			} else {
				cout << "Error with xcacls: Only file owner or member of Administrators group may run command" << endl;
				myAudit.open("audit.txt", ios_base::app);
				myAudit << "Error with xcacls: Only file owner or member of Administrators group may run command" << endl;
				myAudit.close();
			}
		}
	}
}

bool fileExist(string filename) {
	return (permissions.find(filename) != permissions.end())?true:false;
}

void createFile(string filename) {
	if (isLoggedIn) {
		fstream myFile;
		myFile.open(filename, ios_base::out | ios_base::in);  // will not create file

		/*if (myFile.is_open()) {
			remove(filename.c_str());
			cout << ">> REMOVED: " << filename << endl;
		}
		myFile.close();
*/
		if (!fileExist(filename)) {
			myFile.open(filename, ios_base::app);
			myFile.close();
			cout << "File " << filename << " with owner " << whosLoggedIn << " and default permissions created" << endl;
			myAudit.open("audit.txt", ios_base::app);
			myAudit << "File " << filename << " with owner " << whosLoggedIn << " and default permissions created" << endl;
			myAudit.close();
			setPermissions(filename);
		} else {
			cout << "Error: file " << filename << " already exists" << endl;
			myAudit.open("audit.txt", ios_base::app);
			myAudit << "Error: file " << filename << " already exists" << endl;
			myAudit.close();
		}
	} else {
		cout << "Error: Please login to create file" << endl;
		myAudit.open("audit.txt", ios_base::app);
		myAudit << "Error: Please login to create file" << endl;
		myAudit.close();
	}

}

void setPermissions(string filename) {
	if (isAdmin(whosLoggedIn)) {
		permissions[filename].push_back(whosLoggedIn); // filename.txt username
		permissions[filename].push_back(whosLoggedIn+":F"); // adds user rights
		permissions[filename].push_back("Administrators:F"); //adds admin rights
		permissions[filename].push_back("Users:R"); //adds user read rights

	} else {
		permissions[filename].push_back(whosLoggedIn); // filename.txt username
		permissions[filename].push_back(whosLoggedIn+":F"); // adds user rights
		permissions[filename].push_back("Administrators:F"); //adds admin rights
	}
}

string getPermissions(string filename, string username) {
	vector<string> group;
	group = getUserGroup(username);
	string highestUserPermissionLevel = "";
	string highestGroupPermissionLevel = "";

	for (size_t i = 0; i < permissions[filename].size(); i++) {
		if (permissions[filename][i] == (whosLoggedIn+":F")) {
			highestUserPermissionLevel = "F";
		}
		if (permissions[filename][i] == (whosLoggedIn+":R")) {
			highestUserPermissionLevel = "R";
		}
		if (permissions[filename][i] == (whosLoggedIn+":X")) {
			highestUserPermissionLevel = "X";
		}
		if (permissions[filename][i] == (whosLoggedIn+":W")) {
			highestUserPermissionLevel = "W";
		}
		if (permissions[filename][i] == (whosLoggedIn+":D")) {
			highestUserPermissionLevel = "D";
		}
	}

	for (size_t i = 0; i < permissions[filename].size(); i++) {
		for (size_t j = 0; j < group.size(); j++) {
			if (permissions[filename][i] == (group[j]+":F")) {
				highestGroupPermissionLevel = "F";
			}
			if (permissions[filename][i] == (group[j]+":R")) {
				highestGroupPermissionLevel = "R";
			}
			if (permissions[filename][i] == (group[j]+":X")) {
				highestGroupPermissionLevel = "X";
			}
			if (permissions[filename][i] == (group[j]+":W")) {
				highestGroupPermissionLevel = "W";
			}
			if (permissions[filename][i] == (group[j]+":D")) {
				highestGroupPermissionLevel = "D";
			}
			if (permissions[filename][i] == (group[j]+":RW")) {
				highestGroupPermissionLevel = "RW";
			}
		}
	}

	return (highestUserPermissionLevel == "")?highestGroupPermissionLevel:highestUserPermissionLevel;
}

bool canWrite(string userPermissions) {
	if (userPermissions.find("W") != std::string::npos) {
    	return true;
	} else if (userPermissions.find("F") != std::string::npos) {
		return true;
	} else {
		return false;
	}
}

void writeFile(string filename, string text) {
	string userPermissions = getPermissions(filename, whosLoggedIn);
	fstream myFile;
	//cout << ">> permissions for " << whosLoggedIn << " is " << userPermissions << " and canWrite: " <<  (canWrite(userPermissions)?"true":"false") << endl;

	if (canWrite(userPermissions)) {
		myFile.open(filename, std::ios_base::app);
		myFile << text << endl;
		myFile.close();
		cout << "User " << whosLoggedIn << " wrote to " << filename << ": " << text << endl;
		myAudit.open("audit.txt", ios_base::app);
		myAudit << "User " << whosLoggedIn << " wrote to " << filename << ": " << text << endl;
		myAudit.close();
	} else {
		cout << "User " << whosLoggedIn << " denied write access to " << filename << endl;
		myAudit.open("audit.txt", ios_base::app);
		myAudit << "User " << whosLoggedIn << " denied write access to " << filename << endl;
		myAudit.close();
	}

}
void readFile(string filename) {
	string userPermissions = getPermissions(filename,whosLoggedIn);
	if (isLoggedIn) {
		if (fileExist(filename)) {
			if (canRead(userPermissions)) {
				fstream myFile;
				string line;
				myFile.open(filename);
				cout << "User " << whosLoggedIn << " read " << filename << " as:" << endl;
				myAudit.open("audit.txt", ios_base::app);
				myAudit << "User " << whosLoggedIn << " read " << filename << " as:" << endl;

				while (getline(myFile,line)) {
					cout << line << endl;
					myAudit << line << endl;
				}
				myAudit.close();
				myFile.close();
			} else {
				cout << "User " << whosLoggedIn << " denied read access to " << filename << endl;
				myAudit.open("audit.txt", ios_base::app);
				myAudit << "User " << whosLoggedIn << " denied read access to " << filename << endl;
				myAudit.close();
			}
		} else {
			cout << "Error: File " << filename << " does not exist" << endl;
			myAudit.open("audit.txt", ios_base::app);
			myAudit << "Error: File " << filename << " does not exist" << endl;
			myAudit.close();
		}
	} else {
		cout << "Error: no user logged in" << endl;
		myAudit.open("audit.txt", ios_base::app);
		myAudit << "Error: no user logged in" << endl;
		myAudit.close();
	}

}

bool canRead(string userPermissions) {
	if (userPermissions.find("R") != std::string::npos) {
    	return true;
	} else if (userPermissions.find("F") != std::string::npos) {
		return true;
	} else {
		return false;
	}
}

bool addToGroup(string username, string groupname, int numberOfUsers) {
	int temp;
	bool foundUser = false;
	if (groupCount == 1 || isAdmin(whosLoggedIn)) {
		for (size_t m = 0; m < numberOfUsers; m++) {
			if (username == listUsers[m].username) {
				temp = m;
				foundUser = true;
			}
		}
		if (foundUser) {
			for (size_t k = 0; k < groupCount; k++) {
				if (groupname == groups[k][0]) {
					groups[k].push_back(u);
					listUsers[temp].groups.push_back(g);
					return true;
				}
			}
			cout << "Group " << groupname << " does not exist" << endl;
			myAudit.open("audit.txt", ios_base::app);
			myAudit << "Group " << groupname << " does not exist" << endl;
			myAudit.close();
		} else {
			cout << "User " << username << " does not exist" << endl;
			myAudit.open("audit.txt", ios_base::app);
			myAudit << "User " << username << " does not exist" << endl;
			myAudit.close();
		}
	} else {
		cout << "Error: only an Administrator may issue net group command" << endl;
		myAudit.open("audit.txt", ios_base::app);
		myAudit << "Error: only an Administrator may issue net group command" << endl;
		myAudit.close();
	}
	return false;
}

bool createGroup(string groupname, int numberOfUsers) {
	if (groupCount>0) {
		if (isAdmin(whosLoggedIn)) {
			for (size_t j = 0; j < groupCount; j++) {
				if (g == groups[j][0]) {
					groupExists = true;
					break;
				}
			}
			if (!groupExists) {
				groups[groupCount][0] = g;
				cout << "Group " << g << " created" << endl;
				myAudit.open("audit.txt", ios_base::app);
				myAudit << "Group " << g << " created" << endl;
				myAudit.close();
			} else {
				cout << "Error: Group " << g << " already exists" << endl;
				myAudit.open("audit.txt", ios_base::app);
				myAudit << "Error: Group " << g << " already exists" << endl;
				myAudit.close();
				groupCount--;
			}
			groupExists=false;
		} else {
			return false;
		}
	} else {
		groups[groupCount][0] = g;
		cout << "Group " << g << " created" << endl;
		myAudit.open("audit.txt", ios_base::app);
		myAudit << "Group " << g << " created" << endl;
		myAudit.close();
	}
	return true;
}

bool isAdmin(string username) {

	vector<string> userGroup = getUserGroup(whosLoggedIn);

	if (find(userGroup.begin(), userGroup.end(), "Administrators") != userGroup.end() || isFirstRun) {
		return true;
	} else {
		return false;
	}
}

bool isOwner(string filename, string username) {
	return (permissions[filename][0] == username)?true:false;
}

bool createUser(string username, string password, int numberOfUsers) {

	if (isAdmin(whosLoggedIn)) {
		if (!(find(groups[1].begin(), groups[1].end(), username) != groups[1].end())) {
			if (!checkUsername(username)) {
				groups[1].push_back(username);
				listUsers[numberOfUsers].groups.push_back("Users");
				listUsers[numberOfUsers].username = username;
				cout << "User " << username << " created" << endl;
				myAudit.open("audit.txt", ios_base::app);
				myAudit << "User " << username << " created" << endl;
				myAudit.close();
				myAccounts.open("accounts.txt", std::ios_base::app);
				myAccounts << username << " " << password << endl;
				myAccounts.close();
				isFirstRun = false;

				// DEBUG:
				cout << "Notify only on change" << endl;
				myAudit.open("audit.txt", ios_base::app);
				myAudit << "Notify only on change" << endl;
				myAudit.close();
				// -------
			}
		} else {
			cout << "Error: user " << username << " already exists" << endl;
			myAudit.open("audit.txt", ios_base::app);
			myAudit << "Error: user " << username << " already exists" << endl;
			myAudit.close();
			return false;
		}
		return true;
	} else {
		cout << "Error: only an Administrator may issue net user command" << endl;
		myAudit.open("audit.txt", ios_base::app);
		myAudit << "Error: only an Administrator may issue net user command" << endl;
		myAudit.close();
		return false;
	}
}

int checkSetup(vector<string> instructions) {
	if (instructions[0].substr(0, 14) != "net user admin") {
		cout << "Error: First line is not 'net user admin'\nExiting...\n";
		return 1;
	} else if (instructions[1].substr(0, 24) != "net group Administrators") {
		cout << "Error: Second line is not 'net group Administrators'\nExiting...\n";
		return 2;
	} else if (instructions[2].substr(0, 24) != "net group Administrators") {
		cout << "Error: Third line is not 'net group Administrators'\nExiting...\n";
		return 3;
	} else if (instructions[3].substr(0, 15)  != "net group Users") {
		cout << "Error: Fourth line is not 'net group Users'\nExiting...\n";
		return 4;
	} else {
		return 0;
	}
}

int checkUsername(string username) {
	smatch m;
  	regex e ("\\s|\\/|\v|\\:");

	if (username.length()>30) {
		cout << "Error: Username cannot be more than 30 characters" << endl;
		return 1;
	} else if (regex_search (username,m,e)) {
		cout << "Error: Username cannot contain forward slash ('/'), colon (':'), carriage return, form feed, horizontal tab, new line, vertical tab, and space." << endl;
		return 1;
	} else {
		return 0;
	}
}

int checkPassword(string password) {
	smatch m;
  	regex e ("\\s|\v");

	if (password.length()>30) {
		cout << "Error: Password cannot be more than 30 characters" << endl;
		return 1;
	} else if (regex_search (password,m,e)) {
		cout << "Error: Password cannot contain forward carriage return, form feed, horizontal tab, new line, vertical tab, and space." << endl;
		return 1;
	} else {
		return 0;
	}
}

void login(string username, string password) {
	myAccounts.open("accounts.txt");
	wrongUsername = true;
	wrongPassword = true;
	while (!myAccounts.eof()) {
		string u,p;
		myAccounts >> u >> p;
		if (u.compare(username) == 0) {
			if (p.compare(password) == 0) {
				cout << "User " << u << " logged in" << endl;
				myAudit.open("audit.txt", ios_base::app);
				myAudit << "User " << u << " logged in" << endl;
				myAudit.close();
				whosLoggedIn = u;
				isLoggedIn = true;
				wrongPassword = false;
			}
		}
	}
	if (!isLoggedIn && wrongUsername && wrongPassword) {
		cout << "Login failed: invalid username or password" << endl; //  or password
		myAudit.open("audit.txt", ios_base::app);
		myAudit << "Login failed: invalid username or password" << endl;
		myAudit.close();
	}
	myAccounts.close();
}

vector<string> getUserGroup(string username) {
	vector<string> userGroups;
	for (size_t i = 0; i < numberOfUsers; i++) {
		if (username == listUsers[i].username) {
			//cout << "_-_ " << username << " ";
			for (size_t j = 0; j < listUsers[i].groups.size(); j++) {
				userGroups.push_back(listUsers[i].groups[j]);
				//cout << userGroups[j] << " ";
			}
			//cout << "_-_" << endl;
		}
	}
	return userGroups;
}

void logout() {
	cout << "User " << whosLoggedIn << " logged out" << endl;
	myAudit.open("audit.txt", ios_base::app);
	myAudit << "User " << whosLoggedIn << " logged out" << endl;
	myAudit.close();
	whosLoggedIn = "";
	isLoggedIn = false;

}

void netUser(string username, string password) {

}

void netGroup(string username, string password) {

}
