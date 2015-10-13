#include <test/ptutest.h>
#include "string.hh"
#include <stdio.h>

PTest(ProtString, TestConstruct)
{
	String a;
	EXPECT_EQ(0      , a.Length());
	EXPECT_NE(Null   , a.CString());
	EXPECT_STREQ(""  , a.CString());
	String b("hello");
	EXPECT_EQ(5           , b.Length());
	EXPECT_NE(Null        , b.CString());
	EXPECT_STREQ("hello"  , b.CString());
	String c = "abc";
	EXPECT_EQ(3        , c.Length());
	EXPECT_NE(Null     , c.CString());
	EXPECT_STREQ("abc" , c.CString());
	String d('X', 5);
	EXPECT_EQ(5           , d.Length());
	EXPECT_NE(Null        , d.CString());
	EXPECT_STREQ("XXXXX"  , d.CString());
	String e((Byte*)"Hello\0World\0!!", 15);
	EXPECT_EQ(15         , e.Length());
	EXPECT_NE(Null       , e.CString());
	EXPECT_STREQ("Hello" , e.CString());
	EXPECT_MEMEQ("Hello\0World\0!!" , e.CString(), 15);
	String f(c);
	EXPECT_EQ(3           , f.Length());
	EXPECT_NE(Null        , f.CString());
	EXPECT_STREQ("abc"    , f.CString());
	EXPECT_EQ(f.CString() , c.CString());
	String g = d;
	EXPECT_EQ(5           , g.Length());
	EXPECT_NE(Null        , g.CString());
	EXPECT_STREQ("XXXXX"  , g.CString());
	EXPECT_EQ(g.CString() , d.CString());
}

PTest(ProtString, TestNumConbox)
{
	{
		String a = String::FromInt(123);
		EXPECT_EQ(3        , a.Length());
		EXPECT_NE(Null     , a.CString());
		EXPECT_STREQ("123" , a.CString());
	}{
		String a = String::FromInt(-100);
		EXPECT_EQ(4          , a.Length());
		EXPECT_NE(Null       , a.CString());
		EXPECT_STREQ("-100" , a.CString());
	}{
		String a = String::FromInt(100000, 16);
		EXPECT_EQ(5          , a.Length());
		EXPECT_NE(Null       , a.CString());
		EXPECT_STREQ("186A0" , a.CString());
	}{
		String a = String::FromFloat(3.1415);
		EXPECT_LE(6           , a.Length());
		EXPECT_NE(Null        , a.CString());
		EXPECT_MEMEQ("3.1415" , a.CString(), 6);
	}{
		String a = String::FromFloat(3.141592, 2);
		EXPECT_EQ(4         , a.Length());
		EXPECT_NE(Null      , a.CString());
		EXPECT_STREQ("3.14" , a.CString());
	}{
		String a = String::FromFloat(3.141592, 4);
		EXPECT_EQ(6           , a.Length());
		EXPECT_NE(Null        , a.CString());
		EXPECT_STREQ("3.1415" , a.CString());
	}{
		String a = String::FromFloat(3.141592, 0);
		EXPECT_EQ(1      , a.Length());
		EXPECT_NE(Null   , a.CString());
		EXPECT_STREQ("3" , a.CString());
	}{
		String a = String::FromFloat(-3.1415, 3);
		EXPECT_EQ(6           , a.Length());
		EXPECT_NE(Null        , a.CString());
		EXPECT_STREQ("-3.141" , a.CString());
	}{
		String a = String::FromFloat(-3.1415, 7);
		EXPECT_EQ(10              , a.Length());
		EXPECT_NE(Null            , a.CString());
		EXPECT_STREQ("-3.1415000" , a.CString());
	}
}
