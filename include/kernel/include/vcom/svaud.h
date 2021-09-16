/** 
* @file	svaud.h
* @brief	音频相关封装
* 
* 
* @author	jdyu
* @version	1.0
* @date	2007-8-2
* 
* @see		
* 
* <b>History:</b><br>
* <table>
*  <tr> <th>Version	<th>Date		<th>Author	<th>Notes</tr>
*  <tr> <td>1.0		<td>2007-8-2	<td>jdyu	<td>Create this file</tr>
*  <tr> <td>1.1		<td>2008-9-2	<td>jieli	<td>Modify this file for isv</tr>
* </table>
* 
*/
#ifndef __SVAUD_H__
#define __SVAUD_H__

#include "../utils/sutils.h"
#include "utils/g711.h"

namespace sv
{
	////////////////////////////////////////////////////////////////////////
	// 音频头定义
	////////////////////////////////////////////////////////////////////////
#pragma pack(2)
	struct wave_pcm_hdr
	{
		char		riff[4];				// = "RIFF"
		int			size_8;					// = FileSize - 8
		char		wave[4];				// = "WAVE"
		char		fmt[4];					// = "fmt "
		int			dwFmtSize;				// = 下一个结构体的大小 : 16

		short		format_tag; 			// = PCM : 1
		short		channels;				// = 通道数 : 1
		int			samples_per_sec;		// = 采样率 : 8000 | 6000 | 11025 | 16000
		int			avg_bytes_per_sec;		// = 每秒字节数 : dwSamplesPerSec * wBitsPerSample / 8
		short		block_align;			// = 每采样点字节数 : wBitsPerSample / 8
		short		bits_per_sample; 		// = 量化比特数: 8 | 16

		char		data[4];				// = "data";
		int			data_size;	 			// = 纯数据长度 : FileSize - 44 
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
		char		riff[4];				// = "RIFF"
		int			size_8;					// = FileSize - 8
		char		wave[4];				// = "WAVE"
		char		fmt[4];					// = "fmt "
		int			fmt_size;				// = 下一个结构体的大小 : 18

		short		format_tag; 			// = aLaw : 6 | uLaw : 7
		short		channels;				// = 通道数 : 1
		int			samples_per_sec;		// = 采样率 : 8000 | 6000 | 11025 | 16000
		int			avg_bytes_per_sec;		// = 每秒字节数 : dwSamplesPerSec * wBitsPerSample / 8
		short		block_align;			// = 每采样点字节数 : wBitsPerSample / 8
		short		bits_per_sample; 		// = 量化比特数: 8 | 16
		short		cbsize;					// = 下一个结构体的大小 : 0

		char		fact[4];				// = "fact"
		short		unknown1;				// = 4;
		short		unknown2;				// = 0;
		int			data_size_raw; 			// = Datasize : FileSize - 58

		char		data[4];				// = "data";
		int			data_size;	 			// = DataSize : FileSize - 58
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

    enum
    {
        VSPP_AUDIO_SAMPLE_INIT           = 0x00,
        VSPP_AUDIO_SAMPLE_FIRST          = 0x01,
        VSPP_AUDIO_SAMPLE_CONTINUE       = 0x02,
        VSPP_AUDIO_SAMPLE_LAST           = 0x04,
        VSPP_AUDIO_SAMPLE_SUPPRESSED     = 0x08,
        VSPP_AUDIO_SAMPLE_LOST           = 0x10,
        VSPP_AUDIO_SAMPLE_NEW_CHUNK      = 0x20,
        VSPP_AUDIO_SAMPLE_END_CHUNK      = 0x40,

        VSPP_AUDIO_SAMPLE_VALIDBITS      = 0x7f /* to validate the value of sample->status */
    };

