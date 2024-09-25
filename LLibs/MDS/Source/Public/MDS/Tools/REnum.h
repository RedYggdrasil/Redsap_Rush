#pragma once
#include "MDS/RTool.h"

//WIP on Variedic macro
//#define RENUM_MIN1(NAME) NAME, MIN = NAME
//#define RENUM_MIN2(NAME, VALUE) NAME = VALUE, MIN = NAME
//
//#define GET_MACRO(_1,_2,NAME,...) NAME
//#define FOO(...) GET_MACRO(__VA_ARGS__, RENUM_MIN1, RENUM_MIN2)(__VA_ARGS__)


#define RENUM_MIN(NAME) NAME, MIN = NAME
#define RENUM_MAX(NAME) NAME, MAX = NAME 
//Special case when the enum has only one value
#define RENUM_MIN_MAX(NAME) NAME, MIN = NAME, MAX = NAME

#define RENUM_MIN_VAL(NAME, VALUE) NAME = VALUE, MIN = NAME
#define RENUM_MAX_VAL(NAME, VALUE) NAME = VALUE, MAX = NAME 
//Special case when the enum has only one value
#define RENUM_MIN_MAX_VAL(NAME, VALUE) NAME = VALUE, MIN = NAME, MAX = NAME

#define RENUM_COUNT COUNT, NONE = COUNT
#define RENUM_COUNT_VAL(VALUE) COUNT = VALUE, NONE = COUNT


#define RS_DEFINE_ENUM(EnumClass) \
inline constexpr bool operator<(EnumClass lhs, EnumClass rhs) \
{ \
	return (mds::UT_cast(lhs) < mds::UT_cast(rhs)); \
} \
inline constexpr bool operator<=(EnumClass lhs, EnumClass rhs) \
{ \
	return (mds::UT_cast(lhs) <= mds::UT_cast(rhs)); \
} \
inline constexpr bool operator>(EnumClass lhs, EnumClass rhs) \
{ \
	return (mds::UT_cast(lhs) > mds::UT_cast(rhs)); \
} \
inline constexpr bool operator>=(EnumClass lhs, EnumClass rhs) \
{ \
	return (mds::UT_cast(lhs) >= mds::UT_cast(rhs)); \
}  \
inline constexpr EnumClass REnumMin(EnumClass lhs, EnumClass rhs) \
{ \
	return static_cast<EnumClass>((mds::UT_cast(lhs) < mds::UT_cast(rhs))? lhs : rhs); \
}  \
inline constexpr EnumClass REnumMax(EnumClass lhs, EnumClass rhs) \
{ \
	return static_cast<EnumClass>((mds::UT_cast(lhs) > mds::UT_cast(rhs))? lhs : rhs); \
} 

#define RS_DEFINE_ENUM_FLAG(EnumClass) \
inline constexpr EnumClass operator&(EnumClass lhs, EnumClass rhs) \
{ \
	return static_cast<EnumClass>(mds::UT_cast(lhs) & mds::UT_cast(rhs)); \
}\
inline constexpr EnumClass operator|(EnumClass lhs, EnumClass rhs) \
{ \
	return static_cast<EnumClass>(mds::UT_cast(lhs) | mds::UT_cast(rhs)); \
}\
inline constexpr EnumClass operator^(EnumClass lhs, EnumClass rhs) \
{ \
	return static_cast<EnumClass>(mds::UT_cast(lhs) ^ mds::UT_cast(rhs)); \
}\
inline constexpr bool any(EnumClass EInFlag) \
{ \
	return (mds::UT_cast(EInFlag)  != (std::underlying_type<EnumClass>::type)0); \
}