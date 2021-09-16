/** 
 * @file	ISPUtils.h
 * @brief	公用函数库
 *
 *  使用的一些公用函数，这个文件里包含了一些方便使用的函数
 *  考虑到这里提供的函数会在多个项目中使用，因此不要包含ＡＣＥ
 * 
 * @author	jdyu
 * @version	1.0
 * @date	2004-6-8
 * 
 * @see		
 * 
 * <b>History:</b><br>
 * <table>
 *  <tr> <th>Version	<th>Date		<th>Author	<th>Notes</tr>
 *  <tr> <td>1.0		<td>2004-6-8	<td>jdyu	<td>Create this file</tr>
 *  <tr> <td>1.1		<td>2005-9-29	<td>jdyu	<td>进行了梳理</tr>
 * </table>
 * 
 */
#ifndef __SPUTILS_H__
#define __SPUTILS_H__

#include <string>
#include <vector>
#include <list>
#include <map>

/* data type defines */
#define SP_DWORD					unsigned int
#define SP_UINT						unsigned int
#define SP_WORD						unsigned short
#define SP_BYTE						unsigned char

/// HANDLE type
typedef void * IU_HANDLE;

#ifdef WIN32
# define IU_INVALID_HANDLE			INVALID_HANDLE_VALUE
#else  // NOT WIN32
# define IU_INVALID_HANDLE			IU_HANDLE(-1)
#endif // NOT WIN32

// MAX_PATH
#ifndef MAX_PATH
#define MAX_PATH					260
#endif /* MAX_PATH */

/// 计算机名长度
#define ISP_MAXCOMPUTERNAME			16

/// 斜杠定义
#define BSLASH_CHR					'\\'
#define BSLASH_STR					"\\"
#define SLASH_CHR					'/'
#define SLASH_STR					"/"

/// 路径区分符
#ifndef PATH_CHR
# ifdef WIN32
#  define PATH_CHR					BSLASH_CHR
#  define PATH_STR					BSLASH_STR
#else
#  define PATH_CHR					SLASH_CHR
#  define PATH_STR					SLASH_STR
# endif // WIN32
#endif  // PATH_CHR

// 版本号
#define SP_MAKEWORD(a, b)			((SP_WORD)(((SP_BYTE)((SP_DWORD)(a) & 0xff)) | ((SP_WORD)((SP_BYTE)((SP_DWORD)(b) & 0xff))) << 8))
#define SP_MAKELONG(a, b)			((long)(((SP_WORD)((SP_DWORD)(a) & 0xffff)) | ((SP_DWORD)((SP_WORD)((SP_DWORD)(b) & 0xffff))) << 16))
#define SP_LOWORD(l)				((SP_WORD)((SP_DWORD)(l) & 0xffff))
#define SP_HIWORD(l)				((SP_WORD)((SP_DWORD)(l) >> 16))
#define SP_LOBYTE(w)				((SP_BYTE)((SP_DWORD)(w) & 0xff))
#define SP_HIBYTE(w)				((SP_BYTE)((SP_DWORD)(w) >> 8))

#define SP_VERMAKE(maj, min, rev)	SP_MAKELONG(rev, SP_MAKEWORD(min, maj))
#define SP_VERMAJOR(ver)			SP_HIBYTE(SP_HIWORD(ver))
#define SP_VERMINOR(ver)			SP_LOBYTE(SP_HIWORD(ver))
#define SP_VERREVISION(ver)			SP_LOWORD(ver)

// count of array
#define	SP_COUNTOF(arr)				( sizeof arr / sizeof arr[0] )

////////////////////////////////////////////////////////////////////////
// 字节序调整
////////////////////////////////////////////////////////////////////////
// 两个字节的WORD高低位倒转
#define SP_WORD_LE_BE(val)			((SP_WORD) ( (((SP_WORD) (val) & (SP_WORD) 0x00ffU) << 8) | \
												 (((SP_WORD) (val) & (SP_WORD) 0xff00U) >> 8)))
// 四个字节的DWORD高低位倒转
#define SP_DWORD_LE_BE(val)			((SP_DWORD) ( (((SP_DWORD) (val) & (SP_DWORD) 0x000000ffU) << 24) | \
												  (((SP_DWORD) (val) & (SP_DWORD) 0x0000ff00U) <<  8) | \
                                                  (((SP_DWORD) (val) & (SP_DWORD) 0x00ff0000U) >>  8) | \
												  (((SP_DWORD) (val) & (SP_DWORD) 0xff000000U) >> 24)))

////////////////////////////////////////////////////////////////////////
/// 异常捕获的宏
////////////////////////////////////////////////////////////////////////
/// try ... catch
#ifndef SP_CATCH_EXCEPTION_
#  define SP_CATCH_BEGIN()
#  define SP_CATCH_END(handler)
#else
#  define SP_CATCH_BEGIN()			try {
#  define SP_CATCH_END(handler)		} catch(...) { handler; }
#endif

/// try ... finally
#ifndef SP_CATCH_EXCEPTION_
#  define SP_FINALY_BEGIN()
#  define SP_FINALY_END(handler)	handler;
#else
#  if defined(WIN32)
#    define SP_FINALY_BEGIN()		__try {
#    define SP_FINALY_END(handler)	} __finally { handler; }
#  else /* linux */
#    define SP_FINALY_BEGIN()
#    define SP_FINALY_END(handler)	handler
#  endif /* WIN32 */
#endif /* SP_CATCH_EXCEPTION_ */

/**
 * ==========================================================================
 * 通用操作
 * ==========================================================================
 */

/** 
 * @brief	通用操作
 * 
 *  这个名字空间里面包含了在编码中经常用到的一些功能，分为：
 *    1.字符串操作
 *    2.文件操作
 *    3.网络操作
 *    4.操作系统特性
 *  几个大的分类，建议使用。
 * 
 * @author	jdyu
 * @date	2007-7-5
 * 
 * @see		
 * 
 */
