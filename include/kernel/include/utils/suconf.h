/** 
 * @file	suconf.h
 * @brief	读取配置的功能函数
 * 
 *  支持ini文件，注册表等
 * 
 * @author	jdyu
 * @version	1.0
 * @date	2008/8/28
 * 
 * @see		
 * 
 * <b>History:</b><br>
 * <table>
 *  <tr> <th>Version	<th>Date		<th>Author	<th>Notes</tr>
 *  <tr> <td>1.0		<td>2008/8/28	<td>jdyu	<td>Create this file</tr>
 * </table>
 * 
 */
#ifndef __SUCONF_H__
#define __SUCONF_H__

#include <string>
#include <fstream>
#include <stdlib.h>
#include <algorithm>

#include "iFly_Assert.h"
#include "sutils.h"

#ifndef WIN32
#include <algorithm>
#else
#include "utils/sutils.h"
#endif
// configuration
namespace sp
{

/** 
 * @class	ini_file
 * @brief	
 * 
 *  detail...
 * 
 * @author	jdyu
 * @date	2007-7-23
 */
template <class char_t = char>
class ini_file
	{
public:
	int open(const char_t * file)
		{
		return 0;
		}
	int close(void)
		{
		return 0;
		}

	int get_section(const char_t * sec_name)
		{
		return 0;
		}
private:
	// string type
	typedef std::basic_string<char_t>	string;
	// 所有的section
	};


/** 
 * @class	cfgu_null
 * @brief	一个空的cfgunit
 * 
 *  提供空的接口用于实现接口
 * 
 * @author	jdyu
 * @date	2008/8/28
 */
class cfgu_null
	{
public:
	int get_values_count(const char* subkey = 0) const
		{
		return 0;
		}
	const char* get_value_at(int index, const char* subkey = NULL)
		{
		return 0;
		}
	// 读写操作
	int get_value(const char* value_name, unsigned long& value_i, const char* subkey = NULL) const
		{
		return 0;
		}
	int set_value(const char* value_name, unsigned long value, const char* subkey = NULL)
		{
		return 0;
		}
	int get_value(const char* value_name, const char*& value_str, const char* subkey = NULL) const
		{
		return 0;
		}
	int set_value(const char* value_name, const char* value, const char* subkey = NULL)
		{
		return 0;
		}
	};

/** 
 * @class	cfgh_null
 * @brief	一个空的cfg_holder
 * 
 *  为了适应原有的cfgholder而实现
 * 
 * @author	jdyu
 * @date	2008/8/28
 */
template <typename cfgu_t = cfgu_null>
class cfgh_null
	{
public:
	cfgu_t * get_cfgunit(const char* subkey)
		{
		return 0;
		}
	};

/** 
 * @class	attr_val_t
 * @brief	一个属性对
 * 
 *  对应于配置文件里面的attr=val
 * 
 * @author	jdyu
 * @date	2008/8/29
 */
template<typename char_t = char>
class attr_val_t
	{
public:
	// string type
	typedef std::basic_string<char_t> string;
	explicit attr_val_t(const string & attr, const string & val)
		: attr_	(attr)
		, val_	(val)
		{
		}
	attr_val_t(const char * attr, const char* val)
		{
		if ( attr )
			attr_ = attr;
		if ( val )
			val_  = val;
		}
	attr_val_t(const wchar_t * attr, const wchar_t * val)
		{
		if ( attr )
			attr_ = attr;
		if ( val )
			val_  = val;
		}

