#include "MDS/Tools/RResourceStateType.h"
#include <iostream>

const std::string UNKNOWN_NAME = "Unknown";
const std::string NEED_UPLOAD_NAME = "NeedUpload";
const std::string WAITING_UPLOAD_NAME = "WaitingUpload";
const std::string RESOURCE_LIVE_NAME = "ResourceLive";
const std::string& mds::RResourceStateType_Func::ToString(const mds::RResourceStateType InType)
{
	
	switch (InType)
	{
	case mds::RResourceStateType::Unknown :
		return UNKNOWN_NAME;
	case mds::RResourceStateType::NeedUpload:
		return NEED_UPLOAD_NAME;
	case mds::RResourceStateType::WaitingUpload:
		return WAITING_UPLOAD_NAME;
	case mds::RResourceStateType::ResourceLive:
		return RESOURCE_LIVE_NAME;
	default:
		std::cout << "[MDSError] Not implemented default case in switch RResourceStateType !" << std::endl;
		return UNKNOWN_NAME;
	}
}