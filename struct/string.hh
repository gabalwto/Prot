#ifndef __PROT_LIBRARY_STRUCT_STRING_H_
#define __PROT_LIBRARY_STRUCT_STRING_H_

#include "buffer.hh"
#include <string.h>

namespace Prot {
	class String {
	public:
		inline String(){ _pb = new Buffer(); _pb->AddRef(); }
		inline String(const Char* s){new(this)String((const Byte*)s, strlen(s));}
		inline String(const Byte* s, Size l)
		{
			_pb = new Buffer(l);
			_pb->AddRef();
			if (l) {
				Memory::Copy(_pb->buf, s, l);
				_pb->len = l;
			}
		}
		inline String(const String& s)
		{
			_pb = s._pb;
			_pb->AddRef();
		}
		inline String& operator=(const String& other)
		{
			if (_pb == other._pb)
				return *this;
			if (_pb && !_pb->DecRef())
				delete _pb;
			_pb = other._pb;
			_pb->AddRef();
			return *this;
		}
		inline ~String()
		{
			if (!_pb->DecRef())
				delete _pb;
		}
		inline String(Char c, Size l) {
			_pb = new Buffer(l);
			_pb->AddRef();
			if (l) {
				Memory::Fill(_pb->buf, l, c);
				_pb->len = l;
			}
		}
		/** rdx (=radix) default(10) */
		inline static String FromInt(Long num, UInt8 rdx = 10)
		{
			if (rdx > 1 && rdx < 37)
			{
				static const Char ch_cmap[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
				Char buf[80];
				Char* p = buf;
				Bool neg = (num < 0);
				if (neg) num = -num;
				do {
					*p++ = ch_cmap[num % rdx];
					num /= rdx;
				} while (num);
				if (neg) *p++ = '-';
				Size len = p - buf;
				Buffer* pb = new Buffer(len);
				if (len) {
					pb->len = len;
					Char* pd = pb->buf;
					while (len--)
						*pd++ = *(--p);
					*pd = '\0';
				}
				return String(pb);
			}
			return String();
		}
		/** pre (=precision) default(8) */
		inline static String FromFloat(Float num, UInt8 pre = 8)
		{
			if (pre < 0)
				pre = 8;
			Char buf[80];
			Char* p = buf;
			Long num_int = (Long)num;
			Bool neg = (num < 0);
			if (neg) {
				num = -num;
			}
			num_int = (Long)num;
			num -= num_int;
			do {
				*p++ = (num_int % 10) + '0';
				num_int /= 10;
			} while (num_int);
			if (neg) *p++ = '-';
			Size len = p - buf;
			Size buf_len = pre == 0 ? len : len + pre + 1;
			Buffer* pb = new Buffer(buf_len);
			pb->len = buf_len;
			Char* pd = pb->buf;
			if (len) {
				while (len--)
					*pd++ = *(--p);
			}
			if (pre) {
				*pd++ = '.';
				while (pre--) {
					num *= 10;
					Char ch = (Char)num;
					*pd++ = ch + '0';
					num -= ch;
				}
			}
			*pd = '\0';
			return String(pb);
		}
		//inline static String FromFormat(const Char* fmt, ...)
		//{
		//}
		inline Void Upper()
		{
			Size n = _pb->len;
			Buffer* b = _TrySepNewSizeEmpty(n);
			Char* pd = b->buf;
			Char* pr = _pb->buf;
			for (Size i = 0; i < n; i++)
			{
				pd[i] = (pr[i] >= 'a' && pr[i] <= 'z') ? (pr[i] - ' ') : pr[i];
			}
			b->len = n;
			_TrySepRefDecAndPtrSet(b);
		}
		inline const String SubStr(Size start, Size count)
		{
			Size n = _pb->len;
			if (start == 0 && count == n)
				return *this;
			Buffer* p = new Buffer(count);
			String s(p);
			Memory::Copy(s._pb->buf, &_pb->buf[start], count);
			return s;
		}
		inline const Char* CString() { return _pb->buf; }
		inline Size Length() { return _pb->len; }
	private:
		inline String(Buffer* buf) { _pb = buf, _pb->AddRef(); }
		Buffer* _TrySepNewSizeEmpty(Size newsize)
		{
			if (_pb->Ref() < 2)
				return _pb;
			return new Buffer(newsize);
		}
		Void _TrySepRefDecAndPtrSet(Buffer* pnew)
		{
			if (!_pb->DecRef())
				delete _pb;
			pnew->AddRef();
			_pb = pnew;
		}
	private:
		Buffer *_pb;
	};
}

#endif
