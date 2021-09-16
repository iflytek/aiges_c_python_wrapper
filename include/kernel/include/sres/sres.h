/**
* @file	sres.h
* @brief	资源管理的统一定义
*
* 这个文件是系统所有资源的接口定义，主要包含：
*	1.资源基本格式的定义，资源头定义
*	2.资源基类的定义和实现
*	3.资源包和资源管理接口的定义
*
* @author	jdyu
* @version	1.2
* @date	2002-3-5
*
* @see
*
* <b>History:</b><br>
* <table>
*  <tr> <th>Version	<th>Date		<th>Author	<th>Notes</tr>
*  <tr> <td>1.0		<td>2002-3-6	<td>jdyu	<td>Create this file</tr>
*  <tr> <td>1.1		<td>2002-8-5	<td>jdyu	<td>对于原来一些不合理的地方进行修改(资源包的缺省，缺省音库等)</tr>
*  <tr> <td>1.2		<td>2005-10-11	<td>jdyu	<td>根据架构改进的需求进行修改</tr>
* </table>
*
*/
#ifndef __SRES_H__
#define __SRES_H__

#include <errno.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include "utils/endian_op.h"

/* SRESAPI define */
#ifdef	WIN32
#  define	SRESAPI /* __stdcall */
#else
#  define	SRESAPI
#endif /* #ifdef WIN32 */

#if _MSC_VER > 1300
#  pragma warning(push) 
#  pragma warning(disable : 4996)
#endif /* _MSC_VER > 1300 */

// log io interface
struct Log_IO;

/** 
* @brief	通用资源名字空间
* 
*  所有与具体系统无关的资源代码空间，包含资源管理所有的支撑代码
*  使用这里定义的一系列类和方法可以很快组织一个三层架构的资源管理框架
* 
* @author	jdyu
* @date	2007-7-5
* 
* @see		res
* 
*/
namespace sres {

	/* data type defines */
//#define SR_DWORD		unsigned long
#define SR_DWORD		unsigned int //changed by llgu Windows下和Linux下位宽不一样
#define SR_WORD			unsigned short
#define SR_BYTE			unsigned char
#define SR_UINT			unsigned int
#define SR_TIME			long
#define SRES_HANDLE		void *

	/* C++ types */
	typedef std::vector	<std::string> SR_STR_ARR;
	typedef std::pair	<std::string, std::string> SR_STR_PAIR;
	typedef std::vector	<SR_STR_PAIR> SR_STRPAIR_ARR;

	/* interface */
	struct SRes_head;
	struct SRes_Pack;
	struct SRes_Pack_Mngr;
	struct SRes_Cfg;
	class  SRes_Pack_Cfg;
	class  SRes_Pack_Mngr_Cfg;

	////////////////////////////////////////////////////////////////////////
	// 资源定义中需要的一些宏
	////////////////////////////////////////////////////////////////////////

	// 资源头缺省长度
#define SRES_DEFHEADSIZE			1024
	// 资源名称的最大长度
#define SRES_MAXNAMELEN				32
#define SRES_MAXDESLEN				256
	// 最大标志信息长度
#define SRES_MAXFLAGLEN				32
	// 最大作者名称长度
#define SRES_MAXAUTHORLEN			16
	// 最大公司名称长度
#define SRES_MAXCOMPANYLEN			32
	// 最大资源热身文件数量
#define SRES_MAXWARMUPFILE			10
	// 最大Bundle名的长度
#define SRES_MAXBUNDLENAME			16

	// 资源版本
#define SR_MAKEWORD(a, b)			((SR_WORD)(((SR_BYTE)((SR_DWORD)(a) & 0xff)) | ((SR_WORD)((SR_BYTE)((SR_DWORD)(b) & 0xff))) << 8))
#define SR_MAKELONG(a, b)			((long)(((SR_WORD)((SR_DWORD)(a) & 0xffff)) | ((SR_DWORD)((SR_WORD)((SR_DWORD)(b) & 0xffff))) << 16))
#define SR_LOWORD(l)				((SR_WORD)((DWORD_PTR)(l) & 0xffff))
#define SR_HIWORD(l)				((SR_WORD)((DWORD_PTR)(l) >> 16))
#define SR_LOBYTE(w)				((SR_BYTE)((DWORD_PTR)(w) & 0xff))
#define SR_HIBYTE(w)				((SR_BYTE)((DWORD_PTR)(w) >> 8))

#define SRES_MAKEVER(maj, min, rev)	SR_MAKELONG(rev, SR_MAKEWORD(min, maj))
#define SRES_GETMAJOR(ver)			SR_HIBYTE(SR_HIWORD(ver))
#define SRES_GETMINOR(ver)			SR_LOBYTE(SR_HIWORD(ver))
#define SRES_GETREVISION(ver)		SR_LOWORD(ver)

#define IFLY_RES_VER				SRES_MAKEVER(3, 0, 0)

	// 数据类型
	typedef struct res_pos { void* reserved; } * SRES_POSITION;

	// 资源标志串
#define IFLY_RESFILE_FLAG			"iFlyTEK Resource File\0\0"

	// SRES_EXPORT
#ifdef WIN32
#  define SRES_EXPORT				__declspec (dllexport)
#  define SRES_IMPORT				__declspec (dllimport)
#else /* WIN32 */
#  define SRES_EXPORT
#  define SRES_IMPORT
#endif /* WIN32 */

	// @@
	/**=====================================================================
	* 与系统无关的资源接口定义区
	**=====================================================================*/
	// @@

	/**
	* 资源文件的加载方式
	*/
	enum SRES_LOADMODE
	{
		rlm_file = 0,		/* 文件方式(二进制) */
		rlm_memory,			/* 读入内存 */
		rlm_mapping,		/* 内存映射 */
		rlm_sharemem,		/* 共享内存 */
		rlm_getinfo,		/* 只获取信息 */
		rlm_filetext,		/* 文件方式(文本模式) */
	};

	/**
	* 访问控制
	*/
	enum SRES_ACCESSMODE
	{
		ram_readonly = 0,	/* 只读方式 */
		ram_readwrite,		/* 读写方式 */
		ram_append,			/* 追加模式 */
	};

	/**
	* PreLoad 方式
	*/
	enum SRES_PRELOAD
	{
		rpl_auto = 0,		/* 由包属性决定 */
		rpl_true,			/* 加载 */
		rpl_false,			/* 不真正加载 */
		rpl_excl,			/* 排除在外 */
	};

	/* 配置类型 */
	enum SRES_CFG_TYPE
	{
		cfg_auto,		/* 自动识别配置类型 */
		cfg_reg,		/* 注册表型的配置 */
		cfg_ini,		/* ini型的配置 */
		cfg_xml,		/* xml形式的配置文件 */
	};

	/** 
	* @class	SRes_Cfg_b
	* 
	* @brief	所有配置类的基类
	* 
	*  所有资源配置信息都从这个基类派生，可以方便的查询信息，写入配置
	* 
	* @author	jdyu
	* @date	2007-7-5
	* 
	* @see		
	*/
	struct SRes_Cfg_b
	{
		virtual int open(const char* key, int type = 0, const char* cfg = 0) = 0;
		virtual int close(bool save_cfg = false) = 0;
		virtual int save(const char* cfg = 0) = 0;

		virtual int get_value(const char* key, char* value, int & size, const char* sub = 0) = 0;
		virtual int set_value(const char* key, const char* value, const char* sub = 0) = 0;

		virtual int enum_values(char* values[], int & count, const char* sub = 0) = 0;
		virtual int remove_key(const char* key, const char* sub = 0) = 0;

		virtual bool modified(void) const = 0;
		virtual void modified(bool mod_flag) = 0;

		virtual SRES_HANDLE handle(void) = 0;
		virtual void handle(SRES_HANDLE hdl) = 0;

		virtual const char * root(void) const = 0;
	};

	/**
	* @class	 SRes_Cfg
	*
	* @brief	资源的配置接口
	*
	*  每个资源都使用这样一个配置接口，
	*  可以读取和写入配置信息
	*
	* @author	jdyu
	* @date	2005-10-24
	*/
	struct SRes_Cfg
		: public SRes_Cfg_b
	{
		virtual SRES_LOADMODE mode(void) const = 0;
		virtual void mode	(SRES_LOADMODE lm) = 0;

		virtual SRES_ACCESSMODE access(void) const = 0;
		virtual void access	(SRES_ACCESSMODE acc) = 0;

		virtual SRES_PRELOAD preload(void) const = 0;
		virtual void preload(SRES_PRELOAD pl) = 0;

		virtual const char* path(void) const = 0;
		virtual void path	(const char* pt) = 0;

		virtual const char* profile(void) const = 0;
		virtual void profile(const char* pf) = 0;

		virtual const char* name(void) const = 0;
		virtual void name(const char * nm) = 0;
	};

	/**
	* @class	 SRes_head_com
	*
	* @brief	所有类型资源的通用接口
	*
	*  这个接口是所有类型资源需要的头接口，建议所有资源都从这个接口派生。
	*  为了保证资源的通用型，这个类剥去了TTS相关的一些资源头属性
	*
	* @author	jdyu
	* @date	2005-10-25
	*
	* @see		TSResHead   InterPhonic 3.0 4.0广泛使用的资源格式
	*			SRes_head_t 推荐5.0及以后的系统使用的格式
	*
	*/
	struct SRes_head_com
	{
		/// 所有资源通用的接口
		virtual int	head_size(void) = 0;
		virtual void head_size(int size) = 0;

		// 判断字节序
		virtual bool big_endian(void) const = 0;
		virtual void big_endian(bool be) = 0;

		virtual const char* name(void) const = 0;
		virtual void name(const char* nm) = 0;

		virtual const char* bundle(void) const = 0;
		virtual void bundle(const char* bd) = 0;

		virtual SR_DWORD version(void) const = 0;
		virtual void version(SR_DWORD ver) = 0;

		virtual SR_TIME create_time(void) = 0;
		virtual void create_time(SR_TIME tm) = 0;

		virtual SR_TIME modify_time(void) = 0;
		virtual void modify_time(SR_TIME) = 0;

