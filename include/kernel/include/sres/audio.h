/** 
 * @file	audio.h
 * @brief	音频处理的辅助文件
 * 
 *  一些音频常用的处理
 * 
 * @author	jdyu
 * @version	1.0
 * @date	2007-7-30
 * 
 * @see		
 * 
 * <b>History:</b><br>
 * <table>
 *  <tr> <th>Version	<th>Date		<th>Author	<th>Notes</tr>
 *  <tr> <td>1.0		<td>2007-7-30	<td>jdyu	<td>Create this file</tr>
 * </table>
 * 
 */
#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <algorithm>
#include "iFly_TTS.h"
#include "iFly_Assert.h"
#include "sres.h"
#include "log/srlog.h"
// ============================================================================
// namespace to sres
// ============================================================================
namespace sres
{
	////////////////////////////////////////////////////////////////////////
	// 音频头定义
	////////////////////////////////////////////////////////////////////////
#pragma pack(2)
	struct wave_pcm_hdr
	{
		char		riff[4];			// = "RIFF"
		SR_DWORD	size_8;				// = FileSize - 8
		char		wave[4];			// = "WAVE"
		char		fmt[4];				// = "fmt "
		SR_DWORD	dwFmtSize;			// = 下一个结构体的大小 : 16

		SR_WORD		format_tag; 		// = PCM : 1
		SR_WORD		channels;			// = 通道数 : 1
		SR_DWORD	samples_per_sec;	// = 采样率 : 8000 | 6000 | 11025 | 16000
		SR_DWORD	avg_bytes_per_sec;	// = 每秒字节数 : dwSamplesPerSec * wBitsPerSample / 8
		SR_WORD		block_align;		// = 每采样点字节数 : wBitsPerSample / 8
		SR_WORD		bits_per_sample; 	// = 量化比特数: 8 | 16

		char		data[4];			// = "data";
		SR_DWORD	data_size;	 		// = 纯数据长度 : FileSize - 44 
	} ;
#pragma pack()

	const wave_pcm_hdr default_pcmwavhdr = 
	{
		{ 'R', 'I', 'F', 'F' },
		0,
		{'W', 'A', 'V', 'E'},
		{'f', 'm', 't', ' '},
		16,
		1,
		1,
		8000,
		16000,
		2,
		16,
		{'d', 'a', 't', 'a'},
		0
	};

	// 转换wav头的字节序
	inline void convert_pcm_hdr_endian(wave_pcm_hdr* hdr)
	{
		// 针对非字符串转换
		sp::convert_endian_32(&hdr->size_8);
		sp::convert_endian_32(&hdr->dwFmtSize);
		sp::convert_endian_16(&hdr->format_tag);
		sp::convert_endian_16(&hdr->channels);
		sp::convert_endian_32(&hdr->samples_per_sec);
		sp::convert_endian_32(&hdr->avg_bytes_per_sec);
		sp::convert_endian_16(&hdr->block_align);
		sp::convert_endian_16(&hdr->bits_per_sample);
		sp::convert_endian_32(&hdr->data_size);
	}

	////////////////////////////////////////////////////////////////////////
	// A/U率声音数据头的定义
	////////////////////////////////////////////////////////////////////////
#pragma pack(2)
	struct wave_aulaw_hdr
	{
		char		riff[4];			// = "RIFF"
		SR_DWORD	size_8;				// = FileSize - 8
		char		wave[4];			// = "WAVE"
		char		fmt[4];				// = "fmt "
		SR_DWORD	fmt_size;			// = 下一个结构体的大小 : 18
		
		SR_WORD		format_tag; 		// = aLaw : 6 | uLaw : 7
		SR_WORD		channels;			// = 通道数 : 1
		SR_DWORD	samples_per_sec;	// = 采样率 : 8000 | 6000 | 11025 | 16000
		SR_DWORD	avg_bytes_per_sec;	// = 每秒字节数 : dwSamplesPerSec * wBitsPerSample / 8
		SR_WORD		block_align;		// = 每采样点字节数 : wBitsPerSample / 8
		SR_WORD		bits_per_sample; 	// = 量化比特数: 8 | 16
		SR_WORD		cbsize;				// = 下一个结构体的大小 : 0