	const string & attr(void) const
		{
		return attr_;
		}
	const string & val(void) const
		{
		return val_;
		}
	bool empty(void) const
		{
		return val().empty();
		}
	bool valid(void) const
		{
		return !attr().empty();
		}
	bool operator == (const char_t * sec_name) const
		{
		return sp::strcasecmp(attr().c_str(), sec_name) == 0;
		}
private:
	string	attr_;
	string	val_;
	};
template<>
inline
attr_val_t<wchar_t>::attr_val_t(const char * attr, const char* val)
	{
	attr_ = sp::char2wchar(attr);
	val_  = sp::char2wchar(val);
	}
template<>
inline
attr_val_t<char>::attr_val_t(const wchar_t * attr, const wchar_t * val)
	{
	attr_ = sp::wchar2char(attr);
	val_  = sp::wchar2char(val);
	}

/** 
 * @class	ini_section
 * @brief	ini文件的一个section
 * 
 *  把一个section里面所有的属性对存放起来，便于查询
 * 
 * @author	jdyu
 * @date	2007-7-23
 * 
 * @see		
 */
template <class char_t = char, class file_trait = ini_file<char_t>, class cfgu_t = cfgu_null, class cfgh_t = cfgh_null<cfgu_t> >
class ini_section
	{
public:
	int open(const char_t * file, const char_t * section)
	{
#ifdef WIN32
		std::basic_ifstream<char_t> fs(file);
#else
		std::basic_ifstream<char_t> fs(sp::tochar(file).c_str());
#endif
		if ( fs.fail() )
			return -1;

		bool in_section = false;
        char_t line[1000] ={0};
		while ( !fs.eof() )
			{
			line[0] = 0;

			fs.getline(line, SP_COUNTOF(line));
			sp::trim_str(line);
			if ( line[0] == 0 || line[0] == char_t('#') || line[0] == char_t(';') )
				continue ;

			if ( line[0] == char_t('[') )
				{
				string sec_name = line + 1;
				sp::trim_str(sec_name, ']');
				in_section = sp::strcasecmp(sec_name.c_str(), section) == 0;
				continue ;
				}

			if ( in_section )
				{
				string attr, val;
				const char_t split[] = { '=', '\0' };
				sp::get_attrib_and_value(line, attr, val, split, true);
				av_arr_.push_back(attr_val(attr, val));
				}
			}
		return 0;
		}
	int open(const char * file, const char * section, const char * conf_path)
		{
		std::string cfg_file(file);
		if ( !sp::is_file_exist(file) )
			{
			if ( !sp::is_abs_path(file) && conf_path )
				cfg_file = sp::cat_path(conf_path, file);
			}
		return open(cfg_file.c_str(), section);
		}
	int open(const wchar_t * file, const wchar_t * section, const wchar_t * conf_path)
		{
		std::wstring cfg_file(file);
		if ( !sp::is_file_exist(file) )
			{
			if ( !sp::is_abs_path(sp::tochar(file).c_str()) && conf_path )
				cfg_file = sp::towchar(sp::cat_path(sp::wchar2char(conf_path).c_str(), sp::wchar2char(file).c_str()));
			}

		return open(cfg_file.c_str(), section);
		}

	int open(const cfgu_t * cfu)
		{
		SP_ASSERT(cfu);
		if ( cfu == 0 )
			return -1;

		int count = cfu->get_values_count();
		for ( int i = 0; i < count; i++ )
			{
			const char * patt = cfu->get_value_at(i);
			const char * pval = 0;
			cfu->get_value(patt, pval);
			av_arr_.push_back(attr_val(patt, pval));
			}
		return count ? 0 : -1;
		}
	int open(const cfgu_t * cfu, const char_t * section)
		{
		const char_t nstr[] = { '\0' };
		int count = cfu->get_values_count(sp::tochar(section).c_str());
		for ( int i = 0; i < count; i++ )
			{
			const char * patt = cfu->get_value_at(i, sp::tochar(section).c_str());
			const char * pval = 0;
			cfu->get_value(patt, pval);
			av_arr_.push_back(attr_val(patt, pval ? pval : ""));
			}
		return count ? 0 : -1;
		}
	int close(void)
		{
		av_arr_.clear();
		return 0;
		}

	// constructors
	ini_section(const char_t * file = 0, const char_t * section = 0, const char_t * conf = 0)
		{
		open(file, section, conf);
		}
	ini_section(const cfgu_t * cfu, const char_t * section)
		{
		open(cfu, section);
		}
	ini_section(const cfgh_t * cfh, const char_t * section)
		{
		SP_ASSERT(cfh);
		if ( cfh )
			open(cfh->get_cfgunit(sp::tochar(section).c_str()));
		}
	~ini_section(void)
		{
		close();
		}

	// string type
	typedef std::basic_string<char_t>	string;
	typedef attr_val_t<char_t>			attr_val;
	// 内部类型
	typedef std::vector<attr_val>					attr_val_arr;
	typedef typename attr_val_arr::iterator			attr_val_it;
	typedef typename attr_val_arr::const_iterator	attr_val_cit;

	// 键的枚举
	int value_count(void) const
		{
		return (int)av_arr_.size();
		}

	// 检查一个键是否存在
	bool is_key_exist(const char_t * key) const
		{
		if ( key == 0 )
			return false;
		return std::find(av_arr_.begin(), av_arr_.end(), key)
			!= av_arr_.end();
		}

	// 查找键值
	const attr_val * find_attr(const char_t * key) const
		{
		attr_val_cit pi = std::find(av_arr_.begin(), av_arr_.end(), key);
		if ( pi == av_arr_.end() )
			return 0;
		return &(*pi);
		}

