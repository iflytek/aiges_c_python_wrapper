/** 
 * @file	ISPUtils.h
 * @brief	���ú�����
 *
 *  ʹ�õ�һЩ���ú���������ļ��������һЩ����ʹ�õĺ���
 *  ���ǵ������ṩ�ĺ������ڶ����Ŀ��ʹ�ã���˲�Ҫ�������ã�
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
 *  <tr> <td>1.1		<td>2005-9-29	<td>jdyu	<td>����������</tr>
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

/// �����������
#define ISP_MAXCOMPUTERNAME			16

/// б�ܶ���
#define BSLASH_CHR					'\\'
#define BSLASH_STR					"\\"
#define SLASH_CHR					'/'
#define SLASH_STR					"/"

/// ·�����ַ�
#ifndef PATH_CHR
# ifdef WIN32
#  define PATH_CHR					BSLASH_CHR
#  define PATH_STR					BSLASH_STR
#else
#  define PATH_CHR					SLASH_CHR
#  define PATH_STR					SLASH_STR
# endif // WIN32
#endif  // PATH_CHR

// �汾��
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
// �ֽ������
////////////////////////////////////////////////////////////////////////
// �����ֽڵ�WORD�ߵ�λ��ת
#define SP_WORD_LE_BE(val)			((SP_WORD) ( (((SP_WORD) (val) & (SP_WORD) 0x00ffU) << 8) | \
												 (((SP_WORD) (val) & (SP_WORD) 0xff00U) >> 8)))
// �ĸ��ֽڵ�DWORD�ߵ�λ��ת
#define SP_DWORD_LE_BE(val)			((SP_DWORD) ( (((SP_DWORD) (val) & (SP_DWORD) 0x000000ffU) << 24) | \
												  (((SP_DWORD) (val) & (SP_DWORD) 0x0000ff00U) <<  8) | \
                                                  (((SP_DWORD) (val) & (SP_DWORD) 0x00ff0000U) >>  8) | \
												  (((SP_DWORD) (val) & (SP_DWORD) 0xff000000U) >> 24)))

////////////////////////////////////////////////////////////////////////
/// �쳣����ĺ�
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
 * ͨ�ò���
 * ==========================================================================
 */

/** 
 * @brief	ͨ�ò���
 * 
 *  ������ֿռ�����������ڱ����о����õ���һЩ���ܣ���Ϊ��
 *    1.�ַ�������
 *    2.�ļ�����
 *    3.�������
 *    4.����ϵͳ����
 *  ������ķ��࣬����ʹ�á�
 * 
 * @author	jdyu
 * @date	2007-7-5
 * 
 * @see		
 * 
 */
namespace sp
{
	// �ַ�������
	typedef std::vector<std::string>	str_arr;
	typedef std::vector<std::wstring>	wstr_arr;
	typedef std::list  <std::string>	str_list;
	typedef std::list  <std::wstring>	wstr_list;

	// �ַ���ӳ���
	typedef std::pair<std::string, std::string>		str_pair;
	typedef std::pair<std::wstring, std::wstring>	wstr_pair;
	typedef std::map <std::string, std::string>		str_map;
	typedef str_map::value_type		 sm_item;
	typedef str_map::iterator		 sm_it;
	typedef str_map::const_iterator	 sm_cit;
	// �ַ����б�
	typedef std::vector<str_pair>	 str_pair_arr;
	typedef std::vector<wstr_pair>	 wstr_pair_arr;

	/** 
	 * @brief 	last_error
	 * 	
	 *  ��ô������
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
	 * 	��ָ����ֵ�淶����ָ��������
	 * 
	 * @author	jdyu
	 * @date	2004-7-15
	 * @return	template<typename TYPE>TYPE 
	 * @param	const TYPE& val	- [in] ����������ı���
	 * @param	const TYPE& min	- [in] ����
	 * @param	const TYPE& max	- [in] ����
	 * @see		
	 */
	template<typename TYPE>
	TYPE get_val_in_range(const TYPE& val, const TYPE& min, const TYPE& max)
		{ if ( val < min ) return min; if ( val > max ) return max; return val; }

