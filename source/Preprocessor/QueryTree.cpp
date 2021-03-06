#include"QueryTree.h"
#include<algorithm>
#include<iostream>

string relations[] = { "Follows","Follows*","Parent","Parent*","Modifies","Uses","Calls","Calls*","Next","Next*","Affects","Affects*", 
						"Contains", "Contains*", "Sibling"};

QueryTree::QueryTree(){

    isValid = true;
    result = new PreResultNode();
    suchThat = new PreSuchThatNode();
    pattern = new PrePatternNode();
	with = new PreWithNode();
}

QueryTree::~QueryTree(){
}


//getter
bool QueryTree::getValidity(){
    return isValid;
}

PreResultNode* QueryTree::getResult(){
    return result;
}

PreSuchThatNode* QueryTree::getSuchThat(){
    return suchThat;
}

PrePatternNode* QueryTree::getPattern(){
    return pattern;
}

PreWithNode* QueryTree::getWith() {
	return with;
}

vector< vector<string> > QueryTree::getSymbolTable(){
    return symbolTable;
}


//setter
void QueryTree::setValidity(bool boolean){
    isValid = boolean;
}

void QueryTree::setResult(vector<string> table){

    if(table.size()==0) return;
	
	PreResultNode* resultPtr = result;
	PreResultNode* prevPtr = resultPtr;
	for (int i = 0; i < table.size(); i++) {
		string str = table[i];
		string synonym, attr;
		if (str.find(".") == string::npos) {
			synonym = str;
		}
		else {
			vector<string> words = stringToVector(str, ".");
			synonym = words[0];
			attr = words[1];
			if (attr == "") {
				isValid = false;
				return;
			}
		}

		if (isValidResultAttribute(table, synonym, attr)) {
			AttrRef ar(synonym, attr);
			resultPtr->setResult(ar);
		}

		else {
			//cout << "wrong result TREE: " << str << endl;
			isValid = false;
			return;
		}
		PreResultNode* nextNode = new PreResultNode();
		prevPtr = resultPtr;
		resultPtr->setNext(nextNode);
		resultPtr = resultPtr->getNext();
	}
	prevPtr->setNext(NULL);
}

void QueryTree::setSuchThat(vector<string> table){
	//printTable(table);
    if(table.size()==0) return; 

	PreSuchThatNode* suchThatPtr = suchThat;
	PreSuchThatNode* prevPtr = suchThatPtr;
	for (int i = 0; i<table.size(); i++) {
		string str = table[i];
		str = removeSpace(str);

		vector<string> words = stringToVector(str, "(");
		string syn = trim(words[0]);

		vector<string> attributes = stringToVector(words[1], ",");
		string first = trim(attributes[0]);

		vector<string> remain = stringToVector(attributes[1], ")");
		string second = trim(remain[0]);

		if (isValidSuchThatAttribute(syn, first, second)) {
			suchThatPtr->setSynonym(syn);
			suchThatPtr->setFirstAttr(first);
			suchThatPtr->setSecondAttr(second);
		}
		else {
			//cout << "wrong such that TREE: " << str << endl;
			isValid = false;
		}

		PreSuchThatNode* nextNode = new PreSuchThatNode();
		prevPtr = suchThatPtr;
		suchThatPtr->setNext(nextNode);
		suchThatPtr = suchThatPtr->getNext();
	}
	prevPtr->setNext(NULL);
}

