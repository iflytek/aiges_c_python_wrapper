/**
* @file	autobuf.h
* @brief	能够自动伸展的buffer
*
*  缺省可以使用栈内存，不够则使用堆内存，可以自动释放
*
* @author	jdyu
* @version	1.0
* @date	2008/4/28
*
* @see
*
* <b>History:</b><br>
* <table>
*  <tr> <th>Version	<th>Date		<th>Author	<th>Notes</tr>
*  <tr> <td>1.0		<td>2008/4/28	<td>jdyu	<td>Create this file</tr>
* </table>
*
*/
#ifndef __AUTOBUF_H__
#define __AUTOBUF_H__

#include <map>
#include <vector>
#include <algorithm>
#include "iFly_Assert.h"

#define USE_TBB_MALLOC
#ifdef USE_TBB_MALLOC
#include "tbb/scalable_allocator.h"
using namespace tbb;
#endif

#ifdef WIN32
#define ALIGN16 __declspec(align(16))
#else
#define ALIGN16 __attribute__((aligned(16)))
#endif
namespace sp {
	// 缺省尺寸
	const unsigned int ab_default_size = 4096;
	//const unsigned int ab_default_size = 8*60*8000*4;  // 缺省水位为5分钟8KHz语音数据大小
	// 缺省水位
	const unsigned int ab_default_high = 2*1024*1024; //10分钟的预警水位
	// 检查是否越界的看守
	const char	ab_buf_guard[] = "AutoBufferGuard";

	/**
	* @class	auto_buf
	* @brief	自动管理的buffer类
	*
	*  可以和下面的管理器配合使用
	*
	* @author	jdyu
	* @date	2008/4/29
	*/
	template <unsigned int SIZE = ab_default_size, typename TYPE = char, unsigned int HIGH = ab_default_high>
	class auto_buf
	{
	public:
		auto_buf(size_t size = 0, TYPE * data = 0)
			: buf_real_		(0)
			, buf_size_		(0)
			, buf_cap_		(0)
			, high_water_	(0)
		{
			//memset(buf_def_, 0, sizeof(buf_def_));  // 为节省时间这里先不做memset, jswang 2013/09/16
			buf_real_ = buf_def_;
			buf_cap_  = sizeof buf_def_ / sizeof buf_def_[0];
			//buf_cap_  = sizeof buf_def_;   // jswang 2013/09/18
			high_water_ = HIGH;
			strcpy(buf_guard_, ab_buf_guard);
			resize(size);
			if ( data != 0 )
				memcpy(buf_real_, data, size * sizeof(TYPE));
		}
		~auto_buf(void)
		{
			wipe();
		}
		auto_buf(const auto_buf & that)
		{
			this->set(that.data(), that.size());
		}
		auto_buf & operator = (const auto_buf & that)
		{
			this->set(that.data(), that.size());
			return *this;
		}

		TYPE * data(void)
		{
			return buf_real_;
		}
		const TYPE * data(void) const
		{
			return buf_real_;
		}
		unsigned char * byte_data(void)
		{
			return (unsigned char*) data();
		}
		const char * str_data(void)
		{
			return (const char*) data();
		}
		short * short_data(void)
		{
			return (short*) data();
		}
		template <typename T>
		T * t_data(void)
		{
			return (T*) data();
		}

		size_t size(void) const
		{
			return buf_size_;
		}
		size_t capacity(void) const
		{
			return buf_cap_;
		}
		size_t size_bytes(void) const
		{
			return buf_size_ * sizeof(TYPE);
		}
		size_t capacity_bytes(void) const
		{
			return buf_cap_ * sizeof(TYPE);
		}
		bool empty(void) const
		{
			return buf_size_ == 0;
		}

		size_t high_water(void) const
		{
			return high_water_;
		}
		size_t high_water(size_t hw)
		{
			size_t lhw = high_water_;
			if ( hw > ab_default_size )
				high_water_ = hw;
			return lhw;
		}

		int alloc(size_t cap)
		{
			resize(cap);
			return 0;
		}
		int free(bool wipe = false)
		{
			wipe ? this->wipe()	: resize(0);
			return 0;
		}
		void reset(void)
		{
			resize(0);
		}