	/** 
	 * @brief 	ln2
	 *
	 * 	��һ��������2Ϊ�׵�������
	 * 
	 * @author	jdyu
	 * @date	2004-9-2
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	int pw	- [in] ���������
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
	 * 	���õ�ǰ�Ƿ�Ϊdebugginģʽ����debuggingģʽ�£�
	 *  һЩ����������϶����Ϣ
	 * 
	 * @author	jdyu
	 * @date	2006-2-28
	 * @return	No return value.
	 * @param	bool mode	- [in] �����Ƿ�Ϊ����ģʽ
	 * @see		
	 */
	bool debugging_mode(bool mode);
	bool debugging_mode(void);
};

/**
 * ==========================================================================
 * �ַ�����صĲ���
 * ==========================================================================
 */
namespace sp
{
	/** 
	 * @brief 	is_quanjiao
	 *
	 * 	�ж��Ƿ�Ϊȫ���ַ�
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
	 * 	ȥ���ַ���ͷβ�Ŀո�������ַ������������ַ�
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	int ���ش����ո����ַ�������
	 * @param	char* pstr	- [in,out] 
	 * @param	const char ctrim = ' '	- [in] ��Ҫ�޳��ķ���
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
	 *	���ָ���ַ����������е�����
	 *
	 * @author	yigao
	 * @date	2004��6��23��
	 * @return	int 
	 * @param	const char *str [in]��Ҫ��ѯ���ַ���
	 * @param	const char *arr[] [in]��Ҫ��ѯ���ַ�������
	 * @param	int arr_size [in]�����С
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
	 * 	��h:m:s��ʽ���ַ���ת����Ϊ��ֵ
	 * 
	 * @author	jdyu
	 * @date	2004-7-15
	 * @return	int 
	 * @param	const char* strtm	- [in] ���з��ַ���
	 * @param	int & hour	- [out] ����Сʱ
	 * @param	int & min	- [out] ���ط���
	 * @param	int & sec	- [out] ������
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
	// ����·ݵ��ַ���
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
	 * 	��ָ�����ַ����з�Ϊ����Ӵ�
	 *  ��ָ���ķָ��ַ�Ϊ�ֽ�
	 *
	 * @author	jdyu
	 * @date	2004-7-16
	 * @return	int 
	 * @param	const char* str - [in] ������Ҫ�зֵ��ַ���
	 * @param	str_arr & subs_list - [out] �����зֺõ��ַ�������
	 * @param	const char spliter[]	- [in] �ָ���б�
	 * @param	bool trim = true		- [in] �Ƿ���зֵĽ��ȥ�����ߵĿո�
	 * @param	bool ignore_blank = false	- [in] �Ƿ���Կյĵ�Ԫ
	 * @param	bool supp_quote = false	- [in] �Ƿ�֧�����ţ������ڲ����з�
	 * @param	bool supp_quanjiao = false	- [in] �Ƿ�֧��ȫ��
	 * @see		
	 */
	int split_str(const char * str, str_arr & subs_array, const char spliter[] = ",;:", bool trim = true, bool ignore_blank = false, bool supp_quote = false, bool trim_quote = true, bool supp_quanjiao = false);
	int split_str(const wchar_t * str, wstr_arr & subs_array, const wchar_t spliter[] = L",;", bool trim = true, bool ignore_blank = false, bool supp_quote = false, bool trim_quote = true);
	int split_str(const char * str, wstr_arr & subs_array, const wchar_t spliter[] = L",;", bool trim = true, bool ignore_blank = false, bool supp_quote = false, bool trim_quote = true);
	int split_str(const wchar_t * str, str_arr & subs_array, const char spliter[] = ",;:", bool trim = true, bool ignore_blank = false, bool supp_quote = false, bool trim_quote = true);

	/** 
	 * @brief 	dequote_str
	 *  
	 *  ȥ���ַ������ߵ����ţ������ǶԳƵ�����
	 *  
	 * @author	jdyu
	 * @date	2005-11-28
	 * @return	char * 
	 * @param	char * str	- [in/out] ��ȥ�����ַ���
	 * @see		
	 */
	char * dequote_str(char * str);
	wchar_t * dequote_str(wchar_t * str);

	template<typename char_t>
	std::basic_string<char_t> quote_str(const char_t * cstr, char_t quote_chr = char_t('"'));

	/** 
	 * @brief 	get_attrib_and_value
	 *  
	 *  ��������attr="value"�����ԶԷֽ⿪��
	 *  
	 * @author	jdyu
	 * @date	2005-11-23
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char* attr_val	- [in] ��������Զ�
	 * @param	std::string & attr	- [in] ������
	 * @param	std::string & val	- [in] ����ֵ
	 * @see		
	 */
	int get_attrib_and_value(const char* attr_val, std::string & attr, std::string & val, const char * split = "=", bool trim = false);
	int get_attrib_and_value(const wchar_t* attr_val, std::wstring & attr, std::wstring & val, const wchar_t * split = L"=", bool trim = false);