void QueryTree::setPattern(vector<string> table){

	if (table.size()==0) return;

	PrePatternNode* patternPtr = pattern;
	PrePatternNode* prevPtr = patternPtr;
	for (int i = 0; i < table.size(); i++) {
		string str = table[i];
		str = removeSpace(str);

		vector<string> words = stringToVector(str, "(");
		string syn = trim(words[0]);

		if (countWords(str, ",") == 2) {
			vector<string> attributes = stringToVector(words[1], ",");
			string first = trim(attributes[0]);

			int commaIndex = str.find(",");
			string remain = str.substr(commaIndex+1, str.size()-commaIndex);
			int endIndex = remain.rfind(")");
			string second = remain.substr(0, endIndex);
			string third = "";

			if (isValidPatternAttribute(syn, first, second, third) && (isAssign(syn) || isWhile(syn))) {
				patternPtr->setSynonym(syn);
				patternPtr->setFirstAttr(first);
				patternPtr->setSecondAttr(second);
			}
			else {
				//cout << "wrong pattern TREE_1: " << str << endl;
				isValid = false;
			}
		}

		if (countWords(str, ",") == 3) {
			vector<string> attributes = stringToVector(words[1], ",");
			string first = trim(attributes[0]);
			string second = trim(attributes[1]);

			vector<string> remain = stringToVector(attributes[2], ")");
			string third = trim(remain[0]);

			if (isValidPatternAttribute(syn, first, second, third) && isIf(syn)) {
				patternPtr->setSynonym(syn);
				patternPtr->setFirstAttr(first);
				patternPtr->setSecondAttr(second);
				patternPtr->setThirdAttr(third);
			}
			else {
				//cout << "wrong pattern TREE_2: " << str << endl;
				isValid = false;
			}
		}

		PrePatternNode* nextNode = new PrePatternNode();
		prevPtr = patternPtr;
		patternPtr->setNext(nextNode);
		patternPtr = patternPtr->getNext();
	}
	prevPtr->setNext(NULL);
}

void QueryTree::setWith(vector<string> table) {
	//printTable(table);
	if (table.size() == 0) return;

	PreWithNode* withPtr = with;
	PreWithNode* prevPtr = withPtr;
	for (int i = 0; i<table.size(); i++) {
		string str = table[i];
		str = removeSpace(str);

		vector<string> equals = stringToVector(str, "=");
		string left = equals[0];
		string right = equals[1];

		if (isValidWithAttribute(left,right)) {
			if (left.find(".") != string::npos) {
				vector<string> leftWords = stringToVector(left, ".");
				string synonym = leftWords[0];
				string attrName = leftWords[1];
				AttrRef leftAttrRef(synonym, attrName);
				withPtr->setLeftAttrRef(leftAttrRef);
			}
			if (left.find(".") == string::npos) {
				withPtr->setLeftType(left);
			}

			if (right.find(".") != string::npos) {
				vector<string> rightWords = stringToVector(right, ".");
				string synonym = rightWords[0];
				string attrName = rightWords[1];
				AttrRef rightAttrRef(synonym, attrName);
				withPtr->setRightAttrRef(rightAttrRef);
			}
			if (right.find(".") == string::npos) {
				withPtr->setRightType(right);
			}
		}
		else {
			//cout << "wrong with TREE: " << str << endl;
			isValid = false;
		}

		PreWithNode* nextNode = new PreWithNode();
		prevPtr = withPtr;
		withPtr->setNext(nextNode);
		withPtr = withPtr->getNext();
	}
	prevPtr->setNext(NULL);
}

bool QueryTree::isAssign(string str) {
	for (int i = 0; i < symbolTable.size(); i++) {
		for (int j = 0; j < symbolTable[i].size(); j++) {
			if (symbolTable[i][0] == "assign" && symbolTable[i][j] == str) {
				return true;
			}
		}
	}
	return false;
}

bool QueryTree::isVariable(string str) {
	for (int i = 0; i < symbolTable.size(); i++) {
		for (int j = 0; j < symbolTable[i].size(); j++) {
			if (symbolTable[i][0] == "variable" && symbolTable[i][j] == str) {
				return true;
			}
		}
	}
	return false;
}

bool QueryTree::isIf(string str) {
	for (int i = 0; i < symbolTable.size(); i++) {
		for (int j = 0; j < symbolTable[i].size(); j++) {
			if (symbolTable[i][0] == "if" && symbolTable[i][j] == str) {
				return true;
			}
		}
	}
	return false;
}

bool QueryTree::isWhile(string str) {
	for (int i = 0; i < symbolTable.size(); i++) {
		for (int j = 0; j < symbolTable[i].size(); j++) {
			if (symbolTable[i][0] == "while" && symbolTable[i][j] == str) {
				return true;
			}
		}
	}
	return false;
}