		void resize(size_t new_size)
		{
			realloc(new_size);
			buf_size_ = new_size;
		}
		void realloc(size_t new_cap)
		{
			if ( high_water_ > 0 && capacity_bytes() > high_water_ && new_cap == 0 )
				wipe();
			if ( new_cap > buf_cap_ )
			{
				TYPE * obuf = buf_real_;
				size_t new_size = new_cap * sizeof(TYPE) + sizeof ab_buf_guard;
#ifdef USE_TBB_MALLOC
				//采用TBB对齐内存
				buf_real_ = (TYPE*)scalable_aligned_malloc(new_size, 16);
#else           //采用对齐分配内存方式
				buf_real_ = (TYPE*) _aligned_malloc(new_size, 16);
#endif
				if (buf_real_)
				{
					//memset(buf_real_, 0, new_size);  // 为节省时间这里先不做memset, jswang 2013/09/16
					buf_cap_ = new_cap;
					memcpy(buf_real_ + new_cap, ab_buf_guard, sizeof ab_buf_guard);
					if ( buf_size_ > 0 )
					{
                        // modified by bdwei@2011-9-16 19:11:29， big BUG
                        memcpy(buf_real_, obuf, buf_size_ * sizeof(TYPE));
						// memcpy(buf_real_, obuf, buf_size_);
					}

					if ( obuf != buf_def_ )
					{
#ifdef USE_TBB_MALLOC
						scalable_aligned_free(obuf);
#else
						_aligned_free(obuf);
#endif
					}
				}
				else
				{
					SP_ASSERT(buf_real_);
                    //srlog_error("auto_buf::realloc is failed, the size is %d!", new_size);
				}
			}
		}
		void wipe(void)
		{
			SP_ASSERT(memcmp(buf_real_ + buf_cap_, ab_buf_guard, sizeof ab_buf_guard) == 0);
			if ( buf_real_ != buf_def_ )
			{
#ifdef USE_TBB_MALLOC
				scalable_aligned_free(buf_real_);
#else
				_aligned_free(buf_real_);
#endif
			}
			buf_real_ = buf_def_;
			buf_cap_  = sizeof buf_def_ / sizeof buf_def_[0];
			buf_size_ = 0;

            //srlog_dbg("auto_buf::wipe | release buf, size=%d", buf_cap_);
		}

		void set(const TYPE * buf, size_t count)
		{
			if ( count > buf_cap_ )
				resize(count);
			memcpy(buf_real_, buf, count * sizeof(TYPE));
			buf_size_ = count;
		}
		int get(TYPE * buf, size_t & count) const
		{
			int ret = 0;
			memcpy(buf, buf_real_, std::min(count, buf_size_) * sizeof(TYPE));
			if ( count < buf_size_ )
				ret = -1;
			count = std::min(count, buf_size_);
			return 0;
		}
		size_t append(const TYPE * data, size_t count)
		{
			if ( count + buf_size_ > buf_cap_ )
				realloc(count + buf_size_);
			memcpy(this->data() + buf_size_, data, count * sizeof(TYPE));
			buf_size_ += count;
			return buf_size_;
		}
		size_t fill(const TYPE & dat, size_t start = 0, size_t end = 0)
		{
			if ( end == 0 || end > buf_size_ )
				end = buf_size_;
			std::fill(buf_real_ + start, buf_real_ + buf_size_, dat);
			return 0;
		}

		TYPE & at(size_t index)
		{
			SP_ASSERT(index < buf_size_);
			return buf_real_[index];
		}
		TYPE & operator [] (size_t index)
		{
			return at(index);
		}

	private:
		// 缺省的栈缓冲区
#ifdef USE_TBB_MALLOC
		ALIGN16 TYPE buf_def_[SIZE];
#else
		ALIGN16 TYPE buf_def_[SIZE];
#endif
		char		buf_guard_[sizeof ab_buf_guard];
		TYPE *		buf_real_;
		size_t		buf_size_;
		size_t		buf_cap_;
		// 最高的水位，如果尺寸大于这个值，那么每次resize的时候就会释放之
		size_t		high_water_;
	};

	/**
	* @class	mutex_null
	* @brief	空锁
	*
	*  达到虚锁定的功能
	*
	* @author	jdyu
	* @date	2008/4/29
	*/
	class mutex_null
	{
	public:
		int aquire(void)
		{
			return 0;
		}
		int release(void)
		{
			return 0;
		}

		int lock()
		{
			return 0;
		}

		int unlock()
		{
			return 0;
		}
	};