	/** 
	 * @brief 	str_to_attr_vals
	 *  
	 *  ��������attr1="value1" attr2="value2"�����Զ��б��ֳ�Ϊ
	 *  Ӱ�����ʽ
	 *  
	 * @author	jdyu
	 * @date	2005-11-23
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char* str	- [in] ��Ҫ��ȡ��Դ�ַ���
	 * @param	str_map & attrs	- [in] ������ȡ���
	 * @param	bool trim = false	- [in] �Ƿ�ȥ���ո�
	 * @param	bool supp_quote = false	- [in] �Ƿ�֧������
	 * @see		
	 */
	int str_to_attr_vals(const char* str, str_map & attrs, bool trim = false, bool supp_quote = false, const char * splt = " ;,", const char * token = "=");
	int str_to_attr_vals(const char* str, str_pair_arr & attrs, bool trim = false, bool supp_quote = false, const char * splt = " ;,", const char * token = "=");

	/** 
	 * @brief 	strlwr
	 * 
	 * 	�ַ���Сд������д��
	 * 
	 * @author	jdyu
	 * @date	2007-5-30
	 * @return	std::string 
	 * @param	const std::string str	- [in] ����Դ�ַ���
	 * @see		
	 */
	std::string & strlwr(std::string & str);
	std::string & strupr(std::string & str);
	std::wstring & strlwr(std::wstring & str);
	std::wstring & strupr(std::wstring & str);

	/** 
	 * @brief 	str_replace
	 * 
	 * 	�ַ����滻
	 * 
	 * @author	jdyu
	 * @date	2007-5-30
	 * @return	std::string 
	 * @param	const std::string str	- [in] ����Դ�ַ���
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
	 * 	���Դ�Сд�ıȽ�
	 * 
	 * @author	jdyu
	 * @date	2007-5-30
	 * @return	int	- �ȽϵĽ������strcmp��ͬ
	 * @param	const char * str1	- [in] �ַ���һ
	 * @param	const char * str2	- [in] �ַ�����
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
	 * 	�ַ������汾�Ͱ汾���ַ�����ת��
	 * 
	 * @author	jdyu
	 * @date	2007-5-30
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char * str	- [in] 
	 * @param	int & ver	- [out] ���汾.�ΰ汾.�޶���
	 * @param	int & build	- [out] ��ˮ��
	 * @see		
	 */
	int str2ver(const char * str, int & ver, int & build);
	std::string ver2str(int ver, int build);

	/**
	 * @brief 	��ͨ���'*'��'?'���ܵ��Ӵ�����
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
	 * 	�Ƚ�����ʱ�䣬�������Ϊ-1����ʾ��λ��Ч
	 * 
	 * @author	jdyu
	 * @date	2004-10-18
	 * @return	int - <0��ʾǰ���ʱ���磬0��ʾ��ȣ�>0��ʾǰ���ʱ����
	 * @see		
	 */
	int	timecmp(int hour1, int min1, int sec1, int hour2, int min2, int sec2, int day1 = -1, int day2 = -1);

	/** 
	 * @brief 	is_register_key
	 * 
	 * 	�ж��Ƿ���һ��ע������
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
	 * 	���ַ���խ�ַ��Ļ�ת
	 * 
	 * @author	jdyu
	 * @date	2007-6-6
	 * @return	std::wstring 
	 * @param	const char * str		- [in]  �����ת�����ַ���
	 * @param	size_t * cvt_chrs = 0	- [out] ����ʵ��ת�����ַ����� 
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

	// ��խת����linuxֱ��ת
	size_t mbstowcs(wchar_t *wcstr, const char *mbstr, size_t count);
	size_t wcstombs(char *mbstr, const wchar_t *wcstr, size_t count);

	/** 
	 * @brief 	strtoi
	 *  
	 *  ���ַ���ת��Ϊֵ
	 *  
	 * @author	jdyu
	 * @date	2007-6-26
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char * str	- [in] �����ַ���
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
	 *	��õ�ǰʱ��
	 * 
	 * @author	jdyu
	 * @date	2004��5��21��
	 * @return	const char* 
	 * @param	char times[]	- [out] ����ʱ����Ϣ�Ļ�����
	 * @param	bool no_path	- [in]  �Ƿ񲻰���·����ֹ�ķ���
	 * @see		
	 */
	const char * cur_time(char times[], bool no_path = false, bool sht_fmt = false, bool has_msec = true);
	std::string  cur_time(bool no_path = false, bool sht_fmt = false, bool has_msec = true);
	std::wstring wcur_time(bool no_path, bool sht_fmt, bool has_msec = true);
};

