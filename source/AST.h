#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include "TNode.h"

using namespace std;
class AST {
private:
	vector<TNode* > tree;
public:
	AST();
	~AST();
	vector<TNode* > getTree();		//return tree
	bool addToTree(TNode* node);	//add a node  to AST
	void makeChild(TNode* par, TNode* child);	//add a child to a node in AST
	void makeParent(TNode* child, TNode* par);	//add parent to a node in AST
	void printTree();	//printout tree
	vector<TNode* > findChild(TNode* parent);	//return all child of a node
	TNode* findParent(TNode* child);	//return parent of a node
	TNode* findNode(TNode* node); //return pointer to this node in AST 

	vector<TNode* > getLine(int line);//return a list of node* with same line number
	string getProcedure();		//return procedure name of tree
	
	vector<string> findSubtree(AST* subtree, bool contains_);		//find all lines in  tree that contains subtree	
															//case pattern a(_,"x+y")
															//case pattern a(_,_"x+y"_)

	vector <TNode* > getType(string type);//get all type (:stmtList,:while, :assign, :var,:plus,..)
	vector<int> getAssign();			//return all assignments in an ast

	static bool compare(TNode* root1, TNode* root2);		//use for pattern
	static bool compareSubtree(TNode* root1, TNode* root2);
	vector<TNode*> getStmtNode();
};

#endif