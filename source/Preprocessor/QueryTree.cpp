#include"QueryTree.h"
#include<algorithm>
#include<iostream>

string relations[] = { "Follows","Follows*","Parent","Parent*","Modifies","Uses","Calls","Calls*","Next","Next*","Affects","Affects*" };

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
			cout << "wrong result TREE" << endl;
			isValid = false;
			return;
		}
		PreResultNode* nextNode = new PreResultNode();
		resultPtr->setNext(nextNode);
		resultPtr = resultPtr->getNext();
	}
}

void QueryTree::setSuchThat(vector<string> table){
	//printTable(table);
    if(table.size()==0) return; 

	PreSuchThatNode* suchThatPtr = suchThat;
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
			cout << "wrong such that TREE:  " << str << endl;
			isValid = false;
		}

		PreSuchThatNode* nextNode = new PreSuchThatNode();
		suchThatPtr->setNext(nextNode);
		suchThatPtr = suchThatPtr->getNext();
	}
}

void QueryTree::setPattern(vector<string> table){

	if (table.size()==0) return;

	PrePatternNode* patternPtr = pattern;
	for (int i = 0; i < table.size(); i++) {
		string str = table[i];
		str = removeSpace(str);

		vector<string> words = stringToVector(str, "(");
		string syn = trim(words[0]);

		if (countWords(str, ",") == 2) {
			vector<string> attributes = stringToVector(words[1], ",");
			string first = trim(attributes[0]);

			vector<string> remain = stringToVector(attributes[1], ")");
			string second = trim(remain[0]);
			string third = "";

			if (isValidPatternAttribute(syn, first, second, third) && (isAssign(syn) || isWhile(syn))) {
				patternPtr->setSynonym(syn);
				patternPtr->setFirstAttr(first);
				patternPtr->setSecondAttr(second);
			}
			else {
				cout << "wrong pattern TREE_1" << endl;
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
				cout << "wrong pattern TREE_2" << endl;
				isValid = false;
			}
		}

		PrePatternNode* nextNode = new PrePatternNode();
		patternPtr->setNext(nextNode);
		patternPtr = patternPtr->getNext();
	}
}

void QueryTree::setWith(vector<string> table) {
	//printTable(table);
	if (table.size() == 0) return;

	PreWithNode* withPtr = with;
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
			cout << "wrong with TREE" << endl;
			isValid = false;
		}

		PreWithNode* nextNode = new PreWithNode();
		withPtr->setNext(nextNode);
		withPtr = withPtr->getNext();
	}
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

	if (!containWord(syn, relations, 14)) {
		return false;
	}

	if (syn == "Modifies") {
		if (!isValidStmtRef(symbolTable, first) || !isValidEntRef(symbolTable, second)) {
			return false;
		}
		if (first != "_" && !isInteger(first)) {
			if (firstType != "assign" && firstType != "if" && firstType != "while" && firstType != "stmt" && firstType != "prog_line" && firstType != "procedure") {
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
		if (!isValidStmtRef(symbolTable, first) || !isValidEntRef(symbolTable, second)) {
			return false;
		}
		if (first != "_" && !isInteger(first)) {
			if (firstType != "assign" && firstType != "if" && firstType != "while" && firstType != "stmt" && firstType != "prog_line" &&  firstType != "procedure") {
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
			if (secondType != "stmt" && secondType != "assign" && secondType != "prog_line") {
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
			if (firstType != "stmt" && firstType != "assign" && firstType != "prog_line") {
				return false;
			}
		}
		if (second != "_" && !isInteger(second)) {
			if (secondType != "stmt" && secondType != "assign" && secondType != "prog_line") {
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
			if (firstType != "stmt" && firstType != "assign" && firstType != "prog_line") {
				return false;
			}
		}
		if (second != "_" && !isInteger(second)) {
			if (secondType != "stmt" && secondType != "assign" && secondType != "prog_line") {
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

}

bool QueryTree::isValidPatternAttribute(string syn, string first, string second, string third) {
	if (isAssign(syn)) {
		if (!isValidEntRef(symbolTable, first)) {
			return false;
		}

		else if (isValidSynonym(symbolTable, first) && !isVariable(first)) {
			return false;
		}
		
		if (!isValidExpressionSpec(second)) {
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
			cout << "pattern while not variable" << endl;
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
			cout << "line 305" << endl;
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
		if (!isalnum(str.at(i))) {
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
	return false;
}

bool QueryTree::isValidExpressionSpec(string str) {
	if (str.size() == 0) {
		return false;
	}
	if (str == "_") {
		return true;
	}

	if (str.at(0)=='\"' && str.at(str.size()-1)=='\"') {
		string insideQuotes = str.substr(1, str.size() - 2);
		if (insideQuotes.find("_") == string::npos) {
			return true;
		}
	}

	if (str.size() <= 4) {
		return false;
	}

	if (!(str.at(0) == '_'&& str.at(1) == '\"' && str.at(str.size() - 1) == '_' && str.at(str.size() - 2) == '\"')) {
		return false;
	}

	string insideQuotes = str.substr(2, str.size() - 4);
	if (isValidName(insideQuotes) || isInteger(insideQuotes)) {
		return true;
	}

	return true;
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
