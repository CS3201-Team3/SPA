#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sstream>
#include "DesignExtractor.h"

using namespace std;

DesignExtractor::DesignExtractor(){}

DesignExtractor::DesignExtractor(vector<string>parsedInput){
	input = parsedInput;
	initialize();
	buildAST(input);
	for (unsigned i = 0; i < ast.size(); i++) {
		processCallTable(ast.at(i));
		processFollowTable(ast.at(i));
		processParentTable(ast.at(i));
	}

	processProcTable();
	int procNumber = procTable->size();
	for (int i = 0; i < procNumber; i++) {
		completedProc.push_back(false);
	}

	processModTable();

	/*processUseTable();*/
	

	storeToPKB();
}

DesignExtractor::~DesignExtractor(){}

void DesignExtractor::initialize() {
	callTable = new CallTable();
	followTable = new FollowTable();
	parentTable = new ParentTable();
	modTable = new ModifyTable();
	useTable = new UseTable();
	varTable = new VarTable();
	procTable = new ProcTable();
	constTable = new ConstTable();
	ast = vector<AST* >();
	ASTCurParent = vector<TNode*>();
	completedProc = vector<bool>();

	lineNumber = 0;
	stmtLstNumber = 0;
	procedureNumber = -1;
}

void DesignExtractor::storeToPKB() {
	PKB::setASTList(ast);
	PKB::setModifyTable(modTable);
	PKB::setUseTable(useTable);
	PKB::setProcTable(procTable);
	PKB::setVarTable(varTable);
	PKB::setConstTable(constTable);
	PKB::setParentTable(parentTable);
	PKB::setFollowTable(followTable);
}

//-------------------------AST-------------------------//
vector<AST*> DesignExtractor::buildAST(vector<string> input){
	processAST(input);
	return ast;
}

void DesignExtractor::processAST(vector<string> input){
	for (unsigned i = 0; i < input.size(); i++) {
		// Current line in input, not lineNumber marked for stmt
		string curLine = input.at(i);
		size_t curLineLen = curLine.length();

		if (curLine.find(PROCEDURE) != string::npos) {
			// new procedure, clear all old elements of old procedure
			ASTCurParent.clear();

			string theRestOfLine = curLine.substr(PROC_LEN);
			ast.push_back(new AST());
			procedureNumber++;
			processProcedure(theRestOfLine);
		}
		else if (curLine.find(WHILE) != string::npos) {
			string theRestOfLine = curLine.substr(WHILE_LEN);
			lineNumber++;
			processWhile(theRestOfLine, lineNumber);
		}
		else if (curLine.find(EQUAL) != string::npos) {
			size_t posEqualSign = curLine.find(EQUAL);
			size_t posSemicolon = curLine.find(SEMICOLON);

			string leftSide = curLine.substr(0, posEqualSign);
			string rightSide = curLine.substr(posEqualSign + 1, posSemicolon - posEqualSign - 1);
			lineNumber++;

			processAssign(leftSide, rightSide, lineNumber);
		}
		else if (curLine.find(IF) != string::npos && curLine.find(THEN) != string::npos){
			size_t posThen = curLine.find(THEN);
			string controlVar = curLine.substr(IF_LEN, posThen - IF_LEN);

			lineNumber++;
			processIfThen(controlVar, lineNumber);
		}
		else if (curLine.find(ELSE) != string::npos) {
			processElse();
		}
		else if (curLine.find(CALL) != string::npos) {
			size_t posSemicolon = curLine.find(SEMICOLON);
			string callValue = curLine.substr(CALL_LEN, posSemicolon - CALL_LEN);

			lineNumber++;
			processCallAST(callValue, lineNumber);
		}
		else {
			return;
		}

		// Check the appearance of }
		string tempLine = curLine;
		
		// trace multiple }
		while (true) {
			size_t posCloseBracket = tempLine.find(CLOSE_BRACKET);
			
			if (posCloseBracket != string::npos) {
				tempLine.erase(posCloseBracket, 1);

				// remove multiple nodes
				while (ASTCurParent.size() > 0) {
					TNode* curPar = ASTCurParent.at(ASTCurParent.size() - 1);
					string typeCurPar = curPar->getType();
					string valueCurPar = curPar->getValue();
					ASTCurParent.pop_back();
					
					// if 'then' stmtlst -> 'if' stmt is still parent for the 'else' stmtlst
					if (typeCurPar != STMTLST || valueCurPar == THEN) {
						break;
					}
				}
			}
			else {
				break;
			}
		}
	}
}

