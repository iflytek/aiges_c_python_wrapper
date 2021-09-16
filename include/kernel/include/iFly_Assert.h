/** 
 * @file	iFly_Assert.h
 * @brief	iFly使用的Assert定义
 * 
 *  Assert宏定义，这个assert使用起来比较方便，可以打印较多的信息
 * 
 * @author	jdyu
 * @version	1.0
 * @date	2004-6-22
 * 
 * @see		
 * 
 * <b>History:</b><br>
 * <table>
 *  <tr> <th>Version	<th>Date		<th>Author	<th>Notes</tr>
 *  <tr> <td>1.0		<td>2004-6-22	<td>jdyu	<td>Create this file</tr>
 * </table>
 * 
 */
#ifndef __SP_ASSERT_H__
#define __SP_ASSERT_H__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <errno.h>

#ifndef _MSC_VER
	#include <wchar.h>
#endif

/** 
 * @class	SP_ASSERT
 * 
 * @brief	SP使用的ASSERT宏具体定义
 *     
 *     具体用法说明:
 *     1. SP_ASSERT、SP_VERIFY的用法和一般的assert相同。
 * 		  例如 SP_ASSERT( 0 );
 * 		       SP_ASSERT( 0 && "assert text" );
 *     2. SP_ASSERTS、SP_VERIFYS可以携带一个变参的文本参数，用法如下：
 * 		  例如 SP_ASSERTS( expr, ("Error to execute command %s, code = %d", cmd, ret) );
 * 
 * @author	jdyu
 * @date	2004-6-22
 * 
 * @see		ACE_ASSERT
 * 
 * @par 备注：在非windows操作系统下，直接使用ACE的ASSERT宏
 * 
 */
#ifndef SP_ASSERT
#  if defined(_DEBUG)
#    if defined(_WIN32) || defined(ACE_WIN32)
#      include <crtdbg.h>
#      define	SP_ASSERT//					_ASSERTE
#      define	SP_VERIFY//					_ASSERTE
#	   if _MSC_VER < 1300		/* VC 6 */
#        define	SP_ASSERTS(expr, msg)		SP_ASSERT
#        define	SP_ASSERTSW(expr, msg)		SP_ASSERT
#        define	SP_VERIFYS					SP_ASSERTS
#      elif _MSC_VER >= 1400	/* VC 8 */
#		 if !defined(_UNICODE)
#          define	SP_ASSERTS(expr, msg)	//if ( !(expr) ) { _SP_Assert_ ast; ( 1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, ast.assert_msg##msg)) || (_CrtDbgBreak(), 0); }
#          define	SP_VERIFYS				//SP_ASSERTS
#		 else /* UNICODE */
#          define	SP_ASSERTS(expr, msg)	//if ( !(expr) ) { _SP_Assert_ ast; ( 1 != _SP_Assert_::dbg_report(_CRT_ASSERT, __FILE__, __LINE__, NULL, ast.assert_msg##msg)) || (_CrtDbgBreak(), 0); }
#          define	SP_VERIFYS				//SP_ASSERTS
#		 endif /* UNICODE */
#          define	SP_ASSERTSW(expr, msg)	//if ( !(expr) ) { _SP_Assert_ ast; ( 1 != _SP_Assert_::dbg_report(_CRT_ASSERT, __FILE__, L##__LINE__, NULL, ast.assert_msg##msg)) || (_CrtDbgBreak(), 0); }
#	   else  /* VC 7 */
#        define	SP_ASSERTS(expr, msg)		if ( !(expr) ) { _SP_Assert_ ast; _ASSERT_BASE( (expr), ast.assert_msg##msg ); }
#        define	SP_VERIFYS					SP_ASSERTS
#	   endif /* _MSC_VER < 1300 */
#    else   /* NOT WIN32 */
#      include <assert.h>
#      define SP_ASSERT(expr)				if ( !(expr) ) AssertFailed( #expr, __FILE__, __LINE__);
#      define SP_VERIFY						SP_ASSERT
#      define SP_ASSERTS(expr, msg)			if ( !(expr) ) { _SP_Assert_ ast; AssertFaileds( #expr, __FILE__, __LINE__, ast.assert_msg msg ); }
#      define SP_VERIFYS					SP_ASSERTS
#    endif /* NOT WIN32 */
#  else   /* _DEBUG */
#    define	SP_ASSERT(x)					(0)
#    define	SP_VERIFY(x)					(x)
#    define SP_ASSERTS(expr, msg)			SP_ASSERT(expr)
#    define SP_ASSERTSW(expr, msg)			SP_ASSERT(expr)
#    define SP_VERIFYS						SP_ASSERTS
#  endif  /* _DEBUG */
#endif  /* SP_ASSERT */

//////////////////////////////////////////////////////////////////////////
/// 生成assert信息的辅助类
//////////////////////////////////////////////////////////////////////////
class _SP_Assert_
{
public:
	_SP_Assert_(void)
		{ 
			msg_buf_[0] = '\0';
			wsg_buf_[0] = L'\0';
		}