		virtual const char* author(void) const = 0;
		virtual void author(const char* auth) = 0;

		virtual const char* company(void) const = 0;
		virtual void company(const char* com) = 0;

		virtual const char* description(void) const = 0;
		virtual void description(const char* desc) = 0;

		virtual SR_DWORD build(void) const = 0;
		virtual void build(SR_DWORD) = 0;

		/// 返回头缓冲区内存
		virtual SR_BYTE * data(void) const = 0;

		/// 从文件读取或者把信息写入到文件
		virtual int read (const char* file) = 0;
		virtual int write(const char* file) = 0;
	};

	/**
	* @class	 SRes_Resource
	*
	* @brief	最基本的资源接口
	*
	*  系统所有资源必须从这个接口派生，
	*  为了资源类型的通用性，同时又照顾到tts系统的方便性，
	*  加入了res_head()接口，如果不是tts系统可以简单的返回
	*
	* @author	jdyu
	* @date	2005-10-24
	*
	*/
	struct SRes_Resource
	{
		////////////////////////////////////////////////////////////////////
		// 基本方法
		////////////////////////////////////////////////////////////////////
		// 资源的装载和卸载
		virtual int load(SRes_Cfg * cfg) = 0;
		virtual int unload(void) = 0;

		// 重新加载和保存
		virtual int reload(void) = 0;
		virtual int save(void) = 0;

		// 资源的句柄
		virtual SRES_HANDLE handle(void) const = 0;
		// 资源的基本操作
		// 派生类可以重新实现这些基本的操作函数
		virtual int open (SRES_HANDLE & hdl, const char* file, SRES_ACCESSMODE acm, SRES_LOADMODE lm) = 0;
		virtual int close(SRES_HANDLE hdl = 0) = 0;
		virtual int seek (int offset, int origin, SRES_HANDLE hdl = 0) = 0;
		virtual int read (void* data, size_t & size, unsigned int offset = 0, SRES_HANDLE hdl = 0) = 0;
		virtual int write(const void* data, size_t & size, unsigned int offset = 0, SRES_HANDLE hdl = 0) = 0;
		virtual const char * read_line(char* buf, int size, SRES_HANDLE hdl = 0) = 0;

		// 资源的引用计数，注意这里没有多线程保护
		virtual unsigned int increase_ref(void) = 0;
		virtual unsigned int decrease_ref(void) = 0;
		virtual unsigned int reference_count(void) = 0;

		// 检查资源在加载之后是否被外部修改过
		// 是否需要重新加载
		virtual bool need_reload(void) const = 0;
		virtual void need_reload(bool reload = true) = 0;
		virtual bool is_loaded  (void) const = 0;

		// 资源修改标记，和上面不同的是这里的修改是内部的修改，例如资源管理中心
		virtual bool modified(void) const = 0;
		virtual void modified(bool mod) = 0;

		////////////////////////////////////////////////////////////////////
		// 属性获取设置
		////////////////////////////////////////////////////////////////////
		virtual SRes_head_com * com_head(void) const = 0;
		virtual SRes_head *		res_head(void) const = 0;
		virtual SRes_Pack *		package (void) const = 0;
		virtual void			package(SRes_Pack * pack) = 0;

		// 配置接口
		virtual SRes_Cfg *		res_cfg (void) = 0;

		// 获得资源文件的名称
		virtual const char* path(void) const = 0;
		virtual const char* name(void) const = 0;
		virtual int id(void) const = 0;

		// 判断资源是否匹配特定数据
		// (用于查找资源时的匹配，派生类必须实现该函数)
		virtual bool match_token(const char* name, const char* token) const = 0;
		virtual bool match_token(const char* name, int wpara = 0, int lpara = 0) const = 0;
		virtual bool match_token(SR_DWORD token,   int wpara = 0, int lpara = 0) const = 0;

		// 判断资源是否需要分配线程相关的句柄
		// (用于多线程下文件加载方式，派生类必须实现该函数)
		virtual bool is_need_alloc_handle(void) const = 0;

		/// 高级接口
		// 资源的预热
		virtual int warm_up(void) = 0;
		// 编解码
		virtual int encoding(void) = 0;
		virtual int decoding(void) = 0;
		// 调整资源字节序
		virtual int adjust_byte_order(short dest_bo) = 0;

		// 查询接口
		virtual int query_interface(const char* name, void ** intf, void * reserved = 0) = 0;
	};

	/**
	* 资源管理器的初始化方式
	*/
	enum SRES_INITTYPE
	{
		sit_kernel = 0,		/* 合成内核调用的初始化 */
		sit_user,			/* 用户程序调用的 */
		sit_adv_user,		/* 高级用户调用的 */
		sit_config			/* 只读取配置信息，不真正加载资源 */
	};

	/**
	* @class	 SRes_Pack
	*
	* @brief	资源包接口
	*
	*  资源包是一系列资源的集合，一个资源包里面可以包含
	*  多个不同类型的资源。
	*
	* @author	jdyu
	* @date	2005-10-24
	*
	*/
	struct SRes_Pack
	{
		// 初始化、逆初始化
		virtual int initialize(const SRes_Pack_Cfg * cfg, SRes_Pack_Mngr* pmgr) = 0;
		virtual int uninitialize(void) = 0;

		// 信息的查询和写入
		virtual const SRes_Pack_Cfg * pack_cfg(void) const = 0;
		virtual SRes_Pack_Cfg * pack_cfg(void) = 0;

		// 查询资源包属性
		virtual const char* name(void) const = 0;
		virtual const char* path(void) const = 0;
		virtual bool is_default (void) const = 0;
		virtual bool is_standard(void) const = 0;

		// 各个资源的遍历
		virtual SRES_POSITION	first_resource	(int res_type = 0) = 0;
		virtual SRes_Resource *	next_resource	(SRES_POSITION & pos, int res_type = 0) = 0;
		virtual SRes_Resource *	get_resource	(SRES_POSITION pos) = 0;

		// 获得数量
		virtual SR_DWORD		resource_count	(int res_type = 0) = 0;

		// 资源包的匹配
		virtual bool match_token(const char* token) const = 0;

		// 资源的查找
		virtual SRes_Resource * find_resource(const char* name, const char* token) = 0;
		virtual SRes_Resource * find_resource(const char* name, int wpara = 0, int lpara = 0) = 0;
		virtual SRes_Resource *	find_resource(SR_DWORD   token, int wpara = 0, int lpara = 0) = 0;
		// 资源的释放，当find_resource到的资源不再使用时，可以调用这个函数
		virtual int release_resource(SRes_Resource * res) = 0;

		// 资源接口查找，找到指定的资源，同时把和该资源绑定的接口返回回来，相当于find_resource和query_interface的组合
		virtual void * find_interface(const char * res_name, const char * token = 0, const char * intf_name = 0, SRes_Resource ** res = 0) = 0;
		virtual int release_interface(void * res_intf) = 0;

		// 添加和删除资源
		virtual int add_resource(const SRes_Resource * res) = 0;
		virtual int remove_resource(SRES_POSITION pos) = 0;

		// 资源包的预热
		virtual int warm_up(void) = 0;
		virtual int refresh(void) = 0;

		// 获得相关的资源包管理器
		virtual struct SRes_Pack_Mngr* respack_mngr(void) const = 0;

		// 查询VID接口等其他接口
		virtual int query_interface(const char* name, void ** intf, void * reserved = 0) = 0;
	};

	/**
	* @class	SRes_Pack_Mngr
	*
	* @brief	资源管理接口
	*
	*  这个类是系统资源的大总管，将会以全局单体的形式提供，
	*  用户要做的事情就是：
	*	1. 初始化
	*	2. find_package, find resource
	*  3. 逆初始化
	*
	* @author	jdyu
	* @date	2005-11-11
	*/
	struct SRes_Pack_Mngr
	{
		// 初始化、逆初始化
		virtual int initialize(const char * rcfg, SRES_INITTYPE sit = sit_kernel, void * rev = 0) = 0;
		virtual int uninitialize(void) = 0;

		// 各个资源包的遍历
		virtual SRES_POSITION	first_package(void) = 0;
		virtual SRes_Pack *		next_package(SRES_POSITION & pos) = 0;
		virtual SRes_Pack *		package(SRES_POSITION pos) = 0;
		// 获得数量
		virtual unsigned int	pack_count(void) const = 0;

		// 查找资源包
		virtual SRes_Pack *		find_package(const char * name, const char * token = 0) = 0;

		// 获得标准资源包
		virtual SRes_Pack *		std_package(void) const = 0;
		// 获得缺省资源包
		virtual SRes_Pack *		default_package(void) const = 0;
		virtual int				default_package(const char* name) = 0;

		// 获得初始化类型
		virtual SRES_INITTYPE	init_type(void) const = 0;

		// 配置信息
		virtual const SRes_Pack_Mngr_Cfg * pm_cfg(void) const = 0;
		virtual SRes_Pack_Mngr_Cfg * pm_cfg(void) = 0;

		// 日志信息
		virtual Log_IO * logger(void) const = 0;
		virtual void logger(Log_IO * lgr) = 0;

		// 资源包的添加和删除
		virtual int add_package(SRes_Pack * pack, SRES_POSITION & pos) = 0;
		virtual int remove_package(SRES_POSITION pos) = 0;

		// 各个资源包的预热
		virtual int warm_up(void) = 0;

		// 查询VID接口等其他接口
		virtual int query_interface(const char* name, void ** intf, void * reserved = 0) = 0;
	};

