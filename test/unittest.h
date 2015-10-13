#ifndef __PTL_TEST_UTIL_H_
#define __PTL_TEST_UTIL_H_

#include <defines.hh>
#include <system/console.hh>

#include <string.h>
#include <wchar.h>

namespace Prot {
	typedef struct _PTU_TEST_CASE{
		_PTU_TEST_CASE * Next;
		const char* ClassName;
		const char* ModuleName;
		const char* TestName;
		void(*TestFunc)(_PTU_TEST_CASE* ptc);
		int SucceedCount;
		int FailureCount;
		int TotalCount;
		int TestResult;
		//int ReservedBk;
	} PTU_TEST_CASE;

	typedef struct {
		PTU_TEST_CASE* HeadCase;
		PTU_TEST_CASE* LastCase;
		//HANDLE TestHeap;
		int TestCount;
	} PTU_TEST_ROOT;

	/* Test Result Flag Bit */
	// This flag set to ignore test expression after in current case
	//   this flag is used to count total expressions
#define PTU_TRF_PASSIVE  1
#define PTU_TRF_BREAK    2
	// This flag set test result status to fail
#define PTU_TRF_FAILURE  4
	// This flag ignore test result even if PTU_TRF_FAILURE is set
#define PTU_TRF_IGNORE   8

	enum {
		STANDALONE = 1,
		TESTMEONLY = 0x80,
		UTEST_INIT = 0x100,
		UTEST_DONE = 0x200
	};
#define PTU_TRF_STANDALONE (STANDALONE << 16)

	/* Global Test Option Flags */
#define PTUF_SINGLE_THREAD 1

#define PTU_SC_CFPRINTF System::Console::CFPrintF

	class UnitTest
	{
	public:

