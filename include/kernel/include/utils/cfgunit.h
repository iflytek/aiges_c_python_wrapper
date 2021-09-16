/** 
 * @file	ConfigUnit.h
 * @brief	一个通用的配置管理模块，目前可以读写注册表和Ini的配置，
 *			以后会扩展成支持xml格式
 * 
 * detail...
 * 
 * @author	zwzhao
 * @version	1.0
 * @date	2005年11月9日
 * 
 * @see	 * 
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th>版本	<th>日期			<th>作者	<th>备注 </tr>
 *  <tr> <td>1.0	<td>2005年11月9日	<td>zwzhao	<td>创建 </tr>
 * </table>
 */
#ifndef CCONFIGUNIT_BY_ZWZHAO_2005_11_09
#define CCONFIGUNIT_BY_ZWZHAO_2005_11_09

#include <stdlib.h>

/** 
 * @interface	cfgunit
 * 
 * @brief	配置管理的接口模块
 * 
 * detail...
 * 
 * @author	zwzhao
 * @date	2005年11月9日
 * 
 * @see	 * 
 * @par 备注：
 * 
 */
class cfgunit
{
public:
	virtual const char* name() const = 0;
	virtual const char* full_name() const = 0;

	// 读写操作
	virtual int get_value(const char* value_name,
		unsigned long& value_i, const char* subkey = NULL) const = 0;
	
	virtual int set_value(const char* value_name,
		unsigned long value, const char* subkey = NULL)  = 0;
	
	virtual int get_value(const char* value_name,
		const char*& value_str, const char* subkey = NULL) const = 0;
	
	virtual int set_value(const char * value_name,
		const char* value, const char* subkey = NULL) = 0;

	// 获得子键
	virtual int get_subunit_count(const char* subkey = NULL) const = 0;
	virtual cfgunit* get_subunit_at(int index,const char* subkey = NULL) const = 0;
	virtual cfgunit* get_parent() const = 0;
	virtual cfgunit* find_subunit(const char *subkey = NULL) const = 0;
	
	// 获得值键
	virtual int get_values_count(const char* subkey = NULL) const = 0;
	virtual const char* get_value_at(int index, const char* subkey = NULL) const = 0;

	// 添加删除
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