	/**
	* @class	mutex_guard
	* @brief	自动锁定和解锁
	* @author	jdyu
	* @date	2008/4/29
	*/
	template<class MUTEX>
	class mutex_guard
	{
	public:
		mutex_guard(MUTEX & lock)
			: lock_	(&lock)
		{
			lock.acquire();
		}
		mutex_guard(MUTEX * lock)
			: lock_	(lock)
		{
			if ( lock_ )
				lock_->acquire();
		}
		~mutex_guard(void)
		{
			if ( lock_ )
				lock_->release();
		}

	private:
		// the lock
		MUTEX * lock_;
	};

	/**
	* @class	auto_buf_mngr
	* @brief	自动内存管理器
	*
	*  管理多个buffer，提供内存池的功能
	*
	* @author	jdyu
	* @date	2008/4/29
	*
	* @see		auto_buf, mutex_null
	*/
	template<class ATBUF = auto_buf<>,
	class MUTEX = mutex_null>
	class auto_buf_mngr
	{
	public:
		auto_buf_mngr(void)
			: high_water_	(0)
			, last_free_	(-1)
		{
		}
		auto_buf_mngr(size_t count, size_t cap)
			: high_water_	(0)
			, last_free_	(-1)
		{
			cap_ = cap;
			high_water_ = cap * count;
			//开始不真正分配内存
			reserve(count, 0);
		}

		void init(size_t count, size_t cap)
		{
			cap_ = cap;
			high_water_ = cap * count;
			reserve(count, 0);
		}

		~auto_buf_mngr(void)
		{
			clear();
		}

		// 分配一个指定大小的缓冲区
		ATBUF * alloc(size_t cap = 0)
		{
			ATBUF * buf = 0;
			GUARD mon(bufs_lock_);

			if ( cap == 0 )
				cap = cap_;

			// check last free
			if ( last_free_ != -1 )
			{
				ab_assist & as = auto_bufs_[last_free_];
				if ( as.buffer()->capacity() >= cap )
				{
					buf = as.alloc(cap);
					last_free_ = -1;
				}
			}

			size_t first_idle = -1;
			for ( size_t i = 0; i < auto_bufs_.size(); i++ )
			{
				ab_assist & as = auto_bufs_[i];
				if ( !as.busy() )
				{
					if ( first_idle == -1 )
						first_idle = i;

					if ( buf != 0 && last_free_ == -1 )
					{
						last_free_ = i;
						break;
					}

					if ( buf == 0 && as.buffer()->capacity() >= cap )
					{
						buf = as.alloc(cap);
						if ( last_free_ != -1 )
							break;
						else if ( i < auto_bufs_.size() - 1 )
							last_free_ = i + 1;
					}
				}
			}

			if ( buf == 0 && first_idle != -1 )
				buf = auto_bufs_[first_idle].alloc(cap);

			if ( buf == 0 )
			{
				auto_bufs_.push_back(ab_assist(cap, true));
				buf = auto_bufs_.back().buffer();
				bufs_map_.insert(std::make_pair(buf, auto_bufs_.size() - 1));
			}

			data_map_.insert(std::make_pair(buf->byte_data(), buf));
			return buf;
		}
		int free(unsigned char * data)
		{
			GUARD gd(bufs_lock_);
			typename data_map::iterator di = data_map_.find(data);
			if ( di == data_map_.end() )
			{
				SP_ASSERT(0 && "cannot find the buffer to free!!!");
				return -1;
			}

			ATBUF * ab = di->second;
			data_map_.erase(ab->byte_data() );
			typename bufs_map::iterator pi = bufs_map_.find(ab);
			if ( pi == bufs_map_.end() )
			{
				SP_ASSERT(0 && "cannot find the buffer to free!!!");
				return -1;
			}

			size_t frid = pi->second;
			SP_ASSERT(auto_bufs_[frid].buffer() == ab);
			auto_bufs_[frid].free();
			last_free_ = frid;

			// 如果设置了水位，需要判断是否超过
			if ( high_water_ > 0 )
			{
				size_t total_size = 0;
				for ( size_t i = 0; i < auto_bufs_.size(); i++ )
					total_size += auto_bufs_[i].buffer()->capacity();
				if ( total_size > high_water_ )
					ab->wipe();
			}
			return 0;
			//return free(ab);
		}
		int free(ATBUF * ab)
		{
			GUARD gd(bufs_lock_);
			data_map_.erase(ab->byte_data() );
			typename bufs_map::iterator pi = bufs_map_.find(ab);
			if ( pi == bufs_map_.end() )
			{
				SP_ASSERT(0 && "cannot find the buffer to free!!!");
				return -1;
			}

			size_t frid = pi->second;
			SP_ASSERT(auto_bufs_[frid].buffer() == ab);
			auto_bufs_[frid].free();
			last_free_ = frid;

			// 如果设置了水位，需要判断是否超过
			if ( high_water_ > 0 )
			{
				size_t total_size = 0;
				for ( size_t i = 0; i < auto_bufs_.size(); i++ )
					total_size += auto_bufs_[i].buffer()->capacity();
				if ( total_size > high_water_ )
					ab->wipe();
			}

			return 0;
		}
		int free(void)
		{
			GUARD gd(bufs_lock_);
			std::for_each(auto_bufs_.begin(), auto_bufs_.end(), std::mem_fun_ref(&ATBUF::free));
			return 0;
		}
		int clear(void)
		{
			GUARD gd(bufs_lock_);
			for ( bufs_pi pi = auto_bufs_.begin(); pi != auto_bufs_.end(); pi++ )
			{
				ATBUF * buf = pi->buffer();
				buf->wipe();
				delete buf;
			}
			auto_bufs_.clear();
			bufs_map_.clear();
			data_map_.clear();
			return 0;
		}
		int reserve(size_t count, size_t cap)
		{
			GUARD gd(bufs_lock_);
			auto_bufs_.reserve(count);
			for ( size_t i = 0; i < count; i++ )
			{
				auto_bufs_.push_back(ab_assist(cap, false));
				bufs_map_.insert(
					std::make_pair(auto_bufs_.back().buffer(), auto_bufs_.size() - 1));
			}
			return 0;
		}
		int compact(void)
		{
			GUARD gd(bufs_lock_);
			bufs_pi pi = auto_bufs_.begin();
			for ( ; pi != auto_bufs_.end(); pi++ )
			{
				if ( !pi->busy() )
				{
					delete pi->buffer();
					delete *pi;
					pi = auto_bufs_.erase(pi);
				}
			}
			return 0;
		}