namespace sp
{
	// 字符串数组
	typedef std::vector<std::string>	str_arr;
	typedef std::vector<std::wstring>	wstr_arr;
	typedef std::list  <std::string>	str_list;
	typedef std::list  <std::wstring>	wstr_list;

	// 字符串映射表
	typedef std::pair<std::string, std::string>		str_pair;
	typedef std::pair<std::wstring, std::wstring>	wstr_pair;
	typedef std::map <std::string, std::string>		str_map;
	typedef str_map::value_type		 sm_item;
	typedef str_map::iterator		 sm_it;
	typedef str_map::const_iterator	 sm_cit;
	// 字符对列表
	typedef std::vector<str_pair>	 str_pair_arr;
	typedef std::vector<wstr_pair>	 wstr_pair_arr;

	/** 
	 * @brief 	last_error
	 * 	
	 *  获得错误代码
	 *
	 * @author	jdyu
	 * @date	2005-9-29
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @see		
	 */
	int last_error(void);

	/** 
	 * @brief 	get_val_in_range
	 *
	 * 	将指定的值规范化到指定的区间
	 * 
	 * @author	jdyu
	 * @date	2004-7-15
	 * @return	template<typename TYPE>TYPE 
	 * @param	const TYPE& val	- [in] 输入待规整的变量
	 * @param	const TYPE& min	- [in] 下限
	 * @param	const TYPE& max	- [in] 上限
	 * @see		
	 */
	template<typename TYPE>
	TYPE get_val_in_range(const TYPE& val, const TYPE& min, const TYPE& max)
		{ if ( val < min ) return min; if ( val > max ) return max; return val; }

	/** 
	 * @brief 	ln2
	 *
	 * 	求一个整数以2为底的整数次
	 * 
	 * @author	jdyu
	 * @date	2004-9-2
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	int pw	- [in] 输入的整数
	 * @see		
	 */
	# define IU_POW(X) (((X) == 0)?1:(X-=1,X|=X>>1,X|=X>>2,X|=X>>4,X|=X>>8,X|=X>>16,(++X)))
	inline int ln2(unsigned int pw)
	{
		int ln = 0;
		if ( pw != 0 && pw != 1 )
			{ pw = IU_POW(pw); while ( (( pw>>++ln) & 1 ) == 0 ); }
		return ln;
	}

	/** 
	 * @brief 	debugging_mode
	 * 
	 * 	设置当前是否为debuggin模式，在debugging模式下，
	 *  一些函数会输出较多的信息
	 * 
	 * @author	jdyu
	 * @date	2006-2-28
	 * @return	No return value.
	 * @param	bool mode	- [in] 设置是否为调试模式
	 * @see		
	 */
	bool debugging_mode(bool mode);
	bool debugging_mode(void);
};

/**
 * ==========================================================================
 * 字符串相关的操作
 * ==========================================================================
 */
namespace sp
{
	/** 
	 * @brief 	is_quanjiao
	 *
	 * 	判断是否为全角字符
	 *
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	bool	- Return true in success, otherwise return false.
	 * @param	const char* pstr	- [in,out] 
	 * @see		
	 */
	bool is_quanjiao(const char * pstr);

	/** 
	 * @brief 	trim_str
	 *
	 * 	去掉字符串头尾的空格或其他字符，包含控制字符
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	int 返回处处空格后的字符串长度
	 * @param	char* pstr	- [in,out] 
	 * @param	const char ctrim = ' '	- [in] 需要剔除的符号
	 * @see		
	 */
	int trim_str(char * pstr, const char trim_char = ' ', bool trim_gb = false);
	int trim_str(std::string & str, const char trim_char = ' ', bool trim_gb = false);
	int trim_str(char * pstr, const char * strim, bool trim_gb = false);
	int trim_str(std::string & str, const char * strim, bool trim_gb = false);

	int trim_str(wchar_t * pstr, const wchar_t trim_char = L' ');
	int trim_str(std::wstring & str, const wchar_t trim_char = L' ');
	int trim_str(wchar_t * pstr, const wchar_t * strim);
	int trim_str(std::wstring & str, const wchar_t * strim);

	/** 
	 * @brief	search_strarr
	 *
	 *	获得指定字符串在数组中的索引
	 *
	 * @author	yigao
	 * @date	2004年6月23日
	 * @return	int 
	 * @param	const char *str [in]需要查询的字符串
	 * @param	const char *arr[] [in]需要查询的字符串数组
	 * @param	int arr_size [in]数组大小
	*/
	template <typename char_t>
	int search_strarr(const char_t * str, const char_t * arr [], int arr_size, bool case_sensitive = true);
	template <typename char_t, template <typename T, typename A> class cont_t>
	int search_strarr(const char_t * str, cont_t<std::basic_string<char_t>, std::allocator<std::basic_string<char_t> > > & cont, bool case_sensitive = true);

	template <typename char_t, int count>
	int search_strarr(const char_t * str, const char_t * (&arr) [count])
		{
		return search_strarr(str, arr, count, true);
		}
	template <typename char_t, int count>
	int search_strarr(const char_t * str, const char_t * const (&arr) [count])
		{
		return search_strarr(str, (const char_t **)arr, count, true);
		}
	template <typename char_t, int count>
	int search_strarr_case(const char_t * str, const char_t * (&arr) [count])
		{
		return search_strarr(str, arr, count, false);
		}
	template <typename char_t, int count>
	int search_strarr_case(const char_t * str, const char_t * const (&arr) [count])
		{
		return search_strarr(str, arr, count, false);
		}

