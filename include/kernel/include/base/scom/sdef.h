#ifndef __SDEF_INCLUDED
#define __SDEF_INCLUDED

#include <cstddef>

#define _SCOM_BEGIN		namespace scom {
#define _SCOM_END		}


_SCOM_BEGIN

struct null_def_type
	{
	static const char* signature() { return NULL; };
	};

template <typename Ty>
	struct shptr_def
	{
	static const char* signature();	// NOT IMPLEMENT THIS METHOD BY DESIGN.
	};

struct unique { static const int objnum = 1; };
struct multiple { static const int objnum = 10; };

template <typename sit>
	class singleton
	{
public:
	static sit& instance()
		{
		static sit sit_obj;
		return sit_obj;
		}
	};

struct i_shdll_mod
	{
	virtual const char*		name() const = 0;
	virtual const char*		domain() const = 0;
	virtual i_shdll_mod*	prev_mod() const = 0;
	virtual i_shdll_mod*	next_mod() const = 0;
	virtual void*			find_pointer(const char* pname, bool query_api) const = 0;

	virtual bool			set_handle(void* h) = 0;
	virtual bool			on_unload() = 0;

	// set/unset pointer to pointer which is be referenced by other module.
	virtual bool			set_refptr(i_shdll_mod* pmod, void** pptr) = 0;
	virtual bool			unset_refptr(i_shdll_mod* pmod, void** pptr= NULL) = 0;

	// set current module successor as sd_mod;
	virtual void 			prev_mod(i_shdll_mod* sd_mod) = 0;
	virtual void 			next_mod(i_shdll_mod* sd_mod) = 0;
	virtual	~i_shdll_mod() {};
	};

_SCOM_END

struct module_of_unknown {
	static const char* signature() {
		return "roamer";
		}
	};

#ifdef BOOST_ASSERT
# define SCOM_ASSERT BOOST_ASSERT
#elif defined(SP_ASSERT)
# define SCOM_ASSERT SP_ASSERT
#else
#include <assert.h>
# define SCOM_ASSERT assert
#endif

// type 接口名或类名或api名, name 字符串型, 类型名称.
#define DEFINE_SHPTR(type, name) 									\
	namespace scom {												\
	template<> struct shptr_def<type> {								\
		static const char* signature() { return name; } };  }

#define DEFINE_SHIMP(ty, ty_impl, poly)									\
	namespace scom {													\
	template<> struct shptr_impl<ty> {									\
		static ty*	get_instance() { return &(ty_impl::instance()); };	\
		static const int objnum = poly::objnum; }; }

#define DEFINE_SHMOD(mod)											\
	struct module_of_##mod {										\
		static const char* signature() {							\
				return #mod;	}}

#define DEFINE_SHMOD_DLL(mod)				\
	DEFINE_SHMOD(mod);						\
	typedef module_of_##mod module_of_scom;

#endif // __SDEF_INCLUDED