void QueryTree::setSymbolTable(vector<string> terms){
    if(terms.size()==0) return;

    for(int i=0; i<terms.size(); i++){
        if(terms[i].find(",") == string::npos){
            string firstWord = getFirstToken(terms[i]);
            string remainWord = removeFirstToken(terms[i]);
            remainWord = trim(remainWord);
            int index = getSymbolIndex(symbolTable, firstWord);
            if( index != -1){
                remainWord = trim(remainWord);
                symbolTable[index].push_back(remainWord);
            }
            else{
                vector<string> newType;
                newType.push_back(firstWord);
                remainWord = trim(remainWord);
                newType.push_back(remainWord);
                symbolTable.push_back(newType);
            }
        }
        else{
            string firstWord = getFirstToken(terms[i]);
            string remainWord = removeFirstToken(terms[i]);
            remainWord = trim(remainWord);
            vector<string> synonyms = stringToVector(remainWord, ",");
            int index = getSymbolIndex(symbolTable, firstWord);
            if( index != -1){
                for(int j=0; j<synonyms.size(); j++){
                    synonyms[j] = trim(synonyms[j]);
                    symbolTable[index].push_back(synonyms[j]);
                }
            }
            else{
                vector<string> newType;
                newType.push_back(firstWord);
                for(int k=0; k<synonyms.size(); k++){
                    synonyms[k] = trim(synonyms[k]);
                    newType.push_back(synonyms[k]);
                }
                symbolTable.push_back(newType);
            }
        }
    }

	for (int i = 0; i < symbolTable.size(); i++) {
		for (int j = 1; j < symbolTable[i].size(); j++) {
			if (!isValidIdent(symbolTable[i][j])) {
				//cout << "wrong symbol table"  << endl;
				//cout << symbolTable[i][j] << endl;
				isValid = false;
			}
		}
	}

	for (int i = 0; i < symbolTable.size(); i++) {
		for (int j = 1; j < symbolTable[i].size(); j++) {
			string symbol = symbolTable[i][j];
			for (int m = 0; m < symbolTable.size(); m++) {
				for (int n = 1; n < symbolTable[m].size(); n++) {
					if ((i != m || j != n) && (symbolTable[m][n] == symbol)) {
						isValid = false;
						//cout << "duplicated declared synonym" << symbol << endl;
					}
				}
			}
		}
	}
}

string QueryTree::getSynType(vector< vector<string> > table, string str) {
	for (int i = 0; i < table.size(); i++) {
		for (int j = 0; j < table[i].size(); j++) {
			if (table[i][j] == str) {
				return table[i][0];
			}
		}
	}
	return "";
}

int QueryTree::getSymbolIndex(vector< vector<string> > table, string str){
    for(int i=0; i<table.size(); i++){
        if(table[i][0] == str){
            return i;
        }
    }
    return -1;
}

bool QueryTree::isInSymbolTable(vector< vector<string> > table, string str) {
	for (int i = 0; i<table.size(); i++) {
		for (int j = 1; j < table[i].size(); j++) {
			if (table[i][j] == str) {
				return true;
			}
		}
	}
	return false;
}

bool QueryTree::isValidResultAttribute(vector<string> table, string synonym, string attr) {
	if (table.size() == 1 && synonym == "BOOLEAN") {
		return true;
	}
	if (isInSymbolTable(symbolTable, synonym)) {
		if (attr!="") {
			string str = synonym + "." + attr;
			if (!isValidAttrRef(str)) {
				return false;
			}
		}
		return true;
	}
	return false;
}