	/**
	* @class	SRes_Head_Com_T
	*
	* @brief	一个可以定制头结构体的资源头模版
	*
	*  这个模版专门为了一些不想使用缺省资源头的资源而写
	*  比如：有些人认为1024字节的头太长了，自己想实现精简的版本
	*
	* @author	jdyu
	* @date	2005-11-28
	*
	*/
	template <typename HEAD_STRUCT, int HEAD_SIZE, typename HEAD_INTFACE>
	class SRes_Head_Com_T
		: public HEAD_INTFACE
	{
	public:
		// 所有资源通用的接口
		int	head_size(void)
		{ return head_.head_size(); }
		void head_size(int size)
		{ head_.head_size(size); }

		bool big_endian(void) const
		{ return head_.big_endian(); }
		void big_endian(bool be)
		{ head_.big_endian(be); }

		const char* name(void) const
		{ return head_.name(); }
		void name(const char* nm)
		{ head_.name(nm); }

		const char* bundle(void) const
		{ return head_.bundle(); }
		void bundle(const char* bd)
		{ head_.bundle(bd); }

		// 非重要的接口
		SR_TIME create_time(void)
		{ return head_.create_time(); }
		void create_time(SR_TIME tm)
		{ head_.create_time(tm); }

		SR_TIME modify_time(void)
		{ return head_.modify_time(); }
		void modify_time(SR_TIME tm)
		{  head_.modify_time(tm); }

		const char* author(void) const
		{ return head_.author(); }
		void author(const char* auth)
		{ head_.author(auth); }

		const char* company(void) const
		{ return head_.company(); }
		void company(const char* com)
		{ head_.company(com); }

		const char* description(void) const
		{ return head_.description(); }
		void description(const char* desc)
		{ head_.description(desc); }

		SR_DWORD version(void) const
		{ return head_.version(); }
		void version(SR_DWORD ver)
		{ head_.version(ver); }

		SR_DWORD build(void) const
		{ return head_.build(); }
		void build(SR_DWORD bn)
		{ head_.build(bn); }

		/// 返回头缓冲区内存
		virtual SR_BYTE * data(void) const
		{ return (SR_BYTE *)&head_; }

		// 从文件中读取头信息和写入头信息到文件
		virtual int read (const char* file)
		{ return 0; }
		virtual int write(const char* file)
		{ return 0; }

	public:
		// 构造和赋值
		SRes_Head_Com_T(void)
			: head_()
		{ memset(data_, 0, sizeof data_); }
		SRes_Head_Com_T(const HEAD_STRUCT & hd)
		{ set(hd); }
		SRes_Head_Com_T & operator = (const SRes_Head_Com_T & st)
		{ set(&st, st.head_size());	return *this; }

		// 返回头结构体
		HEAD_STRUCT	& head_struct(void)
		{ 
			return head_;	
		}

		// 设置
		void set(const HEAD_STRUCT & hd)
		{ 
			memset(&head_, 0, HEAD_SIZE); 
			head_ = hd;
		}
		void set(const char * hd, int size = HEAD_SIZE)
		{ 
			if ( hd ) 
			{
				memcpy(&head_, hd, std::min<size_t>(size, HEAD_SIZE));
			}
		}

	public:
		// 头的存储结构
		// 标准头信息
		HEAD_STRUCT	head_;
		// 所有资源都有长度为HEAD_SIZE的头结构
		SR_BYTE		data_[HEAD_SIZE - sizeof(HEAD_STRUCT)];
	};

	/**
	* @class	 SRes_Resource_T
	*
	* @brief	一个资源的缺省实现
	*
	*  使用模版的形式给出一个资源的缺省实现，用户可以直接从这个模版派生
	*  简化编程，这个模版可以定制资源头结构，配置类和基类
	*  注意：这个模版仅仅是一个框架，具体的实现很少，用户还需要根据需要派生一个最基本的实现类
	*
	* @author	jdyu
	* @date	2005-11-28
	*
	* @see		SRes_Resource
	*
	*/
	template <class RHEAD, class CFG, class BASE = SRes_Resource>
	class SRes_Resource_T
		: public BASE
	{
	public:
		////////////////////////////////////////////////////////////////////
		// 基本方法
		// 基本上所有资源类都需要重载的方法
		////////////////////////////////////////////////////////////////////
		// 资源的装载和卸载
		virtual int load(SRes_Cfg * cfg)
		{
			if ( cfg )
			{
				cfg_.handle(cfg->handle());
				cfg_.path(cfg->path());
				cfg_.preload(cfg->preload());
				cfg_.access(cfg->access());
				cfg_.profile(cfg->profile());
				cfg_.mode(cfg->mode());
			}
			// read head
			int ret = head_.read(cfg_.path());
			if ( ret != 0 )
				return 0;

			path(cfg_. path());
			name(head_.name());
			if ( cfg_.mode() == rlm_getinfo )
				return 0;

			return open(hdl_, cfg_.path(), cfg_.access(), cfg_.mode());
		}
		virtual int unload(void)
		{
			if ( hdl_!= 0 )
				close(hdl_), hdl_ = 0;
			return 0;
		}

	public:
		////////////////////////////////////////////////////////////////////
		// 高级方法
		// 需要特定功能的类需要重载的方法
		////////////////////////////////////////////////////////////////////
		virtual int reload(void)
		{ int ret = unload(); return load(&cfg_); }
		virtual int save(void)
		{ return 0; }

		// 资源句柄
		virtual SRES_HANDLE handle(void) const
		{ return this->hdl_; }

		// 资源的引用计数
		virtual unsigned int increase_ref(void) 
		{ return ++ ref_count_ == 0 ? ++ ref_count_	: ref_count_; }
		virtual unsigned int decrease_ref(void)
		{ return -- ref_count_ == 0 ? -- ref_count_	: ref_count_; }
		virtual unsigned int reference_count(void)
		{ return ref_count_; }

		// 检查资源在加载之后是否修改过
		virtual bool need_reload(void) const
		{ return need_reload_; }
		virtual void need_reload(bool reload = true)
		{ this->need_reload_ = reload; }
		// 是否加载过
		virtual bool is_loaded(void) const
		{ return hdl_ != 0; }

		// 资源修改标记，和上面不同的是这里的修改是内部的修改，例如资源管理中心
		virtual bool modified(void) const
		{ return modf_; }
		virtual void modified(bool mod)
		{ modf_ = mod; }

		////////////////////////////////////////////////////////////////////
		// 属性获取设置
		////////////////////////////////////////////////////////////////////
		virtual SRes_head_com * com_head(void) const
		{ return (SRes_head_com*)(&this->head_); }
		virtual SRes_head *	res_head(void) const
		{ 
			return (SRes_head*)	 (&this->head_);
		}
		virtual SRes_Pack *	package (void) const
		{ return this->pack_; }
		virtual void package(SRes_Pack * pack)
		{ this->pack_ = pack; }
		// 配置接口
		virtual SRes_Cfg * res_cfg (void)
		{ return &this->cfg_; }

		// 获得资源文件的名称
		virtual const char * path(void) const
		{ return this->fn_.c_str(); }
		virtual const char * name(void) const
		{ return this->name_.c_str(); }
		virtual int id(void) const
		{ return this->id_; }

	public:
		// 派生类可以使用的函数
		void path(const char * fpath)
		{ if ( fpath ) this->fn_ = fpath; }
		void name(const char * nm)
		{ if ( nm ) this->name_ = nm; }
		void id(const int _id)
		{  this->id_ = _id; }
		void res_cfg(SRes_Cfg * cfg)
		{ this->cfg_ = cfg; }
		void handle(SRES_HANDLE hdl)
		{ this->hdl_ = hdl; }

		// 获得句柄
		virtual FILE * file_hdl(SRES_HANDLE hdl = 0) const
		{ return  hdl == 0 ? (FILE*)hdl_ : (FILE*)hdl;  }
		// 获得数据的起始位置
		virtual char * data_begin(void) const
		{ return (char*)handle(); }
		virtual void res_info(SRes_Cfg * cfg, const char * name)
		{
			if ( cfg )
			{
				cfg_.handle(cfg->handle());
				cfg_.path(cfg->path());
				cfg_.preload(cfg->preload());
				cfg_.access(cfg->access());
				cfg_.profile(cfg->profile());
				cfg_.mode(cfg->mode());
			}
			path(cfg_.path());
			this->name(name);
		}

	public:
		// ========================================================================
		// 资源的基本操作
		// 这里提供文件形式和内存形式的缺省实现
		virtual int open (SRES_HANDLE & hdl, const char* file, SRES_ACCESSMODE acm, SRES_LOADMODE lm)
		{
			// 获得尺寸
			if ( fsize_ == 0 )
			{
				struct stat si;
				if ( ::stat(file, &si) == 0 )
					fsize_ = si.st_size;
			}

			// 打开文件
			if ( lm == rlm_file || lm == rlm_filetext )
			{
				std::string op = "r";
				if ( acm != ram_readonly && acm != rlm_sharemem )
					op += "+";
				op += lm == rlm_filetext ? "t" : "b";
				hdl = fopen(file, op.c_str());
				if ( hdl == 0 )
					return -1;
			}
			else if ( cfg_.mode() == rlm_memory && fsize_ > 0 )
			{
				FILE * fp = fopen(file, "rb");
				if ( fp == 0 )
					return -1;

				size_t hsize = head_.head_size();
				if ( hsize > fsize_ )
					return -1;
				size_t dsize = fsize_ - hsize;

				const unsigned int align_size = 32;
				SR_BYTE * data = new SR_BYTE[dsize + align_size];
				align_rev_ = align_size - (size_t(data) % align_size);
				fseek(fp, long(hsize), SEEK_SET);
				size_t rd = fread(data + align_rev_, 1, dsize, fp);
				if ( rd != dsize )
				{
					delete [] data;
					return -1;
				}
				hdl = data;
				fclose(fp);
			}
			return 0;
		}
		// 关闭一个句柄
		virtual int close(SRES_HANDLE hdl = 0)
		{
			const SRES_LOADMODE lm = cfg_.mode();
			if ( lm == rlm_filetext || lm == rlm_file )
			{
				if ( file_hdl(hdl) )
					fclose(file_hdl(hdl));
			}
			else if ( lm == rlm_memory )
			{
				SRES_HANDLE hd = hdl ? hdl : hdl_;
				if ( hd )
					delete [] (SR_BYTE*)hd;
			}
			return 0;
		}
		// 读取数据
		virtual int read (void * data, size_t & size, unsigned int offset = 0, SRES_HANDLE hdl = 0)
		{
			const SRES_LOADMODE lm = cfg_.mode();
			if ( lm == rlm_file || lm == rlm_filetext )
			{
				if ( offset != 0 )
					fseek(file_hdl(hdl), offset, SEEK_SET);
				size = fread(data, 1, size, file_hdl(hdl));
			}
			else
			{
				size_t dsize = fsize_ - head_.head_size();
				if ( dsize > offset )
				{
					size = std::min<size_t>(size, dsize - (size_t)offset);
					memcpy(data, (SR_BYTE*)hdl_ + offset, size);
				}
				else
					size = 0;
			}
			return 0;
		}
		// 写入数据
		virtual int write(const void* data, size_t & size, unsigned int offset = 0, SRES_HANDLE hdl = 0)
		{
			const SRES_LOADMODE lm = cfg_.mode();
			if ( lm == rlm_file || lm == rlm_filetext )
			{
				if ( offset != 0 )
					fseek(file_hdl(hdl), offset, SEEK_SET);
				size = fwrite(data, 1, size, file_hdl(hdl));
			}
			else if ( lm == rlm_memory )
			{
				size_t dsize = fsize_ - head_.head_size();
				if ( dsize > offset )
				{
					size = (unsigned int)std::min<size_t>(size, dsize - (size_t)offset);
					memcpy((SR_BYTE*)hdl_ + offset, data, size);
				}
			}
			return 0;
		}
		// 定位到指定的位置
		virtual int seek (int offset, int origin, SRES_HANDLE hdl = 0)
		{
			if ( cfg_.mode() == rlm_file || cfg_.mode() == rlm_filetext )
				return fseek(file_hdl(hdl), offset, origin);
			return 0;
		}
		// 读取一行，仅对文本文件有效
		virtual const char * read_line(char* buf, int size, SRES_HANDLE hdl = 0)
		{
			if ( cfg_.mode() != rlm_filetext )
				return 0;
			return fgets(buf, size, file_hdl(hdl));
		}

		// ========================================================================
		// 高级接口
		// 资源的预热
		virtual int warm_up(void)
		{ return 0; }
		// 编解码
		virtual int encoding(void)
		{ return 0; }
		virtual int decoding(void)
		{ return 0; }
		// 调整资源字节序
		virtual int adjust_byte_order(short dest_bo)
		{ return 0; }

		// 查询接口
		virtual int query_interface(const char* name, void ** intf, void * reserved = 0)
		{ return -1; }

		// 构造和析构
		SRes_Resource_T(void)
			: head_(), cfg_()
			, hdl_(0), pack_(0), fsize_(0), id_(0), need_reload_(false), modf_(false), ref_count_(0), align_rev_(0)
		{ }
		virtual ~SRes_Resource_T(void)
		{ }
		CFG & config(void)
		{ return cfg_; }
		RHEAD & head(void)
		{ return head_; }
		// 返回资源数据地址
		SR_BYTE * res_data(void) const
		{
			if ( cfg_.mode() == rlm_memory )
				return (SR_BYTE*)hdl_ + align_rev_;
			else
				return 0;
		}

	protected:
		// 判断资源是否匹配特定数据
		// (用于查找资源时的匹配，派生类必须实现该函数)
		virtual bool match_token(const char* nm, const char* token = 0) const
		{ return strcmp(nm, name()) == 0; }
		virtual bool match_token(const char* nm, int wpara, int lpara = 0) const
		{ return strcmp(nm, name()) == 0; }
		virtual bool match_token(SR_DWORD token, int wpara, int lpara = 0) const
		{ return true; }

		// 判断资源是否需要分配线程相关的句柄
		// (用于多线程下文件加载方式，派生类必须实现该函数)
		virtual bool is_need_alloc_handle(void) const
		{ return false; }

	private:
		// 资源头
		RHEAD			head_;
		// 配置信息
		CFG 			cfg_;
		// 资源句柄
		SRES_HANDLE		hdl_;
		unsigned int	fsize_;
		// 资源包
		SRes_Pack *		pack_;
		// 资源路径
		std::string		fn_;
		// 资源名称
		std::string		name_;
		// 资源id
		int				id_;
		// 是否需要重新加载
		bool			need_reload_;
		// 是否修改过
		bool			modf_;
		// 引用计数
		unsigned int	ref_count_;
		// 对齐保留的长度
		unsigned int	align_rev_;
	};

