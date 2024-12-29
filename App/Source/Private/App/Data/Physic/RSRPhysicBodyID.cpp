#include "App/Data/Physic/RSRPhysicBodyID.h"

using namespace RSRush;

RSRPhysicBodyID RSRPBIDProvider::s_nextPhysicBodyID = 1;

RSRPhysicBodyID RSRPBIDProvider::NewBodyID()
{
    RSRPhysicBodyID returnValue = s_nextPhysicBodyID;
    ++s_nextPhysicBodyID;
    return returnValue;
}
