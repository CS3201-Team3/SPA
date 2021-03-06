#include "ParentTable.h"
#include <vector>
#include <string>
#include "ParentSTable.h"
#include <queue>
#include "PKB.h"
#include "ProgLine.h"

ParentTable::ParentTable() {
}

ParentTable::~ParentTable() {
}

bool ParentTable::addToTable(string lineNo, string child) {
	if (isContained(lineNo) == false) {
		ParentEntry_t *entry = new ParentEntry_t(lineNo, child);
		parentTable.push_back(*entry);
		return true;
	}
	for (unsigned i = 0; i < parentTable.size(); i++) {
		if (parentTable.at(i).lineNo == lineNo) {
			for (unsigned j = 0; j < parentTable.at(i).child.size(); j++) {
				if (parentTable.at(i).child.at(j) == child) {
					return false;
				}
			}
			parentTable.at(i).child.push_back(child);
			break;
		}
	}
	return true;
}

vector <string> ParentTable::getChild(string parent) {
	vector <string> returnList;
	for (unsigned i = 0; i < parentTable.size(); i++) {
		if (parentTable.at(i).lineNo == parent) {
			returnList = parentTable.at(i).child;
			break;
		}
	}
	return returnList;
}

/*string ParentTable::getParentSlow(string child) {
	for (unsigned i = 0; i < parentTable.size(); i++) {
		for (unsigned j = 0; j < parentTable.at(i).child.size(); j++) {
			if (parentTable.at(i).child.at(j) == child) {
				return parentTable.at(i).lineNo;
			}
		}
	}
	int line = atoi(child.c_str());
	ProgLine* pl = PKB::getProgLine();
	return pl->getParent(line);
}*/

int ParentTable::size() {
	return parentTable.size();
}

bool ParentTable::isContained(string line) {
	for (unsigned i = 0; i < parentTable.size(); i++) {
		if (parentTable.at(i).lineNo == line) {
			return true;
		}
	}
	return false;
}

vector<ParentEntry_t> ParentTable::getTable() {
	return parentTable;
}

bool ParentTable::isParent(string lineNo, string child) {
	if (!isContained(lineNo)) {
		return false;
	}
	vector<string> childList = getChild(lineNo);
	for (unsigned i = 0; i < childList.size(); i++) {
		if (childList.at(i) == child) {
			return true;
		}
	}
	return false;
}

string ParentTable::getParent(string child) {
	for (unsigned i = 0; i < parentTable.size(); i++) {
		for (unsigned j = 0; j < parentTable.at(i).child.size(); j++) {
			if (parentTable.at(i).child.at(j) == child) {
				return parentTable.at(i).lineNo;
			}
		}
	}
	return "";
}