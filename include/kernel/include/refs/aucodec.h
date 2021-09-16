 /////////////////////////////////////////////////////////////////////////////
//                      Copyright 1999-2003 by iFLYTEK.
//                           All Rights Reserved
/////////////////////////////////////////////////////////////////////////////

#ifndef _AUCODEC_H_
#define _AUCODEC_H_
#include <string>
#include "vcom/type_define.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Export symbols
#if	defined(WIN32)
#	ifdef AUCODEC_EXPORTS
#		define AUCODECAPI __declspec(dllexport)
#	else
#		define AUCODECAPI
#	endif //_USRDLL
#else
#	define	AUCODECAPI
#	include <dlfcn.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// 缺省名称
#if	defined(WIN32)
#	if defined(_DEBUG)
		#define AU_DLL_NAME		"AUCodecd.dll"
#	else
		#define AU_DLL_NAME		"AUCodec.dll"
#	endif
#else
	#define AU_DLL_NAME		"libaucodec.so"
#endif

#define AU_DLL_PFN_CREATE		"CreateAUCodec"
#define AU_DLL_PFN_DESTORY		"DestroyAUCodec"

// AMR和MP3的桢大小
#define AMR_FRAME_IN			320		// L_FRAME16k
#define AMR_FRAME_OUT			37		// block_size[3]
#define MP3_FRAME_IN			576
#define MP3_FRAME_OUT			108


#pragma pack(2)

////////////////////////////////////////////////////////////////////////
// 音频头定义
////////////////////////////////////////////////////////////////////////
struct TWavePCMHdr
{
	char	szRiff[4];			// = "RIFF"
	DWORD	dwSize_8;			// = FileSize - 8
	char	szWave[4];			// = "WAVE"
	char	szFmt[4];			// = "fmt "
	DWORD	dwFmtSize;			// = 下一个结构体的大小 : 16

	WORD	wFormatTag; 		// = PCM : 1
	WORD	wChannels;			// = 通道数 : 1
	DWORD	dwSamplesPerSec;	// = 采样率 : 8000 | 6000 | 11025 | 16000
	DWORD	dwAvgBytesPerSec;	// = 每秒字节数 : dwSamplesPerSec * wBitsPerSample / 8
	WORD	wBlockAlign;		// = 每采样点字节数 : wBitsPerSample / 8
	WORD	wBitsPerSample; 	// = 量化比特数: 8 | 16

	char	szData[4];			// = "data";
	DWORD	dwDatasize; 		// = 纯数据长度 : FileSize - 44
} ;

const TWavePCMHdr DEF_PCMWAVHDR =
{
	{ 'R', 'I', 'F', 'F' },
	0,							// need modify
	{'W', 'A', 'V', 'E'},
	{'f', 'm', 't', ' '},
	16,
	1,
	1,
	8000,						// need modify
	16000,						// need modify
	2,							// need modify
	16, 						// need modify
	{'d', 'a', 't', 'a'},
	0							// need modify
};

////////////////////////////////////////////////////////////////////////
// A/U率声音数据头的定义
////////////////////////////////////////////////////////////////////////
struct TWaveAULAWHdr
{
	char	szRiff[4];			// = "RIFF"
	DWORD	dwSize_8;			// = FileSize - 8
	char	szWave[4];			// = "WAVE"
	char	szFmt[4];			// = "fmt "
	DWORD	dwFmtSize;			// = 下一个结构体的大小 : 18

	WORD	wFormatTag; 		// = aLaw : 6 | uLaw : 7
	WORD	wChannels;			// = 通道数 : 1
	DWORD	dwSamplesPerSec;	// = 采样率 : 8000 | 6000 | 11025 | 16000
	DWORD	dwAvgBytesPerSec;	// = 每秒字节数 : dwSamplesPerSec * wBitsPerSample / 8
	WORD	wBlockAlign;		// = 每采样点字节数 : wBitsPerSample / 8
	WORD	wBitsPerSample; 	// = 量化比特数: 8 | 16
	WORD	wCbSize;			// = 下一个结构体的大小 : 0

	char	szFact[4];			// = "fact"
	WORD	wUnknown1;			// = 4;
	WORD	wUnknown2;			// = 0;
	DWORD	dwDatasize_raw; 	// = Datasize : FileSize - 58

