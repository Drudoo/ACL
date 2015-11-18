#include "access.h"
//#include <typeinfo> //just for debug
vector< vector<string> > groups(10, vector<string>(1)); //Only supports 10 user groups. Variable is global, so we don't have to pass it all the time.


int main(int argc, char const *argv[]) {


	//JUST FOR DEBUG. PLEASE DELETE!

	//permissions["Filename "].push_back("Owner");
	//permissions["Filename "].push_back("Permissions");

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
						log("Error: only an Administrator may issue net group command");
					}
				} else {
					if (addToGroup(u, g, numberOfUsers)) {
						log("User " + u + " added to group " + g);
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
				log("Login failed: simultaneous login not permitted");
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
				if (uORg == "/P") {
					replacePermissions(filename, newPermissions, uORg);
				} else if (uORg == "/D") {
					denyPermissions(filename, newPermissions);
				} else {
					editPermissions(filename, newPermissions, uORg);
				}
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

		if (arg1 == "execute") {
			string filename;
			in >> filename;
			executeFile(filename);
		}

		if (arg1 == "uac") {
			in >> arg2;
			setUAC(whosLoggedIn,arg2);
			//cout << ">> " << getUAC(whosLoggedIn) << ":"<< getUACString(whosLoggedIn) << endl;
		}

		if (arg1 == "program") {
			string filename;
			in >> arg2 >> filename;
			if (arg2 == "execute") {
				programExecute(filename);
			}
			if (arg2 == "create") {
				programCreate(filename);
			}
			if (arg2 == "read") {
				programRead(filename);
			}
			if (arg2 == "write") {
				string filename;
				in >> filename;
				string text;
				getline(in, text);
				text.erase(0,1);
				startThread1 = true;
				threadText = text;
				threadFilename = filename;
			}
		}

		if (arg1=="yes") {
			threadAnswer = arg1;
			startThread2 = true;
		}

		if (startThread1&&startThread2) {
			cout << ">> Starting some threads...\n";
			thread first(programWrite,threadFilename, threadText);
			cout << ">> Thread 1 was started...\n";
			thread second(canContinue,threadAnswer);
			cout << ">> Thread 2 was started...\n";

			first.join();
			second.join();
			cout << ">> Threads were joined...\n";
			startThread1 = false;
			startThread2 = false;
			canContinueExecuting = false;
		}

		if (arg1 == "end") {
			fstream myOutput;
			myOutput.open("groups.txt", ios_base::out | ios_base::in);  // will not create file

			if (myOutput.is_open()) {
				remove("groups.txt");
			}
			myOutput.close();
			myOutput.open("groups.txt", ios_base::app);

			for(map<string, vector<string> >::iterator it = usergroups.begin(); it != usergroups.end(); it++) {
				myOutput << it->first << ": ";
				vector<string> itVector = it->second;
				for (size_t j = 0; j < itVector.size(); j++) {
					myOutput << itVector[j] << " ";
				}
				myOutput << endl;
			}
			myOutput.close();

			myOutput.open("files.txt", ios_base::out | ios_base::in);
			if (myOutput.is_open()) {
				remove("files.txt");
			}
			myOutput.close();
			myOutput.open("files.txt", ios_base::app);
			for(map<string, vector<string> >::iterator it = permissions.begin(); it != permissions.end(); it++) {
				myOutput << it->first << " ";
				vector<string> itVector = it->second;
				for (size_t j = 0; j < itVector.size(); j++) {
					myOutput << itVector[j];
					if (j==0) {
						myOutput << " (";
					}
					if (j>0&&j<itVector.size()-1) {
						myOutput << ",";
					}
				}
				myOutput << ")";
				myOutput << endl;
			}
			myOutput.close();

			//save accounts.txt
			/*
			myAccounts.open("accounts.txt", std::ios_base::app);
			myAccounts << username << " " << password << " " << getUACString(username) << endl;
			myAccounts.close();
			*/

			myAccounts.open("accounts.txt");
			for (size_t i = 0; i < numberOfUsers; i++) {
				myAccounts << listUsers[i].username << " " << listUsers[i].password << " " << getUACString(listUsers[i].username) << endl;
			}
			myAccounts.close();
		}

	}


	cout << "_________TEST-OUTPUT__MAP__________" << endl;
	for(auto it = UAC.cbegin(); it != UAC.cend(); ++it)
	{
	    cout << it->first << " " << it->second << "\n";
	}

/*

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
	for(map<string, vector<string> >::iterator it = permissions.begin(); it != permissions.end(); it++) {
		cout << it->first << ": ";
		vector<string> itVector = it->second;
		for (size_t j = 0; j < itVector.size(); j++) {
			cout << itVector[j] << " ";
		}
		cout << endl;
	}
	cout << "_________TEST-OUTPUT__MAP__________" << endl;
	for(map<string, vector<string> >::iterator it = usergroups.begin(); it != usergroups.end(); it++) {
		cout << it->first << ": ";
		vector<string> itVector = it->second;
		for (size_t j = 0; j < itVector.size(); j++) {
			cout << itVector[j] << " ";
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
*/

	return 0;
}