		char		fact[4];			// = "fact"
		SR_WORD		unknown1;			// = 4;
		SR_WORD		unknown2;			// = 0;
		SR_DWORD	data_size_raw; 		// = Datasize : FileSize - 58

		char		data[4];			// = "data";
		SR_DWORD	data_size;	 		// = DataSize : FileSize - 58
	};
#pragma pack()

	const wave_aulaw_hdr default_aulawwavhdr = 
	{
		{ 'R', 'I', 'F', 'F' },
		0,
		{ 'W', 'A', 'V', 'E' },
		{ 'f', 'm', 't', ' ' },
		18,

		6,
		1,
		8000,
		16000,
		2,
		16,
		0,

		{ 'f', 'a', 'c', 't' },
		4,
		0,
		0,
		{ 'd', 'a', 't', 'a' },
		0,
	};

	// 转换aulaw头的字节序：
	inline void convert_aulaw_hdr_endian(wave_aulaw_hdr* hdr)
	{
		// 针对非字符串转换
		sp::convert_endian_32(&hdr->size_8);
		sp::convert_endian_32(&hdr->fmt_size);
		sp::convert_endian_16(&hdr->format_tag);
		sp::convert_endian_16(&hdr->channels);
		sp::convert_endian_32(&hdr->samples_per_sec);
		sp::convert_endian_32(&hdr->avg_bytes_per_sec);
		sp::convert_endian_16(&hdr->block_align);
		sp::convert_endian_16(&hdr->bits_per_sample);
		sp::convert_endian_16(&hdr->cbsize);
		sp::convert_endian_16(&hdr->unknown1);
		sp::convert_endian_16(&hdr->unknown2);
		sp::convert_endian_32(&hdr->data_size_raw);
		sp::convert_endian_32(&hdr->data_size);
	}

	// format tag
	enum ADF_FMT_TAG
	{
		adf_tag_none	= 0,
		adf_tag_pcm		= 1,
		adf_tag_alaw	= 6,
		adf_tag_ulaw	= 7,
	};

	// 采样率定义
	enum SRES_VOCSAMPRATE
	{
		vsr_defalt		= 0,
		vsr_6k			= 1,
		vsr_8k			= 2,
		vsr_11k			= 3,
		vsr_16k			= 4,
		vsr_count		= vsr_16k,		// 目前支持的采样率数量
	};

	/* 采样率相应的信息 */
	static const struct {
		SR_DWORD	samp_rate_;
		SR_DWORD	default_pcm_;
	} SRES_VSR_INFO[] = {
		/* samp_rate_	default_pcm_	*/
		{ 8000,		TTS_ADF_DEFAULT		},
		{ 6000,		TTS_ADF_PCM6K16B1C	},
		{ 8000,		TTS_ADF_PCM8K16B1C	},
		{ 11025,	TTS_ADF_PCM11K16B1C	},
		{ 16000,	TTS_ADF_PCM16K16B1C	},
	};