		size_t high_water(void) const
		{
			return high_water_;
		}
		size_t high_water(size_t hw)
		{
			size_t lhw = high_water_;
			if ( hw > ab_default_size )
				high_water_ = hw;
			return lhw;
		}

		// 支持单体访问
		static auto_buf_mngr & instance(void)
		{
			static auto_buf_mngr the_ab_mngr;
			return the_ab_mngr;
		}

	private:
		// 缓冲区辅助
		struct ab_assist
		{
			ab_assist(size_t cap, bool alloc)
			{
				abuf_ = new ATBUF(cap);
				busy_ = alloc;
			}
			bool busy(void) const
			{
				return busy_;
			}
			void busy(bool bs)
			{
				busy_ = bs;
			}
			ATBUF * buffer(void)
			{
				return abuf_;
			}

			ATBUF * alloc(size_t cap)
			{
				if (busy_)
					return NULL;

				SP_ASSERT(!busy_);
				busy_ = true;
				abuf_->alloc(cap);
				return abuf_;
			}
			void free(void)
			{
				busy_ = false;
				abuf_->resize(0);
			}

		private:
			ATBUF * abuf_;
			bool	busy_;
		};

		// 内存块容器
		typedef std::vector<ab_assist> bufs_list;
		typedef typename bufs_list::iterator bufs_pi;

		// Mutex Guard
		typedef mutex_guard<MUTEX>	GUARD;

		// 所有的内存块
		bufs_list               auto_bufs_;
		MUTEX                   bufs_lock_;
		// 最高的水位，如果尺寸大于这个值，那么每次free的时候就会彻底释放该缓冲区
		size_t                  high_water_;
		size_t                  cap_;

		// 为优化速度而添加
		size_t                  last_free_;
		// 映射表
		typedef std::map<ATBUF*, size_t> bufs_map;
		bufs_map                bufs_map_;
		typedef std::map<unsigned char*, ATBUF*> data_map;
		data_map                data_map_;
	};

} /* namespace sp */

#endif /*__AUTOBUF_H__*/


/******************************************************/
//修改记录
//	时  间：2014-12-23 14:46:23
//  修改人：顾雷雷
//  备	注：autobuf类，采用宏开关，定义使用TBB Malloc函数进行内存分配或者是对齐内存分配方式
/******************************************************/
