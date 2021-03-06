#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sstream>
#include <stack>
#include "DesignExtractor.h"

using namespace std;

DesignExtractor::DesignExtractor(){
	initialize();
}

DesignExtractor::DesignExtractor(vector<string>parsedInput){
	input = parsedInput;
	initialize();
	buildAST(input);
	for (unsigned i = 0; i < ast.size(); i++) {
		processCallTable(ast.at(i));
		processFollowTable(ast.at(i));
		processParentTable(ast.at(i));
	}
	
	if (callTable->isContainedRecur()) {
		exit(EXIT_FAILURE);
	}
	
	processProcTable();
	processModUseTable();
	
	processNextTable();
	processSTable();
	generateCallLine();
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
	progLine = new ProgLine();
	ast = vector<AST* >();
	ASTCurParent = vector<TNode*>();
	nextTable = new NextTable();
	followSTable = new FollowSTable();
	callSTable = new CallSTable();
	parentSTable = new ParentSTable();
	callLine = new CallLine();

	lineNumber = 0;
	stmtLstNumber = 0;
	procedureNumber = -1;
	rightSideText = "";
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
	PKB::setProgLine(progLine);
	PKB::setNextTable(nextTable);
	PKB::setCallTable(callTable);
	PKB::setCallSTable(callSTable);
	PKB::setFollowSTable(followSTable);
	PKB::setParentSTable(parentSTable);
	//PKB::updateProgLine();
	PKB::setCallLine(callLine);
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

			progLine->addToList(lineNumber, WHILE, curProcName);
			processWhile(theRestOfLine, lineNumber);
		}
		else if (curLine.find(EQUAL) != string::npos) {
			size_t posEqualSign = curLine.find(EQUAL);
			size_t posSemicolon = curLine.find(SEMICOLON);

			string leftSide = curLine.substr(0, posEqualSign);
			// no =, ;, } in rightSideText
			rightSideText = curLine.substr(posEqualSign + 1, posSemicolon - posEqualSign - 1);
			lineNumber++;

			progLine->addToList(lineNumber, ASSIGN, curProcName);
			processAssign(leftSide, lineNumber);
		}
		else if (curLine.find(IF) != string::npos && curLine.find(THEN) != string::npos){
			size_t posThen = curLine.find(THEN);
			string controlVar = curLine.substr(IF_LEN, posThen - IF_LEN);
			lineNumber++;

			progLine->addToList(lineNumber, IF, curProcName);
			processIfThen(controlVar, lineNumber);
		}
		else if (curLine.find(ELSE) != string::npos) {
			processElse();
		}
		else if (curLine.find(CALL) != string::npos) {
			size_t posSemicolon = curLine.find(SEMICOLON);
			string callValue = curLine.substr(CALL_LEN, posSemicolon - CALL_LEN);
			lineNumber++;

			progLine->addToList(lineNumber, CALL, curProcName);
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
	string procName = theRestOfLine.substr(0, posOfOpenBracket);
	curProcName = procName;

	// Put procedure into tree
	// procedure + stmtLst: no line number
	TNode* procNode = new TNode(procName, PROCEDURE, 0);

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
void DesignExtractor::processAssign(string leftSide, int lineNumber){
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

	processRightSideAssign(ast.at(procedureNumber), assignNode, lineNumber);
	ASTCurParent.pop_back();
}

void DesignExtractor::processRightSideAssign(AST* curProcAst, TNode* curPar, int lineNumber) {
	stack<int> bracketStack = stack<int>();
	seperateNodeBracket = queue<TNode*>();
	queue<TNode*> finalQueue = queue<TNode*>();
	int len = rightSideText.length();
	TNode* finalStickNode = new TNode();
	bool noBracket = true;

	// process for bracket
	for (int i = 0; i < len; i++) {
		string curChar = rightSideText.substr(i, 1);

		if (curChar == ROUND_OPEN_BRACKET) {
			bracketStack.push(i);
			noBracket = false;
		}
		else if (curChar == ROUND_CLOSE_BRACKET) {
			int openBracket = bracketStack.top();
			bracketStack.pop();

			string subStr = rightSideText.substr(openBracket + 1, i - openBracket - 1);
			TNode* curNode = processInsideBracket(curProcAst, subStr, lineNumber);
			seperateNodeBracket.push(curNode);

			// use for sticking to parent
			finalStickNode = curNode;

			// (...) -> @...@
			for (int j = openBracket; j <= i; j++) {
				rightSideText.replace(j, 1, FAKE_ROUND_BRACKET);
			}

			if (bracketStack.empty()) {
				TNode* finalNode = seperateNodeBracket.front();
				seperateNodeBracket.pop();
				seperateNodeBracket = queue<TNode*>();

				finalQueue.push(finalNode);
			}
		}
	}

	if (rightSideText.find_first_not_of(FAKE_ROUND_BRACKET) != string::npos || noBracket) {
		seperateNodeBracket = finalQueue;
		finalStickNode = processInsideBracket(curProcAst, rightSideText, lineNumber);
	}

	curPar->setChild(finalStickNode);
	finalStickNode->setParent(curPar);
}

// no ( & ) bracket
TNode* DesignExtractor::processInsideBracket(AST* curProcAst, string subString, int lineNumber) {
	// first: type, second, position
	vector<pair<string, int>> signList = vector<pair<string, int>>();
	bool timesAll = true;

	for (unsigned int i = 0; i < subString.length(); i++) {
		string curChar = subString.substr(i, 1);
		if (curChar == PLUS) {
			signList.push_back(make_pair(PLUS_TEXT, i));
		}
		else if (curChar == MINUS) {
			signList.push_back(make_pair(MINUS_TEXT, i));
		}
		else if (curChar == TIMES) {
			signList.push_back(make_pair(TIMES_TEXT, i));
		}
	}

	TNode* curNode = new TNode();
	TNode* defaultNode = new TNode(NO_VALUE, NO_VALUE, -1);
	TNode* curPlusMinusNode = defaultNode;
	int size = signList.size();

	// only 1 variable in right side
	if (size < 1) {
		TNode* singleNode = new TNode(subString, exprType(subString), lineNumber);
		curProcAst->addToTree(singleNode);
		addToVarConstTable(subString, lineNumber);

		return singleNode;
	}

	// first para
	string varLeft = subString.substr(0, signList.at(0).second);
	TNode* leftNode;
	if (varLeft.find(FAKE_ROUND_BRACKET) == string::npos) {
		string type = exprType(varLeft);
		leftNode = new TNode(varLeft, type, lineNumber);
		curProcAst->addToTree(leftNode);

		addToVarConstTable(varLeft, lineNumber);
	}
	else {
		leftNode = seperateNodeBracket.front();
		seperateNodeBracket.pop();
	}

	TNode* signNodeOutSide = new TNode(NO_VALUE, signList.at(0).first, lineNumber);
	signNodeOutSide->setChild(leftNode);
	leftNode->setParent(signNodeOutSide);
	curProcAst->addToTree(signNodeOutSide);

	curNode = signNodeOutSide;
	if (curNode->getType() == PLUS_TEXT || curNode->getType() == MINUS_TEXT) {
		curPlusMinusNode = curNode;
	}

	for (int i = 1; i < size; i++) {
		string signType1 = signList.at(i - 1).first;
		int posOfSign1 = signList.at(i - 1).second;
		string signType2 = signList.at(i).first;
		int posOfSign2 = signList.at(i).second;

		varLeft = subString.substr(posOfSign1 + 1,
			posOfSign2 - posOfSign1 - 1);
		if (varLeft.find(FAKE_ROUND_BRACKET) == string::npos) {
			string type = exprType(varLeft);
			leftNode = new TNode(varLeft, type, lineNumber);
			curProcAst->addToTree(leftNode);

			addToVarConstTable(varLeft, lineNumber);
		}
		else {
			leftNode = seperateNodeBracket.front();
			seperateNodeBracket.pop();
		}

		TNode* signNode = new TNode(NO_VALUE, signType2, lineNumber);
		/*if (i == 1) {
			signNode->setValue("error1");
		}
		if (i == 2) {
			signNode->setValue("error22");
		}
		if (i == 3) {
			signNode->setValue("error333");
		}
		if (i == 4) {
			signNode->setValue("error4444");
		}*/
		curProcAst->addToTree(signNode);

		if ((signType1 == TIMES_TEXT && signType2 == TIMES_TEXT) ||
			((signType1 == PLUS_TEXT || signType1 == MINUS_TEXT) 
			&& (signType2 == PLUS_TEXT || signType2 == MINUS_TEXT))) {

			curNode->setChild(leftNode);
			leftNode->setParent(curNode);

			signNode->setChild(curNode);
			curNode->setParent(signNode);

			curNode = signNode;
			if (signType1 == PLUS_TEXT || signType1 == MINUS_TEXT) {
				curPlusMinusNode = curNode;
			}
		}
		// start of times
		else if ((signType1 == PLUS_TEXT || signType1 == MINUS_TEXT) && signType2 == TIMES_TEXT) {
			signNode->setChild(leftNode);
			leftNode->setParent(signNode);

			curPlusMinusNode = curNode;
			curNode = signNode;

			timesAll = false;
		}
		else if (signType1 == TIMES_TEXT && (signType2 == PLUS_TEXT || signType2 == MINUS_TEXT)) {
			curNode->setChild(leftNode);
			leftNode->setParent(curNode);

			if (curPlusMinusNode != curNode && curPlusMinusNode != defaultNode) {
				curPlusMinusNode->setChild(curNode);
				curNode->setParent(curPlusMinusNode);
				curNode = curPlusMinusNode;
			}

			signNode->setChild(curNode);
			curNode->setParent(signNode);

			curNode = signNode;
			curPlusMinusNode = curNode;

			timesAll = false;
		}
	}

	string lastVarRight = subString.substr(signList.at(size - 1).second + 1);
	TNode* lastRightNode;

	// Add last variable to AST but havent made the relationship
	if (lastVarRight.find(FAKE_ROUND_BRACKET) == string::npos) {
		string type = exprType(lastVarRight);
		lastRightNode = new TNode(lastVarRight, type, lineNumber);
		curProcAst->addToTree(lastRightNode);

		addToVarConstTable(lastVarRight, lineNumber);
	}
	else {
		lastRightNode = seperateNodeBracket.front();
		seperateNodeBracket.pop();
	}
	
	curNode->setChild(lastRightNode);
	lastRightNode->setParent(curNode);

	if ((curPlusMinusNode != curNode && curPlusMinusNode != defaultNode) && !timesAll) {
		curPlusMinusNode->setChild(curNode);
		curNode->setParent(curPlusMinusNode);
		curNode = curPlusMinusNode;
	}

	return curNode;
}

void DesignExtractor::addToVarConstTable(string var, int lineNumber) {
	string type = exprType(var);

	if (type == VARIABLE) {
		varTable->addVar(var);
	}
	else if (type == CONSTANT) {
		constTable->addToTable(convertNumToStr(lineNumber), var);
	}
}

//---------------------Create Call Table----------------------//
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
void DesignExtractor::processModUseTable() {
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

				// second child of the assignment is the right side, including right subtree
				useVarList = vector<string>();
				TNode* rightChild = curNode->getChildList().at(1);
				processRightBranchAST(rightChild, lineNumStr);

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

						// control var for if-then-else & while container
						string containerLeftChild = curPar->getChildList().at(0)->getValue();
						useTable->addToTable(containerLineNumStr, containerLeftChild);
						// add control Var to useVarList
						useVarList.push_back(containerLeftChild);

						addVarToContainer(containerLineNumStr, useVarList, USE_VAR);
					}
					else if (curParType == PROCEDURE) {
						string procName = curPar->getValue();
						modTable->addToTable(procName, modVar);

						addVarToContainer(procName, useVarList, USE_VAR);
					}

					curNode = curPar;
				}
			}
		}
	}

	processModUseTableWCall();
}