	/**
	* @class	SRes_Pack_T
	*
	* @brief	资源包的缺省实现
	*
	*  资源包的模版适配类，这里实现了一个简化的资源包接口
	*  可以从这个类派生以达到简化的目的
	*
	* @author	jdyu
	* @date	2005-11-29
	*
	*/
	template <class CFG, class BASE = SRes_Pack>
	class SRes_Pack_T
		: public BASE
	{
	public:
		// 信息的查询和写入
		virtual const SRes_Pack_Cfg * pack_cfg(void) const
		{ return &pack_cfg_; }
		virtual SRes_Pack_Cfg * pack_cfg(void)
		{ return &pack_cfg_; }

		// 查询资源包属性
		virtual bool is_default (void) const
		{ return respack_mngr_ && respack_mngr_->default_package() == this; }
		virtual bool is_standard(void) const
		{ return respack_mngr_ && respack_mngr_->std_package() == this; }

		// 各个资源的遍历
		virtual SRES_POSITION	first_resource	(int res_type = 0) = 0;
		virtual SRes_Resource *	next_resource	(SRES_POSITION & pos, int res_type = 0) = 0;
		virtual SRes_Resource *	get_resource	(SRES_POSITION pos) = 0;

		// 获得数量
		virtual SR_DWORD resource_count	(int res_type = 0) = 0;

		// 资源的匹配
		virtual bool match_token(const char* token) const
		{ return false; }

		// 资源的查找
		virtual SRes_Resource * find_resource(const char* name, const char* token)
		{ return 0; }
		virtual SRes_Resource * find_resource(const char* name, int wpara = 0, int lpara = 0)
		{
			if ( wpara == 0 )
				return find_resource(name, "");
			char token[16];
			sprintf(token, "%d", wpara);
			return find_resource(name, token);
		}
		virtual SRes_Resource *	find_resource(SR_DWORD   token, int wpara = 0, int lpara = 0)
		{ return 0; }
		// 资源的释放，当find_resource到的资源不再使用时，可以调用这个函数
		virtual int release_resource(SRes_Resource * res)
		{ return 0; }

		// 资源接口查找，找到指定的资源，同时把和该资源绑定的接口返回回来，相当于find_resource和query_interface的组合
		virtual void * find_interface(const char * res_name, const char * token = 0, const char * intf_name = 0, SRes_Resource ** res = 0)
		{ return 0; }
		virtual int release_interface(void * res_intf)
		{ return -1; }

		// 添加和删除资源
		virtual int add_resource(const SRes_Resource * pResource)
		{ return 0; }
		virtual int remove_resource(SRES_POSITION pos)
		{ return 0; }

		// 资源包的预热
		virtual int warm_up(void)
		{ return 0; }
		virtual int refresh(void)
		{ return 0; }

		// 获得相关的资源包管理器
		virtual SRes_Pack_Mngr* respack_mngr(void) const
		{ return respack_mngr_; }
		virtual void respack_mngr(SRes_Pack_Mngr* resmgr)
		{ respack_mngr_ = resmgr; }

		// 查询VID接口等其他接口
		virtual int query_interface(const char* name, void ** intf, void * reserved = 0)
		{ return 0; }

		SRES_INITTYPE init_type(void) const
		{ return respack_mngr_->init_type(); }

	public:
		// 构造函数
		SRes_Pack_T(void)
			: respack_mngr_(0)
		{ }
		CFG & config(void)
		{ return pack_cfg_; }

	protected:
		// 资源管理器接口
		SRes_Pack_Mngr *	respack_mngr_;
		// 配置信息
		CFG					pack_cfg_;
	};

	/** 
	* @class	SRes_Pack_Mngr_T
	* 
	* @brief	资源包管理器的缺省实现
	* 
	*  提供一个资源包管理器的缺省实现，从这个类派生可以简化编码
	* 
	* @author	jdyu
	* @date	2007-6-1
	* 
	* @see		SRes_Pack_Mngr
	*/
	template <class CFG, class BASE = SRes_Pack_Mngr>
	class SRes_Pack_Mngr_T
		: public BASE
	{
	public:
		// 配置信息
		virtual const SRes_Pack_Mngr_Cfg * pm_cfg(void) const
		{ return &pm_cfg_; }
		virtual SRes_Pack_Mngr_Cfg * pm_cfg(void)
		{ return &pm_cfg_; }

		// 获得初始化类型
		virtual SRES_INITTYPE init_type(void) const
		{ return pm_cfg_.init_type(); }

		// 获得标准资源包
		virtual SRes_Pack* std_package(void) const
		{ return 0; }
		// 获得缺省资源包
		virtual SRes_Pack* default_package(void) const
		{ return 0; }
		virtual int default_package(const char * name)
		{ pm_cfg_.default_pack(name); return 0;	}

		// 获得数量
		virtual unsigned int pack_count(void) const
		{ return 0;	}

		// 资源包的添加和删除
		virtual int add_package(SRes_Pack* pack, SRES_POSITION & pos)
		{ return 0; }
		virtual int remove_package(SRES_POSITION pos)
		{ return 0; }

		// 各个资源包的预热
		virtual int warm_up(void)
		{ return 0; }
		// 查询VID接口等其他接口
		virtual int query_interface(const char* name, void ** intf, void * reserved = 0)
		{ return -1; }

		// 日志信息
		virtual Log_IO * logger(void) const
		{ return 0;	}
		virtual void logger(Log_IO * lgr)
		{ }

	public:
		SRes_Pack_Mngr_T(void)
		{ }
		virtual ~SRes_Pack_Mngr_T(void)
		{ }
		CFG & config(void)
		{ return pm_cfg_; }

	protected:
		// 资源管理器配置
		CFG					pm_cfg_;
	};

	// @@
	/**====================================================================
	* 与系统相关的资源接口定义区
	* 目前考虑了TTS系统的资源需求
	* 后期考虑和前面的定义分开
	*=====================================================================*/
	// @@

