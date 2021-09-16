/**
* @file	sres.h
* @brief	��Դ�����ͳһ����
*
* ����ļ���ϵͳ������Դ�Ľӿڶ��壬��Ҫ������
*	1.��Դ������ʽ�Ķ��壬��Դͷ����
*	2.��Դ����Ķ����ʵ��
*	3.��Դ������Դ����ӿڵĶ���
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
*  <tr> <td>1.1		<td>2002-8-5	<td>jdyu	<td>����ԭ��һЩ������ĵط������޸�(��Դ����ȱʡ��ȱʡ�����)</tr>
*  <tr> <td>1.2		<td>2005-10-11	<td>jdyu	<td>���ݼܹ��Ľ�����������޸�</tr>
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
* @brief	ͨ����Դ���ֿռ�
* 
*  ���������ϵͳ�޹ص���Դ����ռ䣬������Դ�������е�֧�Ŵ���
*  ʹ�����ﶨ���һϵ����ͷ������Ժܿ���֯һ������ܹ�����Դ������
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
#define SR_DWORD		unsigned int //changed by llgu Windows�º�Linux��λ��һ��
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
	// ��Դ��������Ҫ��һЩ��
	////////////////////////////////////////////////////////////////////////

	// ��Դͷȱʡ����
#define SRES_DEFHEADSIZE			1024
	// ��Դ���Ƶ���󳤶�
#define SRES_MAXNAMELEN				32
#define SRES_MAXDESLEN				256
	// ����־��Ϣ����
#define SRES_MAXFLAGLEN				32
	// ����������Ƴ���
#define SRES_MAXAUTHORLEN			16
	// ���˾���Ƴ���
#define SRES_MAXCOMPANYLEN			32
	// �����Դ�����ļ�����
#define SRES_MAXWARMUPFILE			10
	// ���Bundle���ĳ���
#define SRES_MAXBUNDLENAME			16

	// ��Դ�汾
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

	// ��������
	typedef struct res_pos { void* reserved; } * SRES_POSITION;

	// ��Դ��־��
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
	* ��ϵͳ�޹ص���Դ�ӿڶ�����
	**=====================================================================*/
	// @@

	/**
	* ��Դ�ļ��ļ��ط�ʽ
	*/
	enum SRES_LOADMODE
	{
		rlm_file = 0,		/* �ļ���ʽ(������) */
		rlm_memory,			/* �����ڴ� */
		rlm_mapping,		/* �ڴ�ӳ�� */
		rlm_sharemem,		/* �����ڴ� */
		rlm_getinfo,		/* ֻ��ȡ��Ϣ */
		rlm_filetext,		/* �ļ���ʽ(�ı�ģʽ) */
	};

	/**
	* ���ʿ���
	*/
	enum SRES_ACCESSMODE
	{
		ram_readonly = 0,	/* ֻ����ʽ */
		ram_readwrite,		/* ��д��ʽ */
		ram_append,			/* ׷��ģʽ */
	};

	/**
	* PreLoad ��ʽ
	*/
	enum SRES_PRELOAD
	{
		rpl_auto = 0,		/* �ɰ����Ծ��� */
		rpl_true,			/* ���� */
		rpl_false,			/* ���������� */
		rpl_excl,			/* �ų����� */
	};

	/* �������� */
	enum SRES_CFG_TYPE
	{
		cfg_auto,		/* �Զ�ʶ���������� */
		cfg_reg,		/* ע����͵����� */
		cfg_ini,		/* ini�͵����� */
		cfg_xml,		/* xml��ʽ�������ļ� */
	};

	/** 
	* @class	SRes_Cfg_b
	* 
	* @brief	����������Ļ���
	* 
	*  ������Դ������Ϣ��������������������Է���Ĳ�ѯ��Ϣ��д������
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
	* @brief	��Դ�����ýӿ�
	*
	*  ÿ����Դ��ʹ������һ�����ýӿڣ�
	*  ���Զ�ȡ��д��������Ϣ
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
	* @brief	����������Դ��ͨ�ýӿ�
	*
	*  ����ӿ�������������Դ��Ҫ��ͷ�ӿڣ�����������Դ��������ӿ�������
	*  Ϊ�˱�֤��Դ��ͨ���ͣ�������ȥ��TTS��ص�һЩ��Դͷ����
	*
	* @author	jdyu
	* @date	2005-10-25
	*
	* @see		TSResHead   InterPhonic 3.0 4.0�㷺ʹ�õ���Դ��ʽ
	*			SRes_head_t �Ƽ�5.0���Ժ��ϵͳʹ�õĸ�ʽ
	*
	*/
	struct SRes_head_com
	{
		/// ������Դͨ�õĽӿ�
		virtual int	head_size(void) = 0;
		virtual void head_size(int size) = 0;

		// �ж��ֽ���
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

		/// ����ͷ�������ڴ�
		virtual SR_BYTE * data(void) const = 0;

		/// ���ļ���ȡ���߰���Ϣд�뵽�ļ�
		virtual int read (const char* file) = 0;
		virtual int write(const char* file) = 0;
	};

	/**
	* @class	 SRes_Resource
	*
	* @brief	���������Դ�ӿ�
	*
	*  ϵͳ������Դ���������ӿ�������
	*  Ϊ����Դ���͵�ͨ���ԣ�ͬʱ���չ˵�ttsϵͳ�ķ����ԣ�
	*  ������res_head()�ӿڣ��������ttsϵͳ���Լ򵥵ķ���
	*
	* @author	jdyu
	* @date	2005-10-24
	*
	*/
	struct SRes_Resource
	{
		////////////////////////////////////////////////////////////////////
		// ��������
		////////////////////////////////////////////////////////////////////
		// ��Դ��װ�غ�ж��
		virtual int load(SRes_Cfg * cfg) = 0;
		virtual int unload(void) = 0;

		// ���¼��غͱ���
		virtual int reload(void) = 0;
		virtual int save(void) = 0;

		// ��Դ�ľ��
		virtual SRES_HANDLE handle(void) const = 0;
		// ��Դ�Ļ�������
		// �������������ʵ����Щ�����Ĳ�������
		virtual int open (SRES_HANDLE & hdl, const char* file, SRES_ACCESSMODE acm, SRES_LOADMODE lm) = 0;
		virtual int close(SRES_HANDLE hdl = 0) = 0;
		virtual int seek (int offset, int origin, SRES_HANDLE hdl = 0) = 0;
		virtual int read (void* data, size_t & size, unsigned int offset = 0, SRES_HANDLE hdl = 0) = 0;
		virtual int write(const void* data, size_t & size, unsigned int offset = 0, SRES_HANDLE hdl = 0) = 0;
		virtual const char * read_line(char* buf, int size, SRES_HANDLE hdl = 0) = 0;

		// ��Դ�����ü�����ע������û�ж��̱߳���
		virtual unsigned int increase_ref(void) = 0;
		virtual unsigned int decrease_ref(void) = 0;
		virtual unsigned int reference_count(void) = 0;

		// �����Դ�ڼ���֮���Ƿ��ⲿ�޸Ĺ�
		// �Ƿ���Ҫ���¼���
		virtual bool need_reload(void) const = 0;
		virtual void need_reload(bool reload = true) = 0;
		virtual bool is_loaded  (void) const = 0;

		// ��Դ�޸ı�ǣ������治ͬ����������޸����ڲ����޸ģ�������Դ��������
		virtual bool modified(void) const = 0;
		virtual void modified(bool mod) = 0;

		////////////////////////////////////////////////////////////////////
		// ���Ի�ȡ����
		////////////////////////////////////////////////////////////////////
		virtual SRes_head_com * com_head(void) const = 0;
		virtual SRes_head *		res_head(void) const = 0;
		virtual SRes_Pack *		package (void) const = 0;
		virtual void			package(SRes_Pack * pack) = 0;

		// ���ýӿ�
		virtual SRes_Cfg *		res_cfg (void) = 0;

		// �����Դ�ļ�������
		virtual const char* path(void) const = 0;
		virtual const char* name(void) const = 0;
		virtual int id(void) const = 0;

		// �ж���Դ�Ƿ�ƥ���ض�����
		// (���ڲ�����Դʱ��ƥ�䣬���������ʵ�ָú���)
		virtual bool match_token(const char* name, const char* token) const = 0;
		virtual bool match_token(const char* name, int wpara = 0, int lpara = 0) const = 0;
		virtual bool match_token(SR_DWORD token,   int wpara = 0, int lpara = 0) const = 0;

		// �ж���Դ�Ƿ���Ҫ�����߳���صľ��
		// (���ڶ��߳����ļ����ط�ʽ�����������ʵ�ָú���)
		virtual bool is_need_alloc_handle(void) const = 0;

		/// �߼��ӿ�
		// ��Դ��Ԥ��
		virtual int warm_up(void) = 0;
		// �����
		virtual int encoding(void) = 0;
		virtual int decoding(void) = 0;
		// ������Դ�ֽ���
		virtual int adjust_byte_order(short dest_bo) = 0;

		// ��ѯ�ӿ�
		virtual int query_interface(const char* name, void ** intf, void * reserved = 0) = 0;
	};

	/**
	* ��Դ�������ĳ�ʼ����ʽ
	*/
	enum SRES_INITTYPE
	{
		sit_kernel = 0,		/* �ϳ��ں˵��õĳ�ʼ�� */
		sit_user,			/* �û�������õ� */
		sit_adv_user,		/* �߼��û����õ� */
		sit_config			/* ֻ��ȡ������Ϣ��������������Դ */
	};

	/**
	* @class	 SRes_Pack
	*
	* @brief	��Դ���ӿ�
	*
	*  ��Դ����һϵ����Դ�ļ��ϣ�һ����Դ��������԰���
	*  �����ͬ���͵���Դ��
	*
	* @author	jdyu
	* @date	2005-10-24
	*
	*/
	struct SRes_Pack
	{
		// ��ʼ�������ʼ��
		virtual int initialize(const SRes_Pack_Cfg * cfg, SRes_Pack_Mngr* pmgr) = 0;
		virtual int uninitialize(void) = 0;

		// ��Ϣ�Ĳ�ѯ��д��
		virtual const SRes_Pack_Cfg * pack_cfg(void) const = 0;
		virtual SRes_Pack_Cfg * pack_cfg(void) = 0;

		// ��ѯ��Դ������
		virtual const char* name(void) const = 0;
		virtual const char* path(void) const = 0;
		virtual bool is_default (void) const = 0;
		virtual bool is_standard(void) const = 0;

		// ������Դ�ı���
		virtual SRES_POSITION	first_resource	(int res_type = 0) = 0;
		virtual SRes_Resource *	next_resource	(SRES_POSITION & pos, int res_type = 0) = 0;
		virtual SRes_Resource *	get_resource	(SRES_POSITION pos) = 0;

		// �������
		virtual SR_DWORD		resource_count	(int res_type = 0) = 0;

		// ��Դ����ƥ��
		virtual bool match_token(const char* token) const = 0;

		// ��Դ�Ĳ���
		virtual SRes_Resource * find_resource(const char* name, const char* token) = 0;
		virtual SRes_Resource * find_resource(const char* name, int wpara = 0, int lpara = 0) = 0;
		virtual SRes_Resource *	find_resource(SR_DWORD   token, int wpara = 0, int lpara = 0) = 0;
		// ��Դ���ͷţ���find_resource������Դ����ʹ��ʱ�����Ե����������
		virtual int release_resource(SRes_Resource * res) = 0;

		// ��Դ�ӿڲ��ң��ҵ�ָ������Դ��ͬʱ�Ѻ͸���Դ�󶨵Ľӿڷ��ػ������൱��find_resource��query_interface�����
		virtual void * find_interface(const char * res_name, const char * token = 0, const char * intf_name = 0, SRes_Resource ** res = 0) = 0;
		virtual int release_interface(void * res_intf) = 0;

		// ��Ӻ�ɾ����Դ
		virtual int add_resource(const SRes_Resource * res) = 0;
		virtual int remove_resource(SRES_POSITION pos) = 0;

		// ��Դ����Ԥ��
		virtual int warm_up(void) = 0;
		virtual int refresh(void) = 0;

		// �����ص���Դ��������
		virtual struct SRes_Pack_Mngr* respack_mngr(void) const = 0;

		// ��ѯVID�ӿڵ������ӿ�
		virtual int query_interface(const char* name, void ** intf, void * reserved = 0) = 0;
	};

	/**
	* @class	SRes_Pack_Mngr
	*
	* @brief	��Դ����ӿ�
	*
	*  �������ϵͳ��Դ�Ĵ��ܹܣ�������ȫ�ֵ������ʽ�ṩ��
	*  �û�Ҫ����������ǣ�
	*	1. ��ʼ��
	*	2. find_package, find resource
	*  3. ���ʼ��
	*
	* @author	jdyu
	* @date	2005-11-11
	*/
	struct SRes_Pack_Mngr
	{
		// ��ʼ�������ʼ��
		virtual int initialize(const char * rcfg, SRES_INITTYPE sit = sit_kernel, void * rev = 0) = 0;
		virtual int uninitialize(void) = 0;

		// ������Դ���ı���
		virtual SRES_POSITION	first_package(void) = 0;
		virtual SRes_Pack *		next_package(SRES_POSITION & pos) = 0;
		virtual SRes_Pack *		package(SRES_POSITION pos) = 0;
		// �������
		virtual unsigned int	pack_count(void) const = 0;

		// ������Դ��
		virtual SRes_Pack *		find_package(const char * name, const char * token = 0) = 0;

		// ��ñ�׼��Դ��
		virtual SRes_Pack *		std_package(void) const = 0;
		// ���ȱʡ��Դ��
		virtual SRes_Pack *		default_package(void) const = 0;
		virtual int				default_package(const char* name) = 0;

		// ��ó�ʼ������
		virtual SRES_INITTYPE	init_type(void) const = 0;

		// ������Ϣ
		virtual const SRes_Pack_Mngr_Cfg * pm_cfg(void) const = 0;
		virtual SRes_Pack_Mngr_Cfg * pm_cfg(void) = 0;

		// ��־��Ϣ
		virtual Log_IO * logger(void) const = 0;
		virtual void logger(Log_IO * lgr) = 0;

		// ��Դ������Ӻ�ɾ��
		virtual int add_package(SRes_Pack * pack, SRES_POSITION & pos) = 0;
		virtual int remove_package(SRES_POSITION pos) = 0;

		// ������Դ����Ԥ��
		virtual int warm_up(void) = 0;

		// ��ѯVID�ӿڵ������ӿ�
		virtual int query_interface(const char* name, void ** intf, void * reserved = 0) = 0;
	};

	/**
	* @class	SRes_Head_Com_T
	*
	* @brief	һ�����Զ���ͷ�ṹ�����Դͷģ��
	*
	*  ���ģ��ר��Ϊ��һЩ����ʹ��ȱʡ��Դͷ����Դ��д
	*  ���磺��Щ����Ϊ1024�ֽڵ�ͷ̫���ˣ��Լ���ʵ�־���İ汾
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
		// ������Դͨ�õĽӿ�
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

		// ����Ҫ�Ľӿ�
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

		/// ����ͷ�������ڴ�
		virtual SR_BYTE * data(void) const
		{ return (SR_BYTE *)&head_; }

		// ���ļ��ж�ȡͷ��Ϣ��д��ͷ��Ϣ���ļ�
		virtual int read (const char* file)
		{ return 0; }
		virtual int write(const char* file)
		{ return 0; }

	public:
		// ����͸�ֵ
		SRes_Head_Com_T(void)
			: head_()
		{ memset(data_, 0, sizeof data_); }
		SRes_Head_Com_T(const HEAD_STRUCT & hd)
		{ set(hd); }
		SRes_Head_Com_T & operator = (const SRes_Head_Com_T & st)
		{ set(&st, st.head_size());	return *this; }

		// ����ͷ�ṹ��
		HEAD_STRUCT	& head_struct(void)
		{ 
			return head_;	
		}

		// ����
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
		// ͷ�Ĵ洢�ṹ
		// ��׼ͷ��Ϣ
		HEAD_STRUCT	head_;
		// ������Դ���г���ΪHEAD_SIZE��ͷ�ṹ
		SR_BYTE		data_[HEAD_SIZE - sizeof(HEAD_STRUCT)];
	};

	/**
	* @class	 SRes_Resource_T
	*
	* @brief	һ����Դ��ȱʡʵ��
	*
	*  ʹ��ģ�����ʽ����һ����Դ��ȱʡʵ�֣��û�����ֱ�Ӵ����ģ������
	*  �򻯱�̣����ģ����Զ�����Դͷ�ṹ��������ͻ���
	*  ע�⣺���ģ�������һ����ܣ������ʵ�ֺ��٣��û�����Ҫ������Ҫ����һ���������ʵ����
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
		// ��������
		// ������������Դ�඼��Ҫ���صķ���
		////////////////////////////////////////////////////////////////////
		// ��Դ��װ�غ�ж��
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
		// �߼�����
		// ��Ҫ�ض����ܵ�����Ҫ���صķ���
		////////////////////////////////////////////////////////////////////
		virtual int reload(void)
		{ int ret = unload(); return load(&cfg_); }
		virtual int save(void)
		{ return 0; }

		// ��Դ���
		virtual SRES_HANDLE handle(void) const
		{ return this->hdl_; }

		// ��Դ�����ü���
		virtual unsigned int increase_ref(void) 
		{ return ++ ref_count_ == 0 ? ++ ref_count_	: ref_count_; }
		virtual unsigned int decrease_ref(void)
		{ return -- ref_count_ == 0 ? -- ref_count_	: ref_count_; }
		virtual unsigned int reference_count(void)
		{ return ref_count_; }

		// �����Դ�ڼ���֮���Ƿ��޸Ĺ�
		virtual bool need_reload(void) const
		{ return need_reload_; }
		virtual void need_reload(bool reload = true)
		{ this->need_reload_ = reload; }
		// �Ƿ���ع�
		virtual bool is_loaded(void) const
		{ return hdl_ != 0; }

		// ��Դ�޸ı�ǣ������治ͬ����������޸����ڲ����޸ģ�������Դ��������
		virtual bool modified(void) const
		{ return modf_; }
		virtual void modified(bool mod)
		{ modf_ = mod; }

		////////////////////////////////////////////////////////////////////
		// ���Ի�ȡ����
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
		// ���ýӿ�
		virtual SRes_Cfg * res_cfg (void)
		{ return &this->cfg_; }

		// �����Դ�ļ�������
		virtual const char * path(void) const
		{ return this->fn_.c_str(); }
		virtual const char * name(void) const
		{ return this->name_.c_str(); }
		virtual int id(void) const
		{ return this->id_; }

	public:
		// ���������ʹ�õĺ���
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

		// ��þ��
		virtual FILE * file_hdl(SRES_HANDLE hdl = 0) const
		{ return  hdl == 0 ? (FILE*)hdl_ : (FILE*)hdl;  }
		// ������ݵ���ʼλ��
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
		// ��Դ�Ļ�������
		// �����ṩ�ļ���ʽ���ڴ���ʽ��ȱʡʵ��
		virtual int open (SRES_HANDLE & hdl, const char* file, SRES_ACCESSMODE acm, SRES_LOADMODE lm)
		{
			// ��óߴ�
			if ( fsize_ == 0 )
			{
				struct stat si;
				if ( ::stat(file, &si) == 0 )
					fsize_ = si.st_size;
			}

			// ���ļ�
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
		// �ر�һ�����
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
		// ��ȡ����
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
		// д������
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
		// ��λ��ָ����λ��
		virtual int seek (int offset, int origin, SRES_HANDLE hdl = 0)
		{
			if ( cfg_.mode() == rlm_file || cfg_.mode() == rlm_filetext )
				return fseek(file_hdl(hdl), offset, origin);
			return 0;
		}
		// ��ȡһ�У������ı��ļ���Ч
		virtual const char * read_line(char* buf, int size, SRES_HANDLE hdl = 0)
		{
			if ( cfg_.mode() != rlm_filetext )
				return 0;
			return fgets(buf, size, file_hdl(hdl));
		}

		// ========================================================================
		// �߼��ӿ�
		// ��Դ��Ԥ��
		virtual int warm_up(void)
		{ return 0; }
		// �����
		virtual int encoding(void)
		{ return 0; }
		virtual int decoding(void)
		{ return 0; }
		// ������Դ�ֽ���
		virtual int adjust_byte_order(short dest_bo)
		{ return 0; }

		// ��ѯ�ӿ�
		virtual int query_interface(const char* name, void ** intf, void * reserved = 0)
		{ return -1; }

		// ���������
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
		// ������Դ���ݵ�ַ
		SR_BYTE * res_data(void) const
		{
			if ( cfg_.mode() == rlm_memory )
				return (SR_BYTE*)hdl_ + align_rev_;
			else
				return 0;
		}

	protected:
		// �ж���Դ�Ƿ�ƥ���ض�����
		// (���ڲ�����Դʱ��ƥ�䣬���������ʵ�ָú���)
		virtual bool match_token(const char* nm, const char* token = 0) const
		{ return strcmp(nm, name()) == 0; }
		virtual bool match_token(const char* nm, int wpara, int lpara = 0) const
		{ return strcmp(nm, name()) == 0; }
		virtual bool match_token(SR_DWORD token, int wpara, int lpara = 0) const
		{ return true; }

		// �ж���Դ�Ƿ���Ҫ�����߳���صľ��
		// (���ڶ��߳����ļ����ط�ʽ�����������ʵ�ָú���)
		virtual bool is_need_alloc_handle(void) const
		{ return false; }

	private:
		// ��Դͷ
		RHEAD			head_;
		// ������Ϣ
		CFG 			cfg_;
		// ��Դ���
		SRES_HANDLE		hdl_;
		unsigned int	fsize_;
		// ��Դ��
		SRes_Pack *		pack_;
		// ��Դ·��
		std::string		fn_;
		// ��Դ����
		std::string		name_;
		// ��Դid
		int				id_;
		// �Ƿ���Ҫ���¼���
		bool			need_reload_;
		// �Ƿ��޸Ĺ�
		bool			modf_;
		// ���ü���
		unsigned int	ref_count_;
		// ���뱣���ĳ���
		unsigned int	align_rev_;
	};

	/**
	* @class	SRes_Pack_T
	*
	* @brief	��Դ����ȱʡʵ��
	*
	*  ��Դ����ģ�������࣬����ʵ����һ���򻯵���Դ���ӿ�
	*  ���Դ�����������Դﵽ�򻯵�Ŀ��
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
		// ��Ϣ�Ĳ�ѯ��д��
		virtual const SRes_Pack_Cfg * pack_cfg(void) const
		{ return &pack_cfg_; }
		virtual SRes_Pack_Cfg * pack_cfg(void)
		{ return &pack_cfg_; }

		// ��ѯ��Դ������
		virtual bool is_default (void) const
		{ return respack_mngr_ && respack_mngr_->default_package() == this; }
		virtual bool is_standard(void) const
		{ return respack_mngr_ && respack_mngr_->std_package() == this; }

		// ������Դ�ı���
		virtual SRES_POSITION	first_resource	(int res_type = 0) = 0;
		virtual SRes_Resource *	next_resource	(SRES_POSITION & pos, int res_type = 0) = 0;
		virtual SRes_Resource *	get_resource	(SRES_POSITION pos) = 0;

		// �������
		virtual SR_DWORD resource_count	(int res_type = 0) = 0;

		// ��Դ��ƥ��
		virtual bool match_token(const char* token) const
		{ return false; }

		// ��Դ�Ĳ���
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
		// ��Դ���ͷţ���find_resource������Դ����ʹ��ʱ�����Ե����������
		virtual int release_resource(SRes_Resource * res)
		{ return 0; }

		// ��Դ�ӿڲ��ң��ҵ�ָ������Դ��ͬʱ�Ѻ͸���Դ�󶨵Ľӿڷ��ػ������൱��find_resource��query_interface�����
		virtual void * find_interface(const char * res_name, const char * token = 0, const char * intf_name = 0, SRes_Resource ** res = 0)
		{ return 0; }
		virtual int release_interface(void * res_intf)
		{ return -1; }

		// ��Ӻ�ɾ����Դ
		virtual int add_resource(const SRes_Resource * pResource)
		{ return 0; }
		virtual int remove_resource(SRES_POSITION pos)
		{ return 0; }

		// ��Դ����Ԥ��
		virtual int warm_up(void)
		{ return 0; }
		virtual int refresh(void)
		{ return 0; }

		// �����ص���Դ��������
		virtual SRes_Pack_Mngr* respack_mngr(void) const
		{ return respack_mngr_; }
		virtual void respack_mngr(SRes_Pack_Mngr* resmgr)
		{ respack_mngr_ = resmgr; }

		// ��ѯVID�ӿڵ������ӿ�
		virtual int query_interface(const char* name, void ** intf, void * reserved = 0)
		{ return 0; }

		SRES_INITTYPE init_type(void) const
		{ return respack_mngr_->init_type(); }

	public:
		// ���캯��
		SRes_Pack_T(void)
			: respack_mngr_(0)
		{ }
		CFG & config(void)
		{ return pack_cfg_; }

	protected:
		// ��Դ�������ӿ�
		SRes_Pack_Mngr *	respack_mngr_;
		// ������Ϣ
		CFG					pack_cfg_;
	};

	/** 
	* @class	SRes_Pack_Mngr_T
	* 
	* @brief	��Դ����������ȱʡʵ��
	* 
	*  �ṩһ����Դ����������ȱʡʵ�֣���������������Լ򻯱���
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
		// ������Ϣ
		virtual const SRes_Pack_Mngr_Cfg * pm_cfg(void) const
		{ return &pm_cfg_; }
		virtual SRes_Pack_Mngr_Cfg * pm_cfg(void)
		{ return &pm_cfg_; }

		// ��ó�ʼ������
		virtual SRES_INITTYPE init_type(void) const
		{ return pm_cfg_.init_type(); }

		// ��ñ�׼��Դ��
		virtual SRes_Pack* std_package(void) const
		{ return 0; }
		// ���ȱʡ��Դ��
		virtual SRes_Pack* default_package(void) const
		{ return 0; }
		virtual int default_package(const char * name)
		{ pm_cfg_.default_pack(name); return 0;	}

		// �������
		virtual unsigned int pack_count(void) const
		{ return 0;	}

		// ��Դ������Ӻ�ɾ��
		virtual int add_package(SRes_Pack* pack, SRES_POSITION & pos)
		{ return 0; }
		virtual int remove_package(SRES_POSITION pos)
		{ return 0; }

		// ������Դ����Ԥ��
		virtual int warm_up(void)
		{ return 0; }
		// ��ѯVID�ӿڵ������ӿ�
		virtual int query_interface(const char* name, void ** intf, void * reserved = 0)
		{ return -1; }

		// ��־��Ϣ
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
		// ��Դ����������
		CFG					pm_cfg_;
	};

	// @@
	/**====================================================================
	* ��ϵͳ��ص���Դ�ӿڶ�����
	* Ŀǰ������TTSϵͳ����Դ����
	* ���ڿ��Ǻ�ǰ��Ķ���ֿ�
	*=====================================================================*/
	// @@

	////////////////////////////////////////////////////////////////////////
	// ϵͳ����
	////////////////////////////////////////////////////////////////////////
	typedef enum
	{
		st_tts = 0,				// TTSϵͳ
		st_asr = 1,				// ASRϵͳ
	} SSYSTEM_TYPE;

	////////////////////////////////////////////////////////////////////////
	// ��Դ����
	// ---------------------------------------------------------------------
	// ���ؾ��棺
	// Ϊ�˱�֤������Դ�ܹ���ȷ���غ���ǰ�汾��Դ�ļ����ԣ�ֻ��׷����Դ���ͣ�
	// ��Ҫ�����в����µ����ͣ�����Ҫɾ������������ԭ�е���Դ����
	////////////////////////////////////////////////////////////////////////
	typedef enum
	{
		rt_unknown		= 0,	// δ֪����
		rt_lexicon,				// �ʵ�

		rt_count				// ���Ա�ʶ���͵����һ����
	} SRES_TYPE;

	////////////////////////////////////////////////////////////////////////
	// ��Դ����
	////////////////////////////////////////////////////////////////////////
	typedef enum
	{
		rp_system		= 0,	// ϵͳ��Դ
		rp_user,				// �û���Դ
	} SRES_PROPERTY;

	////////////////////////////////////////////////////////////////////////
	// ��������
	////////////////////////////////////////////////////////////////////////
	typedef enum
	{
		rl_unknown		= 0,	// δ֪
		cn_simp			= 936,	// ��������
		cn_trad			= 950,	// ��������
		en_us			= 1252,	// Ӣ��
	} SRES_LANG;

	////////////////////////////////////////////////////////////////////////
	// ��������
	////////////////////////////////////////////////////////////////////////
	typedef enum
	{
		vd_mandarin		= 0,	// ��׼��
		vd_cantonese	= 1,	// ����
		vd_neutral		= 0xFF,	// ������������
		vd_count		= 3,	// ��������
	} SV_DIALECT;

	////////////////////////////////////////////////////////////////////////
	// �ֽ�������
	////////////////////////////////////////////////////////////////////////
