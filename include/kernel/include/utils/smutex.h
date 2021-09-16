/** 
 * @file	smutex.h
 * @brief	������
 * 
 *  ����ʵ�õĻ�������ͨ��fast_mutex�������䣬��ace�Ļ������ӿ�һ��
 * 
 * @author	jdyu
 * @version	1.0
 * @date	2008/9/10
 * 
 * @see		
 * 
 * <b>History:</b><br>
 * <table>
 *  <tr> <th>Version	<th>Date		<th>Author	<th>Notes</tr>
 *  <tr> <td>1.0		<td>2008/9/10	<td>jdyu	<td>Create this file</tr>
 * </table>
 * 
 */
#ifndef __SMUTEX_H__
#define __SMUTEX_H__

#include "utils/fast_mutex.hpp"

// namespace
namespace sp {

/** 
 * @class	srmutex
 * @brief	ʹ��fast_mutex����
 * @author	jdyu
 * @date	2008/09/09
 */
class smutex
	: public fast_mutex
	{
public:
	void acquire(void)
		{
		this->lock();
		}
	void release(void)
		{
		this->unlock();
		}
	};

} /* namespace sp */

#endif /* __SMUTEX_H__ */
