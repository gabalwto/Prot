#include <test/ptutest.h>

#include "allocate.hh"
#include "memops.hh"
#include <stdio.h>
#include <string.h>

PTest(BaseMemory, TestAlloc)
{
	AnyPtr p;
	p = Memory::Alloc(0);
	EXPECT_EQ(Null, p);
	p = Memory::Alloc(100);
	EXPECT_NE(Null, p);
	p = Memory::Reloc(p, 200);
	EXPECT_NE(Null, p);
	p = Memory::Reloc(p, 50);
	EXPECT_NE(Null, p);
	p = Memory::Reloc(p, 0);
	EXPECT_EQ(Null, p);
	p = Memory::Reloc(p, 10);
	EXPECT_NE(Null, p);
	Memory::Free(p);
}

#ifndef NOPTEMPLATE
PTest(BaseMemory, TestAlloc_Template)
{
	Ptr<Int16> p16 = Memory::AllocT<Int16>();
	EXPECT_NE(Null, p16);
	EXPECT_EQ(sizeof(Int16), sizeof(*p16));
	p16 = Memory::RelocT(p16, 10);
	EXPECT_NE(Null, p16);
	Int16* p = &p16[0];
	for (int i = 0; i < 10; i++)
	{
		*p++ = i;
	}
	for (int j = 0; j < 10; j++)
	{
		EXPECT_EQ(j, p16[j]);
	}
}
#endif

PTest(BaseMemory, TestMemops)
{
	//Ptr<Int16> p16 = Memory::Alloc<Int16>(10);
	Int16* p16 = (Int16*)Memory::Alloc(10 * sizeof(Int16));
	Memory::Fill(&p16[0], 10 * sizeof(Int16), 0x5C);
	for (int i = 0; i < 10; i++)
	{
		EXPECT_EQ(0x5c5c, p16[i]);
	}
	const Char* str = "0123456789";
	int len = strlen(str);
	Memory::Copy(&p16[0], (AnyPtr)str, len+1);
	Char* pc = (Char*)p16;
	for (int i = 0; i < len; i++)
	{
		EXPECT_EQ('0' + i, pc[i]);
	}
	EXPECT_EQ(0, strcmp(str, pc));
	EXPECT_EQ(0, Memory::Comp((AnyPtr)str, pc, len + 1));
	EXPECT_NE(Null, Memory::Find(pc, 10 * sizeof(Int16), '0'));
	EXPECT_NE(Null, Memory::Find(pc, 10 * sizeof(Int16), '2'));
	EXPECT_NE(Null, Memory::Find(pc, 10 * sizeof(Int16), '9'));
	EXPECT_NE(Null, Memory::Find(pc, 10 * sizeof(Int16), '\0'));
	EXPECT_NE(Null, Memory::Find(pc, 10 * sizeof(Int16), 0x5c));
	EXPECT_EQ(Null, Memory::Find(pc, 10 * sizeof(Int16), 0xfc));
}

#ifndef NOPTEMPLATE
PTest(BaseMemory, TestMemops_template)
{
	Ptr<Int16> p16 = Memory::AllocT<Int16>(10);
	Int16 fch = 0x0102;
	Memory::FillT(p16, 10, fch);
	for (int i = 0; i < 10; i++)
	{
		EXPECT_EQ(0x0102, p16[i]);
	}
	Ptr<Char> p8 = "0918273645";
	Memory::CopyT(p16, p8.AsPtr<Int16>(), 5);
	for (int j = 0; j < 5; j++)
	{
		EXPECT_NE(fch, p16[j]);
		Ptr<Int8> p = p16.AsPtr<Int8>();
		EXPECT_EQ(0x69, p[0] + p[1]);
		fch = p16[j];
	}
	EXPECT_EQ(0, Memory::CompT(p16, p8.AsPtr<Int16>(), 5));
	EXPECT_NE(0, Memory::CompT(p16, p8.AsPtr<Int16>(), 10));
	Char ch = '0';
	EXPECT_NE(Null, Memory::FindT(p16.AsPtr<Char>(), 10 * sizeof(Int16), ch));
	EXPECT_NE(Null, Memory::FindT(p16.AsPtr<Char>(), 10 * sizeof(Int16), '2'));
	EXPECT_NE(Null, Memory::FindT(p16.AsPtr<Char>(), 10 * sizeof(Int16), '5'));
	EXPECT_NE(Null, Memory::FindT(p16.AsPtr<Char>(), 10 * sizeof(Int16), (Char)0x01));
	EXPECT_NE(Null, Memory::FindT(p16.AsPtr<Char>(), 10 * sizeof(Int16), '\x02'));
	EXPECT_EQ(Null, Memory::FindT(p16.AsPtr<Char>(), 10 * sizeof(Int16), (Char)0xfc));
	EXPECT_NE(Null, Memory::FindT<Int16>(p16, 10, 0x0102));
	EXPECT_EQ(Null, Memory::FindT<Int16>(p16, 10, 0x0000));
}
#endif
