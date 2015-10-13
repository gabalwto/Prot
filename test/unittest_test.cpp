#include <test/ptutest.h>
#include <stdlib.h>
#include "system/console.hh"

PTest(UnitTest, Simple, TESTMEONLY)
{
	EXPECT_EQ(2, 1 + 1);
	EXPECT_NE(4, 1 + 2);
	EXPECT_GT(5, 2 + 2);
	EXPECT_LT(2, 1 + 2);
	EXPECT_GE(5, 2 + 2);
	EXPECT_LE(2, 1 + 2);
	EXPECT_GE(4, 2 + 2);
	EXPECT_LE(3, 1 + 2);
	char a[] = "";
	EXPECT_STREQ("", a);
	EXPECT_STRNE("a", a);
	char b[] = "b";
	EXPECT_STREQ("b", b);
	EXPECT_STRNE("a", b);
	char c[100];
	c[0] = 'a';
	c[1] = 'b';
	c[2] = 'c';
	c[3] = '\0';
	EXPECT_STREQ("abc", c);
	EXPECT_STRNE("c", c);
	EXPECT_MEMEQ("a", c, 1);
	EXPECT_MEMEQ("ab", c, 2);
	EXPECT_MEMEQ("abc", c, 4);
	EXPECT_MEMNE("a", c, 2);
}


static int org_1 = 0;
static int org_2 = 0;
int test_func_add(int& v)
{
	return ++v;
}
PTest(UnitTest, ExprExecOnce_OK, TESTMEONLY)
{
	int org = 0;
	EXPECT_EQ(1, test_func_add(org));
	EXPECT_EQ(1, org);
	EXPECT_EQ(1, org++);
	EXPECT_EQ(2, org);
	EXPECT_EQ(3, ++org);
	EXPECT_EQ(3, org);
	EXPECT_EQ(4, test_func_add(org));
	EXPECT_EQ(4, org);
}
PTest(UnitTest, ExprExecOnce_Fail, TESTMEONLY)
{
	EXPECT_EQ(1, test_func_add(org_1));
	EXPECT_EQ(3, test_func_add(org_1));
	EXPECT_EQ(1, org_2++);
	EXPECT_EQ(3, ++org_2);
	T_IGNORE();
}
PTest(UnitTest, ExprExecOnce_Result, STANDALONE|TESTMEONLY)
{
	EXPECT_EQ(2, org_1);
	EXPECT_EQ(2, org_2);
}



static int test_me_only_i = 0;
PTest(UnitTest, TestMeOnly_Add)
{
	test_me_only_i += 123;
	EXPECT_EQ(123, test_me_only_i);
}
PTest(UnitTest, TestMeOnly_Result, STANDALONE|TESTMEONLY)
{
	EXPECT_EQ(0, test_me_only_i);
}


PTEST_FUNC(my_test_func_add, int* refval)
{
	EXPECT_EQ(123, *refval);
	*refval = 321;
}

PTest(UnitTest, FuncCall, TESTMEONLY)
{
	int tval = 123;
	PTEST_CALL(my_test_func_add, &tval);
	EXPECT_EQ(321, tval);
}

static int test_init_i = 0;
class TestInitDoneClass {
public:
	~TestInitDoneClass()
	{
		if (test_init_i != 333)
		{
			System::Console::CFPrintF(System::Console::Red,
				System::StdErr, "=== Init Down Testing Failed ===\n");
			exit(-1);
		}
	}
};
static TestInitDoneClass tdic_obj;

PTestInit(UnitTest, InitTest)
{
	test_init_i = 111;
}
PTestDone(UnitTest, DoneTest)
{
	test_init_i = 333;
}

PTest(UnitTest, TestInit, TESTMEONLY)
{
	EXPECT_EQ(111, test_init_i);
}