	char	szData[4];			// = "data";
	DWORD	dwDatasize; 		// = DataSize : FileSize - 58
};

const TWaveAULAWHdr DEF_AULAWWAVHDR =
{
	{ 'R', 'I', 'F', 'F' },
	0,							// need modify
	{'W', 'A', 'V', 'E'},
	{'f', 'm', 't', ' '},
	18,
	6,							// need modify
	1,
	8000,						// need modify
	16000,						// need modify
	2,							// need modify
	16,				 			// need modify
	0,
	{'f', 'a', 'c', 't'},
	4,
	0,
	0,							// need modify
	{'d', 'a', 't', 'a'},
	0,							// need modify
};
#pragma pack()

enum au_fmt
{
        __ftPCM   = 1,
        __ftALAW = 6,
        __ftULAW = 7,
};

typedef struct TConvFmtIO
{
    PBYTE   pbData;				// 指向转化输入\输出数据的指针
    DWORD   cbSize;				// 数据的长度(以字节算)
    DWORD   dwFormat;			// 数据的格式
}TConvFmtIO, *PTConvFmtIO;

struct TConvFmtIOEx : public TConvFmtIO
{
	DWORD	dwSR;				//	采样率
	WORD	wChannels;			//	通道数
	WORD	wBits;		        //  比特数
};
typedef TConvFmtIOEx*	PTConvFmtIOEx;


// 采样转化输入输出数据
typedef struct TSRCIO
{
	short*	pData;				// PCM数据
	DWORD	dwCnt;				// 采样点数
	DWORD	dwSR;				// 采样率
} TSRCIO, *PTSRCIO;

typedef struct TMixIO
{
	short* data;
	float ratio;
} TMixIO, *PTMixIO;

typedef enum SRC_Quatity
{
	SRC_FIR_BEST				= 0,
	SRC_FIR_MEDIUM				= 1,
	SRC_FIR_LOW_FAST			= 2,
	SRC_ZERO_ORDER_HOLD_FAST	= 3,
	SRC_LINEAR_FAST				= 4
} SRC_Q;

typedef struct TAudioInfo
{
	DWORD	dwSR;				// 采样率
	WORD	wFmtTag;			// wave格式
	WORD	wBits;				// 量化比特位数
	WORD	wChannels;			// 通道数
	WORD	wBlockAgn;			// 块对齐大小
	float	fDuration;			// 时长
	DWORD	cbFile;				// 文件的长度
	DWORD	cbHead;				// 文件头的长度
	DWORD	cbData;				// 数据的长度
	BYTE*	pbData;
} TAudioInfo, *PTAudioInfo;



// 音频转换接口
struct IAUCodec
{
	/////////////////////////////////////////////////////////////////////////
	// 初始化、逆初始化
	/////////////////////////////////////////////////////////////////////////
	virtual long Initialize() = 0;
	virtual long Uninitialize() = 0;

	/////////////////////////////////////////////////////////////////////////
	// 功能函数
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// 编解码函数，包括Amr, MP3； alaw&ulaw解码；
	// nPcmLen指采样点数，函数AmrEnc，Mp3Enc输出转化后的字节数
	virtual long AmrEnc(short* pnPcm, int nPcmLen, unsigned char* pbAmr) = 0;
	virtual long Mp3Enc(short* pnPcm, int nPcmLen, unsigned char* pbMp3) = 0;
	virtual long AmrDec(BYTE* pbAmr, int nAmrLen, short* pnPcm, int nBufLen) = 0;
	virtual long Mp3Dec(BYTE* pbMp3, int nMp3Len, short* pnPcm, int nBufLen) = 0;
	virtual long AlawDec16(BYTE* pbAlaw, int nAlawLen, short* pnPcm, int& nBufLen) = 0;
	virtual long UlawDec16(BYTE* pbUlaw, int nUlawLen, short* pnPcm, int& nBufLen) = 0;

	/////////////////////////////////////////////////////////////////////////
	// 转换音频格式，包括对pcm、alaw、ulaw、amr、mp3、vox等格式的音频数据的互
	// 相转化，注意只能在相同采样率的数据间进行转换
	virtual long ConvAudioFormat(PTConvFmtIO pIn, PTConvFmtIO pOut) = 0;
	// 估算音频格式转化输出的数据的大小
	virtual long ConvFmtEvalSize(PTConvFmtIO pIn, PTConvFmtIO pOut) = 0;

