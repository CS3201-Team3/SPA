#ifndef HandlerStruct_H
#define HandlerStruct_H
#include <string>
#include <vector>

using namespace std;

typedef struct RSEntry_t {
	string firstAtt;
	string secondAtt;
	int synCount;
	bool extension;
	vector<string> vec;
	vector < pair<string, vector<string>>> table;
	vector <pair<string, string>> ssTable;
} RSEntry;

typedef struct attEntry_t {
	string att;
	vector<int> reAtt;
	vector<int> reClause;
} attEntry;

#endif