void DesignExtractor::processProcedure(string theRestOfLine){
	// remove bracket {
	size_t posOfOpenBracket = theRestOfLine.find(OPEN_BRACKET);
	string value = theRestOfLine.substr(0, posOfOpenBracket);

	// Put procedure into tree
	// procedure + stmtLst: no line number
	TNode* procNode = new TNode(value, PROCEDURE, 0);

	stmtLstNumber++;
	string stmtLstNumText = convertNumToStr(stmtLstNumber);
	TNode* stmtLstNode = new TNode(stmtLstNumText, STMTLST, 0);
	
	ast.at(procedureNumber)->addToTree(procNode);
	ASTCurParent.push_back(procNode);
	
	// Put statementList into tree
	ast.at(procedureNumber)->addToTree(stmtLstNode);
	procNode->setChild(stmtLstNode);
	stmtLstNode->setParent(procNode);

	ASTCurParent.push_back(stmtLstNode);
}

void DesignExtractor::processCallAST(string callValue, int lineNumber) {
	TNode* callNode = new TNode(callValue, CALL, lineNumber);
	ast.at(procedureNumber)->addToTree(callNode);

	TNode* curParent = ASTCurParent.at(ASTCurParent.size() - 1);
	curParent->setChild(callNode);
	callNode->setParent(curParent);
}

/*----If-then-else process functions for buildAST----*/
void DesignExtractor::processIfThen(string controlVar, int lineNumber) {
	// if Node
	TNode* ifNode = new TNode(NO_VALUE, IF, lineNumber);
	ast.at(procedureNumber)->addToTree(ifNode);

	TNode* curParent = ASTCurParent.at(ASTCurParent.size() - 1);
	curParent->setChild(ifNode);
	ifNode->setParent(curParent);
	ASTCurParent.push_back(ifNode);

	// control variable node
	TNode* controlVarNode = new TNode(controlVar, VARIABLE, lineNumber);
	ast.at(procedureNumber)->addToTree(controlVarNode);
	varTable->addVar(controlVar);

	ifNode->setChild(controlVarNode);
	controlVarNode->setParent(ifNode);

	// then stmtLst
	TNode* thenNode = new TNode(THEN, STMTLST, lineNumber);
	ast.at(procedureNumber)->addToTree(thenNode);

	ifNode->setChild(thenNode);
	thenNode->setParent(ifNode);
	ASTCurParent.push_back(thenNode);
}

void DesignExtractor::processElse() {
	// current parent is ifNode
	TNode* curIfNodePar = ASTCurParent.at(ASTCurParent.size() - 1);
	int curParLineNum = curIfNodePar->getLine();

	TNode* elseNode = new TNode(ELSE, STMTLST, curParLineNum);
	ast.at(procedureNumber)->addToTree(elseNode);

	curIfNodePar->setChild(elseNode);
	elseNode->setParent(curIfNodePar);
	ASTCurParent.push_back(elseNode);
}
/*----------------------------------------------------*/

void DesignExtractor::processWhile(string theRestOfLine, int lineNumber){
	// Put whileNode into tree
	TNode* whileNode = new TNode(NO_VALUE, WHILE, lineNumber);
	ast.at(procedureNumber)->addToTree(whileNode);

	TNode* curParent = ASTCurParent.at(ASTCurParent.size() - 1);
	curParent->setChild(whileNode);
	whileNode->setParent(curParent);
	ASTCurParent.push_back(whileNode);

	// Put StmtLst of while into tree
	stmtLstNumber++;
	string stmtLstNumText = convertNumToStr(stmtLstNumber);
	TNode* stmtLstNode = new TNode(stmtLstNumText, STMTLST, 0);

	// Put control variable into tree
	size_t posOfOpenBracket = theRestOfLine.find(OPEN_BRACKET);
	string controlVar = theRestOfLine.substr(0, posOfOpenBracket);

	TNode* controlVarNode = new TNode(controlVar, VARIABLE, lineNumber);
	varTable->addVar(controlVar);

	whileNode->setChild(controlVarNode);
	controlVarNode->setParent(whileNode);
	ast.at(procedureNumber)->addToTree(controlVarNode);

	whileNode->setChild(stmtLstNode);
	stmtLstNode->setParent(whileNode);
	ast.at(procedureNumber)->addToTree(stmtLstNode);
	ASTCurParent.push_back(stmtLstNode);	
}

