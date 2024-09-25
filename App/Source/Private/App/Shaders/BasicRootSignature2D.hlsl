/*
* Root Signatures Layout (CPU)
* - 0: float3           "Color"
* - 1: Correction       "Correction Data"
* - 2: descriptor table "Textures"
*
* Root Signatures Layout (GPU)
* - b0:     float3     "Color"
* - b1:     Correction  "Correction Data"
* - t0...   Texture2D<float4> "Textures"
* 
* --- Statics ---
* - s0      Sampler for "Textures"
*/

#define ROOTSIG2D \
"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
"RootConstants(num32BitConstants=12, b0)," \
"RootConstants(num32BitConstants=4, b1)," \
"DescriptorTable(" \
    "SRV(t0, space = 0, numDescriptors = unbounded, flags = DESCRIPTORS_VOLATILE)" \
")," \
"StaticSampler(s0)"