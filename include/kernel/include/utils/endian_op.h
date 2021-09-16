/*
	该函数主要用于处理字节序相关的问题
*/

#ifndef __ENDIAN_OP_H__
#define __ENDIAN_OP_H__

#define	SRES_BO_LE       ((unsigned short)0)         // Little-Endian Intel平台
#define	SRES_BO_BE       ((unsigned short)0xFFFF)    // Big-Endian
#include <memory.h>

namespace sp
{
	// 检测平台的Endian
	typedef union uEndianCheck
	{
		int value;
		struct
		{
			bool blittle_endian;
			bool filler[3];
		};
	}EndianCheck;

	inline bool is_platform_little_endian()
	{
		static const EndianCheck check_endian = { (int)1 };
		return check_endian.blittle_endian;
	}

	inline unsigned short get_platform_endian()
	{
		return is_platform_little_endian() ? SRES_BO_LE : SRES_BO_BE;
	}

	inline void convert_endian_64( void* lpMem )
	{
		unsigned char * p = (unsigned char*)lpMem;
		p[0] = p[0] ^ p[7];
		p[7] = p[0] ^ p[7];
		p[0] = p[0] ^ p[7];

		p[1] = p[1] ^ p[6];
		p[6] = p[1] ^ p[6];
		p[1] = p[1] ^ p[6];

		p[2] = p[2] ^ p[5];
		p[5] = p[2] ^ p[5];
		p[2] = p[2] ^ p[5];

		p[3] = p[3] ^ p[4];
		p[4] = p[3] ^ p[4];
		p[3] = p[3] ^ p[4];
	}

	inline void convert_endian_32( void* lpMem )
	{
		unsigned char * p = (unsigned char*)lpMem;
		p[0] = p[0] ^ p[3];
		p[3] = p[0] ^ p[3];
		p[0] = p[0] ^ p[3];
		p[1] = p[1] ^ p[2];
		p[2] = p[1] ^ p[2];
		p[1] = p[1] ^ p[2];
	}

	inline void convert_endian_16( void* lpMem )
	{
		unsigned char * p = (unsigned char*)lpMem;
		p[0] = p[0] ^ p[1];
		p[1] = p[0] ^ p[1];
		p[0] = p[0] ^ p[1];
	}

	// 内存映射区域解码:其中mem_buf不被修改，仅仅移动指针位置
	class membuf_decoder
	{
	public:
		membuf_decoder(const char* mem_buf, bool convert)
			: mem_buf_(mem_buf)
			, convert_(convert)
		{
		}

		unsigned get_uint()
		{
			unsigned value;
			memcpy(&value, mem_buf_, sizeof(value));
			mem_buf_ += sizeof(value);

			if (convert_)
			{
			convert_endian_32(&value);
			}
			
			return value;
		}

		int get_int()
		{
			int value;
			memcpy(&value, mem_buf_, sizeof(value));
			mem_buf_ += sizeof(value);
			if (convert_)
			{
				convert_endian_32(&value);
			}

			return value;
		}

		float get_float()
		{
			float value;
			memcpy(&value, mem_buf_, sizeof(value));
			mem_buf_ += sizeof(value);
			if (convert_)
			{
				convert_endian_32(&value);
			}

			return value;
		}

		const char* get_leftbuf()
		{
			return mem_buf_;
		}

	private:
		const char* mem_buf_;
		bool convert_;
	};
};

#endif /*__ENDIAN_OP_H__ */