bool QueryTree::isValidSuchThatAttribute(string syn, string first, string second) {
	string firstType = getSynType(symbolTable, first);
	string secondType = getSynType(symbolTable, second);

	if (!containWord(syn, relations, 17)) {
		return false;
	}

	if (syn == "Modifies") {
		if (!(isValidStmtRef(symbolTable, first) || isValidEntRef(symbolTable, first)) || !isValidEntRef(symbolTable, second)) {
			return false;
		}

		if (first != "_" && !isInteger(first) && first.find("\"")==string::npos) {
			if (firstType != "assign" && firstType != "if" && firstType != "while" && firstType != "stmt" 
				&& firstType != "prog_line" && firstType != "procedure" && firstType != "call" && firstType != "stmtLst") {
				return false;
			}
		}
		if (first == "_") {
			return false;
		}
		if (second != "_" && second.find("\"") == string::npos) {
			if (secondType != "variable") {
				return false;
			}
		}
		return true;
	}

	if (syn == "Uses") {
		if (!(isValidStmtRef(symbolTable, first) || isValidEntRef(symbolTable, first)) || !isValidEntRef(symbolTable, second)) {
			return false;
		}
		if (first != "_" && !isInteger(first) && first.find("\"") == string::npos) {
			if (firstType != "assign" && firstType != "if" && firstType != "while" && firstType != "stmt" 
				&& firstType != "prog_line" &&  firstType != "procedure" && firstType != "call" && firstType != "stmtLst") {
				return false;
			}
		}
		if (first == "_") {
			return false;
		}
		if (second != "_" && second.find("\"")==string::npos){
			if (secondType != "variable") {
				return false;
			}
		}
		return true;
	}

	if (syn == "Parent" || syn == "Parent*") {
		if (!isValidStmtRef(symbolTable, first) || !isValidStmtRef(symbolTable, second)) {
			return false;
		}
		if (first != "_" && !isInteger(first)) {
			if (firstType != "if" && firstType != "while" && firstType != "prog_line" && firstType != "stmt") {
				return false;
			}
		}		
		if (second != "_" && !isInteger(second)) {
			if (secondType != "stmt" && secondType != "assign" && secondType != "prog_line" && secondType != "if" && secondType != "while" 
				&& secondType != "call") {
				return false;
			}
		}
		return true;
	}

	if (syn == "Follows" || syn == "Follows*") {
		if (!isValidStmtRef(symbolTable, first) || !isValidStmtRef(symbolTable, second)) {
			return false;
		}
		if (first != "_" && !isInteger(first)) {
			if (firstType != "stmt" && firstType != "assign" && firstType != "prog_line" && firstType != "if" && firstType != "while"
				&& firstType != "call") {
				return false;
			}
		}
		if (second != "_" && !isInteger(second)) {
			if (secondType != "stmt" && secondType != "assign" && secondType != "prog_line"&& secondType != "if" && secondType != "while"
				&& secondType != "call") {
				return false;
			}
		}
		return true;
	}	

	if (syn == "Next" || syn == "Next*") {
		if (!isValidLineRef(symbolTable, first) || !isValidLineRef(symbolTable, second)) {
			return false;
		}
		if (first != "_" && !isInteger(first)) {
			if (firstType != "stmt" && firstType != "assign" && firstType != "prog_line" && firstType != "if" && firstType != "while"
				&& firstType != "call") {
				return false;
			}
		}
		if (second != "_" && !isInteger(second)) {
			if (secondType != "stmt" && secondType != "assign" && secondType != "prog_line" && secondType != "if" && secondType != "while"
				&& secondType != "call") {
				return false;
			}
		}
		return true;
	}

	if (syn == "Calls" || syn == "Calls*") {
		if (!isValidEntRef(symbolTable, first) || !isValidEntRef(symbolTable, second)) {
			return false;
		}

		if (isInteger(first) || isInteger(second)) {
			return false;
		}

		if (first != "_" && first.find("\"") == string::npos) {
			if (firstType != "procedure" ) {
				return false;
			}
		}
		if (second != "_" && second.find("\"") == string::npos) {
			if (secondType != "procedure") {
				return false;
			}
		}
		return true;
	}

	if (syn == "Affects" || syn == "Affects*") {
		if (!isValidStmtRef(symbolTable, first) || !isValidStmtRef(symbolTable, second)) {
			return false;
		}
		if (first != "_" && !isInteger(first)) {
			if (firstType != "assign" && firstType != "prog_line" && firstType != "stmt") {
				return false;
			}
		}
		if (second != "_" && !isInteger(second)) {
			if (secondType != "assign" && secondType != "prog_line" && secondType != "stmt") {
				return false;
			}
		}
		return true;
	}
	if (syn == "Contains" || syn == "Contains*") {
		if (!isValidNodeRef(first) || !isValidNodeRef(second)) {
			return false;
		}
		return true;
	}
	if (syn == "Sibling"){
		if (!isValidNodeRef(first) || !isValidNodeRef(second)) {
			return false;
		}
		return true;
	}
}