bool canContinue(string answer) {
	cout << "Thread 2 should end...\n";
	if (answer == "yes") {
		canContinueExecuting = true;
		return true;
	} else {
		canContinueExecuting = false;
		return false;
	}
}

void programExecute(string filename) {
	if (isLoggedIn) {
		if (canExecute(whosLoggedIn)) {
			log(">> ehh, execute i guess?"); //fix this please!
		}
	}

}

void programCreate(string filename) {
	if (isLoggedIn) {
		// not sure what to do here. No program actually tries to create stuff.
	}
}

void programRead(string filename) {
	//cout << "<< Program Read: " << filename << " : " << canRead(getPermissions(filename,whosLoggedIn)) << " : " << getPermissions(filename, whosLoggedIn) << endl;
	if (isLoggedIn) {
		if (canRead(getPermissions(filename,whosLoggedIn))) {
			string line;
			fstream myFile;
			myFile.open(filename);
			log("program read " + filename + " as:");
			while (getline(myFile,line)) {
				log(line);
			}
			myFile.close();
		}
	}
}

void programWrite(string filename, string text) {
	//cout << ">> well here we are: " << canContinueExecuting << "\n";

	//cout << ">> well now we continue: " << canContinueExecuting << "\n";
	if (canContinueExecuting) {
		cout << ">> WE ARE WRITING TO FILE!" << endl;
	}
	cout << "Thread 1 should end...\n";
}

void setUAC(string username, string permissions) {
	if (isLoggedIn) {
		UAC[username] = permissions;
		//cout << ">> " << whosLoggedIn << ":" << username << ":" << UAC[username] << ":" << getUACString(username)<< endl;
		log(getUACString(username));
	}
}

string getUAC(string username) {
	return UAC[username];
}

string getUACString(string username) {
	if (getUAC(username) == "Always") {
		return "Always notify";
	} else if (getUAC(username) == "Change") {
		return "Notify only on change";
	} else {
		return "Never notify";
	}
}

void editPermissions(string filename, string newPermissions, string uORg) {
	if (isLoggedIn) {
		if (fileExist(filename)) {
			if (isOwner(filename,whosLoggedIn) || isAdmin(whosLoggedIn)) {
				permissions[filename].push_back(newPermissions);
				log("The ACL for " + filename + " appended by " + whosLoggedIn + " to include " + newPermissions);
			} else {
				log("Error with xcacls: Only file owner or member of Administrators group may run command");
			}
		} else {
			log("Error: File " + filename + " does not exist");
		}
	} else {
		log("Error: Please login to change permissions file");
	}
}

void denyPermissions(string filename, string newPermissions) {
	if (isLoggedIn) {
		if (fileExist(filename)) {
			if (isOwner(filename, whosLoggedIn) || isAdmin(whosLoggedIn)) {
				permissions[filename].push_back(newPermissions+":D");
				string temp = "user";
				for (size_t j = 0; j < groupCount; j++) {
					if (!strncasecmp(groups[j][0],newPermissions)) {
						temp="group";
					}
				}
				log("ACL changed for " + filename +": " + temp + " " + newPermissions +" denied access");
			} else {
				log("Error with xcacls: Only file owner or member of Administrators group may run command");
			}
		} else {
			log("Error: File " + filename + " does not exist");
		}
	} else {
		log("Error: Please login to change permissions file");
	}
}


