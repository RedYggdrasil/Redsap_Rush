#include <iostream>

#include "App/Game/RSRProgramInstance.h"

#include "MDS/Hello.h"
#include "MDS/Tools/RCoordinate.h"
#include "MDS/Tools/RMath.h"
#include "MDS/Tools/RLog.h"
#include "mds/Tools/System/RTimekeeper.h"

#include "Gen_App/Config/AppConfig.h"
#include "App/Libs/WinInclude.h"
#include "App/Tools/ComPointer.hpp"
#include "App/Tools/DebugLayer.h"
#include "App/Tools/ImageLoader.h"
#include "App/Tools/RSRLog.h"

#include "App/Data/RSRCameraData.h"
#include "App/Data/Meshes/RSRMesh3D.h"
#include "App/Data/Meshes/RSRMesh2D.h"
#include "App/Data/Textures/RSRTexture2D.h"



#include "App/Gameplay/PlayerController.h"
#include "App/Gameplay/GameManager.h"


#include "App/Managers/RSRTrenchManager.h"

#include "App/Data/Shaders/ShaderStructures.h"



#include "App/TheTrench/RSRTTScene.h"
#include <GameInput.h>
//#include <DirectXTex.h>
//#include <WICTextureLoader.h>

#include "Tracy.hpp"
#include "TracyC.h"

using Microsoft::WRL::ComPtr;



//Rendeder is : 
// X : Forward
// Y : Right
// Z : Up

//DirectX default is
// X : Right
// Y : Up
// Z : Forward

int main(int argc, char** argv) 
{
    RSRush::RSRProgramInstance program; 
    program.Run(argc, argv);
    return 0;
}