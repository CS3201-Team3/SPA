#include "stdafx.h"
#include "CppUnitTest.h"
#include "DesignExtractor.h"
#include "ModifyTable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(DesignExtractorTest)
	{
	public:

		TEST_METHOD(DECallTable) 
		{
			vector<string>code2 = { "procedureFirst{", "callSecond;", "callThird;","callFourth;}",
			"procedureSecond{", "y=y+3+x;", "callThird;}",
			"procedureThird{", "callFourth;}",
			"procedureFourth{", "y=1;}"};

			DesignExtractor ext2 = DesignExtractor(code2);
			CallTable* callTable = ext2.getCallTable();

			Assert::AreEqual(callTable->size(), 3);
			Assert::AreEqual(callTable->getCallees("First").at(0), (string) "Second");
			Assert::AreEqual(callTable->getCallees("First").at(1), (string) "Third");
			Assert::AreEqual(callTable->getCallees("First").at(2), (string) "Fourth");
			Assert::AreEqual(callTable->getCallees("Second").at(0), (string) "Third");

			Assert::AreEqual(callTable->getCallers("Third").at(0), (string) "First");
			Assert::AreEqual(callTable->getCallers("Third").at(1), (string) "Second");
			Assert::AreEqual(callTable->getCallers("Fourth").at(0), (string) "First");
			Assert::AreEqual(callTable->getCallers("Second").at(0), (string) "First");
		}

		TEST_METHOD(DEModUseTable)
		{
			vector <string> code = { "procedureFirst{","iftthen{", "k=2+u;}", "else{", "c=2+o;}}",
				"procedureSecond{", "x=0;", "whilei{" ,"y=x+2;", "x=3+m;", "callFirst;}" ,"x=y+z;}",
				"procedureThird{", "z=5;", "callSecond;", "callFourth;", "callFifth;}",
				"procedureFourth{", "e=e+1;", "f=1+w;}",
				"procedureFifth{", "r=r+5;}" };

			DesignExtractor ext = DesignExtractor(code);
			ModifyTable* modTable = ext.getModTable();
			UseTable* useTable = ext.getUseTable();
			
			Assert::AreEqual(modTable->isModified("First", "k"), true);
			Assert::AreEqual(modTable->isModified("First", "c"), true);
			Assert::AreEqual(modTable->isModified("First", "t"), false);

			// Test for if-the-else stmt
			Assert::AreEqual(modTable->isModified("1", "t"), false);
			Assert::AreEqual(modTable->isModified("1", "k"), true);
			Assert::AreEqual(modTable->isModified("1", "c"), true);
			Assert::AreEqual(modTable->isModified("2", "k"), true);
			Assert::AreEqual(modTable->isModified("3", "c"), true);

			Assert::AreEqual(modTable->isModified("Second", "k"), true);
			Assert::AreEqual(modTable->isModified("Second", "c"), true);
			Assert::AreEqual(modTable->isModified("Second", "i"), false);
			Assert::AreEqual(modTable->isModified("Second", "x"), true);
			Assert::AreEqual(modTable->isModified("Second", "y"), true);

			// test for while stmt
			Assert::AreEqual(modTable->isModified("5", "i"), false);
			Assert::AreEqual(modTable->isModified("5", "y"), true);
			Assert::AreEqual(modTable->isModified("5", "x"), true);
			// call in procedure Second
			Assert::AreEqual(modTable->isModified("8", "k"), true);
			Assert::AreEqual(modTable->isModified("8", "c"), true);
			// container contains call in procedure Second
			Assert::AreEqual(modTable->isModified("5", "k"), true);
			Assert::AreEqual(modTable->isModified("5", "c"), true);


			// Test for useTable
			Assert::AreEqual(useTable->isUsed("1", "t"), true);
			Assert::AreEqual(useTable->isUsed("First", "u"), true);
			Assert::AreEqual(useTable->isUsed("First", "o"), true);

			Assert::AreEqual(useTable->isUsed("Second", "i"), true);
			Assert::AreEqual(useTable->isUsed("Second", "x"), true);
			Assert::AreEqual(useTable->isUsed("Second", "m"), true);
			Assert::AreEqual(useTable->isUsed("Second", "y"), true);
			Assert::AreEqual(useTable->isUsed("Second", "z"), true);
			Assert::AreEqual(useTable->isUsed("Second", "t"), true);
			Assert::AreEqual(useTable->isUsed("Second", "u"), true);
			Assert::AreEqual(useTable->isUsed("Second", "o"), true);

			// Test for while loop with call 
			Assert::AreEqual(useTable->isUsed("5", "t"), true);
			Assert::AreEqual(useTable->isUsed("5", "u"), true);
			Assert::AreEqual(useTable->isUsed("5", "o"), true);

			Assert::AreEqual(useTable->isUsed("8", "t"), true);
			Assert::AreEqual(useTable->isUsed("8", "u"), true);
			Assert::AreEqual(useTable->isUsed("8", "o"), true);

			Assert::AreEqual(useTable->isUsed("Second", "t"), true);
			Assert::AreEqual(useTable->isUsed("Second", "u"), true);
			Assert::AreEqual(useTable->isUsed("Second", "o"), true);

			// Test for procedure Third
			Assert::AreEqual(useTable->isUsed("Third", "i"), true);
			Assert::AreEqual(useTable->isUsed("Third", "x"), true);
			Assert::AreEqual(useTable->isUsed("Third", "m"), true);
			Assert::AreEqual(useTable->isUsed("Third", "y"), true);
			Assert::AreEqual(useTable->isUsed("Third", "z"), true);

			// Test for call
			Assert::AreEqual(useTable->isUsed("11", "i"), true);
			Assert::AreEqual(useTable->isUsed("11", "x"), true);
			Assert::AreEqual(useTable->isUsed("11", "o"), true);
			Assert::AreEqual(useTable->isUsed("12", "w"), true);
			Assert::AreEqual(useTable->isUsed("13", "r"), true);

			// Procedure First
			Assert::AreEqual(useTable->isUsed("Third", "t"), true);
			Assert::AreEqual(useTable->isUsed("Third", "u"), true);
			Assert::AreEqual(useTable->isUsed("Third", "o"), true);

			
			Assert::AreEqual(useTable->isUsed("Third", "e"), true);
			Assert::AreEqual(useTable->isUsed("Third", "w"), true);
			Assert::AreEqual(useTable->isUsed("Third", "r"), true);
		}

		TEST_METHOD(DEModUseTable_Complicated) {
			vector<string>code2 = { "procedureABC{","i=1;","b=200;","c=a;",
				"ifathen{","whilebeta{","oSCar=1*beta+tmp;","whiletmp{",
				"oSCar=I-(k+j1k*chArlie);}}","whilex{","x=x+1;",
				"ifleftthen{","callSecond;","ifrightthen{","Romeo=Romeo-1;",
				"b=0;","c=delta+l*width+Romeo;}","else{","whilec{","callInit;",
				"c=c-1;}","x=x+1;}}","else{","callInit;}}}","else{",
				"callInit;}}",
				"procedureSecond{","ifxthen{","Y1=0;}","else{",
				"ifleftthen{","callInit;","ifrightthen{","Romeo=Romeo-1;",
				"b=0;","c=delta+l*width+Romeo;}","else{","whilec{",
				"callInit;","c=c-1;}","x=x+1;}}","else{","callInit;}}}",
				"procedureInit{","whilea{","x1=0;}};" };
			DesignExtractor ext = DesignExtractor(code2);

			ModifyTable* modTable = ext.getModTable();
			UseTable* useTable = ext.getUseTable();

			// Test ModifyTable
			Assert::AreEqual(modTable->isModified("8", "oSCar"), true);
			Assert::AreEqual(modTable->isModified("7", "oSCar"), true);
			Assert::AreEqual(modTable->isModified("5", "oSCar"), true);
			Assert::AreEqual(modTable->isModified("4", "oSCar"), true);
			Assert::AreEqual(modTable->isModified("ABC", "oSCar"), true);

			Assert::AreEqual(modTable->isModified("37", "x1"), true);
			Assert::AreEqual(modTable->isModified("36", "x1"), true);
			Assert::AreEqual(modTable->isModified("Init", "x1"), true);

			Assert::AreEqual(modTable->isModified("22", "x1"), true);
			Assert::AreEqual(modTable->isModified("17", "x1"), true);
			Assert::AreEqual(modTable->isModified("13", "x1"), true);
			Assert::AreEqual(modTable->isModified("11", "x1"), true);
			Assert::AreEqual(modTable->isModified("9", "x1"), true);
			Assert::AreEqual(modTable->isModified("4", "x1"), true);

			Assert::AreEqual(modTable->isModified("ABC", "x1"), true);

			//-----------------------Test Use Table--------------------------//
			Assert::AreEqual(useTable->isUsed("36", "a"), true);
			Assert::AreEqual(useTable->isUsed("Init", "a"), true);
			Assert::AreEqual(useTable->isUsed("4", "a"), true);
			Assert::AreEqual(useTable->isUsed("11", "a"), true);
			Assert::AreEqual(useTable->isUsed("17", "a"), true);
			Assert::AreEqual(useTable->isUsed("13", "a"), true);
		}

		TEST_METHOD(DEProcTable)
		{
			vector <string> code = { "procedureFirst{", "x=2;}",
				"procedureSecond{", "y=2;}",
				"procedureThird{", "z=2;}" };
			DesignExtractor ext = DesignExtractor(code);

			ProcTable* procTable = ext.getProcTable();
			Assert::AreEqual(procTable->size(), 3);
			Assert::AreEqual(procTable->indexOf("First"), 0);
			Assert::AreEqual(procTable->indexOf("Second"), 1);
			Assert::AreEqual(procTable->indexOf("Third"), 2);
		}

		TEST_METHOD(DEVarTable) {
			vector <string> code = { "procedureFirst{", "x=2;", "z=3;}",
				"procedureSecond{", "x=0;",  "i=5;" , "whilei{" ,"x=x+2+y;",
				"i=i+1;}" ,"z=z+x+i;", "y=z+2;", "x=x+y+z;}",
				"procedureThird{", "z=5;", "v=qwqewe+erew;}" };
			DesignExtractor ext = DesignExtractor(code);
			VarTable* varTable = ext.getVarTable();

			Assert::AreEqual(varTable->size(), 7);
			Assert::AreEqual(varTable->indexOf("x"), 0);
			Assert::AreEqual(varTable->indexOf("y"), 3);
			Assert::AreEqual(varTable->indexOf("z"), 1);
			Assert::AreEqual(varTable->indexOf("v"), 4);
			Assert::AreEqual(varTable->indexOf("i"), 2);
			Assert::AreEqual(varTable->indexOf("t"), -1);
			Assert::AreEqual(varTable->indexOf("qwqewe") != -1, true);
		}

		TEST_METHOD(DEConstTable) {
			vector <string> code = { "procedureFirst{", "x=2;", "z=3;}",
				"procedureSecond{", "x=0;",  "i=5;" , "whilei{" ,"x=x+2+y;",
				"i=i+1;}" ,"z=z+x+i;", "y=z+2;", "x=x+y+z;}",
				"procedureThird{", "z=5;", "v=z;}" };
			DesignExtractor ext = DesignExtractor(code);

			ConstTable* constTable = ext.getConstTable();
			Assert::AreEqual(constTable->size(), 8);
			Assert::AreEqual(constTable->getConst("1").at(0), (string) "2");
			Assert::AreEqual(constTable->getConst("1").size(), (unsigned) 1);
			Assert::AreEqual(constTable->getConst("2").at(0), (string) "3");
			Assert::AreEqual(constTable->getConst("9").at(0), (string) "2");
			Assert::AreEqual(constTable->getConst("11").at(0), (string) "5");
			Assert::AreEqual(constTable->getConst("13").size(), (unsigned) 0);
		}

		//---------------------------Test Build AST----------------------------//
		TEST_METHOD(TestExtBuildAST) {
			vector <string> code1 = { "procedureFirst{", "x=2;","ifxthen{", "z=3+z;}",
				"else{", "z=1;}}",
				"procedureSecond{", "i=5;", "callThird" , "whilei{" ,"x=x+2+y;}" ,"z=z+2+i;}",
				"procedureThird{", "z=5;", "v=z;}" };

			DesignExtractor ex1 = DesignExtractor(code1);
			vector<AST*> astList = ex1.getASTList();
			// check number of procedures
			Assert::AreEqual(astList.size(), (unsigned) 3);
			
			AST* proc1 = astList.at(0);
			Assert::AreEqual(proc1->getTree().size(), (unsigned)17);
			// test for 1st node: First:procedure
			Assert::AreEqual(proc1->getTree().at(0)->getType(), (string) "procedure");
			Assert::AreEqual(proc1->getTree().at(0)->getValue(), (string)"First");
			// test for 2nd node: stmtLst
			Assert::AreEqual(proc1->getTree().at(0)->getChildList().at(0)->getType(), STMTLST);
			Assert::AreEqual(proc1->getTree().at(1)->getType(), STMTLST);
			Assert::AreEqual(proc1->getTree().at(1)->getChildList().at(1)->getType(), IF);
			Assert::AreEqual(proc1->getTree().at(1)->getChildList().size(), (unsigned)2);
			// test for 3rd node: assign
			Assert::AreEqual(proc1->getTree().at(1)->getChildList().at(0)->getType(), ASSIGN);
			Assert::AreEqual(proc1->getTree().at(2)->getType(), (string) "assign");
			Assert::AreEqual(proc1->getTree().at(2)->getChildList().at(0)->getValue(), (string) "x");
			Assert::AreEqual(proc1->getTree().at(3)->getValue(), (string) "x");
			// test for 4th node: 2:constant
			Assert::AreEqual(proc1->getTree().at(2)->getChildList().at(1)->getValue(), (string) "2");
			Assert::AreEqual(proc1->getTree().at(4)->getValue(), (string) "2");
			// test for 5th node: if
			Assert::AreEqual(proc1->getTree().at(5)->getType(), IF);
			Assert::AreEqual(proc1->getTree().at(5)->getChildList().at(0)->getType(), VARIABLE);
			Assert::AreEqual(proc1->getTree().at(6)->getType(), VARIABLE);
			// test for 7th node: then stmtlst
			Assert::AreEqual(proc1->getTree().at(7)->getType(), STMTLST);
			Assert::AreEqual(proc1->getTree().at(7)->getValue(), THEN);
			// test for 8th, 9th node: assign
			Assert::AreEqual(proc1->getTree().at(8)->getType(), ASSIGN);
			Assert::AreEqual(proc1->getTree().at(8)->getChildList().at(0)->getType(), VARIABLE);
			Assert::AreEqual(proc1->getTree().at(9)->getType(), VARIABLE);
			Assert::AreEqual(proc1->getTree().at(9)->getValue(), (string) "z");
			//test for 12th node: z varible
			Assert::AreEqual(proc1->getTree().at(8)->getChildList().at(1)->getType(), PLUS_TEXT);
			Assert::AreEqual(proc1->getTree().at(12)->getType(), VARIABLE);
			Assert::AreEqual(proc1->getTree().at(12)->getValue(), (string) "z");
			// test for 10th: const 3
			Assert::AreEqual(proc1->getTree().at(10)->getValue(), (string) "3");
			Assert::AreEqual(proc1->getTree().at(11)->getChildList().at(0)->getValue(), (string) "3");
			// test for 11th: PLUS_TEXT
			Assert::AreEqual(proc1->getTree().at(11)->getType(), PLUS_TEXT);
			Assert::AreEqual(proc1->getTree().at(11)->getChildList().at(1)->getValue(), (string) "z");
			// test for 13th node: else:stmtlst
			Assert::AreEqual(proc1->getTree().at(13)->getValue(), ELSE);
			Assert::AreEqual(proc1->getTree().at(13)->getType(), STMTLST);
			Assert::AreEqual(proc1->getTree().at(5)->getChildList().at(2)->getValue(), ELSE);
			// test for 14th node: assign
			Assert::AreEqual(proc1->getTree().at(13)->getChildList().at(0)->getType(), ASSIGN);
			Assert::AreEqual(proc1->getTree().at(14)->getType(), ASSIGN);
			//test for 15th node: z: variable
			Assert::AreEqual(proc1->getTree().at(15)->getValue(), (string) "z");
			Assert::AreEqual(proc1->getTree().at(15)->getType(), VARIABLE);
			// test for 16th node: 1: constant
			Assert::AreEqual(proc1->getTree().at(14)->getChildList().at(1)->getValue(), (string) "1");
			Assert::AreEqual(proc1->getTree().at(14)->getChildList().at(1)->getType(), CONSTANT);
		}

		TEST_METHOD(TestExtBuildAST2) {
			vector<string> code = { "procedureFirst{", "x=c+1;}" };
			DesignExtractor ext = DesignExtractor(code);

			vector<AST*> astList = ext.getASTList();
			AST* ast = astList.at(0);

			Assert::AreEqual(ast->getTree().at(4)->getValue(), (string) "c");
			Assert::AreEqual(ast->getTree().at(5)->getChildList().size(), (size_t) 2);
			Assert::AreEqual(ast->getTree().at(5)->getChildList().at(0)->getValue(), (string) "c");
			Assert::AreEqual(ast->getTree().at(5)->getChildList().at(0)->getType(), VARIABLE);
			Assert::AreEqual(ast->getTree().at(5)->getChildList().at(1)->getValue(), (string) "1");
			Assert::AreEqual(ast->getTree().at(5)->getChildList().at(1)->getType(), CONSTANT);
		}

		TEST_METHOD(DE_AST_SimplestTestCase) {
			vector<string> code = { "procedureFirst{", "x=2;}" };

			DesignExtractor ext = DesignExtractor(code);
			vector<AST*> astList = ext.getASTList();

			AST* ast = astList.at(0);

			Assert::AreEqual(ast->getTree().at(3)->getValue(), (string) "x");
			Assert::AreEqual(ast->getTree().at(4)->getValue(), (string) "2");
			Assert::AreEqual(ast->getTree().at(4)->getType(), CONSTANT);
			Assert::AreEqual(ast->getTree().at(4)->getParent()->getType(), ASSIGN);

			//-------------------------------------------------------------------//
			vector<string> code1 = { "procedureFirst{", "x=t;}" };

			ext = DesignExtractor(code1);
			astList = ext.getASTList();

			ast = astList.at(0);

			Assert::AreEqual(ast->getTree().at(3)->getValue(), (string) "x");
			Assert::AreEqual(ast->getTree().at(4)->getValue(), (string) "t");
			Assert::AreEqual(ast->getTree().at(4)->getType(), VARIABLE);
			Assert::AreEqual(ast->getTree().at(4)->getParent()->getType(), ASSIGN);
		}

		TEST_METHOD(DE_AST_RightBranch) {
			vector<string> code = { "procedureFirst{", "x=y*e1*cs2*cs3;}" };

			DesignExtractor ext = DesignExtractor(code);
			AST* ast = ext.getASTList().at(0);

			Assert::AreEqual(ast->getTree().at(4)->getValue(), (string) "y");

			Assert::AreEqual(ast->getTree().at(5)->getChildList().size(), (unsigned) 2);
			Assert::AreEqual(ast->getTree().at(5)->getChildList().at(0)->getValue(), (string) "y");
			Assert::AreEqual(ast->getTree().at(5)->getChildList().at(1)->getValue(), (string) "e1");
			
			Assert::AreEqual(ast->getTree().at(6)->getValue(), (string) "e1");
			Assert::AreEqual(ast->getTree().at(7)->getType(), TIMES_TEXT);

			Assert::AreEqual(ast->getTree().at(7)->getChildList().size(), (unsigned) 2);
			Assert::AreEqual(ast->getTree().at(7)->getChildList().at(1)->getValue(), (string) "cs2");

			Assert::AreEqual(ast->getTree().at(8)->getValue(), (string) "cs2");
			Assert::AreEqual(ast->getTree().at(9)->getChildList().size(), (unsigned) 2);
			Assert::AreEqual(ast->getTree().at(9)->getChildList().at(1)->getValue(), (string) "cs3");

			Assert::AreEqual(ast->getTree().at(10)->getValue(), (string) "cs3");
		}

		TEST_METHOD(DE_AST_WithAllPlus) {
			vector<string> code = { "procedureFirst{", "x=x+1+y+z;}" };

			DesignExtractor ext = DesignExtractor(code);
			vector<AST*> astList = ext.getASTList();

			AST* ast = astList.at(0);

			Assert::AreEqual(ast->getTree().at(4)->getValue(), (string) "x");
			Assert::AreEqual(ast->getTree().at(4)->getParent()->getChildList().at(1)->getValue(), (string) "1");
			Assert::AreEqual(ast->getTree().at(4)->getParent()->getChildList().at(1)->getType(), CONSTANT);

			Assert::AreEqual(ast->getTree().at(5)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(6)->getValue(), (string) "1");
			Assert::AreEqual(ast->getTree().at(7)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(8)->getValue(), (string) "y");
			Assert::AreEqual(ast->getTree().at(9)->getType(), PLUS_TEXT);

			Assert::AreEqual(ast->getTree().at(10)->getValue(), (string) "z");
			Assert::AreEqual(ast->getTree().at(10)->getParent()->getChildList().at(0)->getChildList().at(1)->getValue(), (string) "y");
		}

		TEST_METHOD(DE_AST_Test1) {
			vector<string> code = { "procedureFirst{", "x=t+i*o*u+r;}" };

			DesignExtractor ext = DesignExtractor(code);
			vector<AST*> astList = ext.getASTList();

			AST* ast = astList.at(0);
			Assert::AreEqual(ast->getTree().at(4)->getValue(), (string) "t");
			Assert::AreEqual(ast->getTree().at(5)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(5)->getChildList().at(0)->getValue(), (string) "t");
			Assert::AreEqual(ast->getTree().at(5)->getChildList().at(1)->getType(), TIMES_TEXT);

			Assert::AreEqual(ast->getTree().at(6)->getValue(), (string) "i");
			Assert::AreEqual(ast->getTree().at(7)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(8)->getValue(), (string) "o");

			Assert::AreEqual(ast->getTree().at(9)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(9)->getChildList().at(0)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(9)->getChildList().at(1)->getValue(), (string) "u");

			Assert::AreEqual(ast->getTree().at(11)->getType(), PLUS_TEXT);
		}

		TEST_METHOD(DE_AST_Test2) {
			vector<string> code = { "procedureFirst{",
				"x=b+(a+c*d*e)+h*g;}" };

			DesignExtractor ext = DesignExtractor(code);
			vector<AST*> astList = ext.getASTList();

			AST* ast = astList.at(0);
			Assert::AreEqual(ast->getTree().at(4)->getValue(), (string) "a");

			Assert::AreEqual(ast->getTree().at(5)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(5)->getChildList().at(0)->getValue(), (string) "a");
			Assert::AreEqual(ast->getTree().at(5)->getChildList().at(1)->getType(), TIMES_TEXT);

			Assert::AreEqual(ast->getTree().at(6)->getValue(), (string) "c");
			Assert::AreEqual(ast->getTree().at(7)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(8)->getValue(), (string) "d");
			Assert::AreEqual(ast->getTree().at(9)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(10)->getValue(), (string) "e");
			Assert::AreEqual(ast->getTree().at(11)->getValue(), (string) "b");

			Assert::AreEqual(ast->getTree().at(12)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(12)->getChildList().at(1)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(12)->getChildList().at(0)->getValue(), (string) "b");

			Assert::AreEqual(ast->getTree().at(13)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(13)->getChildList().at(0)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(13)->getChildList().at(1)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(14)->getValue(), (string) "h");
			Assert::AreEqual(ast->getTree().at(15)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(16)->getValue(), (string) "g");
		}

		TEST_METHOD(DE_AST_MoreComplicated) {
			vector<string> code = { "procedureFirst{",
				"x=(b+c*d*e+h*g)+(((a+b*c)-(10*e))+9)-(d+h*g);}" };

			DesignExtractor ext = DesignExtractor(code);
			vector<AST*> astList = ext.getASTList();

			AST* ast = astList.at(0);

			Assert::AreEqual(ast->getTree().at(4)->getValue(), (string) "b");
			Assert::AreEqual(ast->getTree().at(4)->getParent()->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(5)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(5)->getChildList().at(1)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(5)->getChildList().at(0)->getValue(), (string) "b");
			Assert::AreEqual(ast->getTree().at(6)->getValue(), (string) "c");

			Assert::AreEqual(ast->getTree().at(7)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(8)->getValue(), (string) "d");
			Assert::AreEqual(ast->getTree().at(9)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(9)->getParent()->getType(), PLUS_TEXT);

			Assert::AreEqual(ast->getTree().at(10)->getValue(), (string) "e");
			Assert::AreEqual(ast->getTree().at(11)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(12)->getValue(), (string) "h");

			Assert::AreEqual(ast->getTree().at(13)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(11)->getChildList().at(0)->getChildList().at(0)->getType(), VARIABLE);
			Assert::AreEqual(ast->getTree().at(11)->getChildList().at(0)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(11)->getChildList().at(1)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(11)->getChildList().at(1)->getChildList().at(1)->getValue(), (string) "g");

			Assert::AreEqual(ast->getTree().at(13)->getParent()->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(13)->getChildList().at(0)->getValue(), (string) "h");
			Assert::AreEqual(ast->getTree().at(13)->getChildList().at(1)->getValue(), (string) "g");

			Assert::AreEqual(ast->getTree().at(14)->getValue(), (string) "g");

			Assert::AreEqual(ast->getTree().at(15)->getType(), VARIABLE);
			Assert::AreEqual(ast->getTree().at(15)->getValue(), (string) "a");

			Assert::AreEqual(ast->getTree().at(16)->getChildList().at(0)->getValue(), (string) "a");
			Assert::AreEqual(ast->getTree().at(16)->getChildList().at(1)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(16)->getType(), PLUS_TEXT);

			Assert::AreEqual(ast->getTree().at(17)->getType(), VARIABLE);
			Assert::AreEqual(ast->getTree().at(17)->getValue(), (string) "b");
			Assert::AreEqual(ast->getTree().at(17)->getParent()->getType(), TIMES_TEXT);

			Assert::AreEqual(ast->getTree().at(18)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(18)->getParent()->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(18)->getChildList().at(1)->getValue(), (string) "c");

			Assert::AreEqual(ast->getTree().at(19)->getType(), VARIABLE);
			Assert::AreEqual(ast->getTree().at(19)->getValue(), (string) "c");

			Assert::AreEqual(ast->getTree().at(20)->getType(), CONSTANT);
			Assert::AreEqual(ast->getTree().at(20)->getParent()->getType(), TIMES_TEXT);

			Assert::AreEqual(ast->getTree().at(21)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(21)->getParent()->getType(), MINUS_TEXT);

			Assert::AreEqual(ast->getTree().at(22)->getType(), VARIABLE);
			Assert::AreEqual(ast->getTree().at(22)->getValue(), (string) "e");
			Assert::AreEqual(ast->getTree().at(22)->getParent()->getType(), TIMES_TEXT);

			Assert::AreEqual(ast->getTree().at(23)->getType(), MINUS_TEXT);
			Assert::AreEqual(ast->getTree().at(23)->getChildList().at(0)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(23)->getChildList().at(1)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(23)->getChildList().at(0)->getChildList().at(0)->getValue(), (string) "a");
			Assert::AreEqual(ast->getTree().at(23)->getChildList().at(1)->getChildList().at(0)->getValue(), (string) "10");

			Assert::AreEqual(ast->getTree().at(24)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(24)->getChildList().at(1)->getValue(), (string) "9");
			Assert::AreEqual(ast->getTree().at(24)->getChildList().at(0)->getType(), MINUS_TEXT);

			Assert::AreEqual(ast->getTree().at(25)->getValue(), (string) "9");
			Assert::AreEqual(ast->getTree().at(25)->getType(), CONSTANT);

			Assert::AreEqual(ast->getTree().at(26)->getValue(), (string) "d");
			Assert::AreEqual(ast->getTree().at(26)->getType(), VARIABLE);
			Assert::AreEqual(ast->getTree().at(27)->getType(), PLUS_TEXT);

			Assert::AreEqual(ast->getTree().at(28)->getType(), VARIABLE);
			Assert::AreEqual(ast->getTree().at(28)->getValue(), (string) "h");
			Assert::AreEqual(ast->getTree().at(29)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(30)->getType(), VARIABLE);
			Assert::AreEqual(ast->getTree().at(30)->getValue(), (string) "g");

			Assert::AreEqual(ast->getTree().at(31)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(31)->getChildList().at(0)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(31)->getChildList().at(1)->getType(), PLUS_TEXT);

			Assert::AreEqual(ast->getTree().at(32)->getType(), MINUS_TEXT);
			Assert::AreEqual(ast->getTree().at(32)->getChildList().at(0)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(32)->getChildList().at(1)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(32)->getChildList().at(0)->getChildList().at(0)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(32)->getChildList().at(1)->getChildList().at(1)->getType(), TIMES_TEXT);

			Assert::AreEqual(ast->getTree().at(2)->getChildList().at(0)->getValue(), (string) "x");
			Assert::AreEqual(ast->getTree().at(2)->getChildList().at(1)->getType(), MINUS_TEXT);
		}

		TEST_METHOD(DE_AST_MixPlusTimes) {
			vector<string> code = { "procedureFirst{", "b=a+x*y+a;}" };

			DesignExtractor ext = DesignExtractor(code);
			vector<AST*> astList = ext.getASTList();

			AST* ast = astList.at(0);
			Assert::AreEqual(ast->getTree().at(4)->getValue(), (string) "a");
			Assert::AreEqual(ast->getTree().at(5)->getChildList().at(0)->getValue(), (string) "a");

			Assert::AreEqual(ast->getTree().at(5)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(6)->getValue(), (string) "x");

			Assert::AreEqual(ast->getTree().at(7)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast->getTree().at(5)->getChildList().at(1)->getType(), TIMES_TEXT);

			Assert::AreEqual(ast->getTree().at(8)->getValue(), (string) "y");
			Assert::AreEqual(ast->getTree().at(9)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast->getTree().at(10)->getValue(), (string) "a");

			//--------------------------------------------------------------//
			vector<string> code1 = { "procedureFirst{", "b=x*y+i*r;}" };
			DesignExtractor ext1 = DesignExtractor(code1);
			vector<AST*> astList1 = ext1.getASTList();

			AST* ast1 = astList1.at(0);
			Assert::AreEqual(ast1->getTree().at(4)->getValue(), (string) "x");
			Assert::AreEqual(ast1->getTree().at(5)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast1->getTree().at(6)->getValue(), (string) "y");

			Assert::AreEqual(ast1->getTree().at(7)->getType(), PLUS_TEXT);
			Assert::AreEqual(ast1->getTree().at(7)->getChildList().at(0)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast1->getTree().at(7)->getChildList().at(1)->getType(), TIMES_TEXT);

			Assert::AreEqual(ast1->getTree().at(8)->getValue(), (string) "i");
			Assert::AreEqual(ast1->getTree().at(9)->getType(), TIMES_TEXT);
			Assert::AreEqual(ast1->getTree().at(9)->getChildList().at(0)->getValue(), (string) "i");
			Assert::AreEqual(ast1->getTree().at(9)->getChildList().at(1)->getValue(), (string) "r");
			Assert::AreEqual(ast1->getTree().at(10)->getValue(), (string) "r");
		}

		//-------------------------------------------------------------------------------------//

		TEST_METHOD(DEParentTable) {
			vector <string> code = { "procedureFirst{", "x=2;", "z=3;}",
				"procedureSecond{", "x=0;",  "i=5;" , "whilei{" ,"x=x+2+y;",
				"i=i+1;}" ,"z=z+x+i;", "y=z+2;", "x=x+y+z;}",
				"procedureThird{", "z=5;", "v=z;}" };
			DesignExtractor ext = DesignExtractor(code);

			ParentTable* parentTable = ext.getParentTable();

			vector<string> childrenOfWhile = parentTable->getChild("5");
			Assert::AreEqual(childrenOfWhile.size(), (unsigned) 2);
			Assert::AreEqual(childrenOfWhile.at(0), (string) "6");
			Assert::AreEqual(childrenOfWhile.at(1), (string) "7");
		}

		TEST_METHOD(DEFollowTable) {
			vector <string> code = { "procedureFirst{", "x=2;", "z=3;}",
				"procedureSecond{", "x=0;",  "i=5;" , "whilei{" ,"x=x+2+y;",
				"i=i+1;}" ,"z=z+x+i;", "y=z+2;", "x=x+y+z;}",
				"procedureThird{", "z=5;", "v=z;}" };
			DesignExtractor ext = DesignExtractor(code);

			FollowTable* followTable = ext.getFollowTable();

			Assert::AreEqual(followTable->size(), 8);

			Assert::AreEqual(followTable->getNext("1"), (string) "2");
			Assert::AreEqual(followTable->getPrev("2"), (string) "1");
			Assert::AreEqual(followTable->getNext("2"), (string) "");

			Assert::AreEqual(followTable->getNext("3"), (string) "4");
			Assert::AreEqual(followTable->getPrev("4"), (string) "3");
			Assert::AreEqual(followTable->getNext("4"), (string) "5");
			Assert::AreEqual(followTable->getPrev("5"), (string) "4");

			Assert::AreNotEqual(followTable->getNext("5"), (string) "6");
			Assert::AreNotEqual(followTable->getPrev("6"), (string) "5");
			Assert::AreEqual(followTable->getNext("6"), (string) "7");
			Assert::AreEqual(followTable->getPrev("7"), (string) "6");

			Assert::AreEqual(followTable->getNext("5"), (string) "8");
			Assert::AreEqual(followTable->getPrev("8"), (string) "5");

			Assert::AreNotEqual(followTable->getNext("10"), (string) "11");
			Assert::AreNotEqual(followTable->getPrev("11"), (string) "10");
			Assert::AreEqual(followTable->getNext("11"), (string) "12");
		}

		TEST_METHOD(DEProgLine) {
			vector <string> code = { "procedureFirst{", "x=2;}",
				"procedureSecond{", "ifxthen{", "i=5;}", "else{", "x=4;}" , "whilei{" ,"x=x+2+y;}" ,"z=z+x+i;}",
				"procedureThird{", "z=5;}" };
			DesignExtractor ext = DesignExtractor(code);
			ProgLine* progLine = ext.getProgLine();

			Assert::AreEqual(progLine->getType((string)"1"), ASSIGN);
			Assert::AreEqual(progLine->getProcedure((string)"1"), (string) "First");

			Assert::AreEqual(progLine->getType((string)"2"), IF);
			Assert::AreEqual(progLine->getType((string)"3"), ASSIGN);
			Assert::AreEqual(progLine->getType((string)"4"), ASSIGN);
			Assert::AreEqual(progLine->getType((string)"5"), WHILE);
			Assert::AreEqual(progLine->getProcedure((string)"3"), (string) "Second");

			Assert::AreEqual(progLine->getProcedure((string)"8"), (string) "Third");
		}
		
		TEST_METHOD(TestExtBuildNextTable) {
			vector<string>code2 = {"procedureABC{","i=1;","b=200;","c=a;",
			"ifathen{","whilebeta{","oSCar=1*beta+tmp;","whiletmp{",
				"oSCar=I-(k+j1k*chArlie);}}","whilex{","x=x+1;",
			"ifleftthen{","callSecond;","ifrightthen{","Romeo=Romeo-1;",
			"b=0;","c=delta+l*width+Romeo;}","else{","whilec{","callInit;",
			"c=c-1;}","x=x+1;}}","else{","callInit;}}}","else{",
				"callInit;}}","procedureSecond{","ifxthen{","Y1=0;}","else{",
				"ifleftthen{","callInit;","ifrightthen{","Romeo=Romeo-1;",
					"b=0;","c=delta+l*width+Romeo;}","else{","whilec{",
					"callInit;","c=c-1;}","x=x+1;}}","else{","callInit;}}}",
				"procedureInit{","whilea{","x1=0;}};"};
			DesignExtractor ext = DesignExtractor(code2);
			NextTable* table = ext.getNextTable();
			ParentTable* pTable = ext.getParentTable();
			FollowTable* fTable = ext.getFollowTable();
			Assert::AreEqual(table->size(), 32);
					
			Assert::AreEqual(table->getTable().at(0).lineNo, (string)"1");
			Assert::AreEqual(table->getTable().at(0).nextStmts.size(), (unsigned)1);
			Assert::AreEqual(table->getTable().at(0).nextStmts.at(0), (string)"2");
			Assert::AreEqual(table->getTable().at(1).lineNo, (string) "2");
			Assert::AreEqual(table->getTable().at(1).nextStmts.size(), (unsigned)1);
			Assert::AreEqual(table->getTable().at(1).nextStmts.at(0), (string)"3");
			Assert::AreEqual(table->getTable().at(2).lineNo, (string) "3");
			Assert::AreEqual(table->getTable().at(2).nextStmts.size(), (unsigned)1);
			Assert::AreEqual(table->getTable().at(2).nextStmts.at(0), (string)"4");
			Assert::AreEqual(table->getTable().at(3).lineNo, (string) "4");
			Assert::AreEqual(table->getTable().at(3).nextStmts.size(), (unsigned)2);
			Assert::AreEqual(table->getTable().at(3).nextStmts.at(0), (string)"5");
			Assert::AreEqual(table->getTable().at(3).nextStmts.at(1), (string)"22");
			
			Assert::AreEqual(table->getTable().at(4).lineNo, (string) "5");
			Assert::AreEqual(table->getTable().at(4).nextStmts.size(), (unsigned)2);
			Assert::AreEqual(table->getTable().at(4).nextStmts.at(0), (string)"6");
			Assert::AreEqual(table->getTable().at(4).nextStmts.at(1), (string)"9");

			Assert::AreEqual(table->getTable().at(5).lineNo, (string) "7");

			Assert::AreEqual(table->getTable().at(5).nextStmts.size(), (unsigned)2);
			Assert::AreEqual(table->getTable().at(5).nextStmts.at(0), (string)"5");
			Assert::AreEqual(table->getTable().at(5).nextStmts.at(1), (string)"8");

			Assert::AreEqual(table->getTable().at(6).lineNo, (string) "6");
			Assert::AreEqual(table->getTable().at(6).nextStmts.size(), (unsigned)1);
			Assert::AreEqual(table->getTable().at(6).nextStmts.at(0), (string)"7");
			Assert::AreEqual(table->getTable().at(7).lineNo, (string) "8");
			Assert::AreEqual(table->getTable().at(7).nextStmts.size(), (unsigned)1);
			Assert::AreEqual(table->getTable().at(7).nextStmts.at(0), (string)"7");
			Assert::AreEqual(table->getTable().at(8).lineNo, (string) "9");
			Assert::AreEqual(table->getTable().at(8).nextStmts.size(), (unsigned)1);
			Assert::AreEqual(table->getTable().at(8).nextStmts.at(0), (string)"10");
			Assert::AreEqual(table->getTable().at(9).lineNo, (string) "10");
			Assert::AreEqual(table->getTable().at(9).nextStmts.size(), (unsigned)1);
			Assert::AreEqual(table->getTable().at(9).nextStmts.at(0), (string)"11");
			Assert::AreEqual(table->getTable().at(10).lineNo, (string) "11");
			Assert::AreEqual(table->getTable().at(10).nextStmts.size(), (unsigned)2);
			Assert::AreEqual(table->getTable().at(10).nextStmts.at(0), (string)"12");
			Assert::AreEqual(table->getTable().at(10).nextStmts.at(1), (string)"21");

			Assert::AreEqual(table->getTable().at(11).lineNo, (string) "21");
			Assert::AreEqual(table->getTable().at(11).nextStmts.size(), (unsigned)1);
			Assert::AreEqual(table->getTable().at(11).nextStmts.at(0), (string)"9");
			Assert::AreEqual(table->getTable().at(12).lineNo, (string) "12");
			Assert::AreEqual(table->getTable().at(12).nextStmts.size(), (unsigned)1);
			Assert::AreEqual(table->getTable().at(12).nextStmts.at(0), (string)"13");
			Assert::AreEqual(table->getTable().at(13).lineNo, (string) "13");
			Assert::AreEqual(table->getTable().at(13).nextStmts.size(), (unsigned)2);
			Assert::AreEqual(table->getTable().at(13).nextStmts.at(0), (string)"14");
			Assert::AreEqual(table->getTable().at(13).nextStmts.at(1), (string)"17");

			Assert::AreEqual(table->getTable().at(14).lineNo, (string) "16");
			Assert::AreEqual(table->getTable().at(14).nextStmts.size(), (unsigned)1);
			Assert::AreEqual(table->getTable().at(14).nextStmts.at(0), (string)"9");
			Assert::AreEqual(table->getTable().at(15).lineNo, (string) "20");
			Assert::AreEqual(table->getTable().at(15).nextStmts.size(), (unsigned)1);
			Assert::AreEqual(table->getTable().at(15).nextStmts.at(0), (string)"9");
			Assert::AreEqual(table->getTable().at(16).lineNo, (string) "14");
			Assert::AreEqual(table->getTable().at(16).nextStmts.size(), (unsigned)1);
			Assert::AreEqual(table->getTable().at(16).nextStmts.at(0), (string)"15");
			Assert::AreEqual(table->getTable().at(17).lineNo, (string) "15");
			Assert::AreEqual(table->getTable().at(17).nextStmts.size(), (unsigned)1);
			Assert::AreEqual(table->getTable().at(17).nextStmts.at(0), (string)"16");
			Assert::AreEqual(table->getTable().at(18).lineNo, (string) "17");
			Assert::AreEqual(table->getTable().at(18).nextStmts.size(), (unsigned)2);
			Assert::AreEqual(table->getTable().at(18).nextStmts.at(0), (string)"18");
			Assert::AreEqual(table->getTable().at(18).nextStmts.at(1), (string)"20");

			Assert::AreEqual(table->getTable().at(19).lineNo, (string) "19");
			Assert::AreEqual(table->getTable().at(19).nextStmts.size(), (unsigned)1);
			Assert::AreEqual(table->getTable().at(19).nextStmts.at(0), (string)"17");
			Assert::AreEqual(table->getTable().at(20).lineNo, (string) "18");
			Assert::AreEqual(table->getTable().at(20).nextStmts.size(), (unsigned)1);
			Assert::AreEqual(table->getTable().at(20).nextStmts.at(0), (string)"19");
		}
	};
}