bool QueryTree::isValidPatternAttribute(string syn, string first, string second, string third) {
	if (!isValidQuotation(first) || !isValidQuotation(second)) {
		//cout << "516" << endl;
		return false;
	}

	if (isAssign(syn)) {
		if (!isValidEntRef(symbolTable, first)) {
			//cout << "522" << endl;
			return false;
		}

		else if (isValidSynonym(symbolTable, first) && !isVariable(first)) {
			//cout << "527" << endl;
			return false;
		}
		
		if (!isValidExpressionSpec(second)) {
			//cout << "532" << endl;
			return false;
		}
		return true;
	}

	if (isIf(syn)) {
		if (!isValidEntRef(symbolTable, first)) {
			return false;
		}

		else if (isValidSynonym(symbolTable, first) && !isVariable(first)) {
			return false;
		}

		if (second != "_") {
			return false;
		}

		if (third != "_") {
			return false;
		}
		return true;
	}

	if (isWhile(syn)) {
		if (!isValidEntRef(symbolTable, first)) {
			return false;
		}

		else if (isValidSynonym(symbolTable, first) && !isVariable(first)) {
			return false;
		}

		if (second != "_") {
			return false;
		}
		return true;
	}

	return false;
}

bool QueryTree::isValidWithAttribute(string left, string right) {
	if (isValidRef(left) && isValidRef(right)) {
		string firstType = "";
		string secondType = "";
		firstType = getRefType(left);
		secondType = getRefType(right);
		if (firstType != secondType) {
			return false;
		}
		return true;
	}
	return false;
}

string QueryTree::getRefType(string str) {
	if (str.find("procName") != string::npos || str.find("varName") != string::npos || str.find("\"") != string::npos) {
		return "NAME";
	}
	if (str.find("value") != string::npos || str.find("stmt#") != string::npos || isInteger(str) || getSynType(symbolTable, str) == "prog_line" ||
		getSynType(symbolTable, str) == "assign" || getSynType(symbolTable, str) == "stmt") {
		return "INTEGER";
	}
	return "";
}

bool QueryTree::isValidQuotation(string str) {
	if (str.find("\"") != string::npos){
		if (countWords(str, "\"") != 3) {
			return false;
		}
	}
	return true;
}

bool QueryTree::isValidRef(string str) {
	if (str.length() > 2) {
		if (str.at(0) == '\"' && str.at(str.length() - 1) == '\"') {
			string insideQuotes = str.substr(1, str.length() - 2);
			if (isValidIdent(insideQuotes)) {
				return true;
			}
		}
	}

	if (isValidAttrRef(str) || isInteger(str) || getSynType(symbolTable, str) == "prog_line" || 
		getSynType(symbolTable, str) == "assign" || getSynType(symbolTable, str) == "stmt") {
		return true;
	}
	return false;
}

bool QueryTree::isValidAttrRef(string str) {
	if (str.find(".") != string::npos) {
		vector<string> words = stringToVector(str, ".");
		string first = words[0];
		string second = words[1];
		if (getSynType(symbolTable, first) == "procedure" && second == "procName") {
			return true;
		}
		if (getSynType(symbolTable, first) == "variable" && second == "varName") {
			return true;
		}
		if (getSynType(symbolTable, first) == "constant" && second == "value") {
			return true;
		}
		if (getSynType(symbolTable, first) == "stmt" && second == "stmt#") {
			return true;
		}
		if (getSynType(symbolTable, first) == "assign" && second == "stmt#") {
			return true;
		}
		if (getSynType(symbolTable, first) == "prog_line" && second == "stmt#") {
			return true;
		}
		if (getSynType(symbolTable, first) == "call" && second == "procName") {
			return true;
		}
		if (getSynType(symbolTable, first) == "call" && second == "stmt#") {
			return true;
		}
		if (getSynType(symbolTable, first) == "while" && second == "stmt#") {
			return true;
		}
		if (getSynType(symbolTable, first) == "if" && second == "stmt#") {
			return true;
		}
	}
	return false;
}