	/** 
	 * @brief 	str_to_time
	 *
	 * 	将h:m:s格式的字符串转化成为数值
	 * 
	 * @author	jdyu
	 * @date	2004-7-15
	 * @return	int 
	 * @param	const char* strtm	- [in] 待切分字符串
	 * @param	int & hour	- [out] 返回小时
	 * @param	int & min	- [out] 返回分钟
	 * @param	int & sec	- [out] 返回秒
	 * @see		
	 */
	template <class T>
	int str_to_time(const char * strtm, T & hour, T & min, T & sec);
	template <class T>
	int str_to_time(const char * strtm, T & year, T & mon, T & day, T & hour, T & min, T & sec);
	int str_to_time(const char * strtm, time_t & tm);

	template <class T>
	int time_to_ymdhms(time_t tm, T & year, T & mon, T & day, T & hour, T & mnt, T & sec);
	int time_to_str(time_t tm, std::string & year, std::string & mon, std::string & day, std::string & hour, std::string & mnt, std::string & sec);
	// 获得月份的字符串
	const char * month_str(int mon);

	/** 
	 * @brief 	strsncpy
	 * 	
	 *  strsncpy enhanced strncpy, more safe
	 * 
	 * @author	jdyu
	 * @date	2005-9-29
	 * @return	char * return destination buffer
	 * @param	char *dst	- [in/out] destination string
	 * @param	const char *src	- [in] source string
	 * @param	size_t maxlen	- [in] max length
	 * @see		
	 */
	char * strsncpy (char * dst, const char * src, size_t maxcnt);
	wchar_t * strsncpy(wchar_t * dst, const wchar_t * src, size_t maxcnt);

	/** 
	 * @brief 	split_str
	 *
	 * 	将指定的字符串切分为多个子串
	 *  以指定的分割字符为分界
	 *
	 * @author	jdyu
	 * @date	2004-7-16
	 * @return	int 
	 * @param	const char* str - [in] 传入需要切分的字符串
	 * @param	str_arr & subs_list - [out] 返回切分好的字符串数组
	 * @param	const char spliter[]	- [in] 分割符列表
	 * @param	bool trim = true		- [in] 是否把切分的结果去除两边的空格
	 * @param	bool ignore_blank = false	- [in] 是否忽略空的单元
	 * @param	bool supp_quote = false	- [in] 是否支持引号，引号内部不切分
	 * @param	bool supp_quanjiao = false	- [in] 是否支持全角
	 * @see		
	 */
	int split_str(const char * str, str_arr & subs_array, const char spliter[] = ",;:", bool trim = true, bool ignore_blank = false, bool supp_quote = false, bool trim_quote = true, bool supp_quanjiao = false);
	int split_str(const wchar_t * str, wstr_arr & subs_array, const wchar_t spliter[] = L",;", bool trim = true, bool ignore_blank = false, bool supp_quote = false, bool trim_quote = true);
	int split_str(const char * str, wstr_arr & subs_array, const wchar_t spliter[] = L",;", bool trim = true, bool ignore_blank = false, bool supp_quote = false, bool trim_quote = true);
	int split_str(const wchar_t * str, str_arr & subs_array, const char spliter[] = ",;:", bool trim = true, bool ignore_blank = false, bool supp_quote = false, bool trim_quote = true);

	/** 
	 * @brief 	dequote_str
	 *  
	 *  去掉字符串两边的引号，必须是对称的引号
	 *  
	 * @author	jdyu
	 * @date	2005-11-28
	 * @return	char * 
	 * @param	char * str	- [in/out] 待去除的字符串
	 * @see		
	 */
	char * dequote_str(char * str);
	wchar_t * dequote_str(wchar_t * str);

	template<typename char_t>
	std::basic_string<char_t> quote_str(const char_t * cstr, char_t quote_chr = char_t('"'));

	/** 
	 * @brief 	get_attrib_and_value
	 *  
	 *  把类似于attr="value"的属性对分解开来
	 *  
	 * @author	jdyu
	 * @date	2005-11-23
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char* attr_val	- [in] 输入的属性对
	 * @param	std::string & attr	- [in] 属性名
	 * @param	std::string & val	- [in] 属性值
	 * @see		
	 */
	int get_attrib_and_value(const char* attr_val, std::string & attr, std::string & val, const char * split = "=", bool trim = false);
	int get_attrib_and_value(const wchar_t* attr_val, std::wstring & attr, std::wstring & val, const wchar_t * split = L"=", bool trim = false);

	/** 
	 * @brief 	str_to_attr_vals
	 *  
	 *  把类似于attr1="value1" attr2="value2"的属性对列表拆分成为
	 *  影射表形式
	 *  
	 * @author	jdyu
	 * @date	2005-11-23
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char* str	- [in] 需要提取的源字符串
	 * @param	str_map & attrs	- [in] 返回提取结果
	 * @param	bool trim = false	- [in] 是否去除空格
	 * @param	bool supp_quote = false	- [in] 是否支持引号
	 * @see		
	 */
	int str_to_attr_vals(const char* str, str_map & attrs, bool trim = false, bool supp_quote = false, const char * splt = " ;,", const char * token = "=");
	int str_to_attr_vals(const char* str, str_pair_arr & attrs, bool trim = false, bool supp_quote = false, const char * splt = " ;,", const char * token = "=");

	/** 
	 * @brief 	strlwr
	 * 
	 * 	字符串小写化，大写化
	 * 
	 * @author	jdyu
	 * @date	2007-5-30
	 * @return	std::string 
	 * @param	const std::string str	- [in] 输入源字符串
	 * @see		
	 */
	std::string & strlwr(std::string & str);
	std::string & strupr(std::string & str);
	std::wstring & strlwr(std::wstring & str);
	std::wstring & strupr(std::wstring & str);