    enum
    {
        TTS_ADF_DEFAULT                 = 0,
        TTS_ADF_PCM8K8B1C               = 1,
        TTS_ADF_PCM16K8B1C              = 2,
        TTS_ADF_PCM8K16B1C              = 3,
        TTS_ADF_PCM16K16B1C             = 4,
        TTS_ADF_PCM11K8B1C              = 5,
        TTS_ADF_PCM11K16B1C             = 6,
        TTS_ADF_PCM6K8B1C               = 7,
        TTS_ADF_PCM6K16B1C              = 8,
        TTS_ADF_ALAW16K1C               = 9,
        TTS_ADF_ULAW16K1C               = 10,
        TTS_ADF_ALAW8K1C                = 11,
        TTS_ADF_ULAW8K1C                = 12,
        TTS_ADF_ALAW11K1C               = 13,
        TTS_ADF_ULAW11K1C               = 14,
        TTS_ADF_ALAW6K1C                = 15,
        TTS_ADF_ULAW6K1C                = 16,
        TTS_ADF_ADPCMG7218K4B1C         = 17,
        TTS_ADF_ADPCMG7216K4B1C         = 18,
        TTS_ADF_ADPCMG7233B1C           = 19,
        TTS_ADF_ADPCMG7235B1C           = 20,
        TTS_ADF_VOX6K1C                 = 21,
        TTS_ADF_VOX8K1C                 = 22,
        TTS_ADF_AMR6K1C                 = 23,
        TTS_ADF_AMR8K1C                 = 24,
        TTS_ADF_AMR11K1C                = 25,
        TTS_ADF_AMR16K1C                = 26,
        TTS_ADF_MP36K1C                 = 27,
        TTS_ADF_MP38K1C                 = 28,
        TTS_ADF_MP311K1C                = 29,
        TTS_ADF_MP316K1C                = 30,
    };

    // 基本的音频格式类型
    enum audio_type_id
    {
        audio_type_ulaw			= 0,
        audio_type_alaw,
        audio_type_pcm16b,
        audio_type_pcm8b,
		audio_type_vox,
        audio_type_invalid		= -1,
    };

    // 基本的音频格式类型对应的描述串
    const wchar_t * const audio_type_str[] =
    {
        L"audio/basic",
        L"audio/x-alaw-basic",
        L"audio/L16",
        L"audio/L8",
		L"audio/voxware"
	};

    // 静音段去除方式
    enum silence_suppression_type
    {
        silence_suppression_none		= 0,
        silence_suppression_begin		= 1,
        silence_suppression_end			= 2,
        silence_suppression_interword	= 4,

        silence_suppression_invalid		= -1,
    };

    const wchar_t * const silence_suppression_str[] =
    {
        L"none"
        L"begin",
        L"end",
        L""
        L"interword",
    };

    const wchar_t silence_suppression_token[] = L"silence_suppression";

    // 识别器支持的音频格式ID和TTS保持一致，内部使用
    enum audio_format_id
    {
        audio_format_default		= TTS_ADF_PCM16K16B1C,
        audio_format_ulaw			= TTS_ADF_ULAW8K1C,
        audio_format_ulaw16k		= TTS_ADF_ULAW16K1C,
        audio_format_alaw			= TTS_ADF_ALAW8K1C,
        audio_format_alaw16k		= TTS_ADF_ALAW16K1C,
        audio_format_pcm8k8bit		= TTS_ADF_PCM8K8B1C,
        audio_format_pcm8k16bit		= TTS_ADF_PCM8K16B1C, 
        audio_format_pcm16k8bit		= TTS_ADF_PCM16K8B1C,
        audio_format_pcm16k16bit	= TTS_ADF_PCM16K16B1C,
		audio_format_vox6k4bit		= TTS_ADF_VOX6K1C,
        audio_format_vox8k4bit      = TTS_ADF_VOX8K1C,        
        audio_format_pcm6k16bit     = TTS_ADF_PCM6K16B1C, //add by llgu 2017-11-2
        audio_format_alaw6k         = TTS_ADF_ALAW6K1C,   //add by llgu 2017-12-4
        audio_format_ulaw6k         = TTS_ADF_ULAW6K1C,   //add by llgu 2017-12-4
        audio_format_invalid		= -1,
    };

