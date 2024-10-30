#include "App/Physic/LD/RSRLDReader.h"
#include "App/Physic/Demo01/SObject/RSRPD1SphereSObject.h"
#include "App/Physic/Demo01/SObject/RSRPD1StaticSObject.h"
#include "App/SceneObject/RSRSObject.h"
#include "App/SceneObject/RSROScene.h"
#include "App/Tools/RSRLog.h"
using namespace RSRush;
using namespace DirectX;

static std::shared_ptr<RSRSObject> CreateobjectFromLDElem(const RSRLDElem& InLDElem);

bool RSRush::RSRLDReader::FillLDDatas(const std::vector<RSRLDElem>& InLDElems, RSROScene* InObjectScene)
{
	if (!InObjectScene)
	{
		RSRLog::Log(LOG_ERROR, TEXT("No valid pointer to RSRObjectScene given to 'RSRLDReader'"));
		return false;
	}
	bool bAllSucessfull = true;
	for (const RSRLDElem& LDElem : InLDElems)
	{
		if (std::shared_ptr<RSRSObject> NewObject = CreateobjectFromLDElem(LDElem))
		{
			InObjectScene->AddNewSObject(std::move(NewObject));
			
		}
		else
		{
			bAllSucessfull = false;
		}
	}
	return bAllSucessfull;
}

std::shared_ptr<RSRSObject> CreateobjectFromLDElem(const RSRLDElem& InLDElem)
{
	switch (InLDElem.Type)	
	{
	case RSRLDType::Square:
		return std::make_shared<RSRPD1StaticSObject>(std::move(INIT_INSTANCE_DATA(6)), InLDElem.Transform, WORLD_STATIC, false);
	case RSRLDType::Sphere:
		return std::make_shared<RSRPD1SphereSObject>(std::move(INIT_INSTANCE_DATA(0)), InLDElem.Transform, WORLD_STATIC, false);
	default:
		RSRLog::Log(LOG_ERROR, TEXT("Type '{}' Unhandled in switch for 'CreateobjectFromLDElem' in 'RSRLDReader.cpp' file !"));
		break;
	}
	return std::shared_ptr<RSRSObject>();
}