/*-------------No ;} in right side-------------*/
void DesignExtractor::processAssign(string leftSide, string rightSide, int lineNumber){
	// Put assign node into tree
	TNode* curParent = ASTCurParent.at(ASTCurParent.size() - 1);
	TNode* assignNode = new TNode(NO_VALUE, ASSIGN, lineNumber);

	ast.at(procedureNumber)->addToTree(assignNode);
	curParent->setChild(assignNode);
	assignNode->setParent(curParent);
	ASTCurParent.push_back(assignNode);

	// Add left child first
	TNode* leftVar = new TNode(leftSide, VARIABLE, lineNumber);
	varTable->addVar(leftSide);

	ast.at(procedureNumber)->addToTree(leftVar);
	assignNode->setChild(leftVar);
	leftVar->setParent(assignNode);

	processRightSideAssign(ast.at(procedureNumber), assignNode, rightSide, lineNumber);
	ASTCurParent.pop_back();
}

void DesignExtractor::processRightSideAssign(AST* subAST, TNode* curParent, string rightSide, int lineNumber){
	vector<int> plusList;

	// Find the position of each plus sign
	string tempStr = rightSide;
	while (true) {
		int posOfPlus = tempStr.find(PLUS);

		if(posOfPlus != string::npos){
			// replace the first occurence + by @
			tempStr.replace(posOfPlus, 1, "@");
			plusList.push_back(posOfPlus);
		} else {
			// Fake plus sign
			plusList.push_back(tempStr.length());
			break;
		}
	}

	// Create subtree of assignment
	string leftSubTree = rightSide.substr(0, plusList.at(0));
	string typeOfLeft = exprType(leftSubTree);

	// add const & var into the corresponding table
	if (typeOfLeft == VARIABLE) {
		varTable->addVar(leftSubTree);
	}
	else if (typeOfLeft == CONSTANT) {
		constTable->addToTable(convertNumToStr(lineNumber), leftSubTree);
	}

	TNode* leftSubTreeNode = new TNode(leftSubTree, typeOfLeft, lineNumber);
	subAST->addToTree(leftSubTreeNode);

	for(unsigned i = 0; i < plusList.size() - 1; i++){
		int prevPlus = plusList.at(i);
		int nextPlus = plusList.at(i + 1);

		string rightSubTree = rightSide.substr(prevPlus + 1, nextPlus - prevPlus -1);	
		string typeOfRight = exprType(rightSubTree);

		if (typeOfRight == VARIABLE) {
			varTable->addVar(rightSubTree);
		}
		else if (typeOfRight == CONSTANT) {
			constTable->addToTable(convertNumToStr(lineNumber),rightSubTree);
		}

		TNode* rightSubTreeNode = new TNode(rightSubTree, typeOfRight, lineNumber);
		TNode* plusNode = new TNode(NO_VALUE, PLUS_TEXT, lineNumber);

		subAST->addToTree(rightSubTreeNode);
		
		plusNode->setChild(leftSubTreeNode);
		leftSubTreeNode->setParent(plusNode);
		plusNode->setChild(rightSubTreeNode);
		rightSubTreeNode->setParent(plusNode);
		
		leftSubTreeNode = plusNode;
		subAST->addToTree(leftSubTreeNode);
	}

	// Stick subtree to main tree
	if (curParent->getType() != "") {
		curParent->setChild(leftSubTreeNode);
		leftSubTreeNode->setParent(curParent);
	}
}

//---------------------Create Call Table------------------//
void DesignExtractor::processCallTable(AST* ast) {
	string caller = ast->getProcedure();

	for (unsigned i = 0; i < ast->getTree().size(); i++) {
		TNode* curNode = ast->getTree().at(i);
		string curNodeType = curNode->getType();

		if (curNodeType == CALL) {
			string callee = curNode->getValue();
			callTable->addToTable(caller, callee);
		}
	}
}