	////////////////////////////////////////////////////////////////////////
	// 系统类型
	////////////////////////////////////////////////////////////////////////
	typedef enum
	{
		st_tts = 0,				// TTS系统
		st_asr = 1,				// ASR系统
	} SSYSTEM_TYPE;

	////////////////////////////////////////////////////////////////////////
	// 资源类型
	// ---------------------------------------------------------------------
	// 严重警告：
	// 为了保证其他资源能够正确加载和以前版本资源的兼容性，只能追加资源类型，
	// 不要在其中插入新的类型，更不要删除、重新排列原有的资源类型
	////////////////////////////////////////////////////////////////////////
	typedef enum
	{
		rt_unknown		= 0,	// 未知类型
		rt_lexicon,				// 词典

		rt_count				// 用以标识类型的最后一个。
	} SRES_TYPE;

	////////////////////////////////////////////////////////////////////////
	// 资源属性
	////////////////////////////////////////////////////////////////////////
	typedef enum
	{
		rp_system		= 0,	// 系统资源
		rp_user,				// 用户资源
	} SRES_PROPERTY;

	////////////////////////////////////////////////////////////////////////
	// 语言类型
	////////////////////////////////////////////////////////////////////////
	typedef enum
	{
		rl_unknown		= 0,	// 未知
		cn_simp			= 936,	// 简体中文
		cn_trad			= 950,	// 繁体中文
		en_us			= 1252,	// 英文
	} SRES_LANG;

	////////////////////////////////////////////////////////////////////////
	// 方言类型
	////////////////////////////////////////////////////////////////////////
	typedef enum
	{
		vd_mandarin		= 0,	// 标准音
		vd_cantonese	= 1,	// 粤语
		vd_neutral		= 0xFF,	// 不分语言类型
		vd_count		= 3,	// 方言总数
	} SV_DIALECT;

	////////////////////////////////////////////////////////////////////////
	// 字节序类型
	////////////////////////////////////////////////////////////////////////
//#define	SRES_BO_LE		((SR_WORD)0)		// Little-Endian Intel平台
//#define	SRES_BO_BE		((SR_WORD)0xFFFF)	// Big-Endian

	////////////////////////////////////////////////////////////////////////
	// 声音ID
	////////////////////////////////////////////////////////////////////////
	typedef enum
	{
		vid_invalid		= -1,
		vid_default		=  0,
	} SRES_VOICEID;

	/** 
	* @class	SRes_head_t
	* 
	* @brief	二进制资源文件的文件头信息(新资源的格式)
	* 
	*  所有二进制资源文件的建议使用的文件头信息
	*  便于系统获取资源的一些概况信息
	*  intp50之后的项目建议使用这个格式
	* 
	* @author	jdyu
	* @date	2007-7-5
	* 
	* @see		
	* 
	* @par Note:
	* 
	*/
	struct SRes_head_t
	{
		SR_WORD			byte_order_;	/// 字节序
		SR_WORD			head_size_;		/// 头长度
		char			flag_	[32];	/// 标志信息
		char			name_	[32];	/// 资源名称
		char			bundle_	[32];	/// 资源簇名称
		SR_DWORD		ver_;			/// 资源版本
		SR_DWORD		build_;			/// 资源创建编号

		SR_TIME			create_time_;	/// 创建时间
		SR_TIME			modify_time_;	/// 更新时间
		char			author_	[16];	/// 作者
		char			company_[32];	/// 公司
		char			desc_	[156];	/// 描述信息

		SSYSTEM_TYPE	sys_type_;		/// 系统类型
		SRES_TYPE		res_type_;		/// 资源类型
		SRES_PROPERTY	res_prop_;		/// 资源属性
		SRES_LANG		res_lang_;		/// 资源语言类型
		SV_DIALECT		dialect_;		/// 方言类型
		SR_DWORD		res_id_;		/// 资源ID
		SR_DWORD		vid_;			/// 声音ID
		char			vname_	[32];	/// 声音名称
		SR_DWORD		data_size_;		/// 数据长度
		// 后面可以直接扩展
		// ...
	};

	/// 缺省的头格式
	const SRes_head_t SRES_DEFHEAD =
	{
		SRES_BO_LE,						// byte_order_
		SRES_DEFHEADSIZE,				// head_size_
		IFLY_RESFILE_FLAG,				// flag_
		"",								// name_
		"",								// bundle_
		SRES_MAKEVER(1, 0, 0),			// ver_
		0,								// build_

		0,								// create_time_
		0,								// modify_time_
		"Speech Dept.",					// author_
		"iFLYTEK.",						// company_
		"desc.",							// desc_

		st_tts,							// sys_type_
		rt_unknown,						// res_type_
		rp_system,						// res_prop_
		rl_unknown,						// res_lang_
		vd_mandarin,					// dialect_
		0,								// res_id_
		0,								// vid_
		"",								// vname_
		0								// data_size_
	};

	// 一个简单的访问结构成员的类
	template <class HEAD = SRes_head_t>
	struct SRes_head_c
		: public HEAD
	{
		// 构造函数
		SRes_head_c(void)
		{ memcpy(this, &SRES_DEFHEAD, sizeof SRES_DEFHEAD); }
		SRes_head_c(const SRes_head_t & dh)
		{ memcpy(this, &dh, sizeof dh); }

		// 所有资源通用的接口
		int	head_size(void)
		{
			SR_WORD headsize = this->head_size_;
			// 判断是否需要字节序转换
			if (sp::get_platform_endian() != this->byte_order_)
			{
				sp::convert_endian_16(&headsize);
			}
			return headsize;
		}
		void head_size(int size)
		{ this->head_size_ = size; }

		const char* name(void) const
		{ return this->name_; }
		void name(const char* nm)
		{ strcpy(this->name_, nm); }

		SR_TIME create_time(void)
		{
			SR_TIME createtime = this->create_time_;
			// 判断是否需要字节序转换
			if (sp::get_platform_endian() != this->byte_order_)
			{
				sp::convert_endian_32(&createtime);
			}
			return createtime;
		}

		void create_time(SR_TIME tm)
		{this->create_time_ = tm; }

		SR_TIME modify_time(void)
		{
			SR_TIME modifytime = this->modify_time_;
			// 判断是否需要字节序转换
			if (sp::get_platform_endian() != this->byte_order_)
			{
				sp::convert_endian_32(&modifytime);
			}
			return modifytime;
		}
		void modify_time(SR_TIME tm)
		{ this->modify_time_ = tm; }

		const char* author(void) const
		{ return this->author_; }
		void author(const char* auth)
		{ strcpy(this->author_, auth); }

		const char* company(void) const
		{ return this->company_; }
		void company(const char* com)
		{ strcpy(this->company_, com); }

		const char* description(void) const
		{ return this->desc_; }
		void description(const char* desc)
		{ strcpy(this->desc_, desc); }

		unsigned int res_id(void)
		{
			unsigned resid = this->res_id_;
			// 判断是否需要字节序转换
			if (sp::get_platform_endian() != this->byte_order_)
			{
				sp::convert_endian_32(&resid);
			}
			return resid;
		}

		void res_id(unsigned int id)
		{ this->res_id_ = id; }

		int vid(void) const
		{ return this->vid_; }
		void vid(int vd)
		{ this->vid_ = vd; }

		SR_DWORD version(void) const
		{ return this->ver_; }
		void version(SR_DWORD ver)
		{ this->ver_ = ver; }

		SR_DWORD build(void) const
		{ return this->build_; }
		void build(SR_DWORD bn)
		{ this->build_ = bn; }

		// TTS常用的访问函数
		bool big_endian(void) const
		{ return this->byte_order_ == SRES_BO_BE; }
		void big_endian(bool be)
		{ this->byte_order_ = be ? SRES_BO_BE : SRES_BO_LE; }

		const char* bundle(void) const
		{ return this->bundle_; }
		void bundle(const char* bd)
		{ strcpy(this->bundle_, bd); }

		SSYSTEM_TYPE sys_type(void) const
		{ return this->sys_type_; }
		void sys_type(SSYSTEM_TYPE st)
		{ this->sys_type_ = st; }

		SRES_PROPERTY res_prop(void) const
		{ return this->res_prop_; }
		void res_prop(SRES_PROPERTY rp)
		{ this->res_prop_ = rp; }

		SRES_TYPE res_type(void) const
		{ return this->res_type_; }
		void res_type(SRES_TYPE rt)
		{ this->res_type_ = rt; }

		SRES_LANG res_lang(void) const
		{ return this->res_lang_; }
		void res_lang(SRES_LANG sl)
		{ this->res_lang_ = sl; }

		SV_DIALECT dialect(void) const
		{ return this->dialect_; }
		void dialect(SV_DIALECT sd)
		{ this->dialect_ = sd; }

		unsigned int data_size(void) const
		{ return this->data_size_; }
		void data_size(unsigned int size)
		{ this->data_size_ = size; }
	};

	/**
	* 一个空的头
	*/ 
	struct SRes_head_n
	{
		// 所有资源通用的接口
		int	head_size(void)
		{ return 0; }
		void head_size(int size)
		{ }

		const char* name(void) const
		{ return ""; }
		void name(const char* nm)
		{ }

		SR_TIME create_time(void)
		{ return 0; }
		void create_time(SR_TIME tm)
		{ }

		SR_TIME modify_time(void)
		{ return 0; }
		void modify_time(SR_TIME tm)
		{ }

		const char* author(void) const
		{ return ""; }
		void author(const char* auth)
		{ }

		const char* company(void) const
		{ return ""; }
		void company(const char* com)
		{ }

		const char* description(void) const
		{ return ""; }
		void description(const char* desc)
		{ }

		unsigned int res_id(void)
		{ return 0; }
		void res_id(unsigned int id)
		{ }

		int vid(void) const
		{ return 0; }
		void vid(int vd)
		{ }

		SR_DWORD version(void) const
		{ return 0; }
		void version(SR_DWORD ver)
		{ }

