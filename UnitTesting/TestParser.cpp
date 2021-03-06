#include "stdafx.h"
#include "CppUnitTest.h"
#include "Parser.h"
#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include <stdio.h>
#include <fstream>
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace TestPreprocessor
{
	TEST_CLASS(TestParser)
	{
	public:

		TEST_METHOD(Test_checkSyntax)
		{
			/*Parser p;
			vector<string> input, expected, actual;
			bool result;

			input = { "procedure", "p", "{","x=y+z;", "}" };
			expected = { "procedure", "p", "{","x=y+z;", "}" };
			actual = p.checkSyntax(input);
			result = p.isEqualVector(expected, actual);

			Assert::AreEqual(true, result);*/

			// intentional fail
			Assert::AreEqual(1, 2);

		}

		TEST_METHOD(Test_processStmtType)
		{
			Parser p;
			vector<string> input;
			int i;
			string type;
			string expected, actual;

			i = 0;
			type = "procedure";
			input = { "procedure p{","x=y+z;}" };
			expected = "";
			actual = p.processStmtType(input);
			Assert::AreEqual(expected, actual);


		}

		TEST_METHOD(Test_checkFirstLine)
		{
			Parser p;
			vector<string> input;
			string expected, actual;

			input = { "procedure", "p", "{","x=y+z;", "}" };
			expected = "";
			actual = p.checkFirstLine(input);
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Test_checkProcedure)
		{
			Parser p;
			vector<string> input;
			string expected, actual;

			input = { "procedure", "p", "{","x=y+z;", "}" };
			int startLine = 0;
			expected = "";
			actual = p.checkProcedure(input, startLine);
			Assert::AreEqual(expected, actual);

			input = { "procedure p{","x=y+z", "}" };
			startLine = 0;
			expected = "";
			actual = p.checkProcedure(input, startLine);
			Assert::AreEqual(expected, actual);

			input = { "procedure p {","x=y+z", "}" };
			startLine = 0;
			expected = "";
			actual = p.checkProcedure(input, startLine);
			Assert::AreEqual(expected, actual);

			input = { "procedure", "p{","x=y+z", "}" };
			startLine = 0;
			expected = "";
			actual = p.checkProcedure(input, startLine);
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Test_checkCall)
		{
			Parser p;
			vector<string> input;
			string expected, actual;

			input = { "procedure", "p", "{","x=y+z", "Call", "p", ";" ," }" };
			int startLine = 4;
			expected = "";
			actual = p.checkCall(input, startLine);
			Assert::AreEqual(expected, actual);

			input = { "procedure", "p", "{","x=y+z", "Call p;" ," }" };
			startLine = 4;
			expected = "";
			actual = p.checkCall(input, startLine);
			Assert::AreEqual(expected, actual);
		}


		TEST_METHOD(Test_checkAssign)
		{
			Parser p;
			string input;
			string expected, actual;

			input = "x = y +3 * 4;";
			expected = "";
			actual = p.checkAssign(input);
			Assert::AreEqual(expected, actual);

			input = "x=y+(4*3);";
			expected = "";
			actual = p.checkAssign(input);
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Test_checkIf)
		{
			Parser p;
			vector<string> input;
			string expected, actual;

			input = { "procedure", "p","if i then", "{","x=y+z", "}}" };
			int startLine = 2;
			expected = "";
			actual = p.checkIf(input, startLine);
			Assert::AreEqual(expected, actual);

			input = { "procedure", "if",  "i", "then", "{","x=y+z", "}}" };
			startLine = 1;
			expected = "";
			actual = p.checkIf(input, startLine);
			Assert::AreEqual(expected, actual);
		}


		TEST_METHOD(Test_checkElse)
		{
			Parser p;
			vector<string> input;
			string expected, actual;

			input = { "else {", "p","if i then", "{","x=y+z", "}}" };
			int startLine = 0;
			expected = "";
			actual = p.checkElse(input, startLine);
			Assert::AreEqual(expected, actual);

			input = { "{", "else{p","if i then}", "{","x=y+z", "}}" };
			startLine = 0;
			expected = "";
			actual = p.checkElse(input, startLine);
			Assert::AreEqual(expected, actual);
		}


		TEST_METHOD(Test_checkWhile)
		{
			Parser p;
			vector<string> input;
			string expected, actual;

			input = { "while p{","if i then", "{","x=y+z", "}}" };
			int startLine = 0;
			expected = "";
			actual = p.checkWhile(input, startLine);
			Assert::AreEqual(expected, actual);

			input = { "procedure", "p{","if i then", "{","x=y+z", "}}" };
			expected = "";
			actual = p.checkWhile(input, startLine);
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Test_isInteger)
		{
			Parser p;
			string strInteger = "123";
			string strString = "abc";
			string strAlphaNum = "a3c";

			Assert::AreEqual(true, p.isInteger(strInteger));
			Assert::AreEqual(false, p.isInteger(strString));
			Assert::AreEqual(false, p.isInteger(strAlphaNum));
		}

		TEST_METHOD(Test_isLetter)
		{
			Parser p;
			string strInteger = "123";
			string strString = "abc";
			string strAlphaNum = "a3c";

			Assert::AreEqual(false, p.isLetter(strInteger));
			Assert::AreEqual(true, p.isLetter(strString));
			Assert::AreEqual(false, p.isLetter(strAlphaNum));
		}

		TEST_METHOD(Test_isAlphaNumeric)
		{
			Parser p;
			string strInteger = "123";
			string strString = "abc";
			string strAlphaNum = "a3c";
			string strNotAlphaNum = "3_s";

			Assert::AreEqual(true, p.isAlphaNumeric(strInteger));
			Assert::AreEqual(true, p.isAlphaNumeric(strString));
			Assert::AreEqual(true, p.isAlphaNumeric(strAlphaNum));
			Assert::AreEqual(false, p.isAlphaNumeric(strNotAlphaNum));
		}

		TEST_METHOD(Test_existAlphaNumeric)
		{
			Parser p;
			string strInteger = "123";
			string strString = "abc";
			string strAlphaNum = "a3c";
			string strNotAlphaNum = "3_s";
			string strNoneAlphaNum = "__";

			Assert::AreEqual(true, p.existAlphaNumeric(strInteger));
			Assert::AreEqual(true, p.existAlphaNumeric(strString));
			Assert::AreEqual(true, p.existAlphaNumeric(strAlphaNum));
			Assert::AreEqual(true, p.existAlphaNumeric(strNotAlphaNum));
			Assert::AreEqual(false, p.existAlphaNumeric(strNoneAlphaNum));
		}

		TEST_METHOD(Test_isName)
		{
			Parser p;
			string validName = "a ";
			string invalidName = "1ab3c";
			string invalidName2 = "1$fdk";

			Assert::AreEqual(true, p.isName(validName));
			Assert::AreEqual(false, p.isName(invalidName));
			Assert::AreEqual(false, p.isName(invalidName2));
		}

		TEST_METHOD(Test_isNameOrConstant)
		{
			Parser p;
			string validName = "a23c";
			string validName2 = "23";
			string invalidName = "1ab3c";
			string invalidName2 = "1$fdk";

			Assert::AreEqual(true, p.isNameOrConstant(validName));
			Assert::AreEqual(true, p.isNameOrConstant(validName2));
			Assert::AreEqual(false, p.isNameOrConstant(invalidName));
			Assert::AreEqual(false, p.isNameOrConstant(invalidName2));
		}

		TEST_METHOD(Test_isExpression)
		{
			Parser p;
			string validExpr = " a2    *  (3  +  c)";
			string validExpr2 = "y+(4*3)";
			string invalidExpr = "+1ab3c";
			string invalidExpr2 = "1$fdk+";

			Assert::AreEqual(true, p.isExpression(validExpr));
			Assert::AreEqual(true, p.isExpression(validExpr2));
			Assert::AreEqual(false, p.isExpression(invalidExpr));
			Assert::AreEqual(false, p.isExpression(invalidExpr2));

		}

		TEST_METHOD(Test_isEqualVector)
		{
			Parser p;
			vector<string> input1, input2;
			bool result = true;

			input1 = { "a", "b" };
			input2 = { "   a   b", "x= y +  z * x" };

			result = p.isEqualVector(input1, input1);
			Assert::AreEqual(true, result);

			result = p.isEqualVector(input1, input2);
			Assert::AreEqual(false, result);
		}

		TEST_METHOD(Test_reomoveAllSpaces)
		{
			Parser p;
			vector<string> input, expected, actual;
			bool result = true;

			input = { "   a   b", "x= y +      z * x" };
			expected = { "ab", "x=y+z*x" };
			actual = p.removeAllSpaces(input);
			result = p.isEqualVector(expected, actual);

			Assert::AreEqual(true, result);
		}

		TEST_METHOD(Test_reomoveLineSpaces)
		{
			Parser p;
			string input, expected, actual;
			bool result = true;

			input = " x= y +      z * x";
			expected = "x=y+z*x";
			actual = p.removeLineSpaces(input);

			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Test_isStmtLst)
		{
			Parser p;
			int expected, actual;
			vector<string> input;

			input = { "{   a   b", "x= y +      z * x}" };
			expected = 1;
			actual = p.isStmtLst(input, 0);
			Assert::AreEqual(expected, actual);

			input = { "{   a   b", "x= y +      z * x","}" };
			expected = 2;
			actual = p.isStmtLst(input, 0);
			Assert::AreEqual(expected, actual);

			input = { "{   ","}" };
			expected = -1;
			actual = p.isStmtLst(input, 0);
			Assert::AreEqual(expected, actual);

			input = { "{   ", "x   =   y","}" };
			expected = 2;
			actual = p.isStmtLst(input, 0);
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Test_pairedCurlyBracketsPos)
		{
			Parser p;
			int expected, actual;
			vector<string> input;

			input = { "{   a   b", "x= y +      z * x}" };
			expected = 1;
			actual = p.pairedCurlyBracketsPos(input, 0);
			Assert::AreEqual(expected, actual);

			input = { "{   a   b", "x= y +      z * x","}" };
			expected = 2;
			actual = p.pairedCurlyBracketsPos(input, 0);
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Test_isPairedRoundBrackets)
		{
			Parser p;
			string input;
			bool expected, actual;
			input = " (y +      z * x)";

			expected = true;
			actual = p.isPairedRoundBrackets(input);
			Assert::AreEqual(expected, actual);

			input = " (y +      z * x";
			expected = false;
			actual = p.isPairedRoundBrackets(input);
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Test_checkAllBrackets)
		{
			Parser p;
			vector<string> input;
			bool expected, actual;

			input = { "{   a   b", "x= y +      z * x}", "{ a}" };
			expected = true;
			actual = p.checkAllBrackets(input);
			Assert::AreEqual(expected, actual);

			input = { " (y +      z * x))" };
			expected = false;
			actual = p.checkAllBrackets(input);
			Assert::AreEqual(expected, actual);

			input = { "{   a   b", "x= y +      z * x", " a}" };
			expected = true;
			actual = p.checkAllBrackets(input);
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Test_trimmedString)
		{
			Parser p;
			vector<string> input, expected, actual;
			bool result = true;

			input = { "   a   b", "   ","", "                 x= y +      z * x" };
			expected = { "a   b", "x= y +      z * x" };
			actual = p.trimmedList(input);
			result = p.isEqualVector(expected, actual);

			Assert::AreEqual(true, result);
		}

		TEST_METHOD(Test_trim)
		{
			Parser p;
			string input, actual, expected;

			input = "   a   b";
			expected = "a   b";
			actual = p.trim(input);
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(Test_checkNotEmptyLine)
		{
			Parser p;
			string input;

			bool actual, expected;

			input = "   ";
			expected = false;
			actual = p.checkNotEmptyLine(input);
			Assert::AreEqual(expected, actual);

			input = "   b";
			expected = true;
			actual = p.checkNotEmptyLine(input);
			Assert::AreEqual(expected, actual);
		}

	};
}