	/** 
	 * @brief 	str_replace
	 * 
	 * 	字符串替换
	 * 
	 * @author	jdyu
	 * @date	2007-5-30
	 * @return	std::string 
	 * @param	const std::string str	- [in] 输入源字符串
	 * @see		
	 */
	std::string str_replace(const char * str, const char * src, const char * dst, bool trim = false);
	std::wstring str_replace(const wchar_t * str, const wchar_t * src, const wchar_t * dst, bool trim = false);

	/** 
	 * @fn		strchr
	 * @brief	strchr
	 * @author	jdyu
	 * @date	2008/10/5
	 * @see		
	 */
	char * strchr(const char * str, int val);
	wchar_t * strchr(const wchar_t * str, int val);

	/** 
	 * @brief 	strcasecmp
	 * 
	 * 	忽略大小写的比较
	 * 
	 * @author	jdyu
	 * @date	2007-5-30
	 * @return	int	- 比较的结果，和strcmp相同
	 * @param	const char * str1	- [in] 字符串一
	 * @param	const char * str2	- [in] 字符串二
	 * @see		
	 */
	int strcasecmp(const char * str1, const char * str2);
	int strcasecmp(const wchar_t * str1, const wchar_t * str2);
	int strcasecmp(const std::string & str1, const std::string & str2);
	int strcasecmp(const std::wstring & str1, const std::wstring & str2);
	int strcmp(const char * str1, const char * str2);
	int strcmp(const wchar_t * str1, const wchar_t * str2);

	int strncasecmp(const char *str1, const char *str2, size_t len);
	int strncasecmp(const wchar_t *str1, const wchar_t *str2, size_t len);
	int strncasecmp(const std::string &str1, const std::string &str2, size_t len);
	int strncasecmp(const std::wstring &str1, const std::wstring &str2, size_t len);

	/** 
	 * @brief 	str2ver & ver2str
	 * 
	 * 	字符串到版本和版本到字符串的转换
	 * 
	 * @author	jdyu
	 * @date	2007-5-30
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char * str	- [in] 
	 * @param	int & ver	- [out] 主版本.次版本.修订号
	 * @param	int & build	- [out] 流水号
	 * @see		
	 */
	int str2ver(const char * str, int & ver, int & build);
	std::string ver2str(int ver, int build);

	/**
	 * @brief 	带通配符'*'、'?'功能的子串查找
	 * 
	 * @author	hjye
	 * @date	2004-6-3
	 * @return	bool - Returns true if matched.
	 * @param	const char* src_str - [in] 0-terminated string to search. 
	 * @param	const char* match_str - [in] 0-terminated string to search for, can contain wildcards '*' or '?'.
	 * @param	bool match_case - [in] true means case sensitive.
	 * @see		
	 */
	bool strcmp_wildcard(const char* src_str, const char* match_str, bool match_case = true);

	/** 
	 * @brief 	timecmp
	 *
	 * 	比较两个时间，如果参数为-1，表示该位无效
	 * 
	 * @author	jdyu
	 * @date	2004-10-18
	 * @return	int - <0表示前面的时间早，0表示相等，>0表示前面的时间晚
	 * @see		
	 */
	int	timecmp(int hour1, int min1, int sec1, int hour2, int min2, int sec2, int day1 = -1, int day2 = -1);

	/** 
	 * @brief 	is_register_key
	 * 
	 * 	判断是否是一个注册表键名
	 * 
	 * @author	jdyu
	 * @date	2005-11-16
	 * @return	bool	- Return TRUE in success, otherwise return FALSE.
	 * @param	const char* key	- [in] 
	 * @see		
	 */
	bool is_register_key(const char* key);

	/** 
	 * @brief 	char2wchar wchar2char
	 * 
	 * 	宽字符和窄字符的互转
	 * 
	 * @author	jdyu
	 * @date	2007-6-6
	 * @return	std::wstring 
	 * @param	const char * str		- [in]  输入带转换的字符串
	 * @param	size_t * cvt_chrs = 0	- [out] 返回实际转换的字符数量 
	 * @see		
	 */
	std::wstring char2wchar(const char * str, size_t * cvt_chrs = 0);
	std::string  wchar2char(const wchar_t * wstr, size_t * cvt_chrs = 0);
    std::wstring utf82wchar(const char* src);
    std::string wchar2utf8(const wchar_t* src);
	std::string  tochar(const std::string & str);
	std::string  tochar(const std::wstring & str);
	std::wstring towchar(const std::string & str);
	std::wstring towchar(const std::wstring & str);
    std::string  toutf8(const std::string & str);
    std::string  toutf8(const std::wstring & str);
    std::wstring utf8towchar(const std::string & str);
    std::string  utf8tochar(const std::string & str);

	// 宽窄转换，linux直接转
	size_t mbstowcs(wchar_t *wcstr, const char *mbstr, size_t count);
	size_t wcstombs(char *mbstr, const wchar_t *wcstr, size_t count);

	/** 
	 * @brief 	strtoi
	 *  
	 *  把字符串转化为值
	 *  
	 * @author	jdyu
	 * @date	2007-6-26
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char * str	- [in] 输入字符串
	 * @see		
	 */
	int		strtoi(const char * str);
	int		strtoi(const wchar_t * str);
	double	strtod(const char * str);
	double	strtod(const wchar_t * str);
	float	strtof(const char * str);
	float	strtof(const wchar_t * str);
	bool	strtobool(const char * str, bool def = false);
	bool	strtobool(const wchar_t * str, bool def = false);
	unsigned int strtoui(const char * str);
	unsigned int strtoui(const wchar_t * str);

	unsigned int towlower(unsigned int c);
	int wcscasecmp (const wchar_t * str1, const wchar_t * str2);
	int wcsncasecmp(const wchar_t * str1, const wchar_t * str2, size_t maxcount);