bool QueryTree::isValidIdent(string str) {
	str = trim(str);
	if (str.size() == 0) {
		return false;
	}
	if (!isalpha(str.at(0))) {
		return false;
	}

	for (int i = 0; i<str.size(); i++) {
		if (!(isalnum(str.at(i)) || str.at(i) == '#')) {
			//cout << "line 305" << endl;
			return false;
		}
	}

	return true;
}

bool QueryTree::isValidSynonym(vector< vector<string> > table, string str) {
	if (isValidIdent(str) && isInSymbolTable(table, str)) {
		return true;
	}
	return false;
}

bool QueryTree::isValidName(string str) {
	if (str.size() == 0) {
		return false;
	}
	if (!isalpha(str.at(0))) {
		return false;
	}
	for (unsigned int i = 0; i<str.size(); i++) {
		if (!isalnum(str.at(i)) && str.at(i) != '#') {
			return false;
		}
	}
	return true;
}

bool QueryTree::isValidStmtRef(vector< vector<string> > table, string str) {
	str = trim(str);
	if (str == "_" || isValidSynonym(table, str) || isInteger(str)) {
		return true;
	}

	return false;
}

bool QueryTree::isValidLineRef(vector< vector<string> > table, string str) {
	str = trim(str);
	if (str == "_" || isValidSynonym(table, str) || isInteger(str)) {
		return true;
	}

	return false;
}

bool QueryTree::isValidEntRef(vector< vector<string> > table, string str) {
	str = trim(str);
	if (isValidSynonym(table, str)) {
			return true;
	}
	if (str == "_") {
		return true;
	}
	if (str.length() > 1) {
		if (str.at(0) == '\"' && str.at(str.length() - 1) == '\"') {
			string insideQuotes = str.substr(1, str.length() - 2);
			if (isValidIdent(insideQuotes)) {
				return true;
			}
		}
	}
	if (isInteger(str)) {
		return true;
	}
	return false;
}

bool QueryTree::isValidExpressionSpec(string str) {
	if (str.size() == 0) {
		return false;
	}
	if (str == "_") {
		return true;
	}

	string insideQuotes = str;
	bool isValidFormat = false;

	if (str.at(0)=='\"' && str.at(str.size()-1)=='\"') {
		isValidFormat = true;
		insideQuotes = str.substr(1, str.size() - 2);
	}
	if (str.at(0) == '_'&& str.at(1) == '\"' && str.at(str.size() - 1) == '_' && str.at(str.size() - 2) == '\"') {
		isValidFormat = true;
		insideQuotes = str.substr(2, str.size() - 4);
	}

	if (isValidFormat == false) {
		return false;
	}

	insideQuotes = removeSpace(insideQuotes);
	vector<string> expr = toExprVector(insideQuotes);
	if (isOperation(expr[0]) || isOperation(expr[expr.size() - 1])) {
		return false;
	}
	if (!isValidChar(expr)) {
		return false;
	}
	if (!isValidBracket(str)) {
		return false;
	}

	for (int i = 0; i<expr.size(); i++) {
		int last = expr.size() - 1;

		if (expr[i] == "(") {
			if (i != 0 && !(expr[i - 1] == "(" || isOperation(expr[i - 1]))) {
				return false;
			}
			if (i != last && !(expr[i + 1] == "(" || isValidName(expr[i + 1]) || isInteger(expr[i + 1]))) {
				return false;
			}
		}
		else if (expr[i] == ")") {
			if (i != 0 && !(expr[i - 1] == ")" || isValidName(expr[i - 1]) || isInteger(expr[i - 1]))) {
				return false;
			}
			if (i != last && !(expr[i + 1] == ")" || isOperation(expr[i + 1]))) {
				return false;
			}
		}
		else if (isOperation(expr[i])) {
			if (i != 0 && !(expr[i - 1] == ")" || isValidName(expr[i - 1]) || isInteger(expr[i - 1]))) {
				return false;
			}
			if (i != last && !(expr[i + 1] == "(" || isValidName(expr[i + 1]) || isInteger(expr[i + 1]))) {
				return false;
			}
		}
		else if (isValidName(expr[i]) || isInteger(expr[i])) {
			if (i != 0 && !(expr[i - 1] == "(" || isOperation(expr[i - 1]))) {
				return false;
			}
			if (i != last && !(expr[i + 1] == ")" || isOperation(expr[i + 1]))) {
				return false;
			}
		}
	}

	return true;
}

