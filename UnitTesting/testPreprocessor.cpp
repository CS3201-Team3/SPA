#include <string>
#include <iostream>
#include <vector>
#include "stdafx.h"
#include "CppUnitTest.h"
#include "QueryPreprocessor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace TestPreprocessor
{
	TEST_CLASS(TestStartPreprocessor)
	{
	public:

		TEST_METHOD(no_such_no_pattern)
		{
			QueryPreprocessor pro0;
			string declare0 = "assign a1,a2;";
			string input0 = "Select a1";
			QueryTree* tree0 = pro0.startProcess(declare0, input0);

			Assert::AreEqual(true, tree0->getValidity());
			Assert::AreEqual((string)"a1", tree0->getResult()->getResult());
			return;
		}

		TEST_METHOD(such_pattern)
		{
			QueryPreprocessor pro001;
			string declare001 = "assign a1,a2,a3;while w;stmt s1,s2;variable v1;";
			string input001 = "Select w such that Modifies(a1,v1) pattern a2(_,_\"x\"_)";
			QueryTree* tree001 = pro001.startProcess(declare001, input001);

			Assert::AreEqual(true, tree001->getValidity());
			Assert::AreEqual((string)"w", tree001->getResult()->getResult());
			Assert::AreEqual((string)"Modifies", tree001->getSuchThat()->getSynonym());
			Assert::AreEqual((string)"a1", tree001->getSuchThat()->getFirstAttr());
			Assert::AreEqual((string)"v1", tree001->getSuchThat()->getSecondAttr());
			Assert::AreEqual((string)"a2", tree001->getPattern()->getSynonym());
			Assert::AreEqual((string)"_", tree001->getPattern()->getFirstAttr());
			Assert::AreEqual((string)"_\"x\"_", tree001->getPattern()->getSecondAttr());
			return;
		}

		TEST_METHOD(pattern_such)
		{
			QueryPreprocessor pro100;
			string declare100 = "assign a1,a2,a3;while w;stmt s1,s2;variable v1;";
			string input100 = "Select w pattern a2(_,_\"x\"_) such that Modifies(a1,v1)";
			QueryTree* tree100 = pro100.startProcess(declare100, input100);

			Assert::AreEqual(true, tree100->getValidity());
			Assert::AreEqual((string)"w", tree100->getResult()->getResult());
			Assert::AreEqual((string)"Modifies", tree100->getSuchThat()->getSynonym());
			Assert::AreEqual((string)"a1", tree100->getSuchThat()->getFirstAttr());
			Assert::AreEqual((string)"v1", tree100->getSuchThat()->getSecondAttr());
			Assert::AreEqual((string)"a2", tree100->getPattern()->getSynonym());
			Assert::AreEqual((string)"_", tree100->getPattern()->getFirstAttr());
			Assert::AreEqual((string)"_\"x\"_", tree100->getPattern()->getSecondAttr());
			return;
		}

		TEST_METHOD(such_that_number_attribute)
		{
			QueryPreprocessor pro2;
			string declare2 = "assign a1,a2,a3;while w;stmt s1,s2;variable v1;";
			string input2 = "Select w such that Modifies(a1,v1)";
			QueryTree* tree2 = pro2.startProcess(declare2, input2);
			Assert::AreEqual(true, tree2->getValidity());
			Assert::AreEqual((string)"w", tree2->getResult()->getResult());
			Assert::AreEqual((string)"Modifies", tree2->getSuchThat()->getSynonym());
			Assert::AreEqual((string)"a1", tree2->getSuchThat()->getFirstAttr());
			Assert::AreEqual((string)"v1", tree2->getSuchThat()->getSecondAttr());
			Assert::AreEqual((string)"", tree2->getPattern()->getSynonym());
			Assert::AreEqual((string)"", tree2->getPattern()->getFirstAttr());
			Assert::AreEqual((string)"", tree2->getPattern()->getSecondAttr());
		}

		TEST_METHOD(no_pattern)
		{
			QueryPreprocessor pro3;
			string declare3 = "assign a1,a2,a3;while w;stmt s1,s2;";
			string input3 = "Select w such that Modifies(3,\"x\") pattern a1(_,_\"x\"_)";
			QueryTree* tree3 = pro3.startProcess(declare3, input3);
			Assert::AreEqual(true, tree3->getValidity());
			Assert::AreEqual((string)"w", tree3->getResult()->getResult());
			Assert::AreEqual((string)"Modifies", tree3->getSuchThat()->getSynonym());
			Assert::AreEqual((string)"3", tree3->getSuchThat()->getFirstAttr());
			Assert::AreEqual((string)"\"x\"", tree3->getSuchThat()->getSecondAttr());
			Assert::AreEqual((string)"a1", tree3->getPattern()->getSynonym());
			Assert::AreEqual((string)"_", tree3->getPattern()->getFirstAttr());
			Assert::AreEqual((string)"_\"x\"_", tree3->getPattern()->getSecondAttr());
		}

		TEST_METHOD(no_such_that)
		{
			QueryPreprocessor pro4;
			string declare4 = "assign a1,a2,a3;while w;stmt s1,s2;variable v1;";
			string input4 = "Select w such that Modifies(a1,v1)";
			QueryTree* tree4 = pro4.startProcess(declare4, input4);
			Assert::AreEqual(true, tree4->getValidity());
			Assert::AreEqual((string)"w", tree4->getResult()->getResult());
			Assert::AreEqual((string)"Modifies", tree4->getSuchThat()->getSynonym());
			Assert::AreEqual((string)"a1", tree4->getSuchThat()->getFirstAttr());
			Assert::AreEqual((string)"v1", tree4->getSuchThat()->getSecondAttr());
			Assert::AreEqual((string)"", tree4->getPattern()->getSynonym());
			Assert::AreEqual((string)"", tree4->getPattern()->getFirstAttr());
			Assert::AreEqual((string)"", tree4->getPattern()->getSecondAttr());
		}

		TEST_METHOD(space)
		{
			QueryPreprocessor pro5;
			string declare5 = "assign   a1 ,  a2 ,  a3  ;  while  w ;   stmt s1  ,  s2    ;  variable  v1  ; ";
			string input5 = "  Select w such  that   Modifies  (  a1  , v1  )   pattern  a1(  _  ,  _ \" x \" _   )";
			QueryTree* tree5 = pro5.startProcess(declare5, input5);
			Assert::AreEqual(true, tree5->getValidity());
			Assert::AreEqual((string)"w", tree5->getResult()->getResult());
			Assert::AreEqual((string)"Modifies", tree5->getSuchThat()->getSynonym());
			Assert::AreEqual((string)"a1", tree5->getSuchThat()->getFirstAttr());
			Assert::AreEqual((string)"v1", tree5->getSuchThat()->getSecondAttr());
			Assert::AreEqual((string)"a1", tree5->getPattern()->getSynonym());
			Assert::AreEqual((string)"_", tree5->getPattern()->getFirstAttr());
			Assert::AreEqual((string)"_\"x\"_", tree5->getPattern()->getSecondAttr());
		}

		TEST_METHOD(such_that_attr_not_appear_in_declaration)
		{
			QueryPreprocessor pro6;
			string declare6 = "assign a1, a2, a3; while w; stmt s1, s2;variable v1;";
			string input6 = "Select w such that Modifies(bbbbbb, v1) pattern a2(_, _\"x\"_)";
			QueryTree* tree6 = pro6.startProcess(declare6, input6);
			Assert::AreEqual(false, tree6->getValidity());
		}

		TEST_METHOD(pattern_attr_not_appear_in_declaration)
		{
			QueryPreprocessor pro7;
			string declare7 = "assign a1, a2, a3; while w; stmt s1, s2;variable v1;";
			string input7 = "Select w such that Modifies(a1, v1) pattern a2(a4, _\"x\"_)";
			QueryTree* tree7 = pro7.startProcess(declare7, input7);
			Assert::AreEqual(false, tree7->getValidity());
		}

		TEST_METHOD(only_one_quatation_mark)
		{
			QueryPreprocessor pro8;
			string declare8 = "assign a1, a2, a3; while w; stmt s1, s2;variable v1;";
			string input8 = "Select w such that Modifies(a1, v1) pattern a2(a4,_x\"_)";
			QueryTree* tree8 = pro8.startProcess(declare8, input8);
			Assert::AreEqual(false, tree8->getValidity());
		}

		TEST_METHOD(wrong_pattern_synonym)
		{
			QueryPreprocessor pro9;
			string declare9 = "assign a1, a2, a3; while w; stmt s1, s2;variable v1;";
			string input9 = "Select w such that Modifies(a1,v1) pattern omo(_,_\"x\"_)";
			QueryTree* tree9 = pro9.startProcess(declare9, input9);
			Assert::AreEqual(false, tree9->getValidity());
		}

		TEST_METHOD(wrong_such_that_synonym)
		{
			QueryPreprocessor pro10;
			string declare10 = "assign a1, a2, a3; while w; stmt s1, s2;variable v1;";
			string input10 = "Select w such that blah(a1,v1) pattern a2(_,_\"x\"_)";
			QueryTree* tree10 = pro10.startProcess(declare10, input10);
			Assert::AreEqual(false, tree10->getValidity());
		}

		TEST_METHOD(such_that_lack_of_left_bracket)
		{
			QueryPreprocessor pro11;
			string declare11 = "assign a1, a2, a3; while w; stmt s1, s2;variable v1;";
			string input11 = "Select w such that Modifies a1,v1) pattern a2(_,_\"x\"_)";
			QueryTree* tree11 = pro11.startProcess(declare11, input11);
			Assert::AreEqual(false, tree11->getValidity());
		}

		TEST_METHOD(such_that_lack_of_comma)
		{
			QueryPreprocessor pro12;
			string declare12 = "assign a1, a2, a3; while w; stmt s1, s2;variable v1;";
			string input12 = "Select w such that Modifies(a1 v1) pattern a2(_,_\"x\"_)";
			QueryTree* tree12 = pro12.startProcess(declare12, input12);
			Assert::AreEqual(false, tree12->getValidity());
		}

		TEST_METHOD(pattern_lack_of_right_bracket)
		{
			QueryPreprocessor pro13;
			string declare13 = "assign a1, a2, a3; while w; stmt s1, s2;variable v1;";
			string input13 = "Select w such that Modifies(a1,v1) pattern a2(_,_\"x\"_ ";
			QueryTree* tree13 = pro13.startProcess(declare13, input13);
			Assert::AreEqual(false, tree13->getValidity());
		}

		TEST_METHOD(pattern_lack_of_comma)
		{
			QueryPreprocessor pro14;
			string declare14 = "assign a1, a2, a3; while w; stmt s1, s2;variable v1;";
			string input14 = "Select w such that Modifies(a1,v1) pattern a2(_ _\"x\"_)";
			QueryTree* tree14 = pro14.startProcess(declare14, input14);
			Assert::AreEqual(false, tree14->getValidity());
		}
	};
}