#pragma once

#ifdef COORD_FRU
//DX Float3 Position
#define XFP3(FRW, RHT, UP) DirectX::XMFLOAT3(FRW, RHT, UP)
//DX Float3 Position Forward Right Up
#define XFP3FRU(FRW, RHT, UP) DirectX::XMFLOAT3(FRW, RHT, UP)
//DX Float3 Position Right Up Forward
#define XFP3RUF(RHT, UP, FRW) DirectX::XMFLOAT3(FRW, RHT, UP)

//DX Float3 Rotation
#define XFR3(FRW, RHT, UP) DirectX::XMFLOAT3(FRW, RHT, UP)
//DX Float3 Rotation Forward Right Up
#define XFR3FRU(FRW, RHT, UP) DirectX::XMFLOAT3(FRW, RHT, UP)
//DX Float3 Rotation Right Up Forward
#define XFR3RUF(RHT, UP, FRW) DirectX::XMFLOAT3(FRW, RHT, UP)

//DX Float3 Scale
#define XFS3(FRW, RHT, UP) DirectX::XMFLOAT3(FRW, RHT, UP)
//DX Float3 Scale Forward Right Up
#define XFS3FRU(FRW, RHT, UP) DirectX::XMFLOAT3(FRW, RHT, UP)
//DX Float3 Scale Right Up Forward
#define XFS3RUF(RHT, UP, FRW) DirectX::XMFLOAT3(FRW, RHT, UP)

#elif COORD_RUF
//DX Float3 Position
#define XFP3(RHT, UP, FRW) DirectX::XMFLOAT3(RHT, UP, FRW)
//DX Float3 Position Forward Right Up
#define XFP3FRU(FRW, RHT, UP) DirectX::XMFLOAT3(RHT, UP, FRW)
//DX Float3 Position Right Up Forward
#define XFP3RUF(RHT, UP, FRW) DirectX::XMFLOAT3(RHT, UP, FRW)

//DX Float3 Rotation
#define XFR3(RHT, UP, FRW) DirectX::XMFLOAT3(RHT, UP, FRW)
//DX Float3 Rotation Forward Right Up
#define XFR3FRU(FRW, RHT, UP) DirectX::XMFLOAT3(RHT, UP, FRW)
//DX Float3 Rotation Right Up Forward
#define XFR3RUF(RHT, UP, FRW) DirectX::XMFLOAT3(RHT, UP, FRW)

//DX Float3 Scale
#define XFS3(RHT, UP, FRW) DirectX::XMFLOAT3(RHT, UP, FRW)
//DX Float3 Scale Forward Right Up
#define XFS3FRU(FRW, RHT, UP) DirectX::XMFLOAT3(RHT, UP, FRW)
//DX Float3 Scale Right Up Forward
#define XFS3RUF(RHT, UP, FRW) DirectX::XMFLOAT3(RHT, UP, FRW)
#endif // COORD_FRU