bool QueryTree::isValidBracket(string str) {
	vector<string> brackets;
	for (int i = 0; i<str.size(); i++) {
		if (str.at(i) == '(') {
			brackets.push_back("(");
		}
		if (str.at(i) == ')') {
			if (brackets.size() == 0) {
				return false;
			}
			else {
				brackets.pop_back();
			}
		}
	}
	if (brackets.size() == 0) {
		return true;
	}
	return false;
}

bool QueryTree::isValidChar(vector<string> vec) {
	for (int i = 0; i<vec.size(); i++) {
		if (!isValidName(vec[i]) && !isOperation(vec[i]) && !isInteger(vec[i]) && vec[i] != "(" && vec[i] != ")") {
			return false;
		}
	}
	return true;
}

bool QueryTree::isOperation(string str) {
	if (str == "*" || str == "+" || str == "-") {
		return true;
	}
	return false;
}

vector<string> QueryTree::toExprVector(string str) {
	vector<string> expr;
	for (int i = 0; i<str.size(); i++) {
		if (!isalnum(str.at(i))) {
			string symbol = "";
			symbol = symbol + str.at(i);
			expr.push_back(symbol);
		}
		else {
			string num_var = "";
			for (int k = i; k<str.size() && isalnum(str.at(k)); k++) {
				num_var = num_var + str.at(k);
				i = k;
			}
			expr.push_back(num_var);
		}
	}
	return expr;
}

bool QueryTree::isInteger(string str) {
	if (str.size() == 0) {
		return false;
	}
	for (int i = 0; i < str.size(); i++) {
		if (!isdigit(str.at(i))) {
			return false;
		}
	}
	return true;
}

vector<string> QueryTree::stringToVector(string original, string delimiter){

    string s = original;
    vector<string> words;

    int position = 0;
    string token;
    while ((position = s.find(delimiter)) != string::npos) {
        token = s.substr(0, position);
        words.push_back(token);
        s.erase(0, position + delimiter.length());
    }
    words.push_back(s);
    return words;
}

string QueryTree::removeSpace(string str){
    string result = "";
    for(int i=0; i<str.size(); i++){
        if(str.at(i) != ' '){
            result = result + str.at(i);
        }
    }
    return result;
}

string QueryTree::getFirstToken(string str){
    string result = "";
    for(int i=0; i<str.size(); i++){
        if(str.at(i) != ' ')
            result = result + str.at(i);
        else
            break;
    }
    return result;
}

string QueryTree::trim(string str){
    if (str==""){
        return str;
    }

	int i=0;
	int j=0;
	for (i=0;i<str.size();i++){
		if (str[i]!=' ')
			break;
	}
	for (j=str.size()-1;j>=i;j--){
		if (str[j]!=' ')
			break;
	}
	return str.substr(i,j-i+1);
}

string QueryTree::removeFirstToken(string str){
    str = trim(str);
    string result = "";
    int index=0;

    for(int i=0; i<str.size(); i++){
        if(str.at(i) == ' '){
           index=i;
           break;
        }
    }

    for(int j=index+1; j<str.size(); j++){
        result = result + str.at(j);
    }

    return result;
}

bool QueryTree::containWord(string str, string arr[], int size) {
	for (int i = 0; i<size; i++) {
		if (arr[i] == str) {
			return true;
		}
	}
	return false;
}

string QueryTree::toLowerCase(string str) {
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

void QueryTree::printTable(vector<string> table) {
	for (int i = 0; i<table.size(); i++) {
		cout << table[i] << endl;
	}
}

void QueryTree::printDoubleTable(vector< vector<string> > table) {
	for (int i = 0; i<table.size(); i++) {
		for (int j = 0; j < table[i].size(); j++)
			cout << table[i][j] << endl;
	}
}

int QueryTree::countWords(string str, string delimiter) {
	vector<string> words = stringToVector(str, delimiter);
	return words.size();
}

bool QueryTree::isValidNodeRef(string str) {
	if (isValidSynonym(symbolTable, str) || isInteger(str)) {
		return true;
	}
	return false;
}