void replacePermissions(string filename, string newPermissions, string uORg) {
	if (isLoggedIn) {
		if (fileExist(filename)) {
			if (isOwner(filename,whosLoggedIn) || isAdmin(whosLoggedIn)) {
				string tempUsername = newPermissions;
				tempUsername.resize(whosLoggedIn.length());
				for (size_t i = 1; i < permissions[filename].size(); i++) {

					if (permissions[filename][i].substr(0,whosLoggedIn.length()) == tempUsername) {
						permissions[filename][i] = newPermissions; //only works for the users that's logged in
						log("User " + tempUsername + " access to file " + filename + " changed to " + newPermissions.substr(whosLoggedIn.length()+1) + " by " + whosLoggedIn);
					}
				}

				//replace (permissions[filename].begin(), permissions[filename].end(), 20, 99);
			} else {
				log("Error with xcacls: Only file owner or member of Administrators group may run command");
			}
		} else {
			log("Error: File " + filename + " does not exist");
		}
	} else {
		log("Error: Please login to change permissions file");
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
			log("File " + filename + " with owner " + whosLoggedIn + " and default permissions created");
			setPermissions(filename);
		} else {
			log("Error: file " + filename + " already exists");
		}
	} else {
		log("Error: Please login to create file");
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
	string tempUsername;
	for (size_t i = 1; i < permissions[filename].size(); i++) {
		tempUsername  = permissions[filename][i];
		tempUsername.resize(whosLoggedIn.length());
		if (tempUsername == whosLoggedIn) {


			// some permissions are wrong here.


			//cout << "tempUsername: " << tempUsername << " whosLoggedIn: " << whosLoggedIn << endl;
			//cout << "permissions: " << permissions[filename][i] <<endl;
			highestUserPermissionLevel = permissions[filename][i].substr(tempUsername.length()+1);
		}
	}
	//cout << ">> Permissions for " << username << ": " << highestUserPermissionLevel+highestGroupPermissionLevel<< endl;

	string s1,s2;
	// make as above
	for (size_t i = 1; i < permissions[filename].size(); i++) {
		for (size_t j = 0; j < group.size(); j++) {

			//tempUsername = permissions[filename][i];
			//tempUsername.resize(whosLoggedIn.length());
			size_t pos = permissions[filename][i].find(":");
			s1 = permissions[filename][i].substr(0,pos);
			s2 = group[j];
			if (strncasecmp(s1,s2)==0) {
				//cout << "-->> " << s1 << " " << s2 << endl;
				highestGroupPermissionLevel += permissions[filename][i].substr(s1.length()+1);
			}
		}
	}

	//cout << ">> Permissions for " << username << ":" << highestUserPermissionLevel << ":" << highestGroupPermissionLevel<< endl;
	if (contains(highestUserPermissionLevel,"D") || contains(highestGroupPermissionLevel,"D")) {
		return "D";
	} else {
		return highestUserPermissionLevel+highestGroupPermissionLevel;
	}

}

bool contains(string s1, string s2) {
	if (s1.find(s2) != std::string::npos) {
    	return true;
	} else {
		return false;
	}
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
		log("User " + whosLoggedIn + " wrote to " + filename + ": " + text);
	} else {
		log("User " + whosLoggedIn + " denied write access to " + filename);
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
				log("User " + whosLoggedIn + " read " + filename + " as:");

				while (getline(myFile,line)) {
					log(line);
				}
				myFile.close();
			} else {
				log("User " + whosLoggedIn + " denied read access to " + filename);
			}
		} else {
			log("Error: File " + filename + " does not exist");
		}
	} else {
		log("Error: no user logged in");
	}
}

void executeFile(string filename) {
	string userPermissions = getPermissions(filename,whosLoggedIn);
	//cout << ">> " << userPermissions << endl;
 	if (isLoggedIn) {
 		if (fileExist(filename)) {
			//cout << ">> " << userPermissions << endl;
 			if (canExecute(userPermissions)) {
 				log("File " + filename + " executed by " + whosLoggedIn);
 			} else {
				log("User " + whosLoggedIn + " denied execute access to " + filename);
			}
 		} else {
			log("Error: File " + filename + " does not exist");
		}
 	} else {
		log("Error: no user logged in");
	}
}