	/** 
	 * @brief	cur_time
	 *
	 *	获得当前时间
	 * 
	 * @author	jdyu
	 * @date	2004年5月21日
	 * @return	const char* 
	 * @param	char times[]	- [out] 返回时间信息的缓冲区
	 * @param	bool no_path	- [in]  是否不包含路径禁止的符号
	 * @see		
	 */
	const char * cur_time(char times[], bool no_path = false, bool sht_fmt = false, bool has_msec = true);
	std::string  cur_time(bool no_path = false, bool sht_fmt = false, bool has_msec = true);
	std::wstring wcur_time(bool no_path, bool sht_fmt, bool has_msec = true);
};

/**
 * ==========================================================================
 * 文件相关的操作
 * ==========================================================================
 */
namespace sp
{
	/** 
	 * @brief 	is_file_exist
	 *
	 * 	检查一个文件是否存在
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	bool	- Return true in success, otherwise return false.
	 * @param	const char* lpszFile	- [in] 要检查的文件名
	 * @see		
	 */
	bool is_file_exist(const char * file);
	bool is_file_exist(const wchar_t * file);

	/** 
	 * @brief 	is_dir_exist
	 *
	 * 	检查一个目录是否存在
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	bool	- Return true in success, otherwise return false.
	 * @param	const char* lpszDir	- [in] 目录名
	 * @see		
	 */
	bool is_dir_exist(const char * dir);
	bool is_dir_exist(const wchar_t * dir);
	// 创建目录
	int create_directory(const char * dir, bool fail_if_exist = false, bool recursive = true);
	int path_to_dir_tree(const char * path, str_arr & dir_tree);

	/** 
	 * @brief 	normalize_path
	 *
	 * 	将一个路径归一化，就是把所有的"\"或"/"转化为指定的路径分隔符
	 *  同时去处冗余的符号
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	char* path	- [in,out] 路径信息
	 * @param	const char path_split = PATH_CHR - [in] 路径区分符号
	 * @see		
	 */
	int normalize_path(char path[MAX_PATH], const char path_split = PATH_CHR);
	std::string normalize_path(const char * path, const char path_split = PATH_CHR);

	/** 
	 * @brief 	cat_path
	 *
	 * 	将两个路径信息拼合起来,即 path + more
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	const char* 返回拼接后的地址，就是lppath1
	 * @param	char* path	- [in,out] 前路径，返回拼合后的路径
	 * @param	const char* more	- [in] 后路径
	 * @see		
	 */
	const char* cat_path(char* path, const char* more, const char path_split = PATH_CHR);
	std::string cat_path(const char * path, const char * more, const char path_split = PATH_CHR);

	/** 
	 * @brief 	is_abs_path
	 *
	 * 	是否绝对路径
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	inline 
	 * @param	const char* path	- [in] 待检查的路径
	 * @see		
	 */
	bool is_abs_path(const char* path);

	/** 
	 * @brief 	get_bin_path
	 *
	 * 	获得当前可执行文件的路径
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	char* 返回可执行文件的路径
	 * @param	char szPath[MAX_PATH] - [out] 输入缓冲区，返回可执行文件的路径
	 * @param	IU_HANDLE hmod = 0	- [in] 输入模块的句柄，缺省为当前EXE模块，
	 *				如果传入IU_INVALID_HANDLE则说明获取当前代码所在模块（EXE或DLL）
	 * @see		get_bin_pathname
	 */
	//char* get_bin_path(char path[MAX_PATH], IU_HANDLE hmod = 0);//yfxie
	//std::string get_bin_path(IU_HANDLE hmod = 0);
	char* get_bin_path(const void* func, char path[MAX_PATH], IU_HANDLE hmod = 0);
	std::string get_bin_path(const void* func, IU_HANDLE hmod = 0);
	//
	/** 
	 * @brief 	get_bin_pathname
	 *
	 * 	获得模块的路径名
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	char* 返回模块的路径名
	 * @see		get_bin_path
	 */
	//char* get_bin_pathname(char pathname[MAX_PATH], IU_HANDLE hmod = 0);//yfxie
	//std::string get_bin_pathname(IU_HANDLE hmod = 0);
	char* get_bin_pathname(const void* func, char pathname[MAX_PATH], IU_HANDLE hmod = 0);
	std::string get_bin_pathname(const void* func, IU_HANDLE hmod = 0);
	
	/** 
	 * @brief 	get_bin_name
	 *
	 * 	获得当前模块的不包含路径的文件名
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	char * 返回模块的路径名
	 * @see		get_bin_path
	 */
	char* get_bin_name(char name[MAX_PATH], IU_HANDLE hmod = 0);
	std::string get_bin_name(IU_HANDLE hmod = 0);

	/** 
	 * @brief 	pathname_to_path
	 *
	 * 	从全路径名中取出路径信息
	 * 
	 * @author	jdyu
	 * @date	2004-6-23
	 * @return	char* 
	 * @param	char szPath[MAX_PATH]	- [out] 返回文件的路径
	 * @param	const char* path_name	- [in] 需要取得路径信息的全路径名
	 * @see		
	 */
	char* pathname_to_path(char path[MAX_PATH], const char* path_name, const char path_split = PATH_CHR);
	std::string pathname_to_path(const char* path_name, const char path_split = PATH_CHR);

	/** 
	 * @brief 	pathname_to_name
	 *
	 * 	从全路径名中取出文件名信息
	 * 
	 * @author	jdyu
	 * @date	2004-6-23
	 * @return	char* 
	 * @param	char name[MAX_PATH]	- [out] 返回文件的名称
	 * @param	const char* path_name	- [in] 需要取得路径信息的全路径名
	 * @see		
	 */
	char* pathname_to_name(char name[MAX_PATH], const char* path_name);
	std::string pathname_to_name(const char* path_name);

