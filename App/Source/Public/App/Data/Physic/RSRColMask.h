#pragma once

#include "App/Libs/WinInclude.h"
//#include <DirectXCollision.h>
//UINT64_MAX
namespace RSRush
{

	// enum class MyEnum : unsigned int {
	// 	None = 0,
	// 	Flag1 = 1 << 0,
	// 	Flag2 = 1 << 1,
	// 	Flag3 = 1 << 2,
	// 	// add more flags as needed
	// };
	// 
	// // Enable bitwise operators for MyEnum
	// DEFINE_ENUM_FLAG_OPERATORS(MyEnum);


	enum class ERSRColMask : UINT64
	{
		NONE				= 0,			// 0000 0000	0x00		000  
		WORLD_STATIC		= 1 << 0,		// 0000 0001	0x01		001
		WORLD_DYNAMIC		= 1 << 1,		// 0000 0010	0x02		002
		PAWN				= 1 << 2,		// 0000 0100	0x04		004
		HAZARD				= 1 << 3,		// 0000 1000	0x08		008
		PAWN_PATH			= 1 << 4,		// 0001 0000	0x10		016
		HAZARD_PLACEMENT	= 1 << 5,		// 0010 0000	0x20		032
		/*	EXEMPLE_7 = 1 << 6,			// 0100 0000	0x40		064
			EXEMPLE_8 =		1 << 7			// 1000 0000	0x80		128			*/
		ALL					= UINT64_MAX	// 1111 1111	0xFFFFFFFFFFFFFFFF
	};
	DEFINE_ENUM_FLAG_OPERATORS(RSRush::ERSRColMask);

	struct RSRColBehavior
	{
	public :
		ERSRColMask SelfMask		= ERSRColMask::NONE;
		ERSRColMask InteractMask	= ERSRColMask::ALL;
		ERSRColMask CollideMask		= ERSRColMask::ALL;
	public:
		inline bool Interact(const RSRColBehavior& InOther) const
		{
			return
				((UINT64)this->SelfMask & (UINT64)InOther.InteractMask) != 0
				&&
				((UINT64)this->InteractMask & (UINT64)InOther.SelfMask) != 0;
		}
		inline bool InteractWithMask(const ERSRColMask& InOtherSelf) const
		{
			return ((UINT64)this->InteractMask & (UINT64)InOtherSelf) != 0;
		}
		inline bool Collide(const RSRColBehavior& InOther) const
		{
			return
				((UINT64)this->SelfMask & (UINT64)InOther.CollideMask) != 0
				&&
				((UINT64)this->CollideMask & (UINT64)InOther.SelfMask) != 0;
		}
		inline bool CollideWithMask(const ERSRColMask& InOtherSelf) const
		{
			return ((UINT64)this->CollideMask & (UINT64)InOtherSelf) != 0;
		}
	};

	constexpr RSRColBehavior PLAYER =
	{
		.SelfMask = ERSRColMask::PAWN,
		.InteractMask = ERSRColMask::ALL,
		.CollideMask = ERSRColMask::NONE
	};

	constexpr RSRColBehavior WORLD_STATIC =
	{
		.SelfMask		= ERSRColMask::WORLD_STATIC,
		.InteractMask	= ERSRColMask::ALL ^ (ERSRColMask::WORLD_STATIC/* | OTHERS*/),
		.CollideMask	= ERSRColMask::ALL ^ (ERSRColMask::WORLD_STATIC/* | OTHERS*/)
	};

	constexpr RSRColBehavior WORLD_DYNAMIC =
	{
		.SelfMask		= ERSRColMask::WORLD_DYNAMIC,
		.InteractMask	= ERSRColMask::ALL,
		.CollideMask	= ERSRColMask::ALL
	};

	constexpr RSRColBehavior TEST_HAZARD_PLACEMENT =
	{
		.SelfMask		= ERSRColMask::HAZARD_PLACEMENT,
		.InteractMask	= ERSRColMask::PAWN_PATH,
		.CollideMask	= ERSRColMask::NONE
	};

	constexpr RSRColBehavior PAWN_PATH =
	{
		.SelfMask		= ERSRColMask::PAWN_PATH,
		.InteractMask	= ERSRColMask::HAZARD_PLACEMENT,
		.CollideMask	= ERSRColMask::NONE
	};
#if DEBUG_PHYSIC
	constexpr ERSRColMask DEBUG_PHYSIC_MASK =
		ERSRColMask::NONE
#if DEBUG_PHYSIC_WORLD_STATIC
		| ERSRColMask::WORLD_STATIC
#endif
#if DEBUG_PHYSIC_WORLD_DYNAMIC
		| ERSRColMask::WORLD_DYNAMIC
#endif
#if DEBUG_PHYSIC_PAWN
		| ERSRColMask::PAWN
#endif
#if DEBUG_PHYSIC_HAZARD
		| ERSRColMask::HAZARD
#endif
#if DEBUG_PHYSIC_PAWN_PATH
		| ERSRColMask::PAWN_PATH
#endif
#if DEBUG_PHYSIC_HAZARD_PLACEMENT
		| ERSRColMask::HAZARD_PLACEMENT
#endif
		;
#endif
};