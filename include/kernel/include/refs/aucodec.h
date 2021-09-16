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

// ȱʡ����
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

// AMR��MP3�����С
#define AMR_FRAME_IN			320		// L_FRAME16k
#define AMR_FRAME_OUT			37		// block_size[3]
#define MP3_FRAME_IN			576
#define MP3_FRAME_OUT			108


#pragma pack(2)

////////////////////////////////////////////////////////////////////////
// ��Ƶͷ����
////////////////////////////////////////////////////////////////////////
struct TWavePCMHdr
{
	char	szRiff[4];			// = "RIFF"
	DWORD	dwSize_8;			// = FileSize - 8
	char	szWave[4];			// = "WAVE"
	char	szFmt[4];			// = "fmt "
	DWORD	dwFmtSize;			// = ��һ���ṹ��Ĵ�С : 16

	WORD	wFormatTag; 		// = PCM : 1
	WORD	wChannels;			// = ͨ���� : 1
	DWORD	dwSamplesPerSec;	// = ������ : 8000 | 6000 | 11025 | 16000
	DWORD	dwAvgBytesPerSec;	// = ÿ���ֽ��� : dwSamplesPerSec * wBitsPerSample / 8
	WORD	wBlockAlign;		// = ÿ�������ֽ��� : wBitsPerSample / 8
	WORD	wBitsPerSample; 	// = ����������: 8 | 16

	char	szData[4];			// = "data";
	DWORD	dwDatasize; 		// = �����ݳ��� : FileSize - 44
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
// A/U����������ͷ�Ķ���
////////////////////////////////////////////////////////////////////////
struct TWaveAULAWHdr
{
	char	szRiff[4];			// = "RIFF"
	DWORD	dwSize_8;			// = FileSize - 8
	char	szWave[4];			// = "WAVE"
	char	szFmt[4];			// = "fmt "
	DWORD	dwFmtSize;			// = ��һ���ṹ��Ĵ�С : 18

	WORD	wFormatTag; 		// = aLaw : 6 | uLaw : 7
	WORD	wChannels;			// = ͨ���� : 1
	DWORD	dwSamplesPerSec;	// = ������ : 8000 | 6000 | 11025 | 16000
	DWORD	dwAvgBytesPerSec;	// = ÿ���ֽ��� : dwSamplesPerSec * wBitsPerSample / 8
	WORD	wBlockAlign;		// = ÿ�������ֽ��� : wBitsPerSample / 8
	WORD	wBitsPerSample; 	// = ����������: 8 | 16
	WORD	wCbSize;			// = ��һ���ṹ��Ĵ�С : 0

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
    PBYTE   pbData;				// ָ��ת������\������ݵ�ָ��
    DWORD   cbSize;				// ���ݵĳ���(���ֽ���)
    DWORD   dwFormat;			// ���ݵĸ�ʽ
}TConvFmtIO, *PTConvFmtIO;

struct TConvFmtIOEx : public TConvFmtIO
{
	DWORD	dwSR;				//	������
	WORD	wChannels;			//	ͨ����
	WORD	wBits;		        //  ������
};
typedef TConvFmtIOEx*	PTConvFmtIOEx;


// ����ת�������������
typedef struct TSRCIO
{
	short*	pData;				// PCM����
	DWORD	dwCnt;				// ��������
	DWORD	dwSR;				// ������
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
	DWORD	dwSR;				// ������
	WORD	wFmtTag;			// wave��ʽ
	WORD	wBits;				// ��������λ��
	WORD	wChannels;			// ͨ����
	WORD	wBlockAgn;			// ������С
	float	fDuration;			// ʱ��
	DWORD	cbFile;				// �ļ��ĳ���
	DWORD	cbHead;				// �ļ�ͷ�ĳ���
	DWORD	cbData;				// ���ݵĳ���
	BYTE*	pbData;
} TAudioInfo, *PTAudioInfo;



// ��Ƶת���ӿ�
struct IAUCodec
{
	/////////////////////////////////////////////////////////////////////////
	// ��ʼ�������ʼ��
	/////////////////////////////////////////////////////////////////////////
	virtual long Initialize() = 0;
	virtual long Uninitialize() = 0;

	/////////////////////////////////////////////////////////////////////////
	// ���ܺ���
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// ����뺯��������Amr, MP3�� alaw&ulaw���룻
	// nPcmLenָ��������������AmrEnc��Mp3Enc���ת������ֽ���
	virtual long AmrEnc(short* pnPcm, int nPcmLen, unsigned char* pbAmr) = 0;
	virtual long Mp3Enc(short* pnPcm, int nPcmLen, unsigned char* pbMp3) = 0;
	virtual long AmrDec(BYTE* pbAmr, int nAmrLen, short* pnPcm, int nBufLen) = 0;
	virtual long Mp3Dec(BYTE* pbMp3, int nMp3Len, short* pnPcm, int nBufLen) = 0;
	virtual long AlawDec16(BYTE* pbAlaw, int nAlawLen, short* pnPcm, int& nBufLen) = 0;
	virtual long UlawDec16(BYTE* pbUlaw, int nUlawLen, short* pnPcm, int& nBufLen) = 0;