	// 音频格式的相应信息
	static const struct {
		char		fmt_name_[24];		// 格式字串
		SR_DWORD	def_pcm_fmt_;		// 缺省的PCM处理格式
		SR_DWORD	samp_rate_;			// 采样率
		SR_DWORD	bps_;				// 量化比特数
		SR_DWORD	win_fmt_id_;		// Window定义的格式ID
	} TTS_ADF_INFO[] = {
		/* fmt_name_				def_pcm_fmt_		samp_rate_	bps_	win_fmt_id_	*/
		{ "TTS_ADF_DEFAULT",		TTS_ADF_DEFAULT,		0,		0,		1 },	// 0
		{ "TTS_ADF_PCM8K8B1C",		TTS_ADF_PCM8K16B1C,		8000,	8,		1 },	// 1
		{ "TTS_ADF_PCM16K8B1C",		TTS_ADF_PCM16K16B1C,	16000,	8,		1 },	// 2
		{ "TTS_ADF_PCM8K16B1C",		TTS_ADF_PCM8K16B1C,		8000,	16,		1 },	// 3
		{ "TTS_ADF_PCM16K16B1C",	TTS_ADF_PCM16K16B1C,	16000,	16,		1 },	// 4
		{ "TTS_ADF_PCM11K8B1C",		TTS_ADF_PCM11K16B1C,	11025,	8,		1 },	// 5
		{ "TTS_ADF_PCM11K16B1C",	TTS_ADF_PCM11K16B1C,	11025,	16,		1 },	// 6
		{ "TTS_ADF_PCM6K8B1C",		TTS_ADF_PCM6K16B1C,		6000,	8,		1 },	// 7
		{ "TTS_ADF_PCM6K16B1C",		TTS_ADF_PCM6K16B1C,		6000,	16,		1 },	// 8
		{ "TTS_ADF_ALAW16K1C",		TTS_ADF_PCM16K16B1C,	16000,	8,		6 },	// 9
		{ "TTS_ADF_ULAW16K1C",		TTS_ADF_PCM16K16B1C,	16000,	8,		7 },	// 10
		{ "TTS_ADF_ALAW8K1C",		TTS_ADF_PCM8K16B1C,		8000,	8,		6 },	// 11
		{ "TTS_ADF_ULAW8K1C",		TTS_ADF_PCM8K16B1C,		8000,	8,		7 },	// 12
		{ "TTS_ADF_ALAW11K1C",		TTS_ADF_PCM11K16B1C,	11025,	8,		6 },	// 13
		{ "TTS_ADF_ULAW11K1C",		TTS_ADF_PCM11K16B1C,	11025,	8,		7 },	// 14
		{ "TTS_ADF_ALAW6K1C",		TTS_ADF_PCM6K16B1C,		6000,	8,		6 },	// 15
		{ "TTS_ADF_ULAW6K1C",		TTS_ADF_PCM6K16B1C,		6000,	8,		7 },	// 16
		{ "TTS_ADF_ADPCMG7218K4B1C",TTS_ADF_PCM8K16B1C,		8000,	4,		1 },	// 17
		{ "TTS_ADF_ADPCMG7216K4B1C",TTS_ADF_PCM6K16B1C,		6000,	4,		1 },	// 18
		{ "TTS_ADF_ADPCMG7233B1C",	TTS_ADF_PCM8K16B1C,		8000,	3,		1 },	// 19
		{ "TTS_ADF_ADPCMG7235B1C",	TTS_ADF_PCM8K16B1C,		8000,	5,		1 },	// 20
		{ "TTS_ADF_VOX6K1C",		TTS_ADF_PCM6K16B1C,		6000,	4,		1 },	// 21
		{ "TTS_ADF_VOX8K1C",		TTS_ADF_PCM8K16B1C,		8000,	4,		1 },	// 22

		{ "TTS_ADF_AMR6K1C",		TTS_ADF_PCM6K16B1C,		6000,	0,		1 },	// 23
		{ "TTS_ADF_AMR8K1C",		TTS_ADF_PCM8K16B1C,		8000,	0,		1 },	// 24
		{ "TTS_ADF_AMR11K1C",		TTS_ADF_PCM11K16B1C,	6000,	0,		1 },	// 25
		{ "TTS_ADF_AMR16K1C",		TTS_ADF_PCM16K16B1C,	6000,	0,		1 },	// 26
		{ "TTS_ADF_MP36K1C",		TTS_ADF_PCM6K16B1C,		8000,	0,		1 },	// 27
		{ "TTS_ADF_MP38K1C",		TTS_ADF_PCM8K16B1C,		8000,	0,		1 },	// 28
		{ "TTS_ADF_MP311K1C",		TTS_ADF_PCM11K16B1C,	11025,	0,		1 },	// 29
		{ "TTS_ADF_MP316K1C",		TTS_ADF_PCM16K16B1C,	16000,	0,		1 },	// 30
	};

	/** 
	 * @brief 	adf_to_samp
	 * 
	 * 	得到与声音格式相应的采样率
	 * 
	 * @author	jdyu
	 * @date	2005-12-23
	 * @return	inline SR_DWORD 
	 * @param	SR_DWORD adf	- [in] 
	 * @see		
	 */
	inline SR_DWORD adf_to_samp(SR_DWORD adf)
	{
		SP_ASSERT( adf > 0 && adf <= TTS_ADF_VOX8K1C);
		if ( adf > TTS_ADF_VOX8K1C )
			return 0;
		return TTS_ADF_INFO[adf].samp_rate_;
	}

