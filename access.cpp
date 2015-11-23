#include "access.h"

//vector< vector<string> > groups(10, vector<string>(1)); //Only supports 10 user groups. Variable is global, so we don't have to pass it all the time.



int main(int argc, char const *argv[]) {

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
				instructions.push_back(line); //Load instructions into a vector. So we can read it later.
			}
			myInput.close();
		} else {
			cout << "Error: Cannot open file: " << argv[1] << "\nExiting...\n" << endl;
			return 1;
		}
	}

	myAccounts.open("accounts.txt", ios_base::out | ios_base::in);  // will not create file

	if (myAccounts.is_open()) {
		remove("accounts.txt"); //If accounts exists then remove it.
	}
	myAccounts.open ("accounts.txt"); // Create a clean file
	myAccounts.close();

	myAudit.open("audit.txt", ios_base::out | ios_base::in); //Same with Audit file
	if (myAudit.is_open()) {
		remove("audit.txt");
	}
	myAudit.open("audit.txt", ios_base::app);
	myAudit.close();

	if (checkSetup(instructions)!=0) return 1; //If the first four lines aren't what they should be, exit then program and show error.

	for (size_t i = 0; i < instructions.size(); i++) {  //Go through all the instructions
		istringstream in(instructions[i]);
		in >> arg1; //Load the first argument.

		if (arg1 == "net") {
			in >> arg2; //Load the second argument, so if it is group or user.
			if (arg2 == "group") {
				u = "";
				in >> g >> u; //Load group and username
				if(u=="") { //If username doesn't exists, then we create a group.
					if (createGroup(g)) {
						groupCount++;
					} else {
						log("Error: only an Administrator may issue net group command");
					}
				} else { //Else we add the user to a group.
					if (addToGroup(u, g)) {
						log("User " + u + " added to group " + g);
					}
				}
			}

			if (arg2 == "user") { //Create the user.
				in >> u >> p;
				if(createUser(u, p)) {
					numberOfUsers++; ///remove!!!!!!!!!!
				}
			}
		}

		if (arg1 == "login") { //Login
			in >> u >> p; //Get username and password
			if(isLoggedIn) { //Check if someone is already logged in.
				log("Login failed: simultaneous login not permitted");
			} else {
				login(u,p); //Else login.
			}
		}
		if (arg1 == "logout") { //Logout
			logout();
		}

		if (arg1 == "create") {
			in >> arg2; //Load the filename
			createFile(arg2);
		}
		if (arg1 == "write") {
			in >> arg2; //Load the filename
			string text;
			getline(in, text); //get the text to write
			text.erase(0,1); //Remove the space infront of the line
			writeFile(arg2, text); //Write to file
		}
		if (arg1 == "xcacls") { //Change permissions
			string filename, uORg, newPermissions;
			in >> filename >> arg2; //Load the filename and the arguments
			if (arg2 == "/E") { //Edit permissions
				in >> uORg >> newPermissions; //Load if it is a user or group and the new permissions
				if (uORg == "/P") {
					replacePermissions(filename, newPermissions, uORg);
				} else if (uORg == "/D") {
					denyPermissions(filename, newPermissions);
				} else {
					editPermissions(filename, newPermissions, uORg);
				}
			} else { //Replace permissions
				in >> uORg;
				replacePermissions(filename,newPermissions, uORg);


			}
		}

		if (arg1 == "read") { //Read a file
			string filename;
			in >> filename; //Load filename
			readFile(filename);
		}

		if (arg1 == "execute") { //Execute file
			string filename;
			in >> filename;
			executeFile(filename);
		}

		if (arg1 == "uac") { //Change user UAC
			in >> arg2;
			setUAC(whosLoggedIn,arg2);
		}

		if (arg1 == "program") {
			string filename;
			in >> arg2 >> filename; //Load either exe, create, read, write and the filename
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
				string text;
				getline(in, text); //Get the line of text to write.
				text.erase(0,1); //Remove the first space.
				if (getUAC(whosLoggedIn) == "Never") { //Check the UAC. If it is Never, then just write to file.
					programWrite(filename,text); //Write text to file.
				} else { //If UAC is any other, then check what the next line is.
					string tempAnswer = instructions[i+1]; //get the answer from the user, which is the next instruction.
					tempAnswer.erase( std::remove(tempAnswer.begin(), tempAnswer.end(), '\r'), tempAnswer.end() ); //Remove the Carriage Return from the line.
					if (canContinue(tempAnswer)) { //check if the program can continue.
						programWrite(filename,text); //If it can, then continue and write to file.
					}
				}

			}
		}

		if (arg1 == "end") { //When the program has to end. Write to files.
			fstream myOutput;
			myOutput.open("groups.txt", ios_base::out | ios_base::in);  // will not create file

			if (myOutput.is_open()) {
				remove("groups.txt"); //if the file exists then remove it.
			}
			myOutput.close(); //close and create it.
			myOutput.open("groups.txt", ios_base::app);

			for(auto it : usergroups) { //Go through the map of usergroups.
				myOutput << it.first << ": "; //Save the group name and a colon.
				vector<string> itVector = it.second; //Cycle through the vector of users.
				for (size_t j = 0; j < itVector.size(); j++) {
					myOutput << itVector[j] << " "; //Save the username.
				}
				myOutput << endl;
			}
			myOutput.close(); //Close the file.

			myOutput.open("files.txt", ios_base::out | ios_base::in); //check if file exists then remove it.
			if (myOutput.is_open()) {
				remove("files.txt");
			}
			myOutput.close();
			myOutput.open("files.txt", ios_base::app); //Create the file.
			for (auto it : permissions) { //run through the map permissions.
				myOutput << it.first << " ";
				vector<string> itVector = it.second;
				for (size_t j = 0; j < itVector.size(); j++) { //Run through the vector.
					myOutput << itVector[j];
					if (j==0) { //Make it look better by adding a bracket at the beginning.
						myOutput << " (";
					}
					if (j>0&&j<itVector.size()-1) { //Also add a comma between permissions.
						myOutput << ",";
					}
				}
				myOutput << ")"; //end up with another bracket.
				myOutput << endl;
			}
			myOutput.close(); //Close the file.

			myAccounts.open("accounts.txt");
			for (auto map : userMap) {
				myAccounts << map.first << " " << map.second << " " << getUACString(map.first) << endl;
			}


			myAccounts.close();
		}

	}
	return 0;
}