	/////////////////////////////////////////////////////////////////////////
	// ת����Ƶ��ʽ��������pcm��alaw��ulaw��amr��mp3��vox�ȸ�ʽ����Ƶ���ݵĻ�
	// ��ת����ע��ֻ������ͬ�����ʵ����ݼ����ת��
	virtual long ConvAudioFormat(PTConvFmtIO pIn, PTConvFmtIO pOut) = 0;
	// ������Ƶ��ʽת����������ݵĴ�С
	virtual long ConvFmtEvalSize(PTConvFmtIO pIn, PTConvFmtIO pOut) = 0;

	/////////////////////////////////////////////////////////////////////////
	// ������ת������: ��һ�������ת���У��Ƽ�ʹ���Ǽ����ض���ת���������ڸ�
	// �����ʵ���Ƶת���У�ʹ��ͨ�ú�����

	// ͨ����������ʡ�������������ת������
	virtual long SRCSimple(PTSRCIO pIn, PTSRCIO pOut,
						SRC_Q srct = SRC_FIR_BEST, int nchans = 1) = 0;

	// ����ָ�������ʵĵ�����������ת������
	// ע�⣺ʹ��SRC16KTo11K����ʱҪע����������������뻺�������ȵ�4������,
	// ���ﴫ���nSizeΪ��������
	virtual long SRC16KTo08K(short* pIn, int nSize, short* pOut) = 0;
	virtual long SRC16KTo06K(short* pIn, int nSize, short* pOut) = 0;
	virtual long SRC16KTo11K(short* pIn, int nSize, short* pOut) = 0;
	virtual long SRC08KTo06K(short* pIn, int nSize, short* pOut) = 0;

	/////////////////////////////////////////////////////////////////////////
	// ��������

	// ��ȡ��Ƶ�ļ���Ϣ
	virtual long GetAudioInfo(PCSTR szFile, TAudioInfo& tInfo) = 0;
	// ��ͨ����Ƶ����������bitsֻ��Ϊ8��16
	virtual void WaveMix(BYTE chans, PTMixIO io, long count, BYTE out = 0, BYTE bits = 16) = 0;
	// �̰߳�ȫ��MP3���뺯��
	virtual long MADDec(unsigned char* pbMp3, int& cbMp3, short* pnPcm, int& nCnt, int& nSR) = 0;

	// �̰߳�ȫ��MP3���뺯��(�ļ���ʽת��)
	// ����ǻ����̳߳�ʵ�֣����Կ���prealloc_mem=true
	// ע��Ԥ������ڴ棬����ڵ�ǰ�����߳��С�ֱ��ϵͳ�˳�ʱ���ͷ�
	virtual long MADDec(const char* psz_mp3_in_path, const char* psz_pcm_out_path, bool prealloc_mem = false) = 0;

	// ��֧�ֵ�A/U-lav PCMת��ָ�������ʵ�PCM
	virtual long ConvAudioFormatEx(PTConvFmtIOEx pIn, PTConvFmtIOEx pOut) = 0;
};

typedef IAUCodec *PIAUCodec;

// ������Ƶ��������ӿ�
long AUCODECAPI CreateAUCodec(IAUCodec** ppAUCodec);
typedef long AUCODECAPI (*PROC_AUCODEC_CREATE)(IAUCodec** ppAUCodec);

// �ͷ���Ƶ��������ӿ�
long AUCODECAPI DestroyAUCodec(IAUCodec* pAUCodec);
typedef long AUCODECAPI (*PROC_AUCODEC_DESTROY)(IAUCodec* pAUCodec);

// ��Ƶ�����ĸ�����
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
        m_hAUCodecLib = ::LoadLibraryA(pszPath); // ����DLL
		if( NULL == m_hAUCodecLib )
		{
			return ::GetLastError();
		}
		do
		{
			// ��ù��������������ĵ�ַ
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

			// ����һ���ӿ�
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
			// ��ù��������������ĵ�ַ
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

			// ����һ���ӿ�
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

// AUCodec��̬���ӿ��ʼ��
inline long AUInitialize(const char* pszPath)
{
	CAUHelper::GetAUHelper() = new CAUHelper();
	return CAUHelper::GetAUHelper()->Initialize(pszPath);
}

// AUCodec��̬���ӿ����ʼ��
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
// ������õĺ�

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