	/** 
	 * @brief 	pathname_to_noextname
	 *
	 * 	从全路径名中取出不包含文件扩展名的文件名信息
	 * 
	 * @author	jdyu
	 * @date	2004-6-23
	 * @return	char* 
	 * @param	char name[MAX_PATH]	- [out] 返回文件不包含扩展名的名称
	 * @param	const char* path_name	- [in] 需要取得路径信息的全路径名
	 * @see		
	 */
	char* pathname_to_noextname(char name[MAX_PATH], const char* path_name);
	std::string pathname_to_noextname(const char* path_name);
	char* pathname_to_extname  (char ext[MAX_PATH], const char* path_name, bool no_dot = true);
	std::string pathname_to_extname(const char* path_name, bool no_dot = true);

	/** 
	 * @brief 	get_file_full_path
	 *
	 * 	得到一个文件的全路径名
	 *  主要目的是获取和当前模块在同一个路径下的文件的全路径名
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	char* 
	 * @param	char path_name[]		- [in,out] 缓冲区
	 * @param	const char* file_name	- [in] 指定的文件名
	 * @see		
	 */
	char* get_file_full_path(char path_name[], const char* file_name, const char* rel_path = "bin");
	std::string get_file_full_path(const char * file_name, const char * rel_path = "bin");

	/** 
	 * @brief	get_file_version
	 *
	 *  获得指定文件的版本号信息
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	const char* 返回文件版本
	 * @param	const char* file	- [in] 指定的文件名
	 * @param	char* ver			- [out] 返回文件版本的缓冲区
	 * @see		
	 */
	const char* get_file_version(const char* file, char* ver);
	std::string get_file_version(const char* file);
	const char* get_prod_version(const char* file, char* ver);
	std::string get_prod_version(const char* file);

	int version_compare(const char * ver1, const char * ver2);
	/** 
	 * @brief 	get_lastest_prod_version
	 * 
	 * 	获得一个文件列表的最新的产品版本
	 * 
	 * @author	jdyu
	 * @date	2006-4-4
	 * @return	const char* 返回产品版本
	 * @param	const char* file_list	- [in] 一个文件列表，以逗号区分
	 * @param	char* ver	- [out] 返回版本信息的缓冲区
	 * @see		
	 */
	template <typename PROC>
	const char* get_lastest_version(const char* file_list, char* ver, PROC fn = get_prod_version)
		{
		ver[0] = 0;
		if ( file_list == 0 )
			return 0;

		str_arr arr;
		split_str(file_list, arr, " ,", true, true, true);

		for ( int i = 0; i < (int)arr.size(); i++ )
			{
			char file_ver[100];
			char file[255];
			sp::strsncpy(file, arr[i].c_str(), sizeof file);
			get_file_full_path(file, arr[i].c_str(), 0);

			// get version
			fn(file, file_ver);

			if ( version_compare(ver, file_ver) < 0 )
				sp::strsncpy(ver, file_ver, sizeof file_ver);
			}

		return ver;
		}
	// std version
	template <typename PROC>
	std::string get_lastest_version(const char* file_list, PROC fn = get_prod_version)
		{
		char ver[200]; ver[0] = 0;
		return get_lastest_version(file_list, ver, fn);
		}

	/** 
	 * @brief 	version_compare
	 * 
	 * 	比较两个版本的大小
	 * 
	 * @author	jdyu
	 * @date	2006-4-4
	 * @return	int	- 返回0表明版本相同，小于零表明前面的版本小于后面的版本，否则大于
	 * @param	const char * ver1	- [in] 第一个版本
	 * @param	const char * ver2	- [in] 第二个版本
	 * @see		
	 */
	int version_compare(const char * ver1, const char * ver2);

	/** 
	 * @brief 	get_file_list
	 *  
	 *  获得一个目录下的文件列表
	 *  
	 * @author	jdyu
	 * @date	2005-10-11
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char* dir			- [in]  指定的目录
	 * @param	str_list & flist		- [out] 返回的文件列表
	 * @param	const char* filter = 0	- [in]  使用的过滤器
	 * @param	bool recursive = true	- [in]  是否搜索子目录
	 * @param	ff_proc fproc = 0		- [in]  文件过滤的回调函数，如果被过滤掉返回false
	 * @see		
	 */
	struct ff_proc_default
		{
		bool operator()(const char * file) const
			{ return true; }
		};

	template<class LIST>
	int get_file_list(const char* dir, LIST & flist, const char* filter = 0, bool recursive = true);
	template<class LIST, class FF_PROC>
	int get_file_list(const char* dir, LIST & flist, const char* filter = 0, bool recursive = true, FF_PROC fproc = ff_proc_default());
	template<class LIST>
	int get_dir_list (const char* dir, LIST & dlist, const char* filter = 0, bool recursive = true);
	template<class LIST, class FF_PROC>
	int get_dir_list (const char* dir, LIST & dlist, const char* filter = 0, bool recursive = true, FF_PROC fproc = ff_proc_default());

	/** 
	 * @brief 	get_file_time
	 * 
	 * 	获得文件的创建时间和修改时间
	 * 
	 * @author	jdyu
	 * @date	2005-11-16
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char* file	- [in] 文件名称
	 * @param	time_t & tm_created	- [in] 文件创建时间
	 * @param	time_t & tm_modified	- [in] 文件修改时间
	 * @see		
	 */
	int get_file_time(const char* file, time_t & tm_created, time_t & tm_modified);
	time_t get_file_mtime(const char * file);
	time_t get_file_ctime(const char * file);

