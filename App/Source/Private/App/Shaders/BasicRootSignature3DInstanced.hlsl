/*
* Root Signatures Layout (CPU)
* - 0: CBV                  "ModelViewprojectionConstants"
* - 1: descriptor table     "Textures"
*
* Root Signatures Layout (GPU)                              DWORD Size : 30/64
* - b0:     VSConstants         "VSConstants"               [00 -> 28]
* - t0...   Texture2D<float4>   "Texture3D"                 [28 -> 29]
* - t0...   Texture2D<float4>   "Texture2D"                 [29 -> 30]
* 
* --- Statics ---
* - s0      Sampler for "Texture3D"
* - s1      Sampler for "Texture2D"
*/

#define ROOTSIG3DINSTANCED \
"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
"RootConstants(num32BitConstants=30, b0)," \
"DescriptorTable(" \
    "SRV(t0, space = 0, numDescriptors = unbounded, flags = DESCRIPTORS_VOLATILE)" \
")," \
"DescriptorTable(" \
    "SRV(t0, space = 1, numDescriptors = unbounded, flags = DESCRIPTORS_VOLATILE)" \
")," \
"StaticSampler(s0)," \
"StaticSampler(s1)"