	// 获得字符串值
	string get_str_value(const char_t * key, const char_t * default_val = 0, bool * exist = 0) const
		{
		string ret_val;
		int ret = get_str_value(key, ret_val, default_val);
		if ( exist )
			*exist = ret == 0;
		return ret_val;
		}
	int get_str_value(const char_t * key, string & val, const char_t * default_val = 0) const
		{
		if ( default_val != 0 )
			val = default_val;

		const attr_val * attr = find_attr(key);
		if ( attr == 0 )
			return -1;
		if ( !attr->empty() )
			val = attr->val();
		return 0;
		}

	// 获得int和其他与int兼容的值
	int get_int_value(const char_t * key) const
		{
		int val;
		get_int_value(key, val, val);
		return val;
		}
	int get_int_value(const char_t * key, int & val) const
		{
		const attr_val * attr = find_attr(key);
		if ( attr == 0 )
			return -1;
		if ( !attr->empty() )
			val = sp::strtoi(attr->val().c_str());
		return 0;
		}
	int get_int_value(const char_t * key, int & val, int default_val) const
		{
		val = default_val;
		return get_int_value(key, val);
		}
	template <class T>
	T get_int_value(const char_t * key, const T & def_val, const T & min_val, const T & max_val) const
		{
		int val = get_int_value(key, (int&)def_val);
		return sp::get_val_in_range<T>(val, min_val, max_val);
		}
	template <class T>
	int get_int_value(const char_t * key, T & val, const T & def_val, const T & min_val, const T & max_val) const
		{
		int ret = get_int_value(key, (int&)val, (int)def_val);
		val = sp::get_val_in_range<T>(val, min_val, max_val);
		return ret;
		}

	// 获得bool类型的配置
	bool get_bool_value(const char_t * key) const
		{
		const attr_val * attr = find_attr(key);
		if ( attr == 0 )
			return -1;
		return sp::strtobool(attr->val().c_str());
		}
	int get_bool_value(const char_t * key, bool & val) const
		{
		const attr_val * attr = find_attr(key);
		if ( attr == 0 )
			return -1;
		if ( !attr->empty() )
			val = sp::strtobool(attr->val().c_str());
		return 0;
		}
	int get_bool_value(const char_t * key, bool & val, bool default_val) const
		{
		val = default_val;
		return get_bool_value(key, val);
		}

	// 获得浮点型的配置
	float get_float_value(const char_t * key) const
		{
		float val = 0.f;
		get_float_value(key, val, val);
		return val;
		}
	int get_float_value(const char_t * key, float & val)
		{
		const attr_val * attr = find_attr(key);
		if ( attr == 0 )
			return -1;
		if ( !attr->empty() )
			val = (float)sp::strtod(attr->val().c_str());
		return 0;
		}
	int get_float_value(const char_t * key, float & val, float default_val)
		{
		val = default_val;
		return get_float_value(key, val);
		}
	template<class T>
	int get_float_value(const char_t * key, T & val, const T & def_val, const T & min_val, const T & max_val)
		{
		T real_val;
		int ret = get_float_value(key, real_val, def_val);
		val = sp::get_val_in_range<T>(real_val, min_val, max_val);
		return ret;
		}

	// 枚举所有键
	size_t enum_keys(str_arr & keys) const
		{
		keys.reserve(av_arr_.size());
		for ( size_t i = 0; i < av_arr_.size(); i++ )
			keys.push_back(av_arr_[i].attr());
		return av_arr_.size();
		}

private:
	// 读取的所有值对
	attr_val_arr		av_arr_;
	};

// specialize to different char type
#if 0
template <>
inline
int ini_section<wchar_t>::open(const cfgu_t * cfu)
		{
		SP_ASSERT(cfu);
		if ( cfu == 0 )
			return -1;

		int count = cfu->get_values_count();
		for ( int i = 0; i < count; i++ )
			{
			const char * patt = cfu->get_value_at(i);
			const char * pval = 0;
			cfu->get_value(patt, pval);
			av_arr_.push_back(attr_val(sp::char2wchar(patt), sp::char2wchar(pval)));
			}
		return count ? 0 : -1;
		}
#endif

/// using type
typedef ini_section<char>		inisec;
typedef ini_section<wchar_t>	inisecw;

// 方便使用的函数
inline bool is_section_valid(const char * cfg_file, const char * section)
	{
	inisec sec(cfg_file, section);
	return sec.value_count() != 0;
	}

} /* namespace sp */

#endif /* __SUCONF_H__ */