	/** 
	 * @brief 	get_file_size
	 * 
	 * 	获得文件的大小
	 * 
	 * @author	jdyu
	 * @date	2005-11-16
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char* file	- [in] 文件名称
	 * @param	unsigned int & size	- [in] 文件大小
	 * @see		
	 */
	int get_file_size(const char* file, unsigned int & size);

	/** 
	 * @brief 	is_text_file
	 * 
	 * 	判断一个文件是否文本类型的文件
	 * 
	 * @author	jdyu
	 * @date	2005-11-16
	 * @return	bool	- Return TRUE in success, otherwise return FALSE.
	 * @param	const char * file	- [in] 
	 * @see		
	 */
	bool is_text_file(const char * file, int maxs = 1024);

	/** 
	 * @brief 	mod_handle
	 * 
	 * 	获得当前模块的举柄，即HMODULE
	 * 
	 * @author	jdyu
	 * @date	2007-6-6
	 * @return	void * 返回当前模块的句柄，仅在Windows下有意义
	 * @see		
	 */
	IU_HANDLE mod_handle(void);

	/** 
	 * @brief 	parent_path
	 * 
	 * 	获得父目录
	 * 
	 * @author	jdyu
	 * @date	2007-6-7
	 * @return	std::string 
	 * @param	const char * path	- [in] 输入的路径
	 * @see		
	 */
	std::string parent_path(const char * path);

	/** 
	 * @brief 	system
	 * 
	 * 	执行一个系统命令，和crt里面的system相同
	 * 
	 * @author	jdyu
	 * @date	2007-7-3
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char * cmd	- [in] 需要执行的命令行
	 * @see		::system
	 */
	int system(const char * cmd);
	int system(const wchar_t * cmd);


	/** 
	 * @brief 	cat_audio_file
	 *  
	 *   把音频数据追加到一个音频文件中，支持wave、alaw、ulaw音频头
	 *  
	 * @author	jdyu
	 * @date	2007-7-20
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char * file	- [in] 音频文件名
	 * @param	const void * data	- [in] 音频数据
	 * @param	int bytes			- [in] 数据大小字节数
	 * @param	bool fail_if_exist = false	- [in] 如果文件存在是否失败
	 * @see		
	 */
	int cat_audio_file(const char * file, const void * data, int bytes, bool fail_if_exist = false);

	/** 
	 * @fn		read_text_file
	 * @brief	读取文件内容到字符串
	 * 
	 *  把文件的内容放在字符串
	 * 
	 * @author	jdyu
	 * @date	2008/9/22
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char * file	- [in] 
	 * @param	std::string & text	- [in] 
	 * @see		
	 */
	int read_text_file(const char * file, std::string & text);
	int read_text_file(const char * file, std::wstring & text);
};

// deprecated
namespace sp
{
	/** 
	 * @brief	get_module_cfg
	 * 
	 *  获得当前模块(exe或者dll)的cfg配置文件名称
	 * 
	 * @author	junyan
	 * @date	2004年7月7日
	 * @return	char* 
	 * @param	char path[MAX_PATH] [out] 返回可用的配置文件名称
	 * @param	const char* def_cfg [in] 缺省的配置文件名称（不包含路径）
	 * @param	IU_HANDLE hmod [in] 模块的句柄
	 * @see		
	 * @exception 
	 */
	char* get_module_cfg(char path[MAX_PATH], const char* def_cfg, IU_HANDLE hmod);
	std::string get_module_cfg(const char * def_cfg, IU_HANDLE hmod);

	/** 
	 * @brief 	install_path
	 *
	 * 	获得/设置当前系统的安装路径
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	const char* 
	 * @see		
	 */
	const char* install_path(const char * path = 0);
}

/**
 * ==========================================================================
 * 网络相关的操作
 * ==========================================================================
 */
namespace sp
{
	/** 
	 * @brief 	local_hostname
	 * 
	 *  返回当前主机名称，如果有完全限定域名（fully qualified domain name，FQDN）
	 *  则返回FQDN，否则返回简单主机名，第三个参数指明了是简单主机名，还是FQDN
	 *  
	 * @author	jingzhang
	 * @date	2006-10-19
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	char* hostname	- [out] 
	 * @param	size_t namelen	- [in] 
	 * @param	bool is_fqdn - [out]
	 * @see		
	 */
	int local_hostname(char* hostname, int namelen, bool& is_fqdn);

	/** 
	 * @brief	ip_count
	 *
	 *  返回当前主机上IP的数量
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	int IP数量
	 * @see		
	 */
	int ip_count(void);

	/** 
	 * @brief 	net_adpt_count
	 * 
	 * 	返回本机网卡的硬件数量
	 *  Windows下准确
	 * 
	 * @author	jdyu
	 * @date	2005-12-26
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @see		
	 */
	int net_adpt_count(void);

	/** 
	 * @brief	local_ip
	 *
	 *	获得本地unsigned long型的IP地址
	 * 
	 * @author	jdyu
	 * @date		2004-6-22
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	int& ip	- [in,out] 返回本地的Ip地址，如果失败，返回INADDR_NONE
	 * @see		
	 */
	int local_ip(unsigned long & ip);

	/** 
	 * @brief	local_ip
	 *
	 *  获得字串型的本地Ip地址
	 * 
	 * @author	jdyu
	 * @date		2004-6-22
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	char ip[ISP_MAX_IPADDRESS_LEN]	- [in,out] 返回ip地址
	 * @see		
	 */
	int local_ip(char ip[30]);

	/** 
	 * @brief 	local_ips
	 *
	 * 	获得本地所有IP列表
	 * 
	 * @author	jdyu
	 * @date		2004-6-22
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	str_arr lips - [out] 返回本地所有的IP
	 * @see		
	 */
	int local_ips(str_arr& lips);
	int local_ips(unsigned long ipa[], int & count);