bool canRead(string userPermissions) {
	//cout << "<< Permissions: " << userPermissions << endl;
	if (userPermissions.find("R") != std::string::npos) {
    	return true;
	} else if (userPermissions.find("F") != std::string::npos) {
		return true;
	} else {
		return false;
	}
}

bool canExecute(string userPermissions) {
	if (userPermissions.find("X") != std::string::npos) {
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
					usergroups[groupname].push_back(username);
					return true;
				}
			}
			log("Group " + groupname + " does not exist");
		} else {
			log("User " + username + " does not exist");
		}
	} else {
		log("Error: only an Administrator may issue net group command");
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
				usergroups[g];
				log("Group " + g + " created");
			} else {
				log("Error: Group " + g + " already exists");
				groupCount--;
			}
			groupExists=false;
		} else {
			return false;
		}
	} else {
		groups[groupCount][0] = g;
		usergroups[g];
		log("Group " + g + " created");
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
			if (!checkCommand(username)) {
				if(username!="admin") { //admin should not be part of group Users
					groups[1].push_back(username);
					listUsers[numberOfUsers].groups.push_back("Users");
					usergroups["Users"].push_back(username);
				}
				listUsers[numberOfUsers].username = username;
				listUsers[numberOfUsers].password = password;
				UAC[username] = "Change";
				log("User " + username + " created");
				myAccounts.open("accounts.txt", ios_base::app);
				myAccounts << username << " " << password << endl;
				myAccounts.close();
				isFirstRun = false;

				// DEBUG:
				log(getUACString(username));
				// -------
			}
		} else {
			log("Error: user " + username + " already exists");
			return false;
		}
		return true;
	} else {
		log("Error: only an Administrator may issue net user command");
		return false;
	}
}

int checkSetup(vector<string> instructions) {
	if (instructions[0].substr(0, 14) != "net user admin") {
		log("Error: First line is not 'net user admin'\nExiting...\n");
		return 1;
	} else if (instructions[1].substr(0, 24) != "net group Administrators") {
		log("Error: Second line is not 'net group Administrators'\nExiting...\n");
		return 2;
	} else if (instructions[2].substr(0, 24) != "net group Administrators") {
		log("Error: Third line is not 'net group Administrators'\nExiting...\n");
		return 3;
	} else if (instructions[3].substr(0, 15)  != "net group Users") {
		log("Error: Fourth line is not 'net group Users'\nExiting...\n");
		return 4;
	} else {
		return 0;
	}
}

int checkCommand(string username) {
	smatch m;
  	regex e ("\\s|\\/|\v|\\:");

	if (username.length()>30) {
		log("Error: Username cannot be more than 30 characters");
		return 1;
	} else if (regex_search (username,m,e)) {
		log("Error: Username, group name, or filename  cannot contain forward slash ('/'), colon (':'), carriage return, form feed, horizontal tab, new line, vertical tab, and space.");
		return 1;
	} else {
		return 0;
	}
}

int checkPassword(string password) {
	smatch m;
  	regex e ("\\s|\v");

	if (password.length()>30) {
		log("Error: Password cannot be more than 30 characters");
		return 1;
	} else if (regex_search (password,m,e)) {
		log("Error: Password cannot contain forward carriage return, form feed, horizontal tab, new line, vertical tab, and space.");
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
				log("User " + u + " logged in");
				whosLoggedIn = u;
				isLoggedIn = true;
				wrongPassword = false;
			}
		}
	}
	if (!isLoggedIn && wrongUsername && wrongPassword) {
		log("Login failed: invalid username or password");
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
	log("User " + whosLoggedIn + " logged out");
	whosLoggedIn = "";
	isLoggedIn = false;

}

void log(string text) {
	cout << text << endl;
	myAudit.open("audit.txt", ios_base::app);
	myAudit << text << endl;
	myAudit.close();
}

bool strncasecmp(string s1, string s2) {
	for (size_t i = 0; i < s1.length(); i++) {
		s1[i] = tolower(s1[i]);
	}
	for (size_t i = 0; i < s2.length(); i++) {
		s2[i] = tolower(s2[i]);
	}
	return strcmp(s1.c_str(),s2.c_str());
}