    enum audio_fmt
    {
        __ftPCM   = 1,
        __ftALAW = 6, 
        __ftULAW = 7,
    };
    typedef audio_fmt audio_fmt;

    // 信息矩阵
    // "silence_suppression=interword+end+begin;audio/basic;rate=8000"
    const struct  
    {
        audio_format_id	format_id;
        audio_type_id	type_id;
        int				sample_rate;
        int				bps;
        audio_fmt		au_fmt;
		float			max_cmp_ratio_8k16b;
        const wchar_t	audio_str[32];
    }
    audio_info_table[] =
    {
        /* 音频格式						音频类型				采样率		比特数		pcm|alaw|ulaw	最大压缩比(8k16bit)		描述字符串 */
        { audio_format_default,			audio_type_pcm16b,		16000,		16,			__ftPCM,		0.5,					L"audio/L16;rate=16000" },
        { audio_format_ulaw,			audio_type_ulaw,		8000,		8,			__ftULAW,		2,						L"audio/basic" },
        { audio_format_ulaw16k,			audio_type_ulaw,		16000,		8,			__ftULAW,		1,						L"audio/basic;rate=16000" },
        { audio_format_alaw,			audio_type_alaw,		8000,	    8,			__ftALAW,		2,						L"audio/x-alaw-basic" },
        { audio_format_alaw16k,			audio_type_alaw,		16000,		8,			__ftALAW,		1,						L"audio/x-alaw-basic;rate=16000" },
        { audio_format_pcm8k8bit,		audio_type_pcm8b,		8000,		8,			__ftPCM,		2,						L"audio/L8;rate=8000" },
        { audio_format_pcm8k16bit,		audio_type_pcm16b,		8000,		16,			__ftPCM,		1,						L"audio/L16;rate=8000" },
        { audio_format_pcm16k8bit,		audio_type_pcm8b,		16000,		8,			__ftPCM,		1,						L"audio/L8;rate=16000" },
        { audio_format_pcm16k16bit,		audio_type_pcm16b,		16000,		16,			__ftPCM,		0.5,					L"audio/L16;rate=16000" },
		{ audio_format_vox6k4bit,		audio_type_vox,			6000,		4,			__ftPCM,		4,						L"audio/voxware;rate=6000" },
        { audio_format_vox8k4bit,		audio_type_vox,			8000,		4,			__ftPCM,		4,						L"audio/voxware;rate=8000" },
        { audio_format_pcm6k16bit,		audio_type_pcm16b,		6000,		16,			__ftPCM,		1,					    L"audio/L16;rate=6000"     }, //added by llgu20171109
    };

    // 缺省采样率
    const int audio_default_samples		= 8000;
    // 缺省音频格式
    const int audio_default_fmt			= TTS_ADF_PCM16K16B1C;
    const int audio_default_bps			= 16;
    // 缺省时间片长度
    const int audio_default_time_slice	= 20;
    // 缺省一帧字节数长度
    const int audio_default_frame_samp	= audio_default_samples * audio_default_time_slice / 1000;
    const int audio_default_frame_bytes	= audio_default_frame_samp * audio_default_bps / 8;

    inline
        int get_silence_suppression_type(const wchar_t * ssp_str)
    {
        int ssp_type_ret = silence_suppression_none;

        sp::wstr_arr ssp_arr;
        sp::split_str(ssp_str, ssp_arr, L"+");
        for ( int i = 0; i < (int)ssp_arr.size(); i++ )
        {
            int index = sp::search_strarr(ssp_arr[i].c_str(), silence_suppression_str);
            if ( index == -1 )
                return silence_suppression_invalid;
            ssp_type_ret |= index;
        }
        return ssp_type_ret;
    }

    inline
        audio_type_id get_audio_format_type(const wchar_t * type_str)
    {
        int index = sp::search_strarr(type_str, audio_type_str);
        if ( index == -1 )
            return audio_type_invalid;
        return (audio_type_id)index;
    }