	/** 
	 * @brief 	is_local_host
	 *
	 * 	验一个ip是否本机的IP
	 * 
	 * @author	jdyu
	 * @date		2004-6-22
	 * @return	bool	- Return true in success, otherwise return false.
	 * @param	unsigned long dwIp	- [in,out] 
	 * @see		
	 */
	bool is_local_host(unsigned long ip_addr);
	bool is_local_host(const char* ip_addr);

	/** 
	 * @brief 	is_valid_ip_addr
	 *  
	 *  判断IP地址是否合法，把那些保留的IP地址排除
	 *  
	 * @author	jdyu
	 * @date	2006-1-11
	 * @return	bool	- Return TRUE in success, otherwise return FALSE.
	 * @param	const char* ip_addr	- [in] 
	 * @see		
	 */
	bool is_valid_ip_addr(const char* ip_addr);
	const char * get_first_valid_ip_addr(const str_arr & ips);

	/** 
	 * @brief 	get_sub_net
	 *  
	 *  得到一个ｉｐ地址的网段地址
	 *  
	 * @author	jdyu
	 * @date	2006-1-11
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char * ip_addr	- [in] 
	 * @param	std::string & sub_net	- [in] 
	 * @see		
	 */
	int get_sub_net_addr(const char * ip_addr, std::string & sub_net);

	/** 
	 * @brief 	is_network_alive
	 * 
	 * 	检查网络状态的函数
	 * 
	 * @author	jdyu
	 * @date	2006-6-2
	 * @return	bool	- Return TRUE in success, otherwise return FALSE.
	 * @param	const char * intf	- [in] 
	 * @see		
	 */
	bool is_network_alive(const char * intf);
}

/**
 * ==========================================================================
 * Windows特有的一些操作
 * 操作系统相关的操作
 * ==========================================================================
 */
namespace sp
{
	/** 
	 * @brief 	get_common_path
	 *
	 * 	获得系统Program Files\Common Files目录的路径
	 * 
	 * @author	jdyu
	 * @date	2004-7-23
	 * @return	const char* 返回路径
	 * @see		
	 */
	const char* get_common_path(void);

	/** 
	 * @brief 	get_file_strinfo
	 *
	 * 	获得可执行文件相关信息
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	std::string 
	 * @param	const char* psz_info	- [in] 信息类型，取值类型有：
	 *				Comments			注释信息
	 *				CompanyName 
	 *				FileDescription 
	 *				FileVersion 
	 *				InternalName 
	 *				LegalCopyright 
	 *				LegalTrademarks 
	 *				OriginalFilename 
	 *				ProductName 
	 *				ProductVersion 
	 *				PrivateBuild 
	 *				SpecialBuild 
	 * @param	const char* file = 0	- [in] 文件路径名
	 * @see		
	 */
	std::string get_file_strinfo(const char* psz_info, const char* file = 0);

	/** 
	 * @brief 	EnableDebugPrivNT
	 *
	 * 	调整进程的优先级为Debugger级
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	bool	- Return true in success, otherwise return false.
	 * @see		
	 */
	bool EnableDebugPrivNT(void);

	/** 
	 * @brief 	interact_with_user_open
	 *
	 * 	开启本线程和指定用户桌面的交互功能，返回打开的句柄，必须在
	 *  interact_with_user_close函数中释放
	 *
	 * @author	jdyu
	 * @date	2004-11-2
	 * @return	IU_HANDLE 返回本次打开的句柄
	 * @param	const char* stat = "WinSta0"	- [in] the user's window station name
	 * @param	const char* desk = "Default"	- [in] desktop name
	 * @see		
	 */
	IU_HANDLE interact_with_user_open(const char* stat = "WinSta0", const char* desk = "Default");

	/** 
	 * @brief 	interact_with_user_close
	 *
	 * 	关闭本线程和指定用户桌面的交互功能
	 * 
	 * @author	jdyu
	 * @date	2004-11-2
	 * @return	BOOL	- Return TRUE in success, otherwise return FALSE.
	 * @param	IU_HANDLE hiwu	- [in] 由interact_with_user_open函数返回的句柄
	 * @see		
	 */
	bool interact_with_user_close(IU_HANDLE hiwu);

	// utsname
	struct utsname
	{
		char sysname [257];
		char nodename[257];
		char release [257];
		char version [257];
		char machine [257];
	};
	/** 
	 * @brief 	获取主机信息
	 *
	 *	对于Windows操作系统，部分信息是从注册表中读取
	 * 
	 * @author	yigao
	 * @date	2004-9-14
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @see		
	 */
	int uname(utsname * name);

	/** 
	 * @brief 	error_desc
	 *
	 * 	返回错误信息的描述
	 * 
	 * @author	jdyu
	 * @date	2004-8-17
	 * @return	const char* 
	 * @see		
	 */
	std::string error_desc(void);
};

/**
 * ==========================================================================
 * Linux特有的一些操作
 * ==========================================================================
 */
namespace sp
{
	/** 
	 * @brief 	make_daemon
	 *
	 * 	生成守护进程，后台运行服务程序
	 * 
	 * @author	hjma
	 * @date	2006-2-6
	 * @see		
	 */
	int make_daemon(void);

	/** 
	 * @brief 	get_daemon_pid
	 *
	 * 	获取守护进程的id以进行操作
	 * 
	 * @author	hjma
	 * @date	2006-2-7
	 * @see		
	 */
	long get_daemon_pid(const char* svc_name);
};

// 字符串操作的实现放在sputils.i文件，不需要cpp文件
#pragma warning(push)
#pragma warning(disable : 4996)
#include "sutils.i"
#pragma warning(pop)

#endif /* __SPUTILS_H__ */
