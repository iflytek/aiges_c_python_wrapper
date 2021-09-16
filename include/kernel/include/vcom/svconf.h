/** 
* @file	srconf.h
* @brief	全局配置信息
* 
*  处理isv的全局配置信息
* 
* @author	jdyu
* @version	1.0
* @date	2007-7-25
* 
* @see		
* 
* <b>History:</b><br>
* <table>
*  <tr> <th>Version	<th>Date		<th>Author	<th>Notes</tr>
*  <tr> <td>1.0		<td>2007-7-25	<td>jdyu	<td>Create this file</tr>
* </table>
* 
*/
#ifndef __SRCONF_H__
#define __SRCONF_H__

#include "../utils/suconf.h"
#include "../utils/cfgunit.h"
#include "../base/srbase.h"

// configuration
namespace cfg
{
	// 环境变量的名称
	const wchar_t	sr_home_name[]	= L"VSPP10_HOME";
	const wchar_t	sr_reg_root []	= L"Software\\iFly Info TEK\\vspp\\1.0";

#ifdef WIN32
	const wchar_t	sr_conf_wname[]	= L"conf";
	const char		sr_conf_name []	=  "conf";
	const wchar_t	sr_logs_wdir []	= L"log";
	const char		sr_logs_dir []	=  "log";

#else
	const wchar_t	sr_conf_wname[]	= L"etc";
	const char		sr_conf_name []	=  "etc";
	const char		sr_logs_dir []	=  "log";
#endif

#ifndef _WIN64
	const wchar_t	sr_bin_wdir []	= L"bin_x86";
	const char		sr_bin_dir  []	=  "bin_x86";
	const char		sr_lib_dir	[]	= "lib_x86";
#else
	const wchar_t	sr_bin_wdir []	= L"bin_x64";
	const char		sr_bin_dir  []	=  "bin_x64";
	const char		sr_lib_dir	[]	= "lib_x64";
#endif

	/** 
	* @class	srconf
	* @brief	配置类接口
	* 
	*  获取系统的安装路径等全局信息
	* 
	* @author	jdyu
	* @date	2007-8-8
	*/
	struct srconf_i
	{
		virtual int open(const wchar_t * rev) = 0;
		virtual int close(void) = 0;

		virtual void home_path(const wchar_t * path) = 0;
		virtual void home_path(const char * path) = 0;
		virtual const wchar_t * whome_path(void) const = 0;
		virtual const wchar_t * wconf_path(void) const = 0;
		virtual const wchar_t * wlogs_path(void) const = 0;
		virtual const wchar_t * wbin_path(void) const = 0;
		virtual const char * home_path(void) const = 0;
		virtual const char * conf_path(void) const = 0;
		virtual const char * logs_path(void) const = 0;
		virtual const char * bin_path(void) const = 0;
		virtual std::string log_path_name(const char * log_name) const = 0;
		virtual std::string cfg_path_name(const char * cfg_name) const = 0;
		virtual std::string bin_path_name(const char * bin_name) const = 0;
#ifndef WIN32
		// 非Windows平台下，库文件是放在lib目录下，所以增加了该接口
		virtual const char * lib_path(void) const = 0;
		virtual std::string lib_path_name(const char * bin_name) const = 0;
#endif
	};

	/** 
	* @class	srconf
	* @brief	配置类
	* 
	*  获取系统的安装路径
	* 
	* @author	jdyu
	* @date	2007-8-8
	*/
	class srconf
		: public srconf_i
	{
	public:
		int open(const wchar_t * rev)
		{
			if ( inited_ )
				return 0;

			/* 从环境变量获得 */
#ifdef WIN32
			wchar_t var[MAX_PATH]; var[0] = 0;
			DWORD ret = GetEnvironmentVariableW(sr_home_name, var, SP_COUNTOF(var));
			if ( ret > 0 )
				whome_path_ = var;

			HKEY hKey = 0;
			ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE, sr_reg_root, 0, KEY_QUERY_VALUE, &hKey);
			if ( ret == ERROR_SUCCESS )
			{
				DWORD type;
				wchar_t path[MAX_PATH]; path[0] = 0;
				DWORD size = sizeof path;
				ret = RegQueryValueExW(hKey, L"InstallPath", 0, &type, (LPBYTE)path, &size);
				if ( ret == ERROR_SUCCESS )
					whome_path_ = path;
				RegCloseKey(hKey);
			}
			home_path_ = sp::tochar(whome_path_);
#else // Linux
			char * env = getenv(sp::tochar(sr_home_name).c_str());
			if ( env )
				home_path_ = env;
			whome_path_ = sp::towchar(home_path_);
			printf("home_path : %s\n",home_path_.c_str());
#endif // WIN32

			// 配置文件的路径
			generate_pathes();
			inited_ = true;
			return 0;
		}
		int close(void)
		{
			inited_ = false;
			return 0;
		}