//-------------------Create Follow Table---------------------//
void DesignExtractor::processFollowTable(AST* subAST) {
	// check the whole nodes in the current ast
	for (unsigned i = 0; i < subAST->getTree().size(); i++) {
		// check unique StmtLst
		TNode* parent = subAST->getTree().at(i);
		if (parent->getType() == STMTLST) {
			vector<TNode*> childLst = parent->getChildList();

			for (unsigned j = 0; j < childLst.size() - 1; j++) {
				TNode* preChild = childLst.at(j);
				TNode* nextChild = childLst.at(j + 1);

				int prev = preChild->getLine();
				int next = nextChild->getLine();
				string prevStr = convertNumToStr(prev);
				string nextStr = convertNumToStr(next);

				followTable->addToTable(prevStr, nextStr);
			}
		}
	}
}

//--------------------Create Parent Table-----------------------//
// stmtLst's parent is the parent of stmtLst's child
void DesignExtractor::processParentTable(AST* subAST){
	for (unsigned i = 0; i < subAST->getTree().size(); i++) {
		TNode* middleNode = subAST->getTree().at(i);

		TNode* parStmtLst = middleNode->getParent();
		string typeOfPar = parStmtLst->getType();
		int parLine = parStmtLst->getLine();
		string parLineStr = convertNumToStr(parLine);

		if(typeOfPar == WHILE | typeOfPar == IF) {
			vector<TNode*> childStmtLst = middleNode->getChildList();

			for(unsigned j = 0; j < childStmtLst.size(); j++){
				TNode* child = childStmtLst.at(j);
				int childLine = child->getLine();
				string childLineStr = convertNumToStr(childLine);

				parentTable->addToTable(parLineStr, childLineStr);
			}
		}
	}
}

//------------------------Create Modify Table------------------------//
void DesignExtractor::processModTable() {
	// use AST for assignment
	for (unsigned i = 0; i < ast.size(); i++) {
		AST* curAST = ast.at(i);

		for (unsigned j = 0; j < curAST->getTree().size(); j++) {
			TNode* curNode = curAST->getTree().at(j);
			string curNodeType = curNode->getType();

			if (curNodeType == ASSIGN) {
				int lineNum = curNode->getLine();
				string lineNumStr = convertNumToStr(lineNum);

				// first child is the left side variable
				TNode* leftChildVar = curNode->getChildList().at(0);
				string modVar = leftChildVar->getValue();
				modTable->addToTable(lineNumStr, modVar);

				// containers & procedure also modifies modVar
				// get an assign node, then go up all the way until the root
				while (curNode->getType() != PROCEDURE) {
					TNode* curPar = curNode->getParent();
					string curParType = curPar->getType();

					// add lineNum for container + assign
					// name for proc
					if (curParType == WHILE || curParType == IF) {
						int containerLineNum = curPar->getLine();
						string containerLineNumStr = convertNumToStr(containerLineNum);
						modTable->addToTable(containerLineNumStr, modVar);
					}
					else if (curParType == PROCEDURE) {
						string procName = curPar->getValue();
						modTable->addToTable(procName, modVar);
					}

					curNode = curPar;
				}
			}
		}
	}

	// update ModTable with Call
	// for every procedure
	for (int i = 0; i < procTable->size(); i++) {
		string caller = procTable->getProc(i);
		if (!completedProc.at(i)) {
			vector<string> callees = callTable->getCallees(caller);

			if (callees.size() > 0) {
				for (int j = 0; j < callees.size(); j++) {
					string callee = callees.at(j);
					updateModTable(caller, callee);
				}
			}
		}

		completedProc.at(i) = true;
	}
}
	/*
	* Update modTable for the Call only
	*/
void DesignExtractor::updateModTable(string caller, string callee) {
	vector<string> calleeOfCalleeList = callTable->getCallees(callee);
	int idxCallee = procTable->indexOf(callee);

	// if callee's modTable is completed then add(caller, modVarOfCallee)
	if (calleeOfCalleeList.size() > 0 || !completedProc.at(idxCallee)) {
		for (int i = 0; i < calleeOfCalleeList.size(); i++) {
			updateModTable(callee, calleeOfCalleeList.at(i));
		}
	}

	// base case when callee doesnt call any other procedure
	// add all modVar of callee to caller's modTable
	vector<string> modVarList = modTable->getModified(callee);
	for (int i = 0; i < modVarList.size(); i++) {
		modTable->addToTable(caller, modVarList.at(i));
	}

	// mark the proc as complete after add all modVar into modTable
	// or this given procedure doesnt call any other procedure
	int indexProc = procTable->indexOf(callee);
	completedProc.at(indexProc) = true;
}