/**
 * ==========================================================================
 * �ļ���صĲ���
 * ==========================================================================
 */
namespace sp
{
	/** 
	 * @brief 	is_file_exist
	 *
	 * 	���һ���ļ��Ƿ����
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	bool	- Return true in success, otherwise return false.
	 * @param	const char* lpszFile	- [in] Ҫ�����ļ���
	 * @see		
	 */
	bool is_file_exist(const char * file);
	bool is_file_exist(const wchar_t * file);

	/** 
	 * @brief 	is_dir_exist
	 *
	 * 	���һ��Ŀ¼�Ƿ����
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	bool	- Return true in success, otherwise return false.
	 * @param	const char* lpszDir	- [in] Ŀ¼��
	 * @see		
	 */
	bool is_dir_exist(const char * dir);
	bool is_dir_exist(const wchar_t * dir);
	// ����Ŀ¼
	int create_directory(const char * dir, bool fail_if_exist = false, bool recursive = true);
	int path_to_dir_tree(const char * path, str_arr & dir_tree);

	/** 
	 * @brief 	normalize_path
	 *
	 * 	��һ��·����һ�������ǰ����е�"\"��"/"ת��Ϊָ����·���ָ���
	 *  ͬʱȥ������ķ���
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	char* path	- [in,out] ·����Ϣ
	 * @param	const char path_split = PATH_CHR - [in] ·�����ַ���
	 * @see		
	 */
	int normalize_path(char path[MAX_PATH], const char path_split = PATH_CHR);
	std::string normalize_path(const char * path, const char path_split = PATH_CHR);

	/** 
	 * @brief 	cat_path
	 *
	 * 	������·����Ϣƴ������,�� path + more
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	const char* ����ƴ�Ӻ�ĵ�ַ������lppath1
	 * @param	char* path	- [in,out] ǰ·��������ƴ�Ϻ��·��
	 * @param	const char* more	- [in] ��·��
	 * @see		
	 */
	const char* cat_path(char* path, const char* more, const char path_split = PATH_CHR);
	std::string cat_path(const char * path, const char * more, const char path_split = PATH_CHR);

	/** 
	 * @brief 	is_abs_path
	 *
	 * 	�Ƿ����·��
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	inline 
	 * @param	const char* path	- [in] ������·��
	 * @see		
	 */
	bool is_abs_path(const char* path);

	/** 
	 * @brief 	get_bin_path
	 *
	 * 	��õ�ǰ��ִ���ļ���·��
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	char* ���ؿ�ִ���ļ���·��
	 * @param	char szPath[MAX_PATH] - [out] ���뻺���������ؿ�ִ���ļ���·��
	 * @param	IU_HANDLE hmod = 0	- [in] ����ģ��ľ����ȱʡΪ��ǰEXEģ�飬
	 *				�������IU_INVALID_HANDLE��˵����ȡ��ǰ��������ģ�飨EXE��DLL��
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
	 * 	���ģ���·����
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	char* ����ģ���·����
	 * @see		get_bin_path
	 */
	//char* get_bin_pathname(char pathname[MAX_PATH], IU_HANDLE hmod = 0);//yfxie
	//std::string get_bin_pathname(IU_HANDLE hmod = 0);
	char* get_bin_pathname(const void* func, char pathname[MAX_PATH], IU_HANDLE hmod = 0);
	std::string get_bin_pathname(const void* func, IU_HANDLE hmod = 0);
	
	/** 
	 * @brief 	get_bin_name
	 *
	 * 	��õ�ǰģ��Ĳ�����·�����ļ���
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	char * ����ģ���·����
	 * @see		get_bin_path
	 */
	char* get_bin_name(char name[MAX_PATH], IU_HANDLE hmod = 0);
	std::string get_bin_name(IU_HANDLE hmod = 0);

