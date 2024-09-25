/*
* Root Signatures Layout (CPU)
* - 0: ContextCBV       "ContextConstant"
* - 1: MeshCBV          "ModelConstant"
*
* Root Signatures Layout (GPU)
* - b0:     ContextCBV  "ContextConstant"
* - b1:     MeshCBV     "ModelConstant"
*/

#define ROOTSIGDP \
"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
"RootConstants(num32BitConstants=32, b0)"