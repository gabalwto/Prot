#include <test/ptutest.h>
#include "defines.hh"
#include <stdio.h>

PTest(Basedef, TestSize)
{
	EXPECT_EQ(1, sizeof(Bool));

	EXPECT_EQ(1, sizeof(Int8));
	EXPECT_EQ(2, sizeof(Int16));
	EXPECT_EQ(4, sizeof(Int32));
	EXPECT_EQ(8, sizeof(Int64));

	EXPECT_EQ(1, sizeof(UInt8));
	EXPECT_EQ(2, sizeof(UInt16));
	EXPECT_EQ(4, sizeof(UInt32));
	EXPECT_EQ(8, sizeof(UInt64));

	EXPECT_EQ(1, sizeof(Char));
	EXPECT_EQ(1, sizeof(Byte));

#if (__PLATFORM_BITS_==64)
	EXPECT_EQ(8, sizeof(Long));
	EXPECT_EQ(8, sizeof(Size));
#else
	EXPECT_EQ(4, sizeof(Long));
	EXPECT_EQ(4, sizeof(Size));
#endif

	EXPECT_EQ(sizeof(Void*), sizeof(Size));
}

PTest(Basedef, TestBool)
{
	Bool a = true;
	Bool b = true;
	Bool c = false;

	EXPECT_EQ(a, a);
	EXPECT_EQ(a, b);
	EXPECT_NE(a, c);
	EXPECT_EQ(false, !a);
	EXPECT_EQ(true, !c);
	EXPECT_EQ(true,  a && a);
	EXPECT_EQ(true,  a && b);
	EXPECT_EQ(false, a && c);
	EXPECT_EQ(false, b && c);
	EXPECT_EQ(false, c && c);
	EXPECT_EQ(true,  a || a);
	EXPECT_EQ(true,  a || b);
	EXPECT_EQ(true,  a || c);
	EXPECT_EQ(true,  b || c);
	EXPECT_EQ(false, c || c);
}

PTest(Basedef, TestSign)
{
	{
		Int8 a = -4;
		Int8 b = 1;
		EXPECT_LT(a, b);
		UInt8 c = -4;
		UInt8 d = 1;
		EXPECT_GT(c, d);
	}
	{
		Int16 a = -4;
		Int16 b = 1;
		EXPECT_LT(a, b);
		UInt16 c = -4;
		UInt16 d = 1;
		EXPECT_GT(c, d);
	}
	{
		Int32 a = -4;
		Int32 b = 1;
		EXPECT_LT(a, b);
		UInt32 c = -4;
		UInt32 d = 1;
		EXPECT_GT(c, d);
	}
	{
		Int64 a = -4;
		Int64 b = 1;
		EXPECT_LT(a, b);
		UInt64 c = -4;
		UInt64 d = 1;
		EXPECT_GT(c, d);
	}
	{
		Long a = -4;
		Long b = 1;
		EXPECT_LT(a, b);
		ULong c = -4;
		ULong d = 1;
		EXPECT_GT(c, d);
	}
	{
		Char a = -4;
		Char b = 1;
		EXPECT_LT(a, b);
		Byte c = -4;
		Byte d = 1;
		EXPECT_GT(c, d);
	}
	{
		Float a = -4;
		Float b = 1;
		EXPECT_LT(a, b);
		Size c = -4;
		Size d = 1;
		EXPECT_GT(c, d);
	}
	{
		Bool a = 0;
		Bool b = 1;
		EXPECT_LT(a, b);
		EXPECT_GT(b, a);
		AnyPtr c = (void*)-4;
		AnyPtr d = (void*)1;
		EXPECT_GT(c, d);
		EXPECT_LT(d, c);
	}
}

#ifndef NOPTEMPLATE
PTest(Basedef, TestPtr)
{
	Int16 a = 0x0102;
	Ptr<Int16> p(&a);
	Ptr<Int8> p8;

	EXPECT_EQ(Null, p8);
	EXPECT_EQ(p8, Null);
	EXPECT_EQ(true, p8.IsNull());
	EXPECT_EQ(sizeof(p), sizeof(&a));
	EXPECT_NE(Null, p);
	EXPECT_NE(p, Null);
	EXPECT_EQ(p, &a);
	EXPECT_NE(1, *p);
	EXPECT_EQ(0x0102, *p);

	Int16** pp = (Int16**)&p;
	EXPECT_NE(Null, pp);
	EXPECT_EQ(&a, *pp);

	p8 = p.AsPtr<Int8>();
	EXPECT_EQ(3, p8[0] + p8[1]);
	EXPECT_EQ(false, p.IsNull());
	EXPECT_EQ(false, p8.IsNull());

	p8 = NullPtr;
	EXPECT_EQ(Null, p8);
	EXPECT_EQ(true, p8.IsNull());
}
#endif

