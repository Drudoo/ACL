#include "access.h"
#include <typeinfo> //just for debug
vector< vector<string> > groups(10, vector<string>(1)); //Only supports 10 user groups


int main(int argc, char const *argv[]) {

	vector<string> instructions;
	whosLoggedIn = "admin";
	if (argc < 2) {
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

	if (checkSetup(instructions)!=0) return 1;

	int count = 0;


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
					if (createGroup(g, count)) {
						groupCount++;
					} else {
						cout << "Error: only an Administrator may issue net group command" << endl;
					}
				} else {
					if (addToGroup(u, g, count)) {
						cout << "User " << u << " added to group " << g << endl;
					} else {
						cout << "Error: only an Administrator may issue net group command" << endl;
					}
				}
			}

			if (arg2 == "user") {
				in >> u >> p;
				if(createUser(u, p, count)) {
					count++;
				}
			}
		}

		if (arg1 == "login") {
			in >> u >> p;
			if(isLoggedIn) {
				cout << "Login failed: simultaneous login not permitted" << endl;
			} else {
				login(u,p);
			}
		}
		if (arg1 == "logout") {
			logout();
		}

	}

	cout << "_________ TEST OUTPUT __ STRUCT _______" << endl;


	for (size_t i = 0; i < count; i++) {
		cout << listUsers[i].username << " ";
		for (size_t j = 0; j < listUsers[i].groups.size(); j++) {
			cout << listUsers[i].groups[j] << " ";
		}
		cout << endl;
	}



	//vector<string> testGroups = getUserGroup("admin",count);

	cout << "_________ TEST OUTPUT __ VECTOR_______" << endl;
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
	cout << "_________ TEST OUTPUT _________" << endl;

	for (size_t i = 0; i < count; i++) {
		cout << listUsers[i].username << endl;
	}

	return 0;
}

bool addToGroup(string username, string groupname, int count) {
	int temp;
	bool foundUser = false;
	if (groupCount == 1 || isAdmin(whosLoggedIn, count)) {
		for (size_t m = 0; m < count; m++) {
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
		}

		if (foundUser) {
			cout << "Group " << groupname << " does not exist" << endl;
		} else {
			cout << "User " << username << " does not exist" << endl;
		}
	}

	return false;
}

bool createGroup(string groupname, int count) {
	if (groupCount>0) {
		if (isAdmin(whosLoggedIn, count)) {
			for (size_t j = 0; j < groupCount; j++) {
				if (g == groups[j][0]) {
					groupExists = true;
					break;
				}
			}
			if (!groupExists) {
				groups[groupCount][0] = g;
				cout << "Group " << g << " created" << endl;
			} else {
				cout << "Error: Group " << g << " already exists" << endl;
				groupCount--;
			}
			groupExists=false;
		} else {
			return false;
		}
	} else {
		groups[groupCount][0] = g;
		cout << "Group " << g << " created" << endl;
	}
	return true;
}

bool isAdmin(string username, int count) {
	vector<string> userGroup = getUserGroup(whosLoggedIn, count);

	if (find(userGroup.begin(), userGroup.end(), "Administrators") != userGroup.end() || isFirstRun) {
		return true;
	} else {
		return false;
	}
}

bool createUser(string username, string password, int count) {

	if (isAdmin(whosLoggedIn, count)) {
		if (!(find(groups[1].begin(), groups[1].end(), username) != groups[1].end())) {
			if (!checkUsername(username)) {
				groups[1].push_back(username);
				listUsers[count].groups.push_back("Users");
				listUsers[count].username = username;
				cout << "User " << username << " created" << endl;
				myAccounts.open("accounts.txt", std::ios_base::app);
				myAccounts << username << " " << password << endl;
				myAccounts.close();
				isFirstRun = false;
			}
		} else {
			cout << "Error: user " << username << " already exists" << endl;
			return false;
		}
		return true;
	} else {
		cout << "Error: only an Administrator may issue net user command" << endl;
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
				whosLoggedIn = u;
				isLoggedIn = true;
				wrongPassword = false;
			}
		}
	}
	if (!isLoggedIn && wrongUsername && wrongPassword) {
		cout << "Login failed: invalid username or password" << endl; //  or password
	}
	myAccounts.close();
}

vector<string> getUserGroup(string username, int count) {
	vector<string> userGroups;
	for (size_t i = 0; i < count; i++) {
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
	whosLoggedIn = "";
	isLoggedIn = false;

}

void netUser(string username, string password) {

}

void netGroup(string username, string password) {

}