		SR_DWORD build(void) const
		{ return 0; }
		void build(SR_DWORD bn)
		{ }

		// TTS常用的访问函数
		bool big_endian(void) const
		{ return false; }
		void big_endian(bool be)
		{ }

		const char* bundle(void) const
		{ return 0; }
		void bundle(const char* bd)
		{ }

		SSYSTEM_TYPE sys_type(void) const
		{ return st_tts; }
		void sys_type(SSYSTEM_TYPE st)
		{ }

		SRES_PROPERTY res_prop(void) const
		{ return rp_system; }
		void res_prop(SRES_PROPERTY rp)
		{ }

		SRES_TYPE res_type(void) const
		{ return rt_unknown; }
		void res_type(SRES_TYPE rt)
		{ }

		SRES_LANG res_lang(void) const
		{ return rl_unknown; }
		void res_lang(SRES_LANG sl)
		{ }

		SV_DIALECT dialect(void) const
		{ return vd_mandarin; }
		void dialect(SV_DIALECT sd)
		{ }

		unsigned int data_size(void) const
		{ return 0; }
		void data_size(unsigned int size)
		{ }
	};

	/**
	* @class	SRes_head
	*
	* @brief	合成系统资源头的接口，所有资源需要提供一个资源头的接口
	*
	*  只要用户实现了这个接口，那么资源的格式也就可以自行定义，
	*  不必拘泥于现在的资源头格式
	*
	* @author	jdyu
	* @date	2005-10-24
	*/
	struct SRes_head
		: public SRes_head_com
	{
		// 常用的访问函数
		virtual unsigned int res_id(void) = 0;
		virtual void res_id(unsigned int) = 0;

		virtual SSYSTEM_TYPE sys_type(void) const = 0;
		virtual void sys_type(SSYSTEM_TYPE) = 0;

		virtual SRES_PROPERTY res_prop(void) const = 0;
		virtual void res_prop(SRES_PROPERTY) = 0;

		virtual SRES_TYPE res_type(void) const = 0;
		virtual void res_type(SRES_TYPE) = 0;

		virtual SRES_LANG res_lang(void) const = 0;
		virtual void res_lang(SRES_LANG) = 0;

		virtual int vid(void) const = 0;
		virtual void vid(int vd) = 0;

		virtual SV_DIALECT dialect(void) const = 0;
		virtual void dialect(SV_DIALECT) = 0;
	};

	/**
	* @class	 SRes_Head_T
	*
	* @brief	资源头的模版适配类
	*
	*  这个类处理资源头相关的操作，功能：
	*   1. 派生自基础的头接口，继承其功能
	*   2. 定制头的长度，也可以不使用头
	*
	*/
	template <typename HEAD_STRUCT = SRes_head_c<>, int HEAD_SIZE = SRES_DEFHEADSIZE, typename HEAD_INTFACE = SRes_head>
	class SRes_Head_T
		: public SRes_Head_Com_T<HEAD_STRUCT, HEAD_SIZE, HEAD_INTFACE>
	{
		// parent class
		typedef SRes_Head_Com_T<HEAD_STRUCT, HEAD_SIZE, HEAD_INTFACE> parent;

	public:
		// 常用的访问函数
		int vid(void) const
		{ return parent::head_.vid(); }
		void vid(int vd)
		{ parent::head_.vid(vd); }

		SSYSTEM_TYPE sys_type(void) const
		{ return parent::head_.sys_type(); }
		void sys_type(SSYSTEM_TYPE st)
		{ parent::head_.sys_type(st); }

		SRES_PROPERTY res_prop(void) const
		{ return parent::head_.res_prop(); }
		void res_prop(SRES_PROPERTY rp)
		{ parent::head_.res_prop(rp); }

		SRES_TYPE res_type(void) const
		{ return parent::head_.res_type(); }
		void res_type(SRES_TYPE rt)
		{ parent::head_.res_type(rt); }

		SRES_LANG res_lang(void) const
		{ return parent::head_.res_lang(); }
		void res_lang(SRES_LANG sl)
		{ parent::head_.res_lang(sl); }

		unsigned int res_id(void)
		{ return parent::head_.res_id(); }
		void res_id(unsigned int id)
		{ parent::head_.res_id(id); }

		SV_DIALECT dialect(void) const
		{ return parent::head_.dialect(); }
		void dialect(SV_DIALECT sd)
		{ parent::head_.dialect(sd); }

		unsigned int data_size(void) const
		{ return parent::head_.data_size(); }
		void data_size(unsigned int size)
		{ parent::head_.data_size(size); }

		SRes_Head_T(void)
		{ }
		~SRes_Head_T(void)
		{ }
	};

	///////////////////////////////////////////////////////////////////////
	// 空的配置读取写入类
	////////////////////////////////////////////////////////////////////////
	class SRes_Config_Heap_Null
	{
	public:
		int open(const char* root, int type = 0, const char* cfg = 0)
		{ return 0; }
		int close(bool save_cfg = false)
		{ return 0;	}
		int save(const char* cfg = 0)
		{ return 0;	}

		int get_value(const char* key, char* value, int & size, const char* sub = 0)
		{ return 0;	}
		int set_value(const char* key, const char* value, const char* sub = 0)
		{ return 0;	}

		virtual int enum_values(char * values[], int & count, const char* sub = 0)
		{ return 0;	}
		virtual int remove_key(const char* key, const char* sub = 0)
		{ return 0;	}
	};

	// 配置类
	template <class CFG_BASE = SRes_Cfg_b, class CFG_HEAP = SRes_Config_Heap_Null>
	class SRes_Cfg_Impl_T
		: public CFG_BASE
	{
	public:
		virtual int open (const char* key, int type = 0, const char* cfg = 0)
		{
			if ( key )
				root_ = key;
			return cfg_.open(key, cfg_auto, cfg);
		}
		virtual int close(bool save_cfg = false)
		{ if ( save_cfg ) save(); return 0; }
		virtual int save(const char* cfg = 0)
		{ return 0; }

		virtual int get_value(const char* key, char* value, int & size, const char* sub = 0)
		{ return cfg_.get_value(key, value, size, sub); }
		virtual int set_value(const char* key, const char* value, const char* sub = 0)
		{ return cfg_.set_value(key, value, sub); }

		virtual int enum_values(char* values[], int & count, const char* sub = 0)
		{ return cfg_.enum_values(values, count, sub); }
		virtual int remove_key(const char* key, const char* sub = 0)
		{ return cfg_.remove_key(key, sub); }

		virtual bool modified(void) const
		{ return modf_; }
		virtual void modified(bool mod_flag)
		{ modf_ = mod_flag; }

		virtual SRES_HANDLE handle(void)
		{ return hdl_; }
		virtual void handle(SRES_HANDLE hdl)
		{ hdl_ = hdl; }

		virtual const char * root(void) const
		{ return root_.c_str(); }

		SRes_Cfg_Impl_T(void)
			: modf_	(false)
			, hdl_	(0)
		{ }
		virtual ~SRes_Cfg_Impl_T(void)
		{ }

	private:
		// 配置读取写入类
		CFG_HEAP	cfg_;
		// 修改标记
		bool		modf_;
		// 配置句柄
		SRES_HANDLE hdl_;
		// 根键
		std::string root_;
	};

	/** 
	* @class	SRes_Cfg_i
	* 
	* @brief	每个资源的配置类
	* 
	*  每个资源的配置信息
	* 
	* @author	jdyu
	* @date	2007-7-5
	* 
	* @see		
	*/
	class SRes_Cfg_i
		: public SRes_Cfg_Impl_T<SRes_Cfg>
	{
	public:
		SRES_LOADMODE mode(void) const
		{ return mode_; }
		SRES_ACCESSMODE access(void) const
		{ return access_; }
		SRES_PRELOAD preload(void) const
		{ return preload_; }
		const char* path(void) const
		{ return path_.c_str(); }
		const char* profile(void) const
		{ return profile_.c_str(); }

		void mode(SRES_LOADMODE lm)
		{ mode_ = lm; }
		void access(SRES_ACCESSMODE acc)
		{ access_ = acc; }
		void preload(SRES_PRELOAD pl)
		{ preload_ = pl; }
		void path(const char* pt)
		{ path_ = pt; }
		void profile(const char* pf)
		{ profile_ = pf; }

		virtual const char* name(void) const
		{ return name_.c_str(); }
		virtual void name(const char * nm)
		{ name_ = nm; }

		SRes_Cfg_i(void)
			: mode_(rlm_file), access_(ram_readonly), preload_(rpl_auto)
		{ }

	private:
		SRES_LOADMODE	mode_;				///< 装载方式
		SRES_ACCESSMODE	access_;			///< 访问权限
		SRES_PRELOAD	preload_;			///< 是否预加载
		std::string 	path_;				///< 资源路径
		std::string 	profile_;			///< 资源配置键(注册键)
		std::string		name_;
	};

	////////////////////////////////////////////////////////////////////////
	// 资源包的配置信息
	////////////////////////////////////////////////////////////////////////
	/** 
	* @class	SRes_Pack_Cfg
	* 
	* @brief	资源包的配置信息
	* 
	*  每个资源包都包含这样的一个配置类
	* 
	* @author	jdyu
	* @date	2007-7-5
	* 
	* @see		
	*/
	class SRes_Pack_Cfg
		: public SRes_Cfg_b
	{
	public:
		virtual const char* name(void) const = 0;
		virtual void name(const char* nm) = 0;

		virtual const char* path(void) const = 0;
		virtual void path(const char* pth) = 0;

		virtual const char* desc(void) const = 0;
		virtual void desc(const char* dsc) = 0;

		virtual const char* profile(void) const = 0;
		virtual bool disable(void) const = 0;
	};