void DesignExtractor::processModUseTableWCall() {
	// process mod/use table for call type
	for (unsigned i = 0; i < ast.size(); i++) {
		AST* curAST = ast.at(i);

		for (unsigned j = 0; j < curAST->getTree().size(); j++) {
			TNode* curNode = curAST->getTree().at(j);
			string curNodeType = curNode->getType();

			if (curNodeType == CALL) {
				string callee = curNode->getValue();
				int callStmt = curNode->getLine();
				string callStmtStr = convertNumToStr(callStmt);

				updateModUseTableWCall(callStmtStr, callee);

				// go up to procedure & add all mod/use var to other container
				while (curNode->getType() != PROCEDURE) {
					TNode* curPar = curNode->getParent();
					string curParType = curPar->getType();

					vector<string> modVarListCur = modTable->getModified(callStmtStr);
					vector<string> useVarListCur = useTable->getUsed(callStmtStr);

					if (curParType == WHILE || curParType == IF) {
						int containerLineNum = curPar->getLine();
						string containerLineNumStr = convertNumToStr(containerLineNum);

						addVarToContainer(containerLineNumStr, modVarListCur, MODIFY_VAR);
						addVarToContainer(containerLineNumStr, useVarListCur, USE_VAR);
					}
					else if (curParType == PROCEDURE) {
						string procName = curPar->getValue();

						addVarToContainer(procName, modVarListCur, MODIFY_VAR);
						addVarToContainer(procName, useVarListCur, USE_VAR);
					}

					curNode = curPar;
				}
			}
		}
	}
}

