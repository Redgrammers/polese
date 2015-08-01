#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
using namespace std;

int main();

bool start = true;

string input;
vector<string> keywords;
char* token;

vector<string> global_strings;
vector<void(*)()> global_actions;

int execute (string input);

int length (string lf);
string readfile (string directory, string filename);
string
vector<string> shift (vector<string> v, int l);
void buildin (string name, void (*action)());
string joinstr (vector<string> unjoined, string delimiter);
string exec (string cmd);

void error (string msg);
void nisk (int ind, string msg);

class basicIDE {
	public:
		string version;
		string name;
		string prompt = exec("echo $PS4");
};

class complexIDE: public basicIDE {
	public:
		string STD_HEADER_DIR;
		vector<vector<string(*)()>> BUILTIN_METHODS;
		vector<vector<string(*)()>> DEFINED_METHODS;
};

complexIDE IDE;

int main () {
	IDE.version        = "v1.0.0";
	IDE.name           = "Polese IDE";
	IDE.STD_HEADER_DIR = "/Users/raumaankidwai/polese/headers/";
	
	keywords.resize(1000, "");
	
	buildin("IDE", [] () {
		if (keywords[1] == "help") {
			nisk(2, "\"IDE help\" takes 0 options");
			cout
				<< "IDE help" << endl
				<< "\tBasic Commands" << endl
				<< "\t\t::polese [[                          | Start a .pe file (SOF)" << endl
				<< "\t\tuse \"header.ph\" from self\"directory\" | Use a .ph header file" << endl
				<< "\t\t]]                                   | End a .pe file (EOF)" << endl
				<< "\t\tIDE exit                             | Exit the IDE (IDE only)" << endl
			;
		} else if (keywords[1] == "exit") {
			nisk(2, "\"IDE exit\" takes 0 options");
			exit(0);
		} else {
			nisk(1, "\"" + keywords[1] + "\" is not a member of \"IDE\"");
			error("\"IDE\" is of type `IDEObject`, not `()`");
		}
	});
	buildin("use", [] () {
		if (length(keywords[1]) < 3) {
			error("No header file provided.");
		}
		
		string opening;
		string closing;
		opening.push_back(keywords[1].at(0));
		closing.push_back(keywords[1].at(keywords[1].size() - 1));
		string header = keywords[1].substr(1, length(keywords[1]) - 2);
		
		if (((opening != closing) || (opening != "\"" && opening != "'")) && (opening != "<" || closing != ">")) {
			error("Invalid container: " + opening + closing);
		}
	
		if (keywords[2] == "from") {
			if (opening == "<" || closing == ">") {
				error("\"from\" not expected with std formatted \"" + keywords[1] + "\"");
			} else if (!regex_match(keywords[3], regex("^self$|^self\".+\"$|^<.+>$"))) {
				error("Invalid slink \"" + keywords[3] + "\".");
			} else {
				if (regex_match(keywords[3], regex("^self$"))) {
					execute(readfile("", header));
				} else if (regex_match(keywords[3], regex("^self\".+\"$"))) {
					execute(readfile(keywords[3].substr(5, length(keywords[3]) - 6) + "/", header));
				} else {
					execute(readfile("/" + keywords[3].substr(1, length(keywords[3]) - 2) + "/", header));
				}
			}
		} else if (length(keywords[2]) > 0) {
			error("Invalid keyword \"" + keywords[2] + "\": Expected \"from\"");
		} else if (opening != "<" && closing != ">") {
			error("Expected \"from\" after filename " + keywords[1]);
		} else {
			execute(readfile(IDE.STD_HEADER_DIR, header));
		}
	});
	buildin("::polese", [] () {
		if (keywords[1] != "[[") {
			error("Invalid entrance " + keywords[1]);
		}
	});
	buildin("]]", [] () {});
	buildin("-", [] () {
		cout << joinstr(shift(keywords, 1), " ") << endl;
	});
	
	if (start) {
		cout
			<< IDE.name << " " << IDE.version << endl
			<< "Created by raumaankidwai" << endl
			<< "\"IDE help\" for help, \"IDE exit\" to exit" << endl;
		start = false;
	}
	cout << IDE.prompt;
	
	input = "";
	getline(cin, input);
	
	execute(input);
	
	main();
}

int execute (string input) {
	
	
	if (input == "") {
		IDE.BUILTIN_METHODS.clear();
		return 0;
	}
	
	fill(keywords.begin(), keywords.end(), "");
	
	int counter = 0;
	
	char tmp[1024];
	strcpy(tmp, input.c_str());
	
	token = strtok(tmp, " ");
	while (token != NULL) {
		keywords[counter++] = token;
		token = strtok(NULL, " ");
	}
	
	bool found = false;
	
	int e = 0;
	
	for (auto &i: IDE.BUILTIN_METHODS) {
		cout << "IDE.BUILTIN_METHODS[" << e++ << "][0]() = " << i[0]() << endl;
		if (i[0]() == keywords[0]) {
			found = (i[1](), true);
			break;
		}
	}
	
	if (!found) {
		error("Invalid keyword \"" + keywords[0] + "\"");
	}
	
	IDE.BUILTIN_METHODS.clear();
	return 1;
}


int length (string lf) {
	return strlen(lf.c_str());
}
string readfile (string directory, string filename) {
	ifstream ifs (directory + filename);
	string contents = "";
	
	if (ifs.good()) {
		contents = static_cast<stringstream const&>(stringstream() << ifs.rdbuf()).str();
	}
	
	if (length(contents) < 1) {
		error("Invalid filename \"" + filename + "\"");
	}
	return contents;
}
vector<string> shift (vector<string> v, int l) {
	vector<string> shifted;
	for (; l < v.size(); l ++) {
		if (length(v[l]) > 0) {
			shifted.push_back(v[l]);
		}
	}
	return shifted;
}
void buildin (string name, void (*action)()) {
	::global_strings.push_back(name);
	::global_actions.push_back(*action);
	struct build {
		static string nam () {
			return ::global_strings[0];
		}
		static string act () {
			(::global_actions[0])();
			return "";
		}
	};
	vector<string(*)()> method;
	method.push_back(build::nam);
	method.push_back(build::act);
	IDE.BUILTIN_METHODS.push_back(method);
	::global_strings.clear();
	::global_actions.clear();
	cout << "IDE.BUILTIN_METHODS[" << IDE.BUILTIN_METHODS.size() - 1 << "][0]() = " << IDE.BUILTIN_METHODS[IDE.BUILTIN_METHODS.size() - 1][0]() << endl;
}
string joinstr (vector<string> unjoined, string delimiter) {
	string joined;
	int i = 0;
	for (auto &e: unjoined) {
		joined += e;
		if (++i < unjoined.size()) {
			joined += delimiter;
		}
	}
	return joined;
}
string exec (string c) {
	char* cmd = &c[0u];
	FILE* pipe = popen(cmd, "r");
	if (!pipe) return "ERROR";
	char buffer[128];
	std::string result = "";
	while(!feof(pipe)) {
		if(fgets(buffer, 128, pipe) != NULL) result += buffer;
	}
	pclose(pipe);
	return result;
}


void error (string msg) {
	cerr << "\033[1;31mError:\033[0;17m " << msg << endl;
	
	input = "";
	keywords.clear();
	main();
}
void nisk (int ind, string msg) {
	if (ind == keywords.size() - 1) {
		error(msg);
	}
}