bool canContinue(string answer) { //Checks the user input based on UAC.
	if (answer == "yes") {
		return true;
	} else {
		return false;
	}
}

void programExecute(string filename) { //execute file from program.
	if (isLoggedIn) {
		if (canExecute(whosLoggedIn)) {
			log("Program executed " + filename);
		}
	}

}

void programCreate(string filename) { //Create file for program
	if (isLoggedIn) {
		createFile(filename);
	}
}

void programRead(string filename) { //Read the file.
	if (isLoggedIn) {
		if (canRead(getPermissions(filename,whosLoggedIn))) { //check if the user can read based on permissions.
			string line;
			fstream myFile;
			myFile.open(filename); //open the file.
			log("program read " + filename + " as:");
			while (getline(myFile,line)) { //Read the lines in the file.
				log(line);
			}
			myFile.close(); //Close the file.
		}
	}
}

void programWrite(string filename, string text) { //Write to file.
	fstream myFile;
	if (isLoggedIn) {
		if (canWrite(getPermissions(filename,whosLoggedIn))) { //check if the user can write based on permisssions.
			myFile.open(filename, std::ios_base::app);
			myFile << text << endl;
			myFile.close();
			log("program wrote to " + filename + ": " + text);
		} else {
			log("program denied write access to " + filename);
		}
	}
}

void setUAC(string username, string permissions) { //Set the UAC
	if (isLoggedIn) {
		UAC[username] = permissions;
		log(getUACString(username));
	}
}

string getUAC(string username) { //Get the UAC.
	return UAC[username];
}