	// 资源包配置的缺省实现
	class SRes_Pack_Cfg_i
		: public SRes_Cfg_Impl_T<SRes_Pack_Cfg>
	{
	public:
		const char* name(void) const
		{ return name_.c_str(); }
		void name(const char* nm)
		{ if ( nm ) name_ = nm; }
		const char* path(void) const
		{ return path_.c_str(); }
		void path(const char* pth)
		{ path_ = pth; }
		const char* desc(void) const
		{ return desc_.c_str(); }
		void desc(const char* dsc)
		{ desc_ = dsc; }
		const char* profile(void) const
		{ return profile_.c_str(); }
		bool disable(void) const
		{ return disable_; }

		SRes_Pack_Cfg_i(void)
			: ver_(0), disable_(false) { }
		virtual ~SRes_Pack_Cfg_i(void)
		{ }

	private:
		// 资源包名称
		std::string name_;
		// 安装路径
		std::string path_;
		// 描述信息
		std::string desc_;
		// 注册键
		std::string profile_;
		// 版本信息
		SR_DWORD	ver_;
		// 是否启用
		bool		disable_;
	};

	// 资源管理器配置接口
	class SRes_Pack_Mngr_Cfg
		: public SRes_Cfg_b
	{
	public:
		virtual const char*		path(void)			const = 0;
		virtual const char*		profile(void)		const = 0;
		virtual SRES_INITTYPE	init_type(void)		const = 0;
		virtual const char*		default_pack(void)	const = 0;
	};

	// 缺省实现
	class SRes_Pack_Mngr_Cfg_i
		: public SRes_Cfg_Impl_T<SRes_Pack_Mngr_Cfg>
	{
	public:
		virtual const char*	path(void) const
		{ return path_.c_str(); }
		virtual void path(const char* pth)
		{ path_ = pth; }

		virtual const char*	profile(void) const
		{ return profile_.c_str(); }
		virtual void profile(const char* prf)
		{ profile_ = prf; }

		virtual SRES_INITTYPE init_type(void) const
		{ return init_type_; }
		virtual void init_type(SRES_INITTYPE sit)
		{ init_type_ = sit; }

		virtual const char*	default_pack(void) const
		{ return def_pack_.c_str(); }
		virtual void default_pack(const char* dp)
		{ if ( dp != 0 ) def_pack_ = dp; }

		SRes_Pack_Mngr_Cfg_i(void)
			: path_("resource"), init_type_(sit_kernel), def_pack_("custom")
		{ }

	private:
		// 路径信息
		std::string		path_;
		// 注册键位置
		std::string		profile_;
		// 初始化信息
		SRES_INITTYPE	init_type_;
		// 缺省的资源包
		std::string		def_pack_;
	};

	// ============================================================================
	// 接口管理器
	// 如果多个dll中都会实现同样的接口，那么就需要统一进行管理
	// ============================================================================
	struct SRes_Interface_Mngr
	{
		virtual const char * mngr_name(void) = 0;
		virtual const char * intf_name(void) = 0;
		virtual int intf_add(void * intf) = 0;
		virtual int intf_remove(void * intf) = 0;
	};

	// 缺省的实现
	template <class BASE, class IM = SRes_Interface_Mngr>
	class SRes_Interface_Mngr_T
		: public BASE
		, public IM
	{
	public:
		virtual const char * mngr_name(void)
		{ return mngr_name_.c_str(); }
		virtual const char * intf_name(void)
		{ return intf_name_.c_str(); }
		virtual int intf_add(void * intf, const char * disc)
		{ return 0;	}
		virtual int intf_remove(void * intf)
		{ return 0;	}

		SRes_Interface_Mngr_T(const char * mngr_nm, const char * intf_nm)
			: mngr_name_	(mngr_nm)
			, intf_name_	(intf_nm)
		{ }
		virtual ~SRes_Interface_Mngr_T(void)
		{ }

	protected:
		// 管理员名称
		std::string		mngr_name_;
		// 被管理的接口名称
		std::string		intf_name_;
	};


	//===========================================================================================
	// 辅助类
	//===========================================================================================
	namespace aux {

		// 资源创建接口
		struct res_foundry
		{
			virtual int  res_create(const char * res_name, SRes_Resource ** res_pp) const
			{ return -1; }
			virtual int  res_create(int res_id, SRes_Resource ** res_pp) const
			{ return -1; }
			virtual int  res_destroy(SRes_Resource * res) const
			{ return -1; }
			virtual int  intf_create(const char * intf_name, void ** intf_pp) const
			{ return -1; }
			virtual int  intf_destroy(const char * intf_name, void * intf_p) const
			{ return -1; }
			virtual void release(void) = 0;
		};

		// 资源创建实现
		template <class RES>
		class res_foundry_t
			: public res_foundry
		{
		public:
			res_foundry_t(const char * res_name, int res_id = 0)
				: name_			(res_name)
				, id_			(res_id)
			{
			}
			virtual int res_create(const char * res_name, SRes_Resource ** res_pp) const
			{
				if ( name_.empty() || res_name == 0 || name_ != res_name )
					return -1;
				*res_pp = new RES;
				return 0;
			}
			virtual int res_create(int res_id, SRes_Resource ** res_pp) const
			{
				if ( id_ == 0 || res_id == 0 || id_ != res_id )
					return -1;
				*res_pp = new RES;
				return 0;
			}
			virtual int res_destroy(SRes_Resource * res) const
			{
				if ( ( !name_.empty() && name_ == res->name() ) 
					|| ( id_ != 0 && id_ == res->id() ) )
				{
					delete dynamic_cast<RES*>(res);
					return 0;
				}
				return -1;
			}
			virtual void release(void)
			{
				delete this;
			}

		private:
			std::string name_;
			int			id_;
		};

		// 接口创建实现
		template <class INTF>
		class intf_foundry_t
			: public res_foundry
		{
		public:
			intf_foundry_t(const char * res_name)
				: name_			(res_name)
			{
			}
			virtual int  intf_create(const char * intf_name, void ** intf_pp) const
			{
				if ( name_.empty() || name_ != intf_name )
					return -1;
				*intf_pp = new INTF;
				return 0;
			}
			virtual int  intf_destroy(const char * intf_name, void * intf_p) const
			{
				if ( name_.empty() || name_ != intf_name )
					return -1;
				delete static_cast<INTF*>(intf_p);
				return 0;
			}
			virtual void release(void)
			{
				delete this;
			}

		private:
			std::string name_;
		};

		// 帮助实现接口生成的类
		class res_group
		{
		public:
			res_group(res_foundry * rcfs[], int count)
				: res_fdrs_		(rcfs)
				, fdr_count_	(count)
			{
			}
			virtual ~res_group(void)
			{
				if ( !res_list_.empty() )
					printf("res_group::~res_group not all resource released!\n");

				for ( int i = 0; i < fdr_count_; i++ )
					res_fdrs_[i]->release(), res_fdrs_[i] = 0;
			}

			template <class type>
			int create_resource(const type res_name, SRes_Resource ** res_pp)
			{
				for ( int i = 0; i < fdr_count_; i++ )
				{
					int ret = res_fdrs_[i]->res_create(res_name, res_pp);
					if ( *res_pp )
					{
						res_list_.push_back(*res_pp);
						return ret;
					}
				}
				return -1;
			}
			int destroy_resource(SRes_Resource * res)
			{
				std::list<void*>::iterator pi = std::find(res_list_.begin(), res_list_.end(), res);
				if ( pi == res_list_.end() )
					return -1;

				for ( int i = 0; i < fdr_count_; i++ )
				{
					int ret = res_fdrs_[i]->res_destroy(res);
					if ( ret == 0 )
					{
						res_list_.erase(pi);
						return ret;
					}
				}
				printf("Cannot find resource foundry!");
				return -1;
			}
			int create_interface(const char * intf_name, void ** intf_pp)
			{
				for ( int i = 0; i < fdr_count_; i++ )
				{
					int ret = res_fdrs_[i]->intf_create(intf_name, intf_pp);
					if ( *intf_pp )
					{
						res_list_.push_back(*intf_pp);
						return ret;
					}
				}
				return -1;
			}
			int destroy_interface(const char * intf_name, void * intf_p)
			{
				std::list<void*>::iterator pi = std::find(res_list_.begin(), res_list_.end(), intf_p);
				if ( pi == res_list_.end() )
					return -1;

				for ( int i = 0; i < fdr_count_; i++ )
				{
					int ret = res_fdrs_[i]->intf_destroy(intf_name, intf_p);
					if ( ret == 0 )
					{
						res_list_.erase(pi);
						return ret;
					}
				}
				printf("Cannot find interface foundry!");
				return -1;
			}
		private:
			std::list<void*>	res_list_;
			res_foundry **		res_fdrs_;
			int					fdr_count_;
		};

