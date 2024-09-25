#pragma once
#include "App/Physic/LD/RSRLDElem.h"
#include <vector>

namespace RSRush
{
	class RSROScene;
	class RSRLDReader
	{
	public:
		STATIC_CLASS_R5(RSRLDReader);
	public:
		static bool FillLDDatas(const std::vector<RSRLDElem>& InLDElems, RSROScene* InObjectScene);
	};
};