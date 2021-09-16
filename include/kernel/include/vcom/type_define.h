#ifndef __TYPE_DEFINE_H__
#define __TYPE_DEFINE_H__

//#include "vspp_macro.h"

#ifndef WIN32
	typedef unsigned long       DWORD;
	typedef unsigned short		WORD;
	typedef void*				HINSTANCE;
	typedef unsigned char       BYTE;
	typedef BYTE*				PBYTE;
	typedef unsigned char		byte;
	typedef DWORD				DWORD_PTR;
	typedef const char			PCSTR;
	typedef DWORD				ULONG;
	typedef unsigned long		ULONG_PTR, *PULONG_PTR;
	#define PtrToUlong( p ) ((ULONG)(ULONG_PTR) (p) )

    #define _aligned_malloc(_Size, _Alignment) _mm_malloc(_Size,_Alignment)
	#define _aligned_free(_Memory)	_mm_free(_Memory)

	//#define _aligned_malloc(_Size, _Alignment) malloc(_Size)
	//#define _aligned_free(_Memory)	free(_Memory)

	#define _CrtCheckMemory()
	#ifndef _MAX	/* avoid collision with common (nonconforming) macros */
		#define _MAX	(max)
		#define _MIN	(min)
		#define _IMPL_MAX max
		#define _IMPL_MIN min
	#else
		#define _IMPL_MAX _MAX
		#define _IMPL_MIN _MIN
	#endif
	#define aix_offsetof(s,m) (int)&reinterpret_cast<const volatile char&>((((s *)0)->m))


#endif

#endif // __TYPE_DEFINE_H__