//-----------------Smaller module for ModTable-------------------//
// add all mod/use var from a proc (callee) to its caller
// but doesnt guarantee the proc's mod/use table finishing because of container + stmt in each proc
void DesignExtractor::updateModUseTableWCall(string caller, string callee) {
	vector<string> calleeOfCalleeList = callTable->getCallees(callee);
	int idxCallee = procTable->indexOf(callee);

	// if callee's mod/useTable is completed then add(caller, varOfCallee)
	if (calleeOfCalleeList.size() > 0) {
		for (unsigned int i = 0; i < calleeOfCalleeList.size(); i++) {
			updateModUseTableWCall(callee, calleeOfCalleeList.at(i));
		}
	}

	// base case when callee doesnt call any other procedure
	// add all modVar, useVar of callee to caller's modTable, useTable
	vector<string> modVarList = modTable->getModified(callee);
	vector<string> useVarList2 = useTable->getUsed(callee);

	for (unsigned int i = 0; i < modVarList.size(); i++) {
		modTable->addToTable(caller, modVarList.at(i));
	}

	for (unsigned int i = 0; i < useVarList2.size(); i++) {
		useTable->addToTable(caller, useVarList2.at(i));
	}
}

//--------------Process Right Branch of the Assignment-------------//
void DesignExtractor::processRightBranchAST(TNode* rightNode, string lineNumStr) {
	string rightNodeType = rightNode->getType();

	if (rightNodeType == VARIABLE) {
		string rightNodeValue = rightNode->getValue();
		useVarList.push_back(rightNodeValue);
		useTable->addToTable(lineNumStr, rightNodeValue);
	}
	else {
		vector<TNode*> childList = rightNode->getChildList();
		if (childList.size() > 0) {
			for (unsigned int i = 0; i < childList.size(); i++) {
				TNode* processedNode = childList.at(i);
				processRightBranchAST(processedNode, lineNumStr);
			}
		}
	}
}