	/////////////////////////////////////////////////////////////////////////
	// 采样率转换函数: 在一般的语音转换中，推荐使用那几个特定的转换函数；在高
	// 采样率的音频转换中，使用通用函数。

	// 通用任意采样率、多声道采样率转换函数
	virtual long SRCSimple(PTSRCIO pIn, PTSRCIO pOut,
						SRC_Q srct = SRC_FIR_BEST, int nchans = 1) = 0;

	// 几个指定采样率的单声道采样率转换函数
	// 注意：使用SRC16KTo11K函数时要注意输出缓冲区是输入缓冲区长度的4倍左右,
	// 这里传入的nSize为采样点数
	virtual long SRC16KTo08K(short* pIn, int nSize, short* pOut) = 0;
	virtual long SRC16KTo06K(short* pIn, int nSize, short* pOut) = 0;
	virtual long SRC16KTo11K(short* pIn, int nSize, short* pOut) = 0;
	virtual long SRC08KTo06K(short* pIn, int nSize, short* pOut) = 0;

	/////////////////////////////////////////////////////////////////////////
	// 其它函数

	// 获取音频文件信息
	virtual long GetAudioInfo(PCSTR szFile, TAudioInfo& tInfo) = 0;
	// 多通道音频混音函数，bits只能为8和16
	virtual void WaveMix(BYTE chans, PTMixIO io, long count, BYTE out = 0, BYTE bits = 16) = 0;
	// 线程安全的MP3解码函数
	virtual long MADDec(unsigned char* pbMp3, int& cbMp3, short* pnPcm, int& nCnt, int& nSR) = 0;

	// 线程安全的MP3解码函数(文件方式转换)
	// 如果是基于线程池实现，可以考虑prealloc_mem=true
	// 注：预分配的内存，会绑定在当前工作线程中。直到系统退出时才释放
	virtual long MADDec(const char* psz_mp3_in_path, const char* psz_pcm_out_path, bool prealloc_mem = false) = 0;

	// 将支持的A/U-lav PCM转成指定采样率的PCM
	virtual long ConvAudioFormatEx(PTConvFmtIOEx pIn, PTConvFmtIOEx pOut) = 0;
};

typedef IAUCodec *PIAUCodec;

// 创建音频编解码器接口
long AUCODECAPI CreateAUCodec(IAUCodec** ppAUCodec);
typedef long AUCODECAPI (*PROC_AUCODEC_CREATE)(IAUCodec** ppAUCodec);

// 释放音频编解码器接口
long AUCODECAPI DestroyAUCodec(IAUCodec* pAUCodec);
typedef long AUCODECAPI (*PROC_AUCODEC_DESTROY)(IAUCodec* pAUCodec);