	const char * assert_msg(const char* fmt, ...)
		{
			va_list arg_list;
			va_start(arg_list, fmt);
			#if _MSC_VER >= 1400
			vsprintf_s(msg_buf_, sizeof msg_buf_, fmt, arg_list);
			#elif defined(_MSC_VER)
			vsprintf(msg_buf_, fmt, arg_list);
			#else	// other compile
			vsprintf(msg_buf_, fmt, arg_list);
			#endif
			va_end(arg_list);
			return msg_buf_;
		}
	const wchar_t * assert_msg(const wchar_t* fmt, ...)
		{
			va_list arg_list;
			va_start(arg_list, fmt);
			#if _MSC_VER >= 1400
			vswprintf_s(wsg_buf_, sizeof wsg_buf_ / sizeof wsg_buf_[0], fmt, arg_list);
			#elif defined(_MSC_VER)
			vswprintf(wsg_buf_, fmt, arg_list);
			#else	// other compile
			vswprintf(wsg_buf_, sizeof wsg_buf_ / sizeof wsg_buf_[0], fmt, arg_list);
			#endif
			va_end(arg_list);
			return wsg_buf_;
		}

#if _MSC_VER >= 1400 && defined(_DEBUG)
	static int __cdecl dbg_report(int rt, const char * fn, int ln, const char * mn, const char * fmt, ...)
		{
		char msg[1024];
		va_list arg_list;
		va_start(arg_list, fmt);
		vsprintf_s(msg, sizeof msg, fmt, arg_list);
		va_end(arg_list);
		return _CrtDbgReport(rt, fn, ln, mn, msg);
		}
	static int __cdecl dbg_report(int rt, const char * fn, int ln, const wchar_t * mn, const wchar_t * fmt, ...)
		{
		wchar_t msg[1024];
		va_list arg_list;
		va_start(arg_list, fmt);
		vswprintf_s(msg, sizeof msg / sizeof msg[0], fmt, arg_list);
		va_end(arg_list);

		wchar_t fnw[266]; fnw[0] = 0;
		mbstowcs_s(0, fnw, sizeof fnw / sizeof fnw[0], fn, strlen(fn));
		return _CrtDbgReportW(rt, fnw, ln, mn, msg);
		}
	static int __cdecl dbg_report(int rt, const wchar_t * fn, int ln, const wchar_t * mn, const wchar_t * fmt, ...)
		{
		wchar_t msg[1024];
		va_list arg_list;
		va_start(arg_list, fmt);
		vswprintf_s(msg, sizeof msg / sizeof msg[0], fmt, arg_list);
		va_end(arg_list);
		return _CrtDbgReportW(rt, fn, ln, mn, msg);
		}
#endif // VC8

private:
	char	msg_buf_[1024];
	wchar_t	wsg_buf_[1024];
};

#if !defined(WIN32) && !defined(_WIN64)
	#include <unistd.h>
/* 简单版 */
inline void AssertFailed(const char * expr, const char * filename, unsigned int lineno)
{
	const char assert_str[] = "Assertion failed: %s, file %s:%d, pid %d.\n(A)bort, (R)etry, (I)gnore : ";
	fprintf(stderr, assert_str, expr, filename, lineno, getpid());
    while ( 1 ) 
	{
        switch (getchar()) 
		{
        case 'A':   case 'a':   exit(EINTR);
        case 'I':   case 'i':   return;
		case 'R':	case 'r':	break;
        default:        continue;
        }
        break;
    }
    fprintf(stderr, "Using gdb to debug process %d.\n", getpid());
    getchar();  //  wait for debug
}

/* linux 下增强的ASSERT */
inline void AssertFaileds(const char * expr, const char * filename, unsigned int lineno, const char * msg = 0)
{
	const char assert_str[] = "Assertion failed: %s, file %s:%d, pid %d.\n(A)bort, (R)etry, (I)gnore : ";
	if ( msg )
		fprintf(stderr, "%s\n", msg);

	fprintf(stderr, assert_str, expr, filename, lineno, getpid());
    while ( 1 ) 
	{
        switch (getchar()) 
		{
        case 'A':   case 'a':   exit(EINTR);
        case 'I':   case 'i':   return;
		case 'R':	case 'r':	break;
        default:        continue;
        }
        break;
    }
    fprintf(stderr, "Using gdb to debug process %d.\n", getpid());
    getchar();  //  wait for debug
}

/* 宽字符版本 */
inline void AssertFaileds(const char * expr, const char * filename, unsigned int lineno, const wchar_t * msg = 0)
{
	const wchar_t assert_str[] = L"Assertion failed: %s, file %s:%d, pid %d.\n(A)bort, (R)etry, (I)gnore : ";
	if ( msg )
		fwprintf(stderr, L"%ls\n", msg);

	fwprintf(stderr, assert_str, expr, filename, lineno, getpid());
    while ( 1 ) 
	{
        switch (getchar()) 
		{
        case L'A':  case L'a':  exit(EINTR);
        case L'I':  case L'i':  return;
		case L'R':	case L'r':	break;
        default:        continue;
        }
        break;
    }
    fwprintf(stderr, L"Using gdb to debug process %d.\n", getpid());
    getchar();  //  wait for debug
}

#endif	/* !defined(WIN32) */

//////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <crtdbg.h>
#  define SP_CHECK_MEMORY SP_ASSERT(_CrtCheckMemory());
# else
#  define SP_CHECK_MEMORY
#endif // crtdbg

#endif /* __SP_ASSERT_H__ */