string getUACString(string username) { //get the UAC string. As the UAC is saved as Never, Change and Always, we need a function to return the correct string.
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
		if (fileExist(filename)) { //check if the file exists.
			if (isOwner(filename,whosLoggedIn) || isAdmin(whosLoggedIn)) { //Check if the user is admin or owner. Else we cannot change permissions.
				permissions[filename].push_back(newPermissions); //Add permissions to the map.
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

void denyPermissions(string filename, string newPermissions) { //Set permissions to deny.
	if (isLoggedIn) {
		if (fileExist(filename)) {
			if (isOwner(filename, whosLoggedIn) || isAdmin(whosLoggedIn)) {
				permissions[filename].push_back(newPermissions+":D"); //Add D to user permissions.

				//We have a different output for either user or group. This is because both a user or a group can be denied permissions but to find out which it is we need to check if the name of the newPermissions is a group name or username.
				string temp = "user"; //We start out by assuming it is a user.

				for (auto map : usergroups) { //cycle through the map of usergroups.
					if (!strncasecmp(map.first,newPermissions)) { //If the group name matches a group in the usergroups then set the variable to group.
						temp="group";
					}
				}
				log("ACL changed for " + filename +": " + temp + " " + newPermissions +" denied access"); //Log / print the output.
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
		if (fileExist(filename)) { //check if the file exists.
			if (isOwner(filename,whosLoggedIn) || isAdmin(whosLoggedIn)) { //check if user is admin or owner.
				string tempUsername = newPermissions;
				tempUsername.resize(whosLoggedIn.length());
				for (size_t i = 1; i < permissions[filename].size(); i++) { //Run through the permission[filname] vector.
					if (permissions[filename][i].substr(0,whosLoggedIn.length()) == tempUsername) { //Check if the username is the same as the permissions.
						permissions[filename][i] = newPermissions; //replace the permission
						log("User " + tempUsername + " access to file " + filename + " changed to " + newPermissions.substr(whosLoggedIn.length()+1) + " by " + whosLoggedIn);
					}
				}
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

bool fileExist(string filename) { //Check if the file exists by running through the permissions map to see if the filename is there.
	return (permissions.find(filename) != permissions.end())?true:false;
}

void createFile(string filename) { //Create the file.
	if (isLoggedIn) { //check if anotone is logged in.
		fstream myFile;
		myFile.open(filename, ios_base::out | ios_base::in);  // Check if the file can be opened. But doesn't create the file.
		if (!fileExist(filename)) { //Check if the file exists.
			myFile.open(filename, ios_base::app); //Create the file
			myFile.close(); //Close the file
			log("File " + filename + " with owner " + whosLoggedIn + " and default permissions created");
			setPermissions(filename); //Set permissions based on the filename.
		} else {
			log("Error: file " + filename + " already exists");
		}
	} else {
		log("Error: Please login to create file");
	}

}

void setPermissions(string filename) {
	permissions[filename].push_back(whosLoggedIn); // filename.txt username
	permissions[filename].push_back(whosLoggedIn+":F"); // adds user rights
	permissions[filename].push_back("Administrators:F"); //adds admin rights

	if (isAdmin(whosLoggedIn)) { //if the user is admin, then give users read rights.
		permissions[filename].push_back("Users:R"); //adds user read rights

	}
}

string getPermissions(string filename, string username) { //Get the permissions based on filename and username.
	vector<string> group;
	group = getUserGroup(username); //Get a vector with all the groups a user is in.
	string highestUserPermissionLevel = "";
	string highestGroupPermissionLevel = "";
	string tempUsername;
	for (size_t i = 1; i < permissions[filename].size(); i++) { //Run through the vector in the permissions map and compare it to the user whos logged in.
		tempUsername  = permissions[filename][i];
		tempUsername.resize(whosLoggedIn.length());
		if (tempUsername == whosLoggedIn) {
			highestUserPermissionLevel = permissions[filename][i].substr(tempUsername.length()+1); //Set the user permissions.
		}
	}
	string s1,s2;

	for (size_t i = 1; i < permissions[filename].size(); i++) {
		for (size_t j = 0; j < group.size(); j++) {

			//tempUsername = permissions[filename][i];
			//tempUsername.resize(whosLoggedIn.length());
			size_t pos = permissions[filename][i].find(":");
			s1 = permissions[filename][i].substr(0,pos);
			s2 = group[j];
			if (strncasecmp(s1,s2)==0) {
				highestGroupPermissionLevel += permissions[filename][i].substr(s1.length()+1);
			}
		}
	}


	//cout << ">> " << highestUserPermissionLevel+highestGroupPermissionLevel << endl;
	//Check if either group or user is denied.
	if (contains(highestUserPermissionLevel,"D") || contains(highestGroupPermissionLevel,"D")) {
		return "D";
	} else { // else return both the user and group permissions.
		return highestUserPermissionLevel+highestGroupPermissionLevel;
	}

}

bool contains(string s1, string s2) { //Check if the string s1 contains s2.
	if (s1.find(s2) != std::string::npos) {
    	return true;
	} else {
		return false;
	}
}

bool canWrite(string userPermissions) { //Check if a user can write. User can write if permissions are either W or F.
	if (userPermissions.find("W") != std::string::npos) {
    	return true;
	} else if (userPermissions.find("F") != std::string::npos) {
		return true;
	} else {
		return false;
	}
}

void writeFile(string filename, string text) { //Write to file.
	string userPermissions = getPermissions(filename, whosLoggedIn); //get the permissions based on whos is logged in and the filename.
	fstream myFile;

	if (canWrite(userPermissions)) { //If the user can write then start writing.
		myFile.open(filename, std::ios_base::app); //Append to the file.
		myFile << text << endl;
		myFile.close(); //Close the file.
		log("User " + whosLoggedIn + " wrote to " + filename + ": " + text);
	} else {
		log("User " + whosLoggedIn + " denied write access to " + filename);
	}

}

void readFile(string filename) { //Read from file
	string userPermissions = getPermissions(filename,whosLoggedIn); //get user permissions.
	if (isLoggedIn) { //Check if anyone is logged in.
		if (fileExist(filename)) { //Check if file exists
			if (canRead(userPermissions)) { //check if the user can read.
				fstream myFile;
				string line;
				myFile.open(filename); //Open the file.
				log("User " + whosLoggedIn + " read " + filename + " as:");

				while (getline(myFile,line)) { //Read from file.
					log(line);
				}
				myFile.close(); // Close file.
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

void executeFile(string filename) { //execute file
	string userPermissions = getPermissions(filename,whosLoggedIn); //get permissions
 	if (isLoggedIn) {
 		if (fileExist(filename)) {
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

bool canRead(string userPermissions) { //check if user can read.
	//User can read if permissions contain either R or F.
	if (contains(userPermissions, "R")) {
    	return true;
	} else if (contains(userPermissions, "F")) {
		return true;
	} else {
		return false;
	}
}

bool canExecute(string userPermissions) { //check if user can execute.
	//User can execute if permissions contain either X or F.
	if (contains(userPermissions, "X")) {
    	return true;
	} else if (contains(userPermissions, "F")) {
		return true;
	} else {
		return false;
	}
}

bool addToGroup(string username, string groupname) { //Add user to group.

	if (usergroups[groupname].empty() || isAdmin(whosLoggedIn)) {
		if (userMap.find(username) != userMap.end()) { //check if user exists
			if (usergroups.find(groupname) != usergroups.end()) {
				usergroups[groupname].push_back(username);
				return true;
			} else {
				log("Group " + groupname + " does not exist");
			}
		} else {
			log("User " + username + " does not exist");
			return false;
		}

	} else {
		log("Error: only an Administrator may issue net group command");
	}
	return false;

}

bool createGroup(string groupname) { //Create group

	if (!usergroups.empty()) { //check if we don't have any groups.
		if (isAdmin(whosLoggedIn)) { //check if logged in user is admin.
			if (usergroups.find(groupname) == usergroups.end()) { //check if group already exist
				usergroups[groupname]; //create group.
				log("Group " + groupname + " created");
			} else {
				log("Error: Group " + g + " already exists");
			}
		} else {
			return false;
		}
	} else {
		usergroups[groupname]; //create group.
		log("Group " + g + " created");
	}
	return true;
}

bool isAdmin(string username) { //check if the user is admin.

	vector<string> userGroup = getUserGroup(whosLoggedIn); //get all groups the user is member of.

	if (find(userGroup.begin(), userGroup.end(), "Administrators") != userGroup.end() || isFirstRun) {
		return true; //check if the user is member of group Administrators
	} else {
		return false;
	}
}

bool isOwner(string filename, string username) { //check if the user is owner of file.
	return (permissions[filename][0] == username)?true:false;
}

bool createUser(string username, string password) { //Create a new user.
	if (isAdmin(whosLoggedIn)) { //check if an admin is logged in.
		if (userMap[username]=="") { //check if the user exists
			if (!checkCommand(username)) { //Check if username complies with naming rules
				if (username != "admin") { //check if username is admin
					usergroups["Users"].push_back(username); //if not,add user to group Users
				}
				userMap[username] = password; //add password
				UAC[username] = "Change"; //add UAC
				log("User " + username + " created"); //log results
				myAccounts.open("accounts.txt", ios_base::app); //append to accounts.txt file.
				myAccounts << username << " " << password << endl;
				myAccounts.close();
				isFirstRun = false;

				log(getUACString(username)); //log UAC.
			}
		} else {
			log("Error: user " + username + " already exists");
			return false;
		}
		/*
		if (!(find(groups[1].begin(), groups[1].end(), username) != groups[1].end())) {
			if (!checkCommand(username)) {
				if(username!="admin") { //admin should not be part of group Users
					groups[1].push_back(username);
					listUsers[numberOfUsers].groups.push_back("Users");
					usergroups["Users"].push_back(username);
				}
				userMap[username] = password;
				listUsers[numberOfUsers].username = username;
				listUsers[numberOfUsers].password = password;
				UAC[username] = "Change";
				log("User " + username + " created");
				myAccounts.open("accounts.txt", ios_base::app);
				myAccounts << username << " " << password << endl;
				myAccounts.close();
				isFirstRun = false;

				log(getUACString(username));
			}
		} else {
			log("Error: user " + username + " already exists");
			return false;
		}
		*/
		return true;
	} else {
		log("Error: only an Administrator may issue net user command");
		return false;
	}
}

int checkSetup(vector<string> instructions) { //check the first 4 lines of setup.
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

int checkCommand(string username) {//Check if the username contains allowed characters.
	smatch m;
  	regex e ("\\s|\\/|\v|\\:"); //Use C++11's regex search to make evering a lot easier.
	// \s matches any white space character [\r\n\t\f ]
	// \/ matches /
	// \v matches any vertical whitespace character (e.g. horizontal tab.)
	// \: matches :

	if (username.length()>30) { //check the length of the string
		log("Error: Username cannot be more than 30 characters");
		return 1;
	} else if (regex_search (username,m,e)) { //check if the password contains
		log("Error: Username, group name, or filename  cannot contain forward slash ('/'), colon (':'), carriage return, form feed, horizontal tab, new line, vertical tab, and space.");
		return 1;
	} else {
		return 0;
	}
}

int checkPassword(string password) { //Check if the password contains allowed characters.
	smatch m;
  	regex e ("\\s|\v"); //Use C++11's regex search to make everything a lot easier.
	// \s matches any white space character [\r\n\t\f ]
	// \v matches any vertical whitespace character (e.g. horizontal tab.)

	if (password.length()>30) { //check the length of the string
		log("Error: Password cannot be more than 30 characters");
		return 1;
	} else if (regex_search (password,m,e)) { //check if the password contains
		log("Error: Password cannot contain forward carriage return, form feed, horizontal tab, new line, vertical tab, and space.");
		return 1;
	} else {
		return 0;
	}
}

void login(string username, string password) { //login a user
	myAccounts.open("accounts.txt"); //open the accounts file
	wrongUsername = true;
	wrongPassword = true;
	while (!myAccounts.eof()) { //Run through the file
		string u,p;
		myAccounts >> u >> p; //Load in usernames and passwords.
		if (u.compare(username) == 0) { //compare username with username from file (case sensitive)
			if (p.compare(password) == 0) { //Same for password
				log("User " + u + " logged in"); //If both are correct then we log in.
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

vector<string> getUserGroup(string username) { //get the groups a user is in.
	vector<string> userG; //Vector to store the groups in.

	for (auto map : usergroups) { //Run through the map of groups.
		vector<string> itVector = map.second; //vector of users
		for (size_t i = 0; i < itVector.size(); i++) { //Run through the vector
			if (username == itVector[i]) { //If the username is in the vector of users
				userG.push_back(map.first); //Add the group name to the vector.
			}
		}
	}

	/*
	for (size_t i = 0; i < numberOfUsers; i++) {
		if (username == listUsers[i].username) {
			cout << "_-_ " << username << " ";
			for (size_t j = 0; j < listUsers[i].groups.size(); j++) {
				userG.push_back(listUsers[i].groups[j]);
				cout << userG[j] << " ";
			}
			cout << "_-_" << endl;
		}
	}
	*/

	return userG; //Return the vector of groups.
}

void logout() { //Log any user out.
	log("User " + whosLoggedIn + " logged out");
	whosLoggedIn = "";
	isLoggedIn = false;

}

void log(string text) { // print file in console and log it to audit.txt file.
	cout << text << endl;
	myAudit.open("audit.txt", ios_base::app);
	myAudit << text << endl;
	myAudit.close();
}

bool strncasecmp(string s1, string s2) { //Compare two string and ignore case (case insensitive)
	for (size_t i = 0; i < s1.length(); i++) {
		s1[i] = tolower(s1[i]);
	}
	for (size_t i = 0; i < s2.length(); i++) {
		s2[i] = tolower(s2[i]);
	}
	return strcmp(s1.c_str(),s2.c_str());
}
