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
 * @brief	ϵͳ�ĵײ���
 * 
 * 
 * 
 * @author	wbli
 * @date	2007��4��13��
 * 
 * @see		
 * 
 * @par ��ע��
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
 * @brief	SR�����нӿڵĶ���Ļ��ࡣ
 * 
 * ����Ϊ�ڴ����͹���ȹ����ṩ֧�֡�
 * 
 * @author	wbli
 * @date	2007��5��15��
 * 
 * @see		
 * 
 * @par ��ע��
 * 
 */
struct if_base
	{
	virtual ~if_base() {}; 
	};

_SR_END

#endif // __ISR_BASE_INCLUDED
