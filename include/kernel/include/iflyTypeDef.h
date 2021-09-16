#ifndef __IFLYTYPEDEF_H__
#define __IFLYTYPEDEF_H__



typedef	signed char			ifly_int8_t;	/* 8-bit */
typedef	unsigned char		ifly_uint8_t;	/* 8-bit */
typedef	signed short		ifly_int16_t;	/* 16-bit */
typedef	unsigned short		ifly_uint16_t;	/* 16-bit */
typedef	signed int			ifly_int32_t;	/* 32-bit */
typedef	unsigned int		ifly_uint32_t;	/* 32-bit */
typedef	signed long long	ifly_int64_t;	/* 64-bit */
typedef	unsigned long long	ifly_uint64_t;	/* 64-bit */
typedef void				ifly_void_t;

/* macro */
#ifdef WIN32
#include <crtdbg.h>
#define W_ASSERT						_ASSERT
#define W_INLINE						__inline
#define W_ASSERT_EQUAL(x, y, epsion)	_ASSERT( fabs(x-y) < epsion)
#else
#define W_ASSERT						assert
#define W_INLINE						inline
#define W_ASSERT_EQUAL(x, y, epsion)	assert(fabs(x-y) < epsion)
#endif

#endif //__IFLYTYPEDEF_H__