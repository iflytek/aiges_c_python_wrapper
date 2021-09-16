#ifndef __ISR_BASE_INCLUDED
#define __ISR_BASE_INCLUDED

#include "scom/sdef.h"

#ifndef scom_default_dname
#define scom_default_dname	"sr0."
#endif //scom_default_dname

#define _BEGIN_NAMESPACE(x)	namespace x {
#define _END_NAMESPACE	}

/**
 * @namespace sr
 * @brief	系统的底层类
 * 
 * 
 * 
 * @author	wbli
 * @date	2007年4月13日
 * 
 * @see		
 * 
 * @par 备注：
 * 
 */
#define _SR_BEGIN	namespace sr {
#define _SR_END		}

_SR_BEGIN

typedef const wchar_t* pcwstr;
typedef const char* pcstr;

typedef int	fd_type;
const int invalid_fh = -1;


/** 
 * @class	if_base

 * 
 * @brief	SR中所有接口的定义的基类。
 * 
 * 可以为内存分配和管理等功能提供支持。
 * 
 * @author	wbli
 * @date	2007年5月15日
 * 
 * @see		
 * 
 * @par 备注：
 * 
 */
struct if_base
	{
	virtual ~if_base() {}; 
	};

_SR_END

#endif // __ISR_BASE_INCLUDED