// 音频编解码的辅助类
class CAUHelper
{
public:
	CAUHelper() : m_hAUCodecLib(0),
		m_pAuCodec(0), m_pfnCreateAUCodec(0), m_pfnDestroyAUCodec(0) {
	}
	HINSTANCE				m_hAUCodecLib;
	IAUCodec*				m_pAuCodec;
	PROC_AUCODEC_CREATE		m_pfnCreateAUCodec;
	PROC_AUCODEC_DESTROY	m_pfnDestroyAUCodec;

public:
	int Initialize(const char * pszPath)
	{
		int ret = 0;

		if( m_hAUCodecLib != NULL )
		{
			return ret;
		}
#ifdef WIN32
        m_hAUCodecLib = ::LoadLibraryA(pszPath); // 加载DLL
		if( NULL == m_hAUCodecLib )
		{
			return ::GetLastError();
		}
		do
		{
			// 获得构建和析构函数的地址
			m_pfnCreateAUCodec = (PROC_AUCODEC_CREATE)
				GetProcAddress(m_hAUCodecLib, AU_DLL_PFN_CREATE);
			if( NULL == m_pfnCreateAUCodec ) {
				ret = GetLastError();
				break;
			}
			m_pfnDestroyAUCodec = (PROC_AUCODEC_DESTROY)
				GetProcAddress(m_hAUCodecLib, AU_DLL_PFN_DESTORY);
			if( NULL == m_pfnDestroyAUCodec ) {
				ret = GetLastError();
				break;
			}

			// 创建一个接口
			ret = m_pfnCreateAUCodec(&m_pAuCodec);
			if( ret != 0 ) break;
			ret = m_pAuCodec->Initialize();
			if( ret != 0 ) break;
		}
		while ( 0 );

		if( ret != 0  ) {
			Uninitialize();
		}
#else
		std::string	err_msg_;
		m_hAUCodecLib = dlopen(pszPath, RTLD_LAZY);
		if (NULL == m_hAUCodecLib)
		{
		    #ifdef WIN32
			return (int)dlerror();
			#else
            return (int)errno;
			#endif
		}

		do
		{
			// 获得构建和析构函数的地址
			m_pfnCreateAUCodec = (PROC_AUCODEC_CREATE)dlsym(m_hAUCodecLib, (char*)AU_DLL_PFN_CREATE);
			if (m_pfnCreateAUCodec == NULL)
			{
				break;
			}

			m_pfnDestroyAUCodec = (PROC_AUCODEC_DESTROY)dlsym(m_hAUCodecLib, (char*)AU_DLL_PFN_DESTORY);
			if (m_pfnDestroyAUCodec == NULL)
			{
				break;
			}

			// 创建一个接口
			ret = m_pfnCreateAUCodec(&m_pAuCodec);
			if( ret != 0 )
			{
				break;
			}
			ret = m_pAuCodec->Initialize();
			if( ret != 0 )
			{
				break;
			}
		} while (0);

		if( ret != 0  )
		{
			Uninitialize();
		}
#endif

		return ret;
	}

	long Uninitialize()
	{
		if ( m_pAuCodec )
		{
			m_pAuCodec->Uninitialize();
			m_pfnDestroyAUCodec(m_pAuCodec);
		}

#ifdef WIN32
		if ( m_hAUCodecLib )
		{
			::FreeLibrary(m_hAUCodecLib);
		}
#endif
		memset(this, 0, sizeof(*this));

		return 0;
	}

	static CAUHelper*& GetAUHelper() {
		static CAUHelper* pAUHelper = NULL;
		return pAUHelper;
	}
};

// AUCodec动态链接库初始化
inline long AUInitialize(const char* pszPath)
{
	CAUHelper::GetAUHelper() = new CAUHelper();
	return CAUHelper::GetAUHelper()->Initialize(pszPath);
}

// AUCodec动态链接库逆初始化
inline long AUUninitialize()
{
	long ret = 0;
	if ( CAUHelper::GetAUHelper() != NULL )
	{
		CAUHelper::GetAUHelper()->Uninitialize();
		delete CAUHelper::GetAUHelper();
		CAUHelper::GetAUHelper() = NULL;
	}
	return ret;
}

inline IAUCodec* ISFGetAUCodec()
{
	return CAUHelper::GetAUHelper()->m_pAuCodec;
}

/////////////////////////////////////////////////////////////////////////////
// 方便调用的宏

#define AUConvAudioFormat ISFGetAUCodec()->ConvAudioFormat
#define AUConvFmtEvalSize ISFGetAUCodec()->ConvFmtEvalSize

#define AUAmrEnc		ISFGetAUCodec()->AmrEnc
#define AUAmrDec		ISFGetAUCodec()->AmrDec
#define AUMp3Enc		ISFGetAUCodec()->Mp3Enc
#define AUMp3Dec		ISFGetAUCodec()->Mp3Dec

#define AUSRCSimple		ISFGetAUCodec()->SRCSimple
#define AUSRC16KTo08K	ISFGetAUCodec()->SRC16KTo08K
#define AUSRC16KTo06K	ISFGetAUCodec()->SRC16KTo06K
#define AUSRC16KTo11K	ISFGetAUCodec()->SRC16KTo11K
#define AUSRC08KTo06K	ISFGetAUCodec()->SRC08KTo06K

#define AUGetAudioInfo	ISFGetAUCodec()->GetAudioInfo
#define AUWaveMix		ISFGetAUCodec()->WaveMix

#define AUMADDec		ISFGetAUCodec()->MADDec
#define AUALAWDEC16		ISFGetAUCodec()->AlawDec16
#define AUULAWDEC16		ISFGetAUCodec()->UlawDec16

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _AUCODEC_H_