PTest(Basedef, TestTypeConvert)
{
	{ Int8  a = -4; Int16 b = a; UInt16 c = a; EXPECT_LT(b, 0); EXPECT_GT(c, 0); }
	{ Int8  a = -4; Int32 b = a; UInt32 c = a; EXPECT_LT(b, 0); EXPECT_GT(c, 0); }
	{ Int8  a = -4; Int64 b = a; UInt64 c = a; EXPECT_LT(b, 0); EXPECT_GT(c, 0); }
	{ Int16 a = -4; Int32 b = a; UInt32 c = a; EXPECT_LT(b, 0); EXPECT_GT(c, 0); }
	{ Int16 a = -4; Int64 b = a; UInt64 c = a; EXPECT_LT(b, 0); EXPECT_GT(c, 0); }
	{ Int32 a = -4; Int64 b = a; UInt64 c = a; EXPECT_LT(b, 0); EXPECT_GT(c, 0); }

	{ UInt8  a = -4; Int16 b = a; UInt16 c = a; EXPECT_GT(b, 0); EXPECT_GT(c, 0); }
	{ UInt8  a = -4; Int32 b = a; UInt32 c = a; EXPECT_GT(b, 0); EXPECT_GT(c, 0); }
	{ UInt8  a = -4; Int64 b = a; UInt64 c = a; EXPECT_GT(b, 0); EXPECT_GT(c, 0); }
	{ UInt16 a = -4; Int32 b = a; UInt32 c = a; EXPECT_GT(b, 0); EXPECT_GT(c, 0); }
	{ UInt16 a = -4; Int64 b = a; UInt64 c = a; EXPECT_GT(b, 0); EXPECT_GT(c, 0); }
	{ UInt32 a = -4; Int64 b = a; UInt64 c = a; EXPECT_GT(b, 0); EXPECT_GT(c, 0); }

	{ Int8  a = -4; Float e = a; EXPECT_LT(e, 0); }
	{ Int16 a = -4; Float e = a; EXPECT_LT(e, 0); }
	{ Int32 a = -4; Float e = a; EXPECT_LT(e, 0); }
	{ Int64 a = -4; Float e = a; EXPECT_LT(e, 0); }
	{ Int8  a = 0;  Bool  b = a; EXPECT_EQ(false, b); }
	{ Int16 a = 0;  Bool  b = a; EXPECT_EQ(false, b); }
	{ Int32 a = 0;  Bool  b = a; EXPECT_EQ(false, b); }
	{ Int64 a = 0;  Bool  b = a; EXPECT_EQ(false, b); }
	{ Int8  a = -4; Bool  b = a; EXPECT_EQ(true, b); a = 4; b=a; EXPECT_EQ(true, b); }
	{ Int16 a = -4; Bool  b = a; EXPECT_EQ(true, b); a = 4; b=a; EXPECT_EQ(true, b); }
	{ Int32 a = -4; Bool  b = a; EXPECT_EQ(true, b); a = 4; b=a; EXPECT_EQ(true, b); }
	{ Int64 a = -4; Bool  b = a; EXPECT_EQ(true, b); a = 4; b=a; EXPECT_EQ(true, b); }

	{ Int8  a = -10; Int16 b = a; Int8  c = b; EXPECT_EQ(a, c); }
	{ Int8  a = -10; Int32 b = a; Int8  c = b; EXPECT_EQ(a, c); }
	{ Int8  a = -10; Int64 b = a; Int8  c = b; EXPECT_EQ(a, c); }
	{ Int16 a = -10; Int32 b = a; Int16 c = b; EXPECT_EQ(a, c); }
	{ Int16 a = -10; Int64 b = a; Int16 c = b; EXPECT_EQ(a, c); }
	{ Int32 a = -10; Int64 b = a; Int32 c = b; EXPECT_EQ(a, c); }
	{ Int8  a = -10; UInt16 b = a; Int8  c = b; EXPECT_EQ(a, c); }
	{ Int8  a = -10; UInt32 b = a; Int8  c = b; EXPECT_EQ(a, c); }
	{ Int8  a = -10; UInt64 b = a; Int8  c = b; EXPECT_EQ(a, c); }
	{ Int16 a = -10; UInt32 b = a; Int16 c = b; EXPECT_EQ(a, c); }
	{ Int16 a = -10; UInt64 b = a; Int16 c = b; EXPECT_EQ(a, c); }
	{ Int32 a = -10; UInt64 b = a; Int32 c = b; EXPECT_EQ(a, c); }

	{ Void* a = (Void*)0; AnyPtr b = a; Void* c = b; EXPECT_EQ(a, c); }
	{ Void* a = (Void*)-10; AnyPtr b = a; Void* c = b; EXPECT_EQ(a, c); }
}
