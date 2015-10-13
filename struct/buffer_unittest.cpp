#include <test/ptutest.h>
#include "buffer.hh"

PTest(ProtBuffer, TestAllocate)
{
	Buffer p;
	EXPECT_NE(Null, p.Buf_Addr());
	EXPECT_EQ(0, p.Buf_Size());
	EXPECT_GE(p.Buf_Bound(), p.Buf_Size());
	EXPECT_EQ('\0', p.Buf_Addr()[p.Buf_Size()]);
	p.Resize(10);
	EXPECT_NE(Null, p.Buf_Addr());
	EXPECT_EQ(10, p.Buf_Size());
	EXPECT_GE(p.Buf_Bound(), p.Buf_Size());
	EXPECT_EQ('\0', p.Buf_Addr()[p.Buf_Size()]);
	p.Resize(0);
	EXPECT_NE(Null, p.Buf_Addr());
	EXPECT_EQ(0, p.Buf_Size());
	EXPECT_GE(p.Buf_Bound(), p.Buf_Size());
	EXPECT_EQ('\0', p.Buf_Addr()[p.Buf_Size()]);
}

PTest(ProtBuffer, TestRefops)
{
	Buffer p;
	EXPECT_EQ(0, p.Ref());
	EXPECT_EQ(1, p.AddRef());
	EXPECT_EQ(1, p.Ref());
	EXPECT_EQ(2, p.AddRef());
	EXPECT_EQ(3, p.AddRef());
	EXPECT_EQ(3, p.Ref());
	EXPECT_EQ(2, p.DecRef());
	EXPECT_EQ(2, p.Ref());
	EXPECT_EQ(1, p.DecRef());
	EXPECT_EQ(1, p.Ref());
	EXPECT_EQ(0, p.DecRef());
	EXPECT_EQ(0, p.Ref());
}

PTest(ProtBuffer, TestConstruct)
{
	Buffer p1(10);
	Memory::Copy(p1.buf, "123456", 6);
	p1.len = 6;
	p1.AddRef();
	p1.AddRef();
	EXPECT_EQ(10, p1.Buf_Size());
	EXPECT_EQ(2, p1.Ref());
	EXPECT_EQ('\0', p1.Buf_Addr()[p1.Buf_Size()]);
	Buffer p2(p1);
	EXPECT_EQ(10, p2.Buf_Size());
	EXPECT_EQ(6, p2.len);
	EXPECT_EQ(0, p2.Ref());
	EXPECT_NE(Null, p2.Buf_Addr());
	EXPECT_NE(p1.buf, p2.buf);		// buf can only be refed by pointer
	EXPECT_EQ(0, Memory::Comp(p1.buf, p2.buf, 6));
	EXPECT_EQ('\0', p2.Buf_Addr()[p2.Buf_Size()]);
	Buffer p3 = p1;
	EXPECT_EQ(10, p3.Buf_Size());
	EXPECT_EQ(6, p3.len);
	EXPECT_EQ(0, p3.Ref());
	EXPECT_NE(Null, p3.Buf_Addr());
	EXPECT_NE(p1.buf, p3.buf);		// buf can only be refed by pointer
	EXPECT_EQ(0, Memory::Comp(p1.buf, p3.buf, 6));
	EXPECT_EQ('\0', p3.Buf_Addr()[p3.Buf_Size()]);
	p1.DecRef();
	p1.DecRef();
}