	/** 
	 * @brief 	pathname_to_path
	 *
	 * 	��ȫ·������ȡ��·����Ϣ
	 * 
	 * @author	jdyu
	 * @date	2004-6-23
	 * @return	char* 
	 * @param	char szPath[MAX_PATH]	- [out] �����ļ���·��
	 * @param	const char* path_name	- [in] ��Ҫȡ��·����Ϣ��ȫ·����
	 * @see		
	 */
	char* pathname_to_path(char path[MAX_PATH], const char* path_name, const char path_split = PATH_CHR);
	std::string pathname_to_path(const char* path_name, const char path_split = PATH_CHR);

	/** 
	 * @brief 	pathname_to_name
	 *
	 * 	��ȫ·������ȡ���ļ�����Ϣ
	 * 
	 * @author	jdyu
	 * @date	2004-6-23
	 * @return	char* 
	 * @param	char name[MAX_PATH]	- [out] �����ļ�������
	 * @param	const char* path_name	- [in] ��Ҫȡ��·����Ϣ��ȫ·����
	 * @see		
	 */
	char* pathname_to_name(char name[MAX_PATH], const char* path_name);
	std::string pathname_to_name(const char* path_name);

	/** 
	 * @brief 	pathname_to_noextname
	 *
	 * 	��ȫ·������ȡ���������ļ���չ�����ļ�����Ϣ
	 * 
	 * @author	jdyu
	 * @date	2004-6-23
	 * @return	char* 
	 * @param	char name[MAX_PATH]	- [out] �����ļ���������չ��������
	 * @param	const char* path_name	- [in] ��Ҫȡ��·����Ϣ��ȫ·����
	 * @see		
	 */
	char* pathname_to_noextname(char name[MAX_PATH], const char* path_name);
	std::string pathname_to_noextname(const char* path_name);
	char* pathname_to_extname  (char ext[MAX_PATH], const char* path_name, bool no_dot = true);
	std::string pathname_to_extname(const char* path_name, bool no_dot = true);

	/** 
	 * @brief 	get_file_full_path
	 *
	 * 	�õ�һ���ļ���ȫ·����
	 *  ��ҪĿ���ǻ�ȡ�͵�ǰģ����ͬһ��·���µ��ļ���ȫ·����
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	char* 
	 * @param	char path_name[]		- [in,out] ������
	 * @param	const char* file_name	- [in] ָ�����ļ���
	 * @see		
	 */
	char* get_file_full_path(char path_name[], const char* file_name, const char* rel_path = "bin");
	std::string get_file_full_path(const char * file_name, const char * rel_path = "bin");

	/** 
	 * @brief	get_file_version
	 *
	 *  ���ָ���ļ��İ汾����Ϣ
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	const char* �����ļ��汾
	 * @param	const char* file	- [in] ָ�����ļ���
	 * @param	char* ver			- [out] �����ļ��汾�Ļ�����
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
	 * 	���һ���ļ��б�����µĲ�Ʒ�汾
	 * 
	 * @author	jdyu
	 * @date	2006-4-4
	 * @return	const char* ���ز�Ʒ�汾
	 * @param	const char* file_list	- [in] һ���ļ��б��Զ�������
	 * @param	char* ver	- [out] ���ذ汾��Ϣ�Ļ�����
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
	 * 	�Ƚ������汾�Ĵ�С
	 * 
	 * @author	jdyu
	 * @date	2006-4-4
	 * @return	int	- ����0�����汾��ͬ��С�������ǰ��İ汾С�ں���İ汾���������
	 * @param	const char * ver1	- [in] ��һ���汾
	 * @param	const char * ver2	- [in] �ڶ����汾
	 * @see		
	 */
	int version_compare(const char * ver1, const char * ver2);

	/** 
	 * @brief 	get_file_list
	 *  
	 *  ���һ��Ŀ¼�µ��ļ��б�
	 *  
	 * @author	jdyu
	 * @date	2005-10-11
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char* dir			- [in]  ָ����Ŀ¼
	 * @param	str_list & flist		- [out] ���ص��ļ��б�
	 * @param	const char* filter = 0	- [in]  ʹ�õĹ�����
	 * @param	bool recursive = true	- [in]  �Ƿ�������Ŀ¼
	 * @param	ff_proc fproc = 0		- [in]  �ļ����˵Ļص���������������˵�����false
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
	 * 	����ļ��Ĵ���ʱ����޸�ʱ��
	 * 
	 * @author	jdyu
	 * @date	2005-11-16
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char* file	- [in] �ļ�����
	 * @param	time_t & tm_created	- [in] �ļ�����ʱ��
	 * @param	time_t & tm_modified	- [in] �ļ��޸�ʱ��
	 * @see		
	 */
	int get_file_time(const char* file, time_t & tm_created, time_t & tm_modified);
	time_t get_file_mtime(const char * file);
	time_t get_file_ctime(const char * file);

