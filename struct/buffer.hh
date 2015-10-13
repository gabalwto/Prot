#ifndef __PROT_LIBRARY_STRUCT_BUFFER_H_
#define __PROT_LIBRARY_STRUCT_BUFFER_H_

#include "defines.hh"
#include "allocate.hh"
#include "memops.hh"
#include "bitops.hh"

#include <iostream>

namespace Prot {
	template <typename _ELE, Int32 _FLAG>
	class _PtBuf {
	public:
		inline _PtBuf() { _Init(0);	}
		inline _PtBuf(Size buf_size) { _Init(buf_size); }
		inline Void Resize(Size new_size) {
			if (_FLAG & 1)
			{
				if (new_size <= z_max) {
					if (new_size == y_max)
						return;
					y_max = new_size;
					if ((new_size + new_size) < z_max) {
						if (new_size == 0) {
							if (z_max > 15)
							{
								z_max = 3;
								_ReAlloc(4, 0);
							}
						} else {
							z_max = new_size | 3;
							_ReAlloc(z_max + 1, new_size);
						}
					}
				} else {
					z_max = new_size | 3;
					_ReAlloc(z_max + 1, y_max);
					y_max = new_size;
				}
				_Zend(new_size);
			}
			else
			{
				if (new_size <= z_max) {
					if (new_size == y_max)
						return;
					y_max = new_size;
					if ((new_size + new_size) < z_max) {
						if (new_size == 0) {
							if (z_max > 8)
							{
								z_max = 0;
								_Free();
							}
						} else {
							z_max = Bits::AlignUp(new_size, 4);
							_ReAlloc(z_max, new_size);
						}
					}
				} else {
					z_max = Bits::AlignUp(new_size, 4);
					_ReAlloc(z_max, y_max);
					y_max = new_size;
				}
			}
		}

		inline _ELE* Buf_Addr() { return &buf[0]; }
		inline Size Buf_Size() { return y_max; }
		inline Size Data_Len() { return len; }

		inline Size AddRef() { return ++z_ref; }
		inline Size DecRef() { return --z_ref; }
		inline Size Ref() { return z_ref; }

		/** when never use buf, call this to free internal ptr */
		inline Void Destroy() {
			z_max = y_max = 0;
			if (buf) _Free();
		}
		inline ~_PtBuf() {
#if __PT_DEBUG_
			if (Ref() != 0)
				System::Debug::Break();
#endif
			Destroy();
		}
		inline _PtBuf(const _PtBuf<_ELE, _FLAG>& other) {
			_Init(other.y_max);
			len = other.len;
			tag = other.tag;
			Memory::Copy(buf, other.buf, len);
		}
		inline _PtBuf& operator=(const _PtBuf<_ELE, _FLAG>& other) {
			if (this == &other)
				return *this;
			Resize(other.y_max);
			z_ref = 0;
			len = other.len;
			tag = other.tag;
		}
		inline Size Buf_Bound() { return z_max; }
	public:
		_ELE* buf;	// this pointer may changed by Resize call, do not set this value directly
		Size len;		// user variable for set data length
		Size tag;		// user data for setting additional data, Size can be convert to a pointer
	private:
		Size y_max;		// read only by use Buf_Size
		Size z_max;		// hide only use by Resize
		Size z_ref;		// read only by use Ref()

		inline Void _Free() { _ELE* p = buf; buf = NULL; Memory::Free(p); }
		inline Void _Alloc(Size _Cnt) { buf = Memory::Alloc(_Cnt*sizeof(_ELE)); }
		inline Void _ReAlloc(Size _New, Size _Cpy) { buf = Memory::Reloc(buf, _New*sizeof(_ELE), _Cpy*sizeof(_ELE)); }
		inline Void _Init(Size _Cnt)
		{
			len = z_ref = tag = 0;
			if (_FLAG & 1)
			{
				z_max = _Cnt | 3;		// Align _Cnt + 1 to Bound(4)
				_Alloc(z_max+1);
				_Zend(_Cnt);
			}
			else
			{
				z_max = Bits::AlignUp(_Cnt, 4);
				if (z_max)
					_Alloc(z_max);
			}
			y_max = _Cnt;
		}
		inline Void _Zend(Size _Idx)
		{
			if (_FLAG & 2)
				Memory::Fill(&buf[_Idx], sizeof(_ELE), 0);
			else
				buf[_Idx] = 0;	// TRY use (_FLAG | 2) if compile error
		}
	};

	/******************************* Buffer flags bits *******************************/
	// bit     mask          meaning
	//  0        1            set:safe fill last element , unset: not fill
	//  1        2            set:complex structure type element, or not
	// ohter : currently undefined !
	typedef _PtBuf<Char, 1> Buffer;
	typedef _PtBuf<WChar, 1> WCharBuffer;
}

#endif
