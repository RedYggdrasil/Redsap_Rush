#pragma once
#define NOMINMAX

#define R_VIRTUAL_IMPLICIT 
#define R_OVERRIDE_IMPLICIT 
#define R_PURE = 0
#define R_PURE_OVERRIDE override

#define STATIC_CLASS_R5(ClassName)\
public:\
ClassName() = delete;\
ClassName(const ClassName& _) = delete;\
ClassName& operator=(const ClassName& other) = delete;\
ClassName(ClassName&& other) noexcept = delete;\
ClassName& operator=(ClassName&& other) noexcept = delete;

#define STATIC_STRUCT_R5(StructName) STATIC_CLASS_R5(StructName)