	/** 
	 * @brief 	adf_to_bps
	 * 
	 * 	得到音频格式相应的量化比特数
	 * 
	 * @author	jdyu
	 * @date	2005-12-23
	 * @return	inline SR_DWORD 
	 * @param	SR_DWORD adf	- [in] 
	 * @see		
	 */
	inline SR_DWORD adf_to_bps(SR_DWORD adf)
	{
		SP_ASSERT( adf > 0 && adf <= TTS_ADF_VOX8K1C);
		if ( adf > TTS_ADF_VOX8K1C )
			return 1;
		return TTS_ADF_INFO[adf].bps_;
	}

	/** 
	 * @brief 	SR_DWORDadf_to_win_fmt_id
	 * 
	 * 	得到音频格式对应的在Windows下相应的所谓格式ID
	 * 
	 * @author	jdyu
	 * @date	2005-12-23
	 * @return	inline 
	 * @param	SR_DWORD adf	- [in] 
	 * @see		
	 */
	inline SR_DWORD	adf_to_win_fmt_id(SR_DWORD adf)
	{
		SP_ASSERT( adf > 0 && adf <= TTS_ADF_VOX8K1C);
		if ( adf > TTS_ADF_VOX8K1C )
			return 0;
		return TTS_ADF_INFO[adf].win_fmt_id_;
	}

	/** 
	 * @brief 	adf_to_default_pcm_fmt
	 * 
	 * 	得到与声音格式相应的16bit处理PCM格式
	 * 
	 * @author	jdyu
	 * @date	2005-12-23
	 * @return	inline SR_DWORD 
	 * @param	SR_DWORD adf	- [in] 
	 * @see		
	 */
	inline SR_DWORD adf_to_default_pcm_fmt(SR_DWORD adf)
	{
		SP_ASSERT( adf > 0 && adf <= TTS_ADF_VOX8K1C );
		if ( adf > TTS_ADF_VOX8K1C )
			return 0;
		return TTS_ADF_INFO[adf].def_pcm_fmt_;
	}

	/** 
	 * @brief 	char * adf_to_name
	 * 
	 * 	得到音频格式的字符串名称，用于显示
	 * 
	 * @author	jdyu
	 * @date	2005-12-23
	 * @return	inline const 
	 * @param	SR_DWORD adf	- [in] 
	 * @see		
	 */
	inline const char * adf_to_name(SR_DWORD adf)
	{
		SP_ASSERT( adf >= 0 && adf <= TTS_ADF_VOX8K1C );
		if ( adf > TTS_ADF_VOX8K1C )
			return 0;
		return TTS_ADF_INFO[adf].fmt_name_;
	}

	/** 
	 * @brief 	samp_rate_to_vsr
	 * 
	 * 	得到采样率相应的枚举定义
	 * 
	 * @author	jdyu
	 * @date	2005-12-23
	 * @return	inline SRES_VOCSAMPRATE 
	 * @param	SR_DWORD samp_rate	- [in] 
	 * @see		
	 */
	inline SRES_VOCSAMPRATE samp_rate_to_vsr(SR_DWORD samp_rate)
	{
		switch(samp_rate)
		{
		case 8000:	return vsr_8k;
		case 16000: return vsr_16k;
		case 6000:	return vsr_6k;
		case 11025: return vsr_11k;
		default: SP_ASSERT( 0 && "Invalid audio fmt."); return vsr_defalt;
		}
	}

	/** 
	 * @brief 	vsr_to_samp_rate
	 * 
	 * 	采样率的枚举定义转化为真正的采样率数值
	 * 
	 * @author	jdyu
	 * @date	2005-12-23
	 * @return	inline SR_DWORD 
	 * @param	SRES_VOCSAMPRATE vsr	- [in] 
	 * @see		
	 */
	inline SR_DWORD vsr_to_samp_rate(SRES_VOCSAMPRATE vsr)
	{
		SP_ASSERT(vsr > 0 && vsr <= vsr_count);
		if ( vsr > vsr_count )
			return 0;
		return SRES_VSR_INFO[vsr].samp_rate_;
	}