		void home_path(const wchar_t * path)
		{
			if ( path )
			{
				whome_path_ = path;
				home_path_  = sp::tochar(whome_path_);
				generate_pathes();
			}
		}
		void home_path(const char * path)
		{
			if ( path )
			{
				home_path_ = path;
				whome_path_  = sp::towchar(home_path_);
				generate_pathes();
			}
		}
		const wchar_t * whome_path(void) const
		{
			return whome_path_.c_str();
		}
		const wchar_t * wconf_path(void) const
		{
			return wconf_path_.c_str();
		}
		const wchar_t * wlogs_path(void) const
		{
			return wlogs_path_.c_str();
		}
		const char * home_path(void) const
		{
			return home_path_.c_str();
		}
		const char * conf_path(void) const
		{
			return conf_path_.c_str();
		}
		const char * logs_path(void) const
		{
			return logs_path_.c_str();
		}
		const char * bin_path(void) const
		{
			return bin_path_.c_str();
		}
		const wchar_t * wbin_path(void) const
		{
			return wbin_path_.c_str();
		}
		std::string log_path_name(const char * log_name) const
		{
            SP_ASSERT(log_name);
			return sp::cat_path(logs_path_.c_str(), log_name);
		}
		std::string cfg_path_name(const char * cfg_name) const
		{
			SP_ASSERT(cfg_name);
			if ( sp::is_file_exist(cfg_name) )
				return cfg_name;
			std::string full_path = sp::cat_path(conf_path_.c_str(), cfg_name);
			if ( sp::is_file_exist(full_path.c_str()) )
				return full_path;
			return cfg_name;
		}
		std::string bin_path_name(const char * bin_name) const
		{
			SP_ASSERT(bin_name);
			if ( sp::is_file_exist(bin_name) )
				return bin_name;
			std::string full_path = sp::cat_path(bin_path_.c_str(), bin_name);
			if ( sp::is_file_exist(full_path.c_str()) )
				return full_path;
			return bin_name;
		}
		static srconf & instance(void)
		{
			static srconf the_srconf;
			return the_srconf;
		}

		srconf(void)
			: inited_(false)
		{
			/* 缺省在bin/.. */
			home_path_  = sp::parent_path(sp::get_bin_path(IU_INVALID_HANDLE).c_str());
			whome_path_ = sp::char2wchar(home_path_.c_str());
			generate_pathes();
			open(0);
		}
		virtual ~srconf(void)
		{
			close();
		}

#ifndef WIN32
		const char * lib_path(void) const
		{
			return lib_path_.c_str();
		}

		std::string lib_path_name(const char * lib_name) const
		{
			SP_ASSERT(lib_name);
			if ( sp::is_file_exist(lib_name) )
				return lib_name;
			std::string full_path = sp::cat_path(lib_path_.c_str(), lib_name);
			if ( sp::is_file_exist(full_path.c_str()) )
				return full_path;
			return lib_name;
		}
#endif
	protected:
		// 生成相关路径
		void generate_pathes(void)
		{
			conf_path_	= sp::cat_path(home_path_.c_str(), sr_conf_name);
			logs_path_	= sp::cat_path(home_path_.c_str(), sr_logs_dir);
			bin_path_	= sp::cat_path(home_path_.c_str(), sr_bin_dir);
			lib_path_	= sp::cat_path(home_path_.c_str(), sr_lib_dir);
			wconf_path_ = sp::char2wchar(conf_path_.c_str());
			wlogs_path_	= sp::char2wchar(logs_path_.c_str());
			wbin_path_	= sp::char2wchar(bin_path_.c_str());
			wlib_path_	= sp::char2wchar(lib_path_.c_str());
		}

	private:
		// 安装路径
		std::wstring	whome_path_;
		std::string		home_path_;

		std::wstring	wconf_path_;
		std::string		conf_path_;

		std::wstring	wlogs_path_;
		std::string		logs_path_;

		std::wstring	wbin_path_;
		std::string		bin_path_;

		std::wstring	wlib_path_;
		std::string		lib_path_;

		bool			inited_;
	};

	/// using type
	typedef sp::ini_section<char, sp::ini_file<char>, cfgunit, cfg_holder>			inisec;
	typedef sp::ini_section<wchar_t, sp::ini_file<wchar_t>, cfgunit, cfg_holder>	inisecw;

	// 获取srconf_i的辅助函数
	struct cm_helper
	{
		srconf_i * operator -> (void)
		{
			static srconf_i * the_cfg_mngr = 0;
			if ( the_cfg_mngr )
				return the_cfg_mngr;

			if ( the_cfg_mngr == 0 )
				the_cfg_mngr = &srconf::instance();
			return the_cfg_mngr;
		}
	};

} // namespace cfg

// 方便使用的宏
#define sr_conf_mngr()        (&cfg::srconf::instance()) // (scom::shptr<cfg::srconf_i>() == NULL ? &cfg::srconf::instance() : scom::shptr<cfg::srconf_i>())
#define sr_home_path           sr_conf_mngr()->home_path
#define sr_lib_path            sr_conf_mngr()->lib_path
#define sr_log_pathname(x)     sr_conf_mngr()->log_path_name(x).c_str()
#define sr_cfg_pathname(x)     sr_conf_mngr()->cfg_path_name(x).c_str()
#define sr_lib_pathname(x)     sr_conf_mngr()->lib_path_name(x).c_str()

#ifdef WIN32
#define sr_bin_path            sr_conf_mngr()->bin_path
#define sr_bin_pathname(x)     sr_conf_mngr()->bin_path_name(x).c_str()
#else
#define sr_bin_path            sr_conf_mngr()->lib_path
#define sr_bin_pathname(x)     sr_conf_mngr()->lib_path_name(x).c_str()
#endif

DEFINE_SHPTR(cfg::srconf_i, "srconf");
DEFINE_SHMOD(srconf);

#endif /* __SRCONF_H__ */