void DesignExtractor::processUseTable() {
	for (unsigned i = 0; i < ast.size(); i++) {
		AST* curAST = ast.at(i);
		for (unsigned j = 0; j < curAST->getTree().size(); j++) {
			TNode* curNode = curAST->getTree().at(j);
			string curNodeType = curNode->getType();
		}
	}
}

//------------------Create procedure table------------------//
void DesignExtractor::processProcTable() {
	for (unsigned i = 0; i < ast.size(); i++) {
		AST* curAST = ast.at(i);
		TNode* procNode = curAST->getTree().at(0);
		procTable->addProc(procNode->getValue());
	}
}

/*bool DesignExtractor::processModTable() {	
	int lineNumber = 0;
	for (unsigned i = 0; i < input.size(); i++) {			
		string line = input.at(i);
		lineNumber = getRealLineNumber(lineNumber, line);
		unsigned pos = line.find(EQUAL);

		if (pos != string::npos){
			string var = line.substr(0, pos);
			modTable->add(lineNumber, var);							
			varTable->addVar(var);
		}
	}
	return true;
}

bool DesignExtractor::processUseTable() {
	int lineNumber = 0;

	for (unsigned i = 0; i < input.size(); i++) {
		string line = input.at(i);
		string type = line.substr(0, 5);
		if (type == WHILE) {
			string var = line.substr(5, line.size() - 6);
			lineNumber = getRealLineNumber(lineNumber, line);

			useTable->add(lineNumber, var);			// start after while (5), length = size - start - '{'
			varTable->addVar(var);
		}
		else {								
			unsigned pos = line.find(EQUAL);
			lineNumber = getRealLineNumber(lineNumber, line);

			if (pos != string::npos) {
				line = line.substr(pos + 1, line.size() - pos - 1);			//remove =
				pos = line.find(PLUS);
				while (pos != string::npos) {
					string var = line.substr(0, pos);

					if (!isConst(var)) {	
						useTable->add(lineNumber, var);				
						varTable->addVar(var);
					}
					else {
						constTable->addToTable(lineNumber, var);		
					}
					line = line.substr(pos + 1, line.size() - pos - 1);		//remove +
					pos = line.find(PLUS);
				}
				if (line.find(CLOSE_BRACKET) != string::npos) {
					line = line.substr(0, line.size() - 2);			//remove '}' and ';'
				}
				else {
					line = line.substr(0, line.size() - 1);		//remove ';'
				}
				if (!isConst(line)) {
					useTable->add(lineNumber, line);					
					varTable->addVar(line);
				}
				else {
					constTable->addToTable(lineNumber, line);
				}
			}
		}
	}
	return true;
}

bool DesignExtractor::isConst(string var){
	return (isdigit(var[0]));
}*/


// Getter
CallTable* DesignExtractor::getCallTable() {
	return callTable;
}

FollowTable* DesignExtractor::getFollowTable(){
	return followTable;
}

ParentTable* DesignExtractor::getParentTable(){
	return parentTable;
}

ModifyTable* DesignExtractor::getModTable() {
	return modTable;
}

UseTable* DesignExtractor::getUseTable() {
	return useTable;
}

VarTable* DesignExtractor::getVarTable() {
	return varTable;
}

ConstTable* DesignExtractor::getConstTable() {
	return constTable;
}

ProcTable* DesignExtractor::getProcTable() {
	return procTable;
}

vector<AST*> DesignExtractor::getASTList() {
	return ast;
}

AST* DesignExtractor::buildSubtree(string pattern) {
	AST* subAST = new AST();
	processRightSideAssign(subAST, new TNode(), pattern, 0);

	return subAST;
}

// Smaller modules
string DesignExtractor::convertNumToStr(int stmtLstNumber){
	ostringstream osstream;
	osstream << stmtLstNumber;
	string stmtLstNumText = osstream.str();

	return stmtLstNumText;
}

// Check the string is number or variable
// Return the type of VARIABLE or CONSTANT.
string DesignExtractor::exprType(string numText){
	char firstDigit = numText.at(0);

	if(isdigit(firstDigit)){
		return CONSTANT;
	} else {
		return VARIABLE;
	}
}

int DesignExtractor::getRealLineNumber(int lineNumber, string line) {
	// Keep track lineNumber from input
	unsigned posOfProc = line.find(PROCEDURE);
	if (posOfProc == string::npos) {
		lineNumber++;
	}

	return lineNumber;
}