		#define PTU_TEST_DIFF_FUNC_DEFINE(FName,TCop) \
		template <typename T>\
		static bool Test##FName(PTU_TEST_CASE* ptc, const char* file_, int line_, const char* expstr, const char* actstr, int expval, T* actval) \
		{\
			ptc->TotalCount ++;\
			if (ptc->TestResult & PTU_TRF_BREAK) return true;\
			bool r = (((T*)expval) TCop actval);\
			if (r) {\
				if (expval != 0) { PTU_SC_CFPRINTF(System::Console::DarkCyan, _sf_, "%s(%ld)\n", file_, line_);\
					PTU_SC_CFPRINTF(System::Console::DarkYellow, _sf_, "Pointer compare with Non-NULL int, for (%s) == %d\n", expstr, expval); }\
				AddSuccess(ptc); return r; } \
			PTU_SC_CFPRINTF(System::Console::DarkCyan, _sf_, "%s(%ld)\n", file_, line_);\
			if (expval != 0) { PTU_SC_CFPRINTF(System::Console::DarkYellow, _sf_, "Pointer compare with Non-NULL int, for (%s) == %d\n", expstr, expval); }\
			_pf_("You expect :  %s  "#TCop"  %s\n", actstr, expstr);\
			PrintFailure("Actually,", actstr, actval);\
			PrintFailure("   while ", expstr, expval);\
			AddFailure(ptc);\
			return r;\
		}\
		template <typename T1, typename T2>\
		static bool Test##FName(PTU_TEST_CASE* ptc, const char* file_, int line_, const char* expstr, const char* actstr, T1 expval, T2 actval) \
		{\
			ptc->TotalCount ++;\
			if (ptc->TestResult & PTU_TRF_BREAK) return true;\
			bool r = (expval TCop actval);\
			if (r) { AddSuccess(ptc); return r; } \
			PTU_SC_CFPRINTF(System::Console::DarkCyan, _sf_, "%s(%ld)\n", file_, line_);\
			_pf_("You expect :  %s  "#TCop"  %s\n", actstr, expstr);\
			PrintFailure("Actually,", actstr, actval);\
			PrintFailure("   while ", expstr, expval);\
			AddFailure(ptc);\
			return r;\
		}

		#define PTU_TEST_CMPR_FUNC_DEFINE(FName,TCop) \
		template <typename T1, typename T2>\
		static bool Test##FName(PTU_TEST_CASE* ptc, const char* file_, int line_, const char* expstr, const char* actstr, T1 expval, T2 actval) \
		{\
			ptc->TotalCount ++;\
			if (ptc->TestResult & PTU_TRF_BREAK) return true;\
			bool r = (expval TCop actval);\
			if (r) { AddSuccess(ptc); return r; } \
			PTU_SC_CFPRINTF(System::Console::DarkCyan, _sf_, "%s(%ld)\n", file_, line_);\
			_pf_("You expect :  %s  "#TCop"  %s\n", expstr, actstr);\
			PrintFailure("Actually,", expstr, expval);\
			PrintFailure("   while ", actstr, actval);\
			AddFailure(ptc);\
			return r;\
		}

		PTU_TEST_DIFF_FUNC_DEFINE(Equal, == )
		PTU_TEST_DIFF_FUNC_DEFINE(Noteq, != )
		PTU_TEST_CMPR_FUNC_DEFINE(Great, > )
		PTU_TEST_CMPR_FUNC_DEFINE(Lesst, < )
		PTU_TEST_CMPR_FUNC_DEFINE(GrtEq, >= )
		PTU_TEST_CMPR_FUNC_DEFINE(LesEq, <= )


		static inline int PTUSSTRCMP(const char* a, const char* b)
		{
			if (!a || !b)
				return a == b ? 0 : (!a ? -1 : 1);
			return strcmp(a, b);
		}

		static inline int PTUSSTRCMP(const wchar_t* a, const wchar_t* b)
		{
			if (!a || !b)
				return a == b ? 0 : (!a ? -1 : 1);
			return wcscmp(a, b);
		}

		template <typename T1, typename T2>
		static bool TestStreq(PTU_TEST_CASE* ptc, const char* file_, int line_, const char* expstr, const char* actstr, T1 expval, T2 actval)
		{
			ptc->TotalCount++;
			if (ptc->TestResult & PTU_TRF_BREAK) return true;
			bool r = (0 == PTUSSTRCMP(expval, actval));
			if (r) {
				AddSuccess(ptc);
				return r;
			}
			PTU_SC_CFPRINTF(System::Console::DarkCyan, _sf_, "%s(%ld)\n", file_, line_);
			_pf_("You expect STRING : %s\n  is equalant to  : %s\n", actstr, expstr);
			PrintFailure("Actually,", actstr, actval);
			PrintFailure("   while ", expstr, expval);
			AddFailure(ptc);
			return r;
		}

		template <typename T1, typename T2>
		static bool TestStrne(PTU_TEST_CASE* ptc, const char* file_, int line_, const char* expstr, const char* actstr, T1 expval, T2 actval)
		{
			ptc->TotalCount++;
			if (ptc->TestResult & PTU_TRF_BREAK) return true;
			bool r = (0 != PTUSSTRCMP(expval, actval));
			if (r) {
				AddSuccess(ptc);
				return r;
			}
			PTU_SC_CFPRINTF(System::Console::DarkCyan, _sf_, "%s(%ld)\n", file_, line_);
			_pf_("You expect STRING : %s\n  is differ from  : %s\n", actstr, expstr);
			PrintFailure("Actually,", actstr, actval);
			PrintFailure("   while ", expstr, expval);
			AddFailure(ptc);
			return r;
		}

		static inline int PTUSMEMCMP(const void* a, const void* b, int len)
		{
			if (!a || !b)
				return a == b ? 0 : (!a ? -1 : 1);
			return memcmp(a, b, len);
		}

		template <typename T1, typename T2>
		static bool TestMemeq(PTU_TEST_CASE* ptc, const char* file_, int line_, const char* expstr, const char* actstr, T1 expval, T2 actval, int length)
		{
			ptc->TotalCount++;
			if (ptc->TestResult & PTU_TRF_BREAK) return true;
			bool r = (0 == PTUSMEMCMP(expval, actval, length));
			if (r) {
				AddSuccess(ptc);
				return r;
			}
			PTU_SC_CFPRINTF(System::Console::DarkCyan, _sf_, "%s(%ld)\n", file_, line_);
			_pf_("You expect MEMORY : %s\n %2d bytes same as : %s\n", actstr, length, expstr);
			PrintFailmem("Actually,", actstr, actval, length);
			PrintFailmem("   while ", expstr, expval, length);
			AddFailure(ptc);
			return r;
		}

		template <typename T1, typename T2>
		static bool TestMemne(PTU_TEST_CASE* ptc, const char* file_, int line_, const char* expstr, const char* actstr, T1 expval, T2 actval, int length)
		{
			ptc->TotalCount++;
			if (ptc->TestResult & PTU_TRF_BREAK) return true;
			bool r = (0 != PTUSMEMCMP(expval, actval, length));
			if (r) {
				AddSuccess(ptc);
				return r;
			}
			PTU_SC_CFPRINTF(System::Console::DarkCyan, _sf_, "%s(%ld)\n", file_, line_);
			_pf_("You expect MEMORY : %s\n %2d bytes diff to : %s\n", actstr, length, expstr);
			PrintFailmem("Actually,", actstr, actval, length);
			PrintFailmem("   while ", expstr, expval, length);
			AddFailure(ptc);
			return r;
		}


		template <typename T>
		static bool TestTrue(PTU_TEST_CASE* ptc, const char* file_, int line_, const char* expstr, T expval)
		{
			ptc->TotalCount++;
			if (ptc->TestResult & PTU_TRF_BREAK) return true;
			if (expval) {
				AddSuccess(ptc);
				return true;
			}
			PTU_SC_CFPRINTF(System::Console::DarkCyan, _sf_, "%s(%ld)\n", file_, line_);
			_pf_("You expect : %s\n  is True.\n", expstr);
			PrintFailure("Actually,", expstr, expval);
			_pf_("   which is False.\n");
			AddFailure(ptc);
			return false;
		}

		template <typename T>
		static bool TestFalse(PTU_TEST_CASE* ptc, const char* file_, int line_, const char* expstr, T expval)
		{
			ptc->TotalCount++;
			if (ptc->TestResult & PTU_TRF_BREAK) return true;
			if (!(expval)) {
				AddSuccess(ptc);
				return true;
			}
			PTU_SC_CFPRINTF(System::Console::DarkCyan, _sf_, "%s(%ld)\n", file_, line_);
			_pf_("You expect : %s\n  is False.\n", expstr);
			PrintFailure("Actually,", expstr, expval);
			_pf_("   which is True.\n");
			AddFailure(ptc);
			return false;
		}

		static inline void AddSuccess(PTU_TEST_CASE* ptc)
		{
			ptc->SucceedCount++;
		}
		static inline void AddFailure(PTU_TEST_CASE* ptc)
		{
			ptc->FailureCount++;
		}

		static void PrintFailmem(const char* headstr, const char* exp, const void* val, int len);

		template <typename T>
		static inline void PrintFailure(const char* headstr, const char* exp, T& val)
		{
			_pf_("%s %s is %p\n", headstr, exp, (void*)val);
		}
		static inline void PrintFailure(const char* headstr, const char* exp, bool val)
		{
			_pf_("%s %s is %s\n", headstr, exp, val ? "true" : "false");
		}
		static inline void PrintFailure(const char* headstr, const char* exp, int val)
		{
			_pf_("%s %s is %ld\n", headstr, exp, val);
		}
		static inline void PrintFailure(const char* headstr, const char* exp, unsigned int val)
		{
			_pf_("%s %s is %lu\n", headstr, exp, val);
		}
		static inline void PrintFailure(const char* headstr, const char* exp, long val)
		{
			_pf_("%s %s is %ld\n", headstr, exp, val);
		}
		static inline void PrintFailure(const char* headstr, const char* exp, unsigned long val)
		{
			_pf_("%s %s is %lu\n", headstr, exp, val);
		}
		static inline void PrintFailure(const char* headstr, const char* exp, long long val)
		{
			_pf_("%s %s is %lld\n", headstr, exp, val);
		}
		static inline void PrintFailure(const char* headstr, const char* exp, unsigned long long val)
		{
			_pf_("%s %s is %llu\n", headstr, exp, val);
		}
		static inline void PrintFailure(const char* headstr, const char* exp, char val)
		{
			_pf_("%s %s is '%c' (0x%02x)\n", headstr, exp, val, (unsigned char)val);
		}
		static inline void PrintFailure(const char* headstr, const char* exp, unsigned char val)
		{
			_pf_("%s %s is %d (0x%02x)\n", headstr, exp, val, val);
		}
		static inline void PrintFailure(const char* headstr, const char* exp, char* val)
		{
			_pf_("%s %s is \"%s\"\n", headstr, exp, val);
		}
		static inline void PrintFailure(const char* headstr, const char* exp, const char* val)
		{
			_pf_("%s %s is \"%s\"\n", headstr, exp, val);
		}
		static inline void PrintFailure(const char* headstr, const char* exp, const wchar_t* val)
		{
			_pf_("%s %s is L\"%ls\"\n", headstr, exp, val);
		}
		static inline void PrintFailure(const char* headstr, const char* exp, void* val)
		{
			_pf_("%s %s is %p\n", headstr, exp, val);
		}
		static inline void PrintFailure(const char* headstr, const char* exp, double val)
		{
			_pf_("%s %s is %lf\n", headstr, exp, val);
		}
//#ifdef _WIN32
//		static inline void PrintFailure(const char* headstr, const char* exp, GUID val)
//		{
//			_pf_("%s %s is {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\n", headstr, exp,
//				val.Data1, val.Data2, val.Data3, val.Data4[0], val.Data4[1],
//				val.Data4[2], val.Data4[3], val.Data4[4], val.Data4[5], val.Data4[6], val.Data4[7]);
//		}
//#endif

		static const char* AddTestCase(void(*tfunc)(PTU_TEST_CASE*), const char* c, const char* m, const char* n, int flags = 0);

		static int RunTest();
		static Void InitTest();
		static Void DoneTest();
		static Void AddOption(const Char* key, const Char* val);

		static Void SetOutput(System::STDFILE outdev);

	private:
		//static AnyPtr Allocz(Size n);

		static PTU_TEST_ROOT _tr_;
		static System::STDFILE _sf_;
		static Int32(*_pf_)(const Char* fmt, ...);
		static UInt32 _nc_;		// count
		static UInt32 _fl_;		// flags
	};

#define PTC() (_ptc__)

#define T_EQ(expect, actual) UnitTest::TestEqual(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)
#define A_EQ(expect, actual) \
if (!UnitTest::TestEqual(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)) \
    PTC()->TestResult |= PTU_TRF_PASSIVE|PTU_TRF_BREAK;

#define T_NE(expect, actual) UnitTest::TestNoteq(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)
#define A_NE(expect, actual) \
if (!UnitTest::TestNoteq(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)) \
    PTC()->TestResult |= PTU_TRF_PASSIVE|PTU_TRF_BREAK;

#define T_GT(expect, actual) UnitTest::TestGreat(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)
#define A_GT(expect, actual) \
if (!UnitTest::TestGreat(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)) \
    PTC()->TestResult |= PTU_TRF_PASSIVE|PTU_TRF_BREAK;

#define T_LT(expect, actual) UnitTest::TestLesst(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)
#define A_LT(expect, actual) \
if (!UnitTest::TestLesst(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)) \
    PTC()->TestResult |= PTU_TRF_PASSIVE|PTU_TRF_BREAK;

#define T_GE(expect, actual) UnitTest::TestGrtEq(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)
#define A_GE(expect, actual) \
if (!UnitTest::TestGrtEq(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)) \
    PTC()->TestResult |= PTU_TRF_PASSIVE|PTU_TRF_BREAK;

#define T_LE(expect, actual) UnitTest::TestLesEq(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)
#define A_LE(expect, actual) \
if (!UnitTest::TestLesEq(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)) \
    PTC()->TestResult |= PTU_TRF_PASSIVE|PTU_TRF_BREAK;

#define T_STREQ(expect, actual) UnitTest::TestStreq(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)
#define A_STREQ(expect, actual) \
if (!UnitTest::TestStreq(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)) \
    PTC()->TestResult |= PTU_TRF_PASSIVE|PTU_TRF_BREAK;

#define T_STRNE(expect, actual) UnitTest::TestStrne(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)
#define A_STRNE(expect, actual) \
if (!UnitTest::TestStrne(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual)) \
    PTC()->TestResult |= PTU_TRF_PASSIVE|PTU_TRF_BREAK;

#define T_MEMEQ(expect, actual, length) UnitTest::TestMemeq(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual, length)
#define A_MEMEQ(expect, actual, length) \
if (!UnitTest::TestMemeq(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual, length)) \
    PTC()->TestResult |= PTU_TRF_PASSIVE|PTU_TRF_BREAK;

#define T_MEMNE(expect, actual, length) UnitTest::TestMemne(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual, length)
#define A_MEMNE(expect, actual, length) \
if (!UnitTest::TestMemne(PTC(), __FILE__, __LINE__, #expect, #actual, expect, actual, length)) \
    PTC()->TestResult |= PTU_TRF_PASSIVE|PTU_TRF_BREAK;

#define T_TRUE(expect) UnitTest::TestTrue(PTC(), __FILE__, __LINE__, #expect, expect)
#define A_TRUE(expect) \
if (!UnitTest::TestTrue(PTC(), __FILE__, __LINE__, #expect, expect)) \
    PTC()->TestResult |= PTU_TRF_PASSIVE|PTU_TRF_BREAK;

#define T_FALSE(expect) UnitTest::TestFalse(PTC(), __FILE__, __LINE__, #expect, expect)
#define A_FALSE(expect) \
if (!UnitTest::TestFalse(PTC(), __FILE__, __LINE__, #expect, expect)) \
    PTC()->TestResult |= PTU_TRF_PASSIVE|PTU_TRF_BREAK;



#define T_IGNORE() PTC()->TestResult |= PTU_TRF_IGNORE;

#define PTU_TEST_CLASS_NAME(module,name) "PTU_TEST_CASE_"#module"_"#name
#define PTU_TEST_CLASS_NAME_(module,name) PTU_TEST_CASE_##module##_##name


#if (__PLATFORM_OSYS_ == PTOS_WINDOWS)
#define PTEST_FUNC(name,args, ...) void name(PTU_TEST_CASE* PTC(), args, __VA_ARGS__)
#define PTEST_CALL(name,args, ...) name(PTC(), args, __VA_ARGS__)

#define PTest(module, name, ...)              \
class PTU_TEST_CLASS_NAME_(module,name) \
	{\
public:\
    static const char* test_class; \
    static void PTU_TestFunc(PTU_TEST_CASE*); \
	};\
const char* PTU_TEST_CLASS_NAME_(module,name) \
    ::test_class = UnitTest::AddTestCase(\
                        PTU_TEST_CLASS_NAME_(module,name)::PTU_TestFunc,\
                        PTU_TEST_CLASS_NAME(module,name),\
                        #module, #name, __VA_ARGS__);\
void PTU_TEST_CLASS_NAME_(module,name)\
    ::PTU_TestFunc(PTU_TEST_CASE* PTC())

#else
#define PTEST_FUNC(name,args, ...) void name(PTU_TEST_CASE* PTC(), args, ## __VA_ARGS__)
#define PTEST_CALL(name,args, ...) name(PTC(), args, ## __VA_ARGS__)

#define PTest(module, name, ...)              \
class PTU_TEST_CLASS_NAME_(module,name) \
	{\
public:\
    static const char* test_class; \
    static void PTU_TestFunc(PTU_TEST_CASE*); \
	};\
const char* PTU_TEST_CLASS_NAME_(module,name) \
    ::test_class = UnitTest::AddTestCase(\
                        PTU_TEST_CLASS_NAME_(module,name)::PTU_TestFunc,\
                        PTU_TEST_CLASS_NAME(module,name),\
                        #module, #name, ## __VA_ARGS__);\
void PTU_TEST_CLASS_NAME_(module,name)\
    ::PTU_TestFunc(PTU_TEST_CASE* PTC())

#endif

#define PTU_TEST_CLASS_NAME_X_(module,name,anot) PTU_TEST_CASE_##module##_##name##_##anot

#define PTestInit(module, name) namespace {\
class PTU_TEST_CLASS_NAME_X_(module, name, __I_n_i_t) \
	{\
public:\
	static const char* test_class; \
	static void PTU_TestFunc(PTU_TEST_CASE*); \
	}; } \
const char* PTU_TEST_CLASS_NAME_X_(module, name, __I_n_i_t) \
::test_class = UnitTest::AddTestCase(\
PTU_TEST_CLASS_NAME_X_(module, name, __I_n_i_t)::PTU_TestFunc, \
PTU_TEST_CLASS_NAME(module, name), \
#module, #name, UTEST_INIT);\
void PTU_TEST_CLASS_NAME_X_(module, name, __I_n_i_t)\
::PTU_TestFunc(PTU_TEST_CASE* PTC())

#define PTestDone(module, name) namespace {\
class PTU_TEST_CLASS_NAME_X_(module, name, __D_o_n_e) \
{\
public:\
	static const char* test_class; \
	static void PTU_TestFunc(PTU_TEST_CASE*); \
}; } \
const char* PTU_TEST_CLASS_NAME_X_(module, name, __D_o_n_e) \
::test_class = UnitTest::AddTestCase(\
PTU_TEST_CLASS_NAME_X_(module, name, __D_o_n_e)::PTU_TestFunc, \
PTU_TEST_CLASS_NAME(module, name), \
#module, #name, UTEST_DONE);\
void PTU_TEST_CLASS_NAME_X_(module, name, __D_o_n_e)\
::PTU_TestFunc(PTU_TEST_CASE* PTC())


} // namespace Prot

#endif