    inline
        const wchar_t * get_audio_str(int audio_fmt)
    {
        for ( int i = 0; i < SP_COUNTOF(audio_info_table); i++ )
        {
            if ( audio_info_table[i].format_id == audio_fmt )
                return audio_info_table[i].audio_str;
        }
        return 0;
    }

    inline bool is_audio_format_support(int audio_fmt)
    {
        return audio_fmt == audio_format_ulaw	             
            || audio_fmt == audio_format_ulaw16k	     
            || audio_fmt == audio_format_alaw            
            || audio_fmt == audio_format_alaw16k	         
            || audio_fmt == audio_format_pcm8k8bit	 
            || audio_fmt == audio_format_pcm16k8bit	 
            || audio_fmt == audio_format_pcm8k16bit	 
            || audio_fmt == audio_format_pcm16k16bit;
    } 

    inline int au_decoder(void * samples,const int bytes,const sv::audio_format_id src_adf,
        const sv::audio_format_id dst_adf,void * out_data,int & samples_cnt)
    {
        //SP_ASSERT(samples && bytes >= 0);
        short * src_s, * dst_s;
        unsigned char * src_u, * dst_u;
        int i = 0;

        if (src_adf == sv::audio_format_pcm8k16bit)
        {
            samples_cnt = bytes / sizeof(short);
            src_s = (short *)samples;
            dst_u = (unsigned char *)out_data;
            switch(dst_adf)
            {
            case sv::audio_format_pcm8k16bit:
                memcpy(out_data,samples,sizeof(short) * samples_cnt);
                break;
            case sv::audio_format_alaw:
                for (i = 0;i < samples_cnt;i++)
                {
                    *dst_u = linear2alaw(*src_s);
                    src_s ++; dst_u ++;
                }
                break;
            case sv::audio_format_ulaw:
                for (i = 0;i < samples_cnt;i++)
                {
                    *dst_u = linear2ulaw(*src_s);
                    src_s ++; dst_u ++;
                }
                break;
            }
        }
        else if (src_adf == sv::audio_format_alaw)
        {
            src_u = (unsigned char *)samples;
            samples_cnt = bytes;
            switch(dst_adf)
            {
            case sv::audio_format_pcm8k16bit:
                dst_s = (short *)out_data;
                for (i = 0;i < samples_cnt;i++)
                {
                    *dst_s = alaw2linear(*src_u);
                    dst_s ++; src_u ++;
                }
                break;
            case sv::audio_format_alaw:
                memcpy(out_data,samples,sizeof(unsigned char) * samples_cnt);
                break;
            case sv::audio_format_ulaw:
                dst_u = (unsigned char *)out_data;
                for (i = 0;i < samples_cnt;i++)
                {
                    *dst_u = alaw2ulaw(*src_u);
                    dst_u ++; src_u ++;
                }
                break;
            }
        }
        else if (src_adf == sv::audio_format_ulaw)
        {   
            samples_cnt = bytes;
            src_u = (unsigned char *)samples;
            switch(dst_adf)
            {
            case sv::audio_format_pcm8k16bit:
                dst_s = (short *)out_data;
                for (i = 0;i < samples_cnt;i++)
                {
                    *dst_s = ulaw2linear(*src_u);
                    dst_s ++; src_u ++;
                }
                break;
            case sv::audio_format_alaw:
                dst_u = (unsigned char *)out_data;
                for (i = 0;i < samples_cnt;i++)
                {
                    *dst_u = ulaw2alaw(*src_u);
                    dst_u ++; src_u ++;
                }
                break;
            case sv::audio_format_ulaw:
                memcpy(out_data,samples,sizeof(unsigned char) * samples_cnt);
                break;
            }
        }
        else if ( src_adf ==  sv::audio_format_pcm16k16bit)
        {
            samples_cnt = bytes / sizeof(short);
            src_s = (short *)samples;
            dst_s = (short *)out_data;

            switch( dst_adf )
            {
            case audio_format_pcm8k16bit:
                samples_cnt = samples_cnt/2;
                for ( int i = 0; i < samples_cnt; i++ )
                {
                    *dst_s = *src_s;
                    dst_s++; 
                    src_s += 2;
                }
                samples_cnt = bytes / (2*sizeof(short));
                break;
            }
        }
        return 0;
    }
    /** 
    * @fn		parse_audio_str
    * @brief	从音频格式字符串得到格式ID
    * 
    *  从音频格式字符串得到格式ID
    * 
    * @author	jdyu
    * @date	2007-9-13
    * @return	audio_type 
    * @param	const wchar_t * str	- [in] 格式字符串
    * @see		
    */
    inline
        audio_format_id parse_audio_str(const wchar_t * str, int & sil_supp_type)
    {
        sil_supp_type = silence_suppression_none;
        if ( str == 0 )
            return audio_format_pcm8k16bit;

        sp::wstr_arr aud_info;
        sp::split_str(str, aud_info, L";");

        audio_type_id type_id = audio_type_invalid;
        int sample_rate = 8000;

        for ( int i = 0; i < (int)aud_info.size(); i++ )
        {
            std::wstring token;
            std::wstring value;

            sp::get_attrib_and_value(aud_info[i].c_str(), token, value);
            if ( token == L"rate" )
                sample_rate = sp::strtoi(value.c_str());
            else if ( token == silence_suppression_token )
                sil_supp_type = get_silence_suppression_type(value.c_str());
            else
                type_id = get_audio_format_type(aud_info[i].c_str());
        }

        for ( int j = 1; j < SP_COUNTOF(audio_info_table); j++ )
        {
            if ( audio_info_table[j].type_id == type_id
                && audio_info_table[j].sample_rate == sample_rate )
                return audio_info_table[j].format_id;
        }
        return audio_format_invalid;
    }

