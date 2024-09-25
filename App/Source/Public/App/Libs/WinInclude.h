#pragma once

//Include collection from Lötwig Fusel's "Project Setup & ComPointer | D3D12 Beginners Tutorial [D3D12Ez]"
//https://youtu.be/rF30qoUFDq8?si=2-gFm6AVt39a3hJZ&t=606

#define NOMINMAX
#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#ifdef _DEBUG
#include <d3d12sdklayers.h>
#include <dxgidebug.h>
#endif // _DEBUG

#include <wincodec.h>

#include "App/Game/RSRPaths.h"


//Do while false is to do code one on a macro
#define _RF_FAILED(expr) do {if (FAILED(expr)) { return false; } } while (false)
#define _RF_FALSE(expr) do {if (!expr) { return false; } } while (false)

#define _R_FAILED(expr) do {if (FAILED(expr)) { return; } } while (false)
#define _R_FALSE(expr) do {if (!expr) { return; } } while (false)

#define _RF_FAILED_LOG(expr, log) do {if (FAILED(expr)) { log; return false; } } while (false)