	/** 
	 * @brief 	default_pcm_from_samp_rate
	 * 
	 * 	得到某采样率下缺省的PCM格式
	 * 
	 * @author	jdyu
	 * @date	2005-12-23
	 * @return	inline SR_DWORD 
	 * @param	SR_DWORD samp_rate	- [in] 
	 * @see		
	 */
	inline SR_DWORD default_pcm_from_samp_rate(SR_DWORD samp_rate)
	{
		SRES_VOCSAMPRATE vsr = samp_rate_to_vsr(samp_rate);
		if ( vsr > vsr_count )
			return 0;
		SP_ASSERT(vsr);
		return SRES_VSR_INFO[vsr].default_pcm_;
	}

	#if _MSC_VER >= 1400
	#pragma warning (push)
	#pragma warning (disable : 4996)
	#endif
	/** 
	 * @brief 	get_audio_file_detail
	 * 
	 * 	获得一个文件的格式信息
	 * 
	 * @author	jdyu
	 * @date	2005-12-23
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const char* file		- [in] 
	 * @param	SR_DWORD & adf			- [in] 
	 * @param	SR_DWORD & head_size	- [in] 
	 * @param	SR_DWORD & data_size	- [in] 
	 * @see		
	 */
	inline
	int get_audio_file_detail(const char* file, SR_DWORD & adf, SR_DWORD & head_size, SR_DWORD & data_size)
	{
		int ret = 0;
		SR_DWORD fmt = 0;
		SR_DWORD hs  = 0;

		data_size = 0;
		FILE* fp = fopen(file, "rb");
		if ( 0 == fp )
			return -1;

		fseek(fp, 0, SEEK_END);
		SR_DWORD ds = ftell(fp);

		// 判断有没有Wav头
		if ( ds >= sizeof(wave_aulaw_hdr) )
		{
#ifdef WIN32
			wave_aulaw_hdr head;
			wave_pcm_hdr * pcm = reinterpret_cast<wave_pcm_hdr*>(&head);
			rewind(fp);
			fread(&head, 1, sizeof head, fp);
#else
			wave_aulaw_hdr head;
			wave_pcm_hdr pcm_head;
			rewind(fp);
			fread(&head, 1, sizeof head, fp);
			rewind(fp);
			fread(&pcm_head, 1, sizeof pcm_head, fp);

			// 判断字节序:
			if (!sp::is_platform_little_endian())
			{
				convert_pcm_hdr_endian(&pcm_head);
				convert_aulaw_hdr_endian(&head);
			}

			wave_pcm_hdr * pcm = &pcm_head;
#endif
			if ( strncmp(head.riff, "RIFF", 4) == 0 )
			{
				
				#define	SR_GAFD(pcm8, pcm16, alaw, ulaw)	\
					if ( head.format_tag == 1 )				\
						{ fmt = head.bits_per_sample == 8 ? pcm8 : pcm16; hs = 44; ds = std::min<SR_DWORD>(pcm->data_size, ds - hs); }	\
					else if ( head.format_tag == 6 )												\
						{ fmt = alaw; hs = 58; ds = std::min<SR_DWORD>(head.data_size, ds - hs); }	\
					else if ( head.format_tag == 7 )												\
						{ fmt = ulaw; hs = 58; ds = std::min<SR_DWORD>(head.data_size, ds - hs); }	

				switch ( pcm->samples_per_sec )
				{
				case 8000:
					SR_GAFD(TTS_ADF_PCM8K8B1C, TTS_ADF_PCM8K16B1C, TTS_ADF_ALAW8K1C, TTS_ADF_ULAW8K1C);
					break;
				case 16000:
					SR_GAFD(TTS_ADF_PCM16K8B1C, TTS_ADF_PCM16K16B1C, TTS_ADF_ALAW16K1C, TTS_ADF_ULAW16K1C);
					break;
				case 6000:
					SR_GAFD(TTS_ADF_PCM6K8B1C, TTS_ADF_PCM6K16B1C, TTS_ADF_ALAW6K1C, TTS_ADF_ULAW6K1C);
					break;
				case 11025:
					SR_GAFD(TTS_ADF_PCM11K8B1C, TTS_ADF_PCM11K16B1C, TTS_ADF_ALAW11K1C, TTS_ADF_ULAW11K1C);
					break;
				}
				#undef SR_GAFD
			}
			else
			{
				// 没有音频头，靠扩展名判断
				if ( strstr(file, ".pcm") || strstr(file, ".wav") )
					fmt = adf;
				else // 其他情况，假设跟输出格式是一样的
					fmt = TTS_ADF_DEFAULT;
			}
		}

		fclose(fp);
		adf		  = fmt;
		data_size = ds;
		head_size = hs;
		return ret;
	}