//---------------Smaller modules for Use Table---------------//
void DesignExtractor::addVarToContainer(string container, vector<string> varList, string type) {
	for (unsigned int i = 0; i < varList.size(); i++) {
		string iVar = varList.at(i);

		if (type == USE_VAR) {
			useTable->addToTable(container, iVar);
		}
		else if (type == MODIFY_VAR) {
			modTable->addToTable(container, iVar);
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

AST* DesignExtractor::buildSubtree(string pattern) {
	AST* subAST = new AST();
	rightSideText = pattern;

	TNode* curParent = new TNode();
	subAST->addToTree(curParent);
	processRightSideAssign(subAST, curParent, 0);

	return subAST;
}

//---------------------------Next Table-------------------------------//
bool DesignExtractor::processNextTable() {
	for (int i = 1; i <= progLine->numOfLines(); i++) {
		string type = progLine->getType(to_string(i));
		if (type == ASSIGN || type == CALL) {
			string next = followTable->getNext(to_string(i));
			if (next != NO_VALUE) {
				nextTable->addToTable(to_string(i), next);
			}
		}
		else if (type == WHILE) {
			vector<string> childList = parentTable->getChild(to_string(i));
			vector<int> cvtChildList;
			for (unsigned j = 0; j < childList.size(); j++) {
				cvtChildList.push_back(atoi(childList.at(j).c_str()));
			}
			int firstChild = cvtChildList.at(0);
			int lastChild = cvtChildList.at(0);
			for (unsigned j = 1; j < cvtChildList.size(); j++) {
				if (cvtChildList.at(j) < firstChild) {
					firstChild = cvtChildList.at(j);
				}
				if (cvtChildList.at(j) > lastChild) {
					lastChild = cvtChildList.at(j);
				}
			}
			nextTable->addToTable(to_string(i), to_string(firstChild));
			if (progLine->getType(to_string(lastChild)) != IF) {
				nextTable->addToTable(to_string(lastChild), to_string(i));
			}
			string next = followTable->getNext(to_string(i));
			if (next != NO_VALUE) {
				nextTable->addToTable(to_string(i), next);
			}
		}
		else if (type == IF) {
			vector<string> childList = parentTable->getChild(to_string(i));
			vector<string> firstChilds;
			for (unsigned j = 0; j < childList.size(); j++) {
				if (followTable->getPrev(childList.at(j)) == "") {
					firstChilds.push_back(childList.at(j));
				}
			}
			if (firstChilds.size() != 2) {	//check that only 2 childs that are then and else
				return false;
			}
			string thenChild;
			string elseChild;
			if (atoi(firstChilds.at(0).c_str()) < atoi(firstChilds.at(1).c_str())) {
				thenChild = firstChilds.at(0);
				elseChild = firstChilds.at(1);
			}
			else {
				thenChild = firstChilds.at(1);
				elseChild = firstChilds.at(0);
			}
			nextTable->addToTable(to_string(i), thenChild);
			nextTable->addToTable(to_string(i), elseChild);
			string lastThen = findLast(thenChild);
			string lastElse = findLast(elseChild);
			string next = nearestNext(to_string(i));
			if (next != NO_VALUE) {
				if (lastThen != NO_VALUE) {
					nextTable->addToTable(lastThen, next);
				}
				if (lastElse != NO_VALUE) {
					nextTable->addToTable(lastElse, next);
				}
			}
		}
	}
	return true;
}

string DesignExtractor::findLast(string line) {
	while (followTable->getNext(line) != NO_VALUE) {
		line = followTable->getNext(line);
	}
	if (progLine->getType(line) == ASSIGN || progLine->getType(line) == CALL
		|| progLine->getType(line) == WHILE) {
		return line;
	}
	return NO_VALUE;
}

string DesignExtractor::nearestNext(string line) {
	if (followTable->getNext(line) != NO_VALUE) {
		return followTable->getNext(line);
	}
	else if (progLine->getType(line) == WHILE) {
		return line;
	}
	else if (parentTable->getParent(line) == NO_VALUE) {
		return NO_VALUE;
	}
	else return nearestNext(parentTable->getParent(line));
}

void DesignExtractor::processSTable() {
	generateCallSTable();
	generateParentSTable();
	generateFollowSTable();
}

void DesignExtractor::generateCallSTable() {
	for (int i = 0; i < callTable->size(); i++) {
		queue<string> lineQ;
		string line = callTable->getTable().at(i).caller;
		lineQ.push(line);

		while (!lineQ.empty()) {
			string cur = lineQ.front();
			lineQ.pop();
			vector<string> callees = callTable->getCallees(cur);
			for (unsigned j = 0; j < callees.size(); j++) {
				lineQ.push(callees.at(j));
				callSTable->addToTable(line, callees.at(j));
			}
		}
	}
}

void DesignExtractor::generateFollowSTable() {
	for (int i = 0; i < followTable->size(); i++) {
		queue<string> lineQ;
		string line = followTable->getTable().at(i).prev;
		lineQ.push(line);

		while (!lineQ.empty()) {
			string cur = lineQ.front();
			lineQ.pop();
			string next = followTable->getNext(cur);
			if (next != NO_VALUE) {
				lineQ.push(next);
				followSTable->addToTable(line, next);
			}
		}
	}
}

void DesignExtractor::generateParentSTable() {
	for (int i = 0; i < parentTable->size(); i++) {
		queue<string> lineQ;
		string line = parentTable->getTable().at(i).lineNo;
		lineQ.push(line);

		while (!lineQ.empty()) {
			string cur = lineQ.front();
			lineQ.pop();
			vector<string> childList = parentTable->getChild(cur);
			for (unsigned j = 0; j < childList.size(); j++) {
				lineQ.push(childList.at(j));
				parentSTable->addToTable(line, childList.at(j));
			}
		}
	}
}

// Getter
CallTable* DesignExtractor::getCallTable() {
	return callTable;
}

FollowTable* DesignExtractor::getFollowTable() {
	return followTable;
}

ParentTable* DesignExtractor::getParentTable() {
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

ProgLine* DesignExtractor::getProgLine() {
	return progLine;
}

vector<AST*> DesignExtractor::getASTList() {
	return ast;
}

NextTable* DesignExtractor::getNextTable() {
	return nextTable;
}

// Smaller modules
string DesignExtractor::convertNumToStr(int stmtLstNumber) {
	ostringstream osstream;
	osstream << stmtLstNumber;
	string stmtLstNumText = osstream.str();

	return stmtLstNumText;
}

// Check the string is number or variable
// Return the type of VARIABLE or CONSTANT.
string DesignExtractor::exprType(string numText) {
	char firstDigit = numText.at(0);

	if (isdigit(firstDigit)) {
		return CONSTANT;
	}
	else {
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

void DesignExtractor::generateCallLine() {
	for (unsigned i = 0; i < ast.size(); i++) {
		vector<TNode*> tree = ast.at(i)->getTree();
		for (unsigned j = 0; j < tree.size(); j++) {
			if (tree.at(j)->getType() == CALL) {
				callLine->addToTable(to_string(tree.at(j)->getLine()), tree.at(j)->getValue());
			}
		}
	}
}