    inline int get_samprate(audio_format_id fmt_id)
    {
        for ( int j = 1; j < SP_COUNTOF(audio_info_table); j++ )
        {
            if ( audio_info_table[j].format_id == fmt_id)
                return audio_info_table[j].sample_rate;
        }
        return 16000;
    }

    inline int get_bits(audio_format_id fmt_id)
    {
        for ( int j = 1; j < SP_COUNTOF(audio_info_table); j++ )
        {
            if ( audio_info_table[j].format_id == fmt_id)
                return audio_info_table[j].bps;
        }
        return 8;
    }

    inline int get_au_fmt(audio_format_id fmt_id)
    {
        for ( int j = 1; j < SP_COUNTOF(audio_info_table); j++ )
        {
            if ( audio_info_table[j].format_id == fmt_id)
                return audio_info_table[j].au_fmt;
        }
        return __ftPCM;
    }

    inline
        audio_format_id parse_audio_str(const wchar_t * str)
    {
        int sil_supp = silence_suppression_none;
        return parse_audio_str(str, sil_supp);
    }

	inline 
	float get_max_cmp_ratio_8k16b(audio_format_id fmt_id)
	{
		for ( int j = 1; j < SP_COUNTOF(audio_info_table); j++ )
		{
			if ( audio_info_table[j].format_id == fmt_id)
				return audio_info_table[j].max_cmp_ratio_8k16b;
		}

		return 1;
	}

	inline
	int get_audio_buf_detail(const void * buffer, int size, audio_format_id & adf, int & head_size, int & data_size)
	{
		int ret = 0;
		int fmt = 0;
		int hs  = 0;

		data_size = 0;
		int ds = size;

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
				{ fmt = head.bits_per_sample == 8 ? pcm8 : pcm16; hs = 44; ds = std::min<unsigned int>(pcm->data_size, ds - hs); }	\
			else if ( head.format_tag == 6 )												\
				{ fmt = alaw; hs = 58; ds = std::min<unsigned int>(head.data_size, ds - hs); }	\
			else if ( head.format_tag == 7 )												\
				{ fmt = ulaw; hs = 58; ds = std::min<unsigned int>(head.data_size, ds - hs); }	\
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

		adf = (audio_format_id)fmt;
		data_size = ds;
		head_size = hs;
		return ret;
	}

} /* namespace sv */
#endif /* __SVAUD_H__ */