//#define	SRES_BO_LE		((SR_WORD)0)		// Little-Endian Intelƽ̨
//#define	SRES_BO_BE		((SR_WORD)0xFFFF)	// Big-Endian

	////////////////////////////////////////////////////////////////////////
	// ����ID
	////////////////////////////////////////////////////////////////////////
	typedef enum
	{
		vid_invalid		= -1,
		vid_default		=  0,
	} SRES_VOICEID;

	/** 
	* @class	SRes_head_t
	* 
	* @brief	��������Դ�ļ����ļ�ͷ��Ϣ(����Դ�ĸ�ʽ)
	* 
	*  ���ж�������Դ�ļ��Ľ���ʹ�õ��ļ�ͷ��Ϣ
	*  ����ϵͳ��ȡ��Դ��һЩ�ſ���Ϣ
	*  intp50֮�����Ŀ����ʹ�������ʽ
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
		SR_WORD			byte_order_;	/// �ֽ���
		SR_WORD			head_size_;		/// ͷ����
		char			flag_	[32];	/// ��־��Ϣ
		char			name_	[32];	/// ��Դ����
		char			bundle_	[32];	/// ��Դ������
		SR_DWORD		ver_;			/// ��Դ�汾
		SR_DWORD		build_;			/// ��Դ�������

		SR_TIME			create_time_;	/// ����ʱ��
		SR_TIME			modify_time_;	/// ����ʱ��
		char			author_	[16];	/// ����
		char			company_[32];	/// ��˾
		char			desc_	[156];	/// ������Ϣ

		SSYSTEM_TYPE	sys_type_;		/// ϵͳ����
		SRES_TYPE		res_type_;		/// ��Դ����
		SRES_PROPERTY	res_prop_;		/// ��Դ����
		SRES_LANG		res_lang_;		/// ��Դ��������
		SV_DIALECT		dialect_;		/// ��������
		SR_DWORD		res_id_;		/// ��ԴID
		SR_DWORD		vid_;			/// ����ID
		char			vname_	[32];	/// ��������
		SR_DWORD		data_size_;		/// ���ݳ���
		// �������ֱ����չ
		// ...
	};

	/// ȱʡ��ͷ��ʽ
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

	// һ���򵥵ķ��ʽṹ��Ա����
	template <class HEAD = SRes_head_t>
	struct SRes_head_c
		: public HEAD
	{
		// ���캯��
		SRes_head_c(void)
		{ memcpy(this, &SRES_DEFHEAD, sizeof SRES_DEFHEAD); }
		SRes_head_c(const SRes_head_t & dh)
		{ memcpy(this, &dh, sizeof dh); }

		// ������Դͨ�õĽӿ�
		int	head_size(void)
		{
			SR_WORD headsize = this->head_size_;
			// �ж��Ƿ���Ҫ�ֽ���ת��
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
			// �ж��Ƿ���Ҫ�ֽ���ת��
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
			// �ж��Ƿ���Ҫ�ֽ���ת��
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
			// �ж��Ƿ���Ҫ�ֽ���ת��
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

		// TTS���õķ��ʺ���
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
	* һ���յ�ͷ
	*/ 
	struct SRes_head_n
	{
		// ������Դͨ�õĽӿ�
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

		// TTS���õķ��ʺ���
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
	* @brief	�ϳ�ϵͳ��Դͷ�Ľӿڣ�������Դ��Ҫ�ṩһ����Դͷ�Ľӿ�
	*
	*  ֻҪ�û�ʵ��������ӿڣ���ô��Դ�ĸ�ʽҲ�Ϳ������ж��壬
	*  ���ؾ��������ڵ���Դͷ��ʽ
	*
	* @author	jdyu
	* @date	2005-10-24
	*/
	struct SRes_head
		: public SRes_head_com
	{
		// ���õķ��ʺ���
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
	* @brief	��Դͷ��ģ��������
	*
	*  ����ദ����Դͷ��صĲ��������ܣ�
	*   1. �����Ի�����ͷ�ӿڣ��̳��书��
	*   2. ����ͷ�ĳ��ȣ�Ҳ���Բ�ʹ��ͷ
	*
	*/
	template <typename HEAD_STRUCT = SRes_head_c<>, int HEAD_SIZE = SRES_DEFHEADSIZE, typename HEAD_INTFACE = SRes_head>
	class SRes_Head_T
		: public SRes_Head_Com_T<HEAD_STRUCT, HEAD_SIZE, HEAD_INTFACE>
	{
		// parent class
		typedef SRes_Head_Com_T<HEAD_STRUCT, HEAD_SIZE, HEAD_INTFACE> parent;

	public:
		// ���õķ��ʺ���
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
	// �յ����ö�ȡд����
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

	// ������
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
		// ���ö�ȡд����
		CFG_HEAP	cfg_;
		// �޸ı��
		bool		modf_;
		// ���þ��
		SRES_HANDLE hdl_;
		// ����
		std::string root_;
	};

	/** 
	* @class	SRes_Cfg_i
	* 
	* @brief	ÿ����Դ��������
	* 
	*  ÿ����Դ��������Ϣ
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
		SRES_LOADMODE	mode_;				///< װ�ط�ʽ
		SRES_ACCESSMODE	access_;			///< ����Ȩ��
		SRES_PRELOAD	preload_;			///< �Ƿ�Ԥ����
		std::string 	path_;				///< ��Դ·��
		std::string 	profile_;			///< ��Դ���ü�(ע���)
		std::string		name_;
	};

	////////////////////////////////////////////////////////////////////////
	// ��Դ����������Ϣ
	////////////////////////////////////////////////////////////////////////
	/** 
	* @class	SRes_Pack_Cfg
	* 
	* @brief	��Դ����������Ϣ
	* 
	*  ÿ����Դ��������������һ��������
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

	// ��Դ�����õ�ȱʡʵ��
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
		// ��Դ������
		std::string name_;
		// ��װ·��
		std::string path_;
		// ������Ϣ
		std::string desc_;
		// ע���
		std::string profile_;
		// �汾��Ϣ
		SR_DWORD	ver_;
		// �Ƿ�����
		bool		disable_;
	};

	// ��Դ���������ýӿ�
	class SRes_Pack_Mngr_Cfg
		: public SRes_Cfg_b
	{
	public:
		virtual const char*		path(void)			const = 0;
		virtual const char*		profile(void)		const = 0;
		virtual SRES_INITTYPE	init_type(void)		const = 0;
		virtual const char*		default_pack(void)	const = 0;
	};

	// ȱʡʵ��
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
		// ·����Ϣ
		std::string		path_;
		// ע���λ��
		std::string		profile_;
		// ��ʼ����Ϣ
		SRES_INITTYPE	init_type_;
		// ȱʡ����Դ��
		std::string		def_pack_;
	};

	// ============================================================================
	// �ӿڹ�����
	// ������dll�ж���ʵ��ͬ���Ľӿڣ���ô����Ҫͳһ���й���
	// ============================================================================
	struct SRes_Interface_Mngr
	{
		virtual const char * mngr_name(void) = 0;
		virtual const char * intf_name(void) = 0;
		virtual int intf_add(void * intf) = 0;
		virtual int intf_remove(void * intf) = 0;
	};

	// ȱʡ��ʵ��
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
		// ����Ա����
		std::string		mngr_name_;
		// ������Ľӿ�����
		std::string		intf_name_;
	};


	//===========================================================================================
	// ������
	//===========================================================================================
	namespace aux {

		// ��Դ�����ӿ�
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

		// ��Դ����ʵ��
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

		// �ӿڴ���ʵ��
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

		// ����ʵ�ֽӿ����ɵ���
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

		/// ��Դ����������ڵĺ�
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
	// ��������
	//===========================================================================================

	/// �����Դ���������ⲿʵ��
	inline SRes_Pack_Mngr * res_mngr(void);

	/// ������Դ������Դ
	template <class RES>
	inline RES * find_pack_resource(SRes_Pack * pack, const char * name, const char * token = 0)
	{
		RES * res = dynamic_cast<RES*>
			(pack->find_resource(name, token));
		return res;
	}

	/// ������Դ������Դ
	template <class RES>
	inline RES * find_pack_resource(SRes_Pack_Mngr * pm, const char * pack_name, const char * res_name, const char * token = 0)
	{
		SRes_Pack * pack = pm->find_package(pack_name);
		if ( pack == 0 )
			return 0;
		return find_pack_resource<RES>(pack, res_name, token);
	}

	/// ������Դ������Դ
	template <class RES>
	inline RES * find_pack_resource(const char * pack_name, const char * res_name, const char * token = 0)
	{
		return find_pack_resource<RES>(res_mngr(), pack_name, res_name, token);
	}

	/// ������Դ���Ľӿ�
	template <class INTF>
	inline INTF * find_pack_interface(SRes_Pack * pack, const char * intf_name, void * rev = 0)
	{
		INTF * intf = 0;
		int ret = pack->query_interface(intf_name, (void**)&intf, rev);
		return intf;
	}

	/// ������Դ���Ľӿ�
	template <class INTF>
	inline INTF * find_pack_interface(SRes_Pack_Mngr * pm, const char * pack_name, const char * intf_name, void * rev = 0)
	{
		SRes_Pack * pack = pm->find_package(pack_name);
		if ( pack == 0 )
			return 0;
		return find_pack_interface<INTF>(pack, intf_name, rev);
	}

	/// ������Դ���Ľӿ�
	template <class INTF>
	inline INTF * find_pack_interface(const char * pack_name, const char * intf_name, void * rev = 0)
	{
		return find_pack_interface<INTF>(res_mngr(), pack_name, intf_name, rev);
	}

	/// ������Դ���Ľӿ�
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

	/// ���ر�׼����ָ��
	inline SRes_Pack* res_std_package(void)
	{
		return res_mngr()->std_package();
	}

	/// ���ұ�׼������Դ
	template <class RES>
	inline RES * find_std_resource(const char * name, const char * token = 0)
	{
		RES * res = static_cast<RES*>
			(res_std_package()->find_resource(name, token));
		SP_ASSERT(res);
		return res;
	}

	/// ���ұ�׼������Դ�ӿ�
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

	/// ����ȱʡ��Դ����ָ��
	inline SRes_Pack* res_default_package()
	{
		return res_mngr()->default_package();
	}

	/// ����ȱʡ��Դ������Դ
	template <class RES>
	inline RES * find_default_resource(const char * name, const char * token = 0)
	{
		RES * res = static_cast<RES*>
			(res_default_package()->find_resource(name, token));
		SP_ASSERT(res);
		return res;
	}

	/// ����ȱʡ������Դ�ӿ�
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

	/// ����λ����Դ�������Ľӿ�
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
	// ���ڴ�����Դ��API
	// �û�����Դ����ʹ�ü����������д���
	// ��Դ�����ӿ�ֻ��Ҫ����֮һ�Ϳ�����
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

	/// ͨ����Դ��������Դ����
	typedef int (SRESAPI *proc_sres_create_from_name)
		(const char* name, SRes_Resource** res, SRes_Pack * pack, SRES_INITTYPE init);
	/// ͨ���ļ�·����������Դ����
	typedef int (SRESAPI *proc_sres_create_from_file)
		(const char* resfile, SRes_Resource** res, SRes_Pack * pack, SRES_INITTYPE init);
	/// ͨ����ԴID������Դ����
	typedef int (SRESAPI *proc_sres_create_from_id)
		(int id, SRes_Resource** res, SRes_Pack * pack, SRES_INITTYPE init);
	/// �ͷ���Դ���󣬴���ȥ����Դ���󽫱��ͷ�
	typedef int (SRESAPI *proc_sres_destroy_resource)
		(SRes_Resource * res, SRES_INITTYPE init);

	/// ��ѯ֧�ֵ�������Դ�����б�
	/// ���ص������б��ԡ�res_a=ver,res_b=ver,res_c=ver,res_d=ver������ʽ
	typedef int (SRESAPI *proc_sres_query_support_resources)
		(char* res_list, int * len, SRES_INITTYPE init);

	/// ͨ���ӿ����ƴ���ĳЩ�ӿڣ�����Vid_Mngr
	typedef int (SRESAPI *proc_sres_create_interface)
		(const char * intf_name, void ** intf, SRes_Pack * pack, SRES_INITTYPE init, void * reserved);
	/// �ͷ�ָ���Ľӿڣ�����Vid_Mngr
	typedef int (SRESAPI *proc_sres_destroy_interface)
		(const char * intf_name, void * intf, SRES_INITTYPE init);

	/// �ϲ��ӿڣ������½��ĺϲ��ӿ�
	typedef int (SRESAPI *proc_sres_create_interface_merger)
		(const char * intf_name, void ** intfm, void * intfs[], int intfc, SRES_INITTYPE init, void * reserved);
	/// �ͷ�ָ���Ľӿںϲ���
	typedef int (SRESAPI *proc_sres_destroy_interface_merger)
		(const char * intf_name, void * intf, SRES_INITTYPE init);

	/// ͨ���ӿ����ƴ���ĳЩ�ӿڵĹ�����������Vid_Mngr
	typedef int (SRESAPI *proc_sres_create_interface_mngr)
		(const char * intf_name, void ** intfm, SRes_Pack_Mngr * rcmngr, SRES_INITTYPE init, void * reserved);
	/// �ͷ�ָ���Ľӿڹ�����
	typedef int (SRESAPI *proc_sres_destroy_interface_mngr)
		(const char * intf_name, void * intfm, SRES_INITTYPE init);

	/// ��ѯ��ģ��֧�ֵĽӿ��б�
	/// ���ص������б��ԡ�intf_a=ver,intf_b=ver������ʽ
	typedef int (SRESAPI *proc_sres_query_support_intfaces)
		(char * intf_list, int * len, SRES_INITTYPE init);

	//===========================================================================================
	// ���ڻ����Դ��������API
	// ��resmgmtģ��ʵ��
	//===========================================================================================

	/// ������Դ����������
	int SRESAPI sres_mgmt_open(SRes_Pack_Mngr ** pack_mngr, const char * rcfg, void * reserved);
	typedef int (SRESAPI *proc_sres_mgmt_open)(SRes_Pack_Mngr ** pack_mngr, const char * rcfg, void * reserved);

	/// ������Դ����������
	int SRESAPI sres_mgmt_close(SRes_Pack_Mngr * pack_mngr);
	typedef int (SRESAPI *proc_sres_mgmt_close)(SRes_Pack_Mngr * pack_mngr);

	/// �����Դ��ڣ�����������Դ��������Դ�������ӿڡ����ٽӿ�
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
