#ifndef ProgLine_H
#define ProgLine_H

#include <string>
#include <vector>

using namespace std;
typedef struct stmt_t{
	string procedure;
	string type;
	//vector<string> followS;
	//string parent;
	//vector<string> parentS;
	//vector<string> next;
	stmt_t(string proc, string typ) {
		procedure = proc;
		type = typ;
		//parent = "";
	}
} stmt;

class ProgLine {
private: vector<stmt_t> progLine;		//list contains all stmt and its type (assign/if/while/call)
public: 
		ProgLine();
		~ProgLine();
		bool addToList(int lineNo, string type, string proc);		//add a stmt to List, return true if successful
		string getType(string lineNo);					//get Type of a stmt
		string getProcedure(string lineNo);			//get Procedure of a stmt
		int numOfLines();			//return number of lines of whole program
		vector<string> getLinesOfProc(string procedure);		//get all stmt of a proc
		vector<string> getLinesOfType(string type);	//get all stmt of a type
		vector<stmt_t> getTable();
		vector<string> getAssignsOfProc(string procedure);
		
		void updateFollowS(int line, string s);
		void updateParent(int line, string s);
		void updateParentS(int line, string s);
		void updateNext(int line, string s);

		vector<string> getFollowS(int line);
		string getParent(int line);
		vector<string> getParentS(int line);
		vector<string> getNext(int line);
};

#endif