	/** 
	 * @brief 	get_file_size
	 * 
	 * 	����ļ��Ĵ�С
	 * 
	 * @author	jdyu
	 * @date	2005-11-16
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char* file	- [in] �ļ�����
	 * @param	unsigned int & size	- [in] �ļ���С
	 * @see		
	 */
	int get_file_size(const char* file, unsigned int & size);

	/** 
	 * @brief 	is_text_file
	 * 
	 * 	�ж�һ���ļ��Ƿ��ı����͵��ļ�
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
	 * 	��õ�ǰģ��ľٱ�����HMODULE
	 * 
	 * @author	jdyu
	 * @date	2007-6-6
	 * @return	void * ���ص�ǰģ��ľ��������Windows��������
	 * @see		
	 */
	IU_HANDLE mod_handle(void);

	/** 
	 * @brief 	parent_path
	 * 
	 * 	��ø�Ŀ¼
	 * 
	 * @author	jdyu
	 * @date	2007-6-7
	 * @return	std::string 
	 * @param	const char * path	- [in] �����·��
	 * @see		
	 */
	std::string parent_path(const char * path);

	/** 
	 * @brief 	system
	 * 
	 * 	ִ��һ��ϵͳ�����crt�����system��ͬ
	 * 
	 * @author	jdyu
	 * @date	2007-7-3
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char * cmd	- [in] ��Ҫִ�е�������
	 * @see		::system
	 */
	int system(const char * cmd);
	int system(const wchar_t * cmd);


	/** 
	 * @brief 	cat_audio_file
	 *  
	 *   ����Ƶ����׷�ӵ�һ����Ƶ�ļ��У�֧��wave��alaw��ulaw��Ƶͷ
	 *  
	 * @author	jdyu
	 * @date	2007-7-20
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char * file	- [in] ��Ƶ�ļ���
	 * @param	const void * data	- [in] ��Ƶ����
	 * @param	int bytes			- [in] ���ݴ�С�ֽ���
	 * @param	bool fail_if_exist = false	- [in] ����ļ������Ƿ�ʧ��
	 * @see		
	 */
	int cat_audio_file(const char * file, const void * data, int bytes, bool fail_if_exist = false);

	/** 
	 * @fn		read_text_file
	 * @brief	��ȡ�ļ����ݵ��ַ���
	 * 
	 *  ���ļ������ݷ����ַ���
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
	 *  ��õ�ǰģ��(exe����dll)��cfg�����ļ�����
	 * 
	 * @author	junyan
	 * @date	2004��7��7��
	 * @return	char* 
	 * @param	char path[MAX_PATH] [out] ���ؿ��õ������ļ�����
	 * @param	const char* def_cfg [in] ȱʡ�������ļ����ƣ�������·����
	 * @param	IU_HANDLE hmod [in] ģ��ľ��
	 * @see		
	 * @exception 
	 */
	char* get_module_cfg(char path[MAX_PATH], const char* def_cfg, IU_HANDLE hmod);
	std::string get_module_cfg(const char * def_cfg, IU_HANDLE hmod);

	/** 
	 * @brief 	install_path
	 *
	 * 	���/���õ�ǰϵͳ�İ�װ·��
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
 * ������صĲ���
 * ==========================================================================
 */
namespace sp
{
	/** 
	 * @brief 	local_hostname
	 * 
	 *  ���ص�ǰ�������ƣ��������ȫ�޶�������fully qualified domain name��FQDN��
	 *  �򷵻�FQDN�����򷵻ؼ�������������������ָ�����Ǽ�������������FQDN
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
	 *  ���ص�ǰ������IP������
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	int IP����
	 * @see		
	 */
	int ip_count(void);

	/** 
	 * @brief 	net_adpt_count
	 * 
	 * 	���ر���������Ӳ������
	 *  Windows��׼ȷ
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
	 *	��ñ���unsigned long�͵�IP��ַ
	 * 
	 * @author	jdyu
	 * @date		2004-6-22
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	int& ip	- [in,out] ���ر��ص�Ip��ַ�����ʧ�ܣ�����INADDR_NONE
	 * @see		
	 */
	int local_ip(unsigned long & ip);

