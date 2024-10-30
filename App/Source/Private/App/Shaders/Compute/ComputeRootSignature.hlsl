/*
* Root Signatures Layout (CPU)
* - 0: CBV                  "Datas"
* - 1: descriptor table     "TextureBuffers"
*
* Root Signatures Layout (GPU)                              DWORD Size : 62/64
* - b0:     VSConstants         "VSConstants"               [00 -> 60]
* - t0...   Texture2D<float4>   "Texture3D"                 [60 -> 61]
* - t0...   Texture2D<float4>   "Texture2D"                 [61 -> 62]
* 
* --- Statics ---
* - s0      Sampler for "Texture3D"
* - s1      Sampler for "Texture2D"
*/

#define ROOTSIGCOMPUTE \
"DescriptorTable(" \
    "UAV(u0, space = 0, numDescriptors = unbounded, flags = DESCRIPTORS_VOLATILE)" \
")"