	/** 
	 * @fn		get_audio_buf_detail
	 * @brief	根据音频缓冲区获得音频信息
	 * 
	 *  分析音频是不是有头，如果有那么根据头信息获得音频格式等信息
	 * 
	 * @author	jdyu
	 * @date	2008/8/8
	 * @return	int	- Return 0 in success, otherwise return error code.
	 * @param	const void * buffer		- [in] 音频缓冲区
	 * @param	SR_DWORD size			- [in] 音频缓冲区的大小
	 * @param	SR_DWORD & adf			- [out] 返回音频格式
	 * @param	SR_DWORD & head_size	- [out] 返回头尺寸
	 * @param	SR_DWORD & data_size	- [out] 返回数据尺寸
	 * @see		
	 */
	inline
	int get_audio_buf_detail(const void * buffer, SR_DWORD size, SR_DWORD & adf, SR_DWORD & head_size, SR_DWORD & data_size)
	{
		int ret = 0;
		SR_DWORD fmt = 0;
		SR_DWORD hs  = 0;

		data_size = 0;
		SR_DWORD ds = size;
		
		// 判断有没有Wav头
		if ( ds >= sizeof(wave_aulaw_hdr) )
		{
#ifdef WIN32
			const wave_aulaw_hdr & head = *reinterpret_cast<const wave_aulaw_hdr*>(buffer);
			const wave_pcm_hdr * pcm = reinterpret_cast<const wave_pcm_hdr*>(&head);
#else
			wave_aulaw_hdr head;
			wave_pcm_hdr pcm_head;
			memcpy(&head, buffer, sizeof head);
			memcpy(&pcm_head, buffer, sizeof pcm_head);

			// 判断字节序:
			if (!sp::is_platform_little_endian())
			{
				convert_pcm_hdr_endian(&pcm_head);
				convert_aulaw_hdr_endian(&head);
			}

			wave_pcm_hdr * pcm = &pcm_head;
#endif
			if ( strncmp(head.riff, "RIFF", 4) == 0 )
			{
				#define	SR_GAFD(pcm8, pcm16, alaw, ulaw)	\
					if ( head.format_tag == 1 )				\
						{ fmt = head.bits_per_sample == 8 ? pcm8 : pcm16; hs = 44; ds = std::min<SR_DWORD>(pcm->data_size, ds - hs); }	\
					else if ( head.format_tag == 6 )												\
						{ fmt = alaw; hs = 58; ds = std::min<SR_DWORD>(head.data_size, ds - hs); }	\
					else if ( head.format_tag == 7 )												\
						{ fmt = ulaw; hs = 58; ds = std::min<SR_DWORD>(head.data_size, ds - hs); }	\
					else																			\
						{ ret = -1; }

				switch ( pcm->samples_per_sec )
				{
				case 8000:
					SR_GAFD(TTS_ADF_PCM8K8B1C, TTS_ADF_PCM8K16B1C, TTS_ADF_ALAW8K1C, TTS_ADF_ULAW8K1C);
					break;
				case 16000:
					SR_GAFD(TTS_ADF_PCM16K8B1C, TTS_ADF_PCM16K16B1C, TTS_ADF_ALAW16K1C, TTS_ADF_ULAW16K1C);
					break;
				case 6000:
					SR_GAFD(TTS_ADF_PCM6K8B1C, TTS_ADF_PCM6K16B1C, TTS_ADF_ALAW6K1C, TTS_ADF_ULAW6K1C);
					break;
				case 11025:
					SR_GAFD(TTS_ADF_PCM11K8B1C, TTS_ADF_PCM11K16B1C, TTS_ADF_ALAW11K1C, TTS_ADF_ULAW11K1C);
					break;
				}
			}
		}
		else if ( ds >= sizeof(wave_pcm_hdr) )
		{
#ifdef WIN32
			const wave_pcm_hdr & head = *reinterpret_cast<const wave_pcm_hdr*>(buffer);
			const wave_pcm_hdr * pcm = &head;
#else
			wave_pcm_hdr head;
			wave_pcm_hdr pcm_head;
			memcpy(&head, buffer, sizeof head);
			memcpy(&pcm_head, buffer, sizeof pcm_head);		

			// 判断字节序:
			if (!sp::is_platform_little_endian())
			{
				convert_pcm_hdr_endian(&pcm_head);
				convert_pcm_hdr_endian(&head);
			}

			wave_pcm_hdr * pcm = &pcm_head;
#endif
			
			if ( strncmp(head.riff, "RIFF", 4) == 0 )
			{
				switch ( pcm->samples_per_sec )
				{
				case 8000:
					SR_GAFD(TTS_ADF_PCM8K8B1C, TTS_ADF_PCM8K16B1C, TTS_ADF_ALAW8K1C, TTS_ADF_ULAW8K1C);
					break;
				case 16000:
					SR_GAFD(TTS_ADF_PCM16K8B1C, TTS_ADF_PCM16K16B1C, TTS_ADF_ALAW16K1C, TTS_ADF_ULAW16K1C);
					break;
				case 6000:
					SR_GAFD(TTS_ADF_PCM6K8B1C, TTS_ADF_PCM6K16B1C, TTS_ADF_ALAW6K1C, TTS_ADF_ULAW6K1C);
					break;
				case 11025:
					SR_GAFD(TTS_ADF_PCM11K8B1C, TTS_ADF_PCM11K16B1C, TTS_ADF_ALAW11K1C, TTS_ADF_ULAW11K1C);
					break;
				}
				#undef SR_GAFD
				// correct head size 
				hs = 44;
				ds = ds - hs;
			}
		}

		adf       = fmt;
		data_size = ds;
		head_size = hs;
		return ret;
	}
	#if _MSC_VER >= 1400
	#pragma warning (pop)
	#endif

