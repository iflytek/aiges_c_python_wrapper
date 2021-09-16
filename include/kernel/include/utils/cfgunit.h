/** 
 * @file	ConfigUnit.h
 * @brief	һ��ͨ�õ����ù���ģ�飬Ŀǰ���Զ�дע����Ini�����ã�
 *			�Ժ����չ��֧��xml��ʽ
 * 
 * detail...
 * 
 * @author	zwzhao
 * @version	1.0
 * @date	2005��11��9��
 * 
 * @see	 * 
 * @par �汾��¼��
 * <table border=1>
 *  <tr> <th>�汾	<th>����			<th>����	<th>��ע </tr>
 *  <tr> <td>1.0	<td>2005��11��9��	<td>zwzhao	<td>���� </tr>
 * </table>
 */
#ifndef CCONFIGUNIT_BY_ZWZHAO_2005_11_09
#define CCONFIGUNIT_BY_ZWZHAO_2005_11_09

#include <stdlib.h>

/** 
 * @interface	cfgunit
 * 
 * @brief	���ù���Ľӿ�ģ��
 * 
 * detail...
 * 
 * @author	zwzhao
 * @date	2005��11��9��
 * 
 * @see	 * 
 * @par ��ע��
 * 
 */
class cfgunit
{
public:
	virtual const char* name() const = 0;
	virtual const char* full_name() const = 0;

	// ��д����
	virtual int get_value(const char* value_name,
		unsigned long& value_i, const char* subkey = NULL) const = 0;
	
	virtual int set_value(const char* value_name,
		unsigned long value, const char* subkey = NULL)  = 0;
	
	virtual int get_value(const char* value_name,
		const char*& value_str, const char* subkey = NULL) const = 0;
	
	virtual int set_value(const char * value_name,
		const char* value, const char* subkey = NULL) = 0;

	// ����Ӽ�
	virtual int get_subunit_count(const char* subkey = NULL) const = 0;
	virtual cfgunit* get_subunit_at(int index,const char* subkey = NULL) const = 0;
	virtual cfgunit* get_parent() const = 0;
	virtual cfgunit* find_subunit(const char *subkey = NULL) const = 0;
	
	// ���ֵ��
	virtual int get_values_count(const char* subkey = NULL) const = 0;
	virtual const char* get_value_at(int index, const char* subkey = NULL) const = 0;

	// ���ɾ��
	virtual int append_sub(const char* key_name,
		cfgunit** pp_sub = NULL, const char* subkey = NULL) = 0;
	virtual int remove_sub(const char* key_name, const char* subkey = NULL) = 0;
	virtual int remove_value(const char* value_name, const char* subkey = NULL) = 0;
};

const char addin_dirname[] = "addins";

class cfg_holder
{
public:
	virtual int open(const char* holder_name = NULL, bool append = false, const char * cfg_dir = 0) = 0;
	virtual int flush() = 0;
	virtual int destroy() = 0;
	
	virtual cfgunit*	get_cfgunit(const char* subkey = NULL) const = 0;
	virtual const char*	get_install_path() const = 0;
};

#include "../base/srbase.h"
DEFINE_SHPTR(cfg_holder, "cfg_holder");

#endif /* CCONFIGUNIT_BY_ZWZHAO_2005_11_09 */