	/** 
	 * @brief	local_ip
	 *
	 *  ����ִ��͵ı���Ip��ַ
	 * 
	 * @author	jdyu
	 * @date		2004-6-22
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	char ip[ISP_MAX_IPADDRESS_LEN]	- [in,out] ����ip��ַ
	 * @see		
	 */
	int local_ip(char ip[30]);

	/** 
	 * @brief 	local_ips
	 *
	 * 	��ñ�������IP�б�
	 * 
	 * @author	jdyu
	 * @date		2004-6-22
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	str_arr lips - [out] ���ر������е�IP
	 * @see		
	 */
	int local_ips(str_arr& lips);
	int local_ips(unsigned long ipa[], int & count);

	/** 
	 * @brief 	is_local_host
	 *
	 * 	��һ��ip�Ƿ񱾻���IP
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
	 *  �ж�IP��ַ�Ƿ�Ϸ�������Щ������IP��ַ�ų�
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
	 *  �õ�һ������ַ�����ε�ַ
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
	 * 	�������״̬�ĺ���
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
 * Windows���е�һЩ����
 * ����ϵͳ��صĲ���
 * ==========================================================================
 */
namespace sp
{
	/** 
	 * @brief 	get_common_path
	 *
	 * 	���ϵͳProgram Files\Common FilesĿ¼��·��
	 * 
	 * @author	jdyu
	 * @date	2004-7-23
	 * @return	const char* ����·��
	 * @see		
	 */
	const char* get_common_path(void);

	/** 
	 * @brief 	get_file_strinfo
	 *
	 * 	��ÿ�ִ���ļ������Ϣ
	 * 
	 * @author	jdyu
	 * @date	2004-6-22
	 * @return	std::string 
	 * @param	const char* psz_info	- [in] ��Ϣ���ͣ�ȡֵ�����У�
	 *				Comments			ע����Ϣ
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
	 * @param	const char* file = 0	- [in] �ļ�·����
	 * @see		
	 */
	std::string get_file_strinfo(const char* psz_info, const char* file = 0);

	/** 
	 * @brief 	EnableDebugPrivNT
	 *
	 * 	�������̵����ȼ�ΪDebugger��
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
	 * 	�������̺߳�ָ���û�����Ľ������ܣ����ش򿪵ľ����������
	 *  interact_with_user_close�������ͷ�
	 *
	 * @author	jdyu
	 * @date	2004-11-2
	 * @return	IU_HANDLE ���ر��δ򿪵ľ��
	 * @param	const char* stat = "WinSta0"	- [in] the user's window station name
	 * @param	const char* desk = "Default"	- [in] desktop name
	 * @see		
	 */
	IU_HANDLE interact_with_user_open(const char* stat = "WinSta0", const char* desk = "Default");

	/** 
	 * @brief 	interact_with_user_close
	 *
	 * 	�رձ��̺߳�ָ���û�����Ľ�������
	 * 
	 * @author	jdyu
	 * @date	2004-11-2
	 * @return	BOOL	- Return TRUE in success, otherwise return FALSE.
	 * @param	IU_HANDLE hiwu	- [in] ��interact_with_user_open�������صľ��
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
	 * @brief 	��ȡ������Ϣ
	 *
	 *	����Windows����ϵͳ��������Ϣ�Ǵ�ע����ж�ȡ
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
	 * 	���ش�����Ϣ������
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
 * Linux���е�һЩ����
 * ==========================================================================
 */
namespace sp
{
	/** 
	 * @brief 	make_daemon
	 *
	 * 	�����ػ����̣���̨���з������
	 * 
	 * @author	hjma
	 * @date	2006-2-6
	 * @see		
	 */
	int make_daemon(void);

	/** 
	 * @brief 	get_daemon_pid
	 *
	 * 	��ȡ�ػ����̵�id�Խ��в���
	 * 
	 * @author	hjma
	 * @date	2006-2-7
	 * @see		
	 */
	long get_daemon_pid(const char* svc_name);
};

// �ַ���������ʵ�ַ���sputils.i�ļ�������Ҫcpp�ļ�
#pragma warning(push)
#pragma warning(disable : 4996)
#include "sutils.i"
#pragma warning(pop)

#endif /* __SPUTILS_H__ */