	/** 
	 * @brief 	get_time_of_audio
	 * 
	 * 	获得一段Wave的播放时间，毫秒为单位
	 * 
	 * @author	jdyu
	 * @date	2006-4-15
	 * @return	inline SR_DWORD 
	 * @param	int adf	- [in] 
	 * @param	int bytes	- [in] 
	 * @see		
	 */
	inline SR_DWORD get_time_of_audio(int adf, int bytes)
	{
		SR_DWORD sr =  adf_to_samp(adf);
		if ( sr == 0 )
			return 0;
		SR_DWORD bps = adf_to_bps(adf);
		if ( bps == 0 )
			return 0;

		double samp_count = (double) bytes * 8 / bps;
		SR_DWORD tm = (SR_DWORD)(samp_count * 1000. / sr);
		return tm;
	}

	/** 
	 * @brief 	get_adf_from_sr_bps_tag
	 * 
	 * 	根据采样率、量化比特数、格式标记获得音频格式
	 * 
	 * @author	jdyu
	 * @date	2006-4-15
	 * @return	inline SR_DWORD 
	 * @param	int adf	- [in] 
	 * @param	int bytes	- [in] 
	 * @see		
	 */
	inline SR_DWORD get_adf_from_sr_bps_tag(SR_DWORD sr, SR_DWORD bps, ADF_FMT_TAG fmt_tag = adf_tag_pcm)
	{
		int count = sizeof TTS_ADF_INFO / sizeof TTS_ADF_INFO[0];
		for ( int i = 1; i < count; i++ )
		{
			if ( TTS_ADF_INFO[i].samp_rate_ == sr
				&& TTS_ADF_INFO[i].bps_ == bps
				&& TTS_ADF_INFO[i].win_fmt_id_ == fmt_tag )
				return i;
		}
		return TTS_ADF_DEFAULT;
	}

    //与VADWaveType 类型枚举值相同,add by llgu  20171102
    static const wchar_t* audio_format_names[] = 
    {
        L"audio/L16;rate=8000",                      /*格式为 8k-16bit PCM */
        L"audio/x-alaw-basic",                       /*格式为8k A-law  */
        L"audio/basic",                              /*格式为8k U-law */
        L"audio/L16;rate=16000",                     /*格式为16k-16bit PCM*/
        L"audio/x-alaw-basic;rate=16000",            /*格式为16k A-law*/
        L"audio/basic; rate=16000",                  /*格式为16k U-law*/ 
        L"audio/L16;rate=6000",                      /*格式为 6k-16bit PCM */
    };

}; /* namespace sres */

#endif /* __AUDIO_H__ */
