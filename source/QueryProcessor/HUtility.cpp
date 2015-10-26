#include "../QueryProcessor/HUtility.h"

HUtility::HUtility() {}
HUtility::~HUtility() {}
static vector <vector<string>> symTable;
//Set symbol table
void HUtility::setSymTable(vector<vector<string>> symbolTable) {
	symTable = symbolTable;
}
//Get tables
vector<string> HUtility::getAssignTable() {
	vector<string> ansVec;
	ProgLine* progLine = PKB::getProgLine();
	ansVec = progLine->getLinesOfType("assign");
	return ansVec;
}

void HUtility::getUseTable(vector<UseEntry_t> &useTab, vector<string> &userTable, vector<string> &usedTable) {
	UseTable* UseTable = PKB::getUseTable();
	useTab = UseTable->getTable();
	userTable = toConvention(useTab, true);
	usedTable = toConvention(useTab, 2);
}

bool HUtility::getParentTable(PreResultNode * result, string &firstAtt, vector<string> &parTable,
	string &secondAtt, vector<string> &nestTable) {
	vector<ParentEntry_t> parTab;
	ParentTable* ParentTable = PKB::getParentTable();
	parTab = ParentTable->getTable();
	if (parTab.empty()) {
		return false;
	}
	if (result->getResult().getSynonym() == firstAtt) {
		parTable = HUtility().toConvention(parTab, 1);
	}
	if (result->getResult().getSynonym() == secondAtt) {
		nestTable = HUtility().toConvention(parTab, 2);
	}
	if (result->getResult().getSynonym() != firstAtt && result->getResult().getSynonym() != secondAtt) {
		parTable = HUtility().toConvention(parTab, 1);
	}
	return true;
}

void HUtility::getModifyTable(vector<pair<string, vector<string>>> &modTable) {
	vector<ModifyEntry_t> modTab = PKB::getModifyTable()->getTable();
	for (int i = 0; i < modTab.size(); i++) {
		pair<string, vector<string>> temp;
		temp.first = modTab[i].modifier;
		temp.second = modTab[i].modifiedVar;
		modTable.push_back(temp);
	}
}

void HUtility::getFollowTable(vector<pair<string, string>> &folTable) {
	vector<FollowEntry_t> folTab = PKB::getFollowTable()->getTable();
	for (int i = 0; i < folTab.size(); i++) {
		pair<string, string> temp;
		temp.first = folTab[i].prev;
		temp.second = folTab[i].next;
		folTable.push_back(temp);
	}
}

void HUtility::getNextTable(vector<pair<string, vector<string>>> &nextTable) {
	vector<NextEntry_t> nextTab = PKB::getNextTable()->getTable();
	for (int i = 0; i < nextTab.size(); i++) {
		pair<string, vector<string>> temp;
		temp.first = nextTab[i].lineNo;
		temp.second = nextTab[i].nextStmts;
		nextTable.push_back(temp);
	}
}

vector<pair<string, vector<string>>> HUtility::getConstTable() {
	vector<ConstEntry_t> temp1 = PKB::getConstTable()->getTable();
	vector<pair<string, vector<string>>> ansVec;
	for (int i = 0; i < temp1.size(); i++) {
		pair<string, vector<string>> temp;
		temp.first = temp1[i].line;
		temp.second = temp1[i].constants;
		ansVec.push_back(temp);
	}
	return ansVec;
}

//Other utilities
string HUtility::getSymMean(string sym) {
	for (int i = 0; i < symTable.size(); i++) {
		vector<string> current = symTable[i];
		for (int j = 0; j < current.size(); j++) {
			if (current[j] == sym) {
				return current[0];
			}
		}
	}
	return "";
}

int HUtility::getPos(vector<int> intVec) {
	for (int i = 0; i < intVec.size(); i++) {
		if (intVec[i] == 1) {
			return i;
		}
	}
	return -1;
}

bool HUtility::isInt(string &secondAtt)
{
	try {
		int number = stoi(secondAtt);
		return true;
	}
	catch (exception e) {
		return false;
	}
}

bool HUtility::contain(vector<string> vec, string str) {
	for (int i = 0; i < vec.size(); i++) {
		if (find(begin(vec), end(vec), str) != end(vec)) {
			return true;
		}
	}
	return false;
}

void HUtility::rmEString(vector<string> vec) {
	if (vec.size() > 0) {
		for (int i = 0; i < vec.size(); i++) {
			if (vec[i] == "" || vec[i] == "na") {
				vec.erase(vec.begin() + i);
			}
		}
	}
}

//To convetion
vector<string> HUtility::toConvention(vector<ParentEntry_t>  table, int x) {
	vector<string> ansVec;
	if (x == 1) {
		for (int i = 0; i < table.size(); i++) {
			ansVec.push_back(table[i].lineNo);
		}
	}
	else {
		for (int i = 0; i < table.size(); i++) {
			for (int j = 0; j < table[i].child.size(); j++) {
				ansVec.push_back(table[i].child[j]);
			}
		}
	}
	return ansVec;
}
vector<string> HUtility::toConvention(vector<UseEntry_t> table, bool x) {
	vector<string> ansVec;
	for (int i = 0; i < table.size(); i++) {
		ansVec.push_back(table[i].userLine);
	}
	return ansVec;
}
vector<string> HUtility::toConvention(vector<UseEntry_t> table, int x) {
	vector<string> ansVec;
	for (int i = 0; i < table.size(); i++) {
		for (int j = 0; j < table[i].usedVar.size(); j++) {
			ansVec.push_back(table[i].usedVar[j]);
		}
	}
	return ansVec;
}

//Implement intersection method (case pair (n1, v1) and select v or n
vector<string> HUtility::intersection(vector<string> vec1, vector<string> vec2) {
	vector<string> ansVec;
	for (size_t i = 0; i < vec1.size(); i++) {
		string current = vec1[i];
		if (find(vec2.begin(), vec2.end(), current) != vec2.end()) {
			ansVec.push_back(current);
		}
	}
	return ansVec;
}

vector<string> HUtility::intersection(vector<string> vec1, vector<pair<string, string>> vec2) {
	vector<string> ansVec;
	for (size_t i = 0; i < vec1.size(); i++) {
		string current = vec1[i];
		for (size_t j = 0; j < (sizeof vec2); j++) {
			if (vec2[j].second == current) {
				ansVec.push_back(current);
			}
		}
	}
	return ansVec;
}

vector < pair<string, vector<string>>> HUtility::intersection(vector<string> vec1, vector < pair<string, vector<string>>> vec2) {
	vector < pair<string, vector<string>>> ansVec;
	for (size_t i = 0; i < vec1.size(); i++) {
		string current = vec1[i];
		for (size_t j = 0; j < (sizeof vec2); j++) {
			if (vec2[j].first == current) {
				ansVec.push_back(vec2[j]);
			}
		}
	}
	return ansVec;
}

vector <string> HUtility::intersection(vector<string> vec1, vector < pair<string, vector<string>>> vec2, bool check) {
	vector <string> ansVec;
	for (size_t i = 0; i < vec1.size(); i++) {
		string current = vec1[i];
		for (size_t j = 0; j < (sizeof vec2); j++) {
			if (vec2[j].first == current) {
				ansVec.push_back(current);
			}
		}
	}
	return ansVec;
}