		/// 资源创建销毁入口的宏
#define SRES_ENTRY_BEGIN(mod_name)	static sres::aux::res_foundry* name##_res_entrys_[] = {
#define		SRES_RES_ENTRY_NAME(res_name, res_cls)	new sres::aux::res_foundry_t<res_cls>(res_name, 0),
#define		SRES_RES_ENTRY_ID(  res_id,   res_cls)	new sres::aux::res_foundry_t<res_cls>(0, res_id),
#define SRES_ENTRY_END(mod_name)						};	\
	sres::aux::res_group name##_res_group_(name##_res_entrys_, sizeof(name##_res_entrys_)/sizeof(name##_res_entrys_[0]));		\
	extern "C" int SRES_EXPORT SRESAPI sres_create_from_name(const char* name, SRes_Resource** res, SRes_Pack * pack, SRES_INITTYPE init)	\
		{ return name##_res_group_.create_resource(name, res); }				\
		extern "C" int SRES_EXPORT SRESAPI sres_create_from_id(int id, SRes_Resource** res, SRes_Pack * pack, SRES_INITTYPE init)	\
		{ return name##_res_group_.create_resource(id, res); }					\
		extern "C" int SRES_EXPORT SRESAPI sres_destroy_resource(SRes_Resource * res, SRES_INITTYPE init)	\
		{ return name##_res_group_.destroy_resource(res); }

#define SRES_RESOURCE_ENTRY(res_name, res_cls, mod_name)	\
	SRES_ENTRY_BEGIN(mod_name)	\
	SRES_RES_ENTRY_NAME(res_name, res_cls)	\
	SRES_ENTRY_END(mod_name)
	} /* namespace sres::aux */

	//===========================================================================================
	// 辅助函数
	//===========================================================================================

	/// 获得资源管理器，外部实现
	inline SRes_Pack_Mngr * res_mngr(void);

	/// 查找资源包的资源
	template <class RES>
	inline RES * find_pack_resource(SRes_Pack * pack, const char * name, const char * token = 0)
	{
		RES * res = dynamic_cast<RES*>
			(pack->find_resource(name, token));
		return res;
	}

	/// 查找资源包的资源
	template <class RES>
	inline RES * find_pack_resource(SRes_Pack_Mngr * pm, const char * pack_name, const char * res_name, const char * token = 0)
	{
		SRes_Pack * pack = pm->find_package(pack_name);
		if ( pack == 0 )
			return 0;
		return find_pack_resource<RES>(pack, res_name, token);
	}

	/// 查找资源包的资源
	template <class RES>
	inline RES * find_pack_resource(const char * pack_name, const char * res_name, const char * token = 0)
	{
		return find_pack_resource<RES>(res_mngr(), pack_name, res_name, token);
	}

	/// 查找资源包的接口
	template <class INTF>
	inline INTF * find_pack_interface(SRes_Pack * pack, const char * intf_name, void * rev = 0)
	{
		INTF * intf = 0;
		int ret = pack->query_interface(intf_name, (void**)&intf, rev);
		return intf;
	}

	/// 查找资源包的接口
	template <class INTF>
	inline INTF * find_pack_interface(SRes_Pack_Mngr * pm, const char * pack_name, const char * intf_name, void * rev = 0)
	{
		SRes_Pack * pack = pm->find_package(pack_name);
		if ( pack == 0 )
			return 0;
		return find_pack_interface<INTF>(pack, intf_name, rev);
	}

	/// 查找资源包的接口
	template <class INTF>
	inline INTF * find_pack_interface(const char * pack_name, const char * intf_name, void * rev = 0)
	{
		return find_pack_interface<INTF>(res_mngr(), pack_name, intf_name, rev);
	}

	/// 查找资源包的接口
	template <class INTF>
	inline INTF * find_res_interface(const char * pack_name, const char * res_name, const char * token = 0, const char * intf_name = 0, void * rev = 0)
	{
		SRes_Resource * res = find_pack_resource<SRes_Resource>(pack_name, res_name, token);
		if ( res == 0 )
			return 0;

		INTF * intf = 0;
		int ret = res->query_interface(intf_name, (void**)&intf, rev);
		return intf;
	}

	/// 返回标准包的指针
	inline SRes_Pack* res_std_package(void)
	{
		return res_mngr()->std_package();
	}

	/// 查找标准包的资源
	template <class RES>
	inline RES * find_std_resource(const char * name, const char * token = 0)
	{
		RES * res = static_cast<RES*>
			(res_std_package()->find_resource(name, token));
		SP_ASSERT(res);
		return res;
	}

	/// 查找标准包的资源接口
	template <class INTF>
	inline INTF * find_std_interface(const char * res_name, const char * token = 0, const char * intf_name = 0)
	{
		SRes_Resource * res = find_std_resource<SRes_Resource>(res_name, token);
		if ( res == 0 )
			return 0;

		INTF * intf = 0;
		int ret = res->query_interface(intf_name, (void**)&intf, 0);
		return intf;
	}

	/// 返回缺省资源包的指针
	inline SRes_Pack* res_default_package()
	{
		return res_mngr()->default_package();
	}

	/// 查找缺省资源包的资源
	template <class RES>
	inline RES * find_default_resource(const char * name, const char * token = 0)
	{
		RES * res = static_cast<RES*>
			(res_default_package()->find_resource(name, token));
		SP_ASSERT(res);
		return res;
	}

	/// 查找缺省包的资源接口
	template <class INTF>
	inline INTF * find_default_interface(const char * res_name, const char * token = 0, const char * intf_name = 0)
	{
		SRes_Resource * res = find_default_interface<SRes_Resource>(res_name, token);
		if ( res == 0 )
			return 0;

		INTF * intf = 0;
		int ret = res->query_interface(intf_name, (void**)&intf, 0);
		return intf;
	}

	/// 查找位于资源管理器的接口
	template <class INTF>
	inline INTF * find_interface(const char * intf_name)
	{
		INTF * intf = 0;
		int ret = res_mngr()->query_interface(intf_name, (void**)&intf, 0);
		return intf;
	}

} /* sres */

// to skip sres:: :-)
using namespace sres;

// api defines
#ifdef __cplusplus
extern "C" {
#endif

	//===========================================================================================
	// 用于创建资源的API
	// 用户的资源可以使用几个函数进行创建
	// 资源创建接口只需要其中之一就可以了
	//===========================================================================================
	const char PN_create_from_name			[] = "sres_create_from_name";
	const char PN_create_from_id			[] = "sres_create_from_id";
	const char PN_create_from_file			[] = "sres_create_from_file";
	const char PN_destroy_resource			[] = "sres_destroy_resource";
	const char PN_query_support_resources	[] = "sres_query_support_resources";

	const char PN_create_interface			[] = "sres_create_interface";
	const char PN_destroy_interface			[] = "sres_destroy_interface";
	const char PN_create_interface_mngr		[] = "sres_create_interface_mngr";
	const char PN_destroy_interface_mngr	[] = "sres_destroy_interface_mngr";
	const char PN_create_interface_merger	[] = "sres_create_interface_merger";
	const char PN_destroy_interface_merger	[] = "sres_destroy_interface_merger";
	const char PN_query_support_intfaces	[] = "sres_query_support_interfaces";

	const char PN_sres_mgmt_open			[] = "sres_mgmt_open";
	const char PN_sres_mgmt_close			[] = "sres_mgmt_close";
	const char PN_sres_mgmt_add_entry		[] = "sres_mgmt_add_entry";

	/// 通过资源名创建资源对象
	typedef int (SRESAPI *proc_sres_create_from_name)
		(const char* name, SRes_Resource** res, SRes_Pack * pack, SRES_INITTYPE init);
	/// 通过文件路径名创建资源对象
	typedef int (SRESAPI *proc_sres_create_from_file)
		(const char* resfile, SRes_Resource** res, SRes_Pack * pack, SRES_INITTYPE init);
	/// 通过资源ID创建资源对象
	typedef int (SRESAPI *proc_sres_create_from_id)
		(int id, SRes_Resource** res, SRes_Pack * pack, SRES_INITTYPE init);
	/// 释放资源对象，传进去的资源对象将被释放
	typedef int (SRESAPI *proc_sres_destroy_resource)
		(SRes_Resource * res, SRES_INITTYPE init);

	/// 查询支持的所有资源名称列表
	/// 返回的名称列表以“res_a=ver,res_b=ver,res_c=ver,res_d=ver”的形式
	typedef int (SRESAPI *proc_sres_query_support_resources)
		(char* res_list, int * len, SRES_INITTYPE init);

	/// 通过接口名称创建某些接口，例如Vid_Mngr
	typedef int (SRESAPI *proc_sres_create_interface)
		(const char * intf_name, void ** intf, SRes_Pack * pack, SRES_INITTYPE init, void * reserved);
	/// 释放指定的接口，例如Vid_Mngr
	typedef int (SRESAPI *proc_sres_destroy_interface)
		(const char * intf_name, void * intf, SRES_INITTYPE init);

	/// 合并接口，返回新建的合并接口
	typedef int (SRESAPI *proc_sres_create_interface_merger)
		(const char * intf_name, void ** intfm, void * intfs[], int intfc, SRES_INITTYPE init, void * reserved);
	/// 释放指定的接口合并器
	typedef int (SRESAPI *proc_sres_destroy_interface_merger)
		(const char * intf_name, void * intf, SRES_INITTYPE init);

	/// 通过接口名称创建某些接口的管理器，例如Vid_Mngr
	typedef int (SRESAPI *proc_sres_create_interface_mngr)
		(const char * intf_name, void ** intfm, SRes_Pack_Mngr * rcmngr, SRES_INITTYPE init, void * reserved);
	/// 释放指定的接口管理器
	typedef int (SRESAPI *proc_sres_destroy_interface_mngr)
		(const char * intf_name, void * intfm, SRES_INITTYPE init);

	/// 查询该模块支持的接口列表
	/// 返回的名称列表以“intf_a=ver,intf_b=ver”的形式
	typedef int (SRESAPI *proc_sres_query_support_intfaces)
		(char * intf_list, int * len, SRES_INITTYPE init);

	//===========================================================================================
	// 用于获得资源管理器的API
	// 由resmgmt模块实现
	//===========================================================================================

	/// 创建资源管理器对象
	int SRESAPI sres_mgmt_open(SRes_Pack_Mngr ** pack_mngr, const char * rcfg, void * reserved);
	typedef int (SRESAPI *proc_sres_mgmt_open)(SRes_Pack_Mngr ** pack_mngr, const char * rcfg, void * reserved);

	/// 销毁资源管理器对象
	int SRESAPI sres_mgmt_close(SRes_Pack_Mngr * pack_mngr);
	typedef int (SRESAPI *proc_sres_mgmt_close)(SRes_Pack_Mngr * pack_mngr);

	/// 添加资源入口，包含创建资源、销毁资源、创建接口、销毁接口
	int SRESAPI sres_mgmt_add_entry(const char * entry_name, void * res_entry, SRes_Pack_Mngr * pack_mngr);
	typedef int (SRESAPI *proc_sres_mgmt_add_entry)(const char * entry_name, void * res_entry, SRes_Pack_Mngr * pack_mngr);

	// option to ingore stdcall
#if 0
# ifdef _MSC_VER
#  pragma comment(linker, "/EXPORT:sres_create_from_id=_sres_create_from_id@16")
#  pragma comment(linker, "/EXPORT:sres_create_from_name=_sres_create_from_name@16")
#  pragma comment(linker, "/EXPORT:sres_destroy_resource=_sres_destroy_resource@8")
# endif // _MSC_VER
#endif

#if _MSC_VER > 1300
# pragma warning(pop)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SRES_H__ */
