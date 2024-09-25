
set(MDS_NAME "MDS")

set (MDS_PUBLIC_SRC "Source/Public/${MDS_NAME}/")
set (MDS_PRIVTE_SRC "Source/Private/${MDS_NAME}/")


# Header file are not compiled into object (or rather are just copied into objects on include), so their inclusion into the project is optional
# Only their 'target_include_directories' is needed to be usable as #include
# List of included sources
set(MDS_SOURCES
	${MDS_PUBLIC_SRC}Hello.h						${MDS_PRIVTE_SRC}Hello.cpp
	${MDS_PUBLIC_SRC}RTool.h						${MDS_PRIVTE_SRC}RTool.cpp
	${MDS_PUBLIC_SRC}Tools/RTransform.h				${MDS_PRIVTE_SRC}Tools/RTransform.cpp
	${MDS_PUBLIC_SRC}Algorithm/RVoxelSurface.h		${MDS_PRIVTE_SRC}Algorithm/RVoxelSurface.cpp
	${MDS_PUBLIC_SRC}Algorithm/RGreeble.h			${MDS_PUBLIC_SRC}Algorithm/RGreebleImpls.h ${MDS_PRIVTE_SRC}Algorithm/RGreeble.cpp
	${MDS_PUBLIC_SRC}Algorithm/RGreedyMesh.h		${MDS_PRIVTE_SRC}Algorithm/RGreedyMesh.cpp
	${MDS_PUBLIC_SRC}Defines/MDS.h					${MDS_PRIVTE_SRC}Defines/MDS.cpp
	${MDS_PUBLIC_SRC}Threads/RTaskManager.h			${MDS_PRIVTE_SRC}Threads/RTaskManager.cpp
	${MDS_PUBLIC_SRC}Threads/RTask.h				${MDS_PRIVTE_SRC}Threads/RTask.cpp
	${MDS_PUBLIC_SRC}Threads/RTaskFunction.h		${MDS_PRIVTE_SRC}Threads/RTaskFunction.cpp
	${MDS_PUBLIC_SRC}Tools/REnum.h					${MDS_PRIVTE_SRC}Tools/REnum.cpp
	${MDS_PUBLIC_SRC}Tools/Templates/Singleton.h	${MDS_PRIVTE_SRC}Tools/Templates/Singleton.cpp
	${MDS_PUBLIC_SRC}Tools/RCollection.h			${MDS_PRIVTE_SRC}Tools/RCollection.cpp
	${MDS_PUBLIC_SRC}Tools/RMathCollection.h		${MDS_PRIVTE_SRC}Tools/RMathCollection.cpp
	${MDS_PUBLIC_SRC}Tools/RIterators.h				${MDS_PRIVTE_SRC}Tools/RIterators.cpp
	${MDS_PUBLIC_SRC}Tools/RLog.h					${MDS_PRIVTE_SRC}Tools/RLog.cpp
	${MDS_PUBLIC_SRC}Tools/RMath.h					${MDS_PRIVTE_SRC}Tools/RMath.cpp
	${MDS_PUBLIC_SRC}Tools/Math/RMath2D.h			${MDS_PRIVTE_SRC}Tools/Math/RMath2D.cpp
	${MDS_PUBLIC_SRC}Tools/RPin.h					${MDS_PRIVTE_SRC}Tools/RPin.cpp
	${MDS_PUBLIC_SRC}Tools/RResourceStateType.h		${MDS_PRIVTE_SRC}Tools/RResourceStateType.cpp
	
	${MDS_PUBLIC_SRC}Tools/System/RKeyCodes.h		${MDS_PRIVTE_SRC}Tools/System/RKeyCodes.cpp
	${MDS_PUBLIC_SRC}Tools/System/RInputKeys.h		${MDS_PRIVTE_SRC}Tools/System/RInputKeys.cpp
	${MDS_PUBLIC_SRC}Tools/System/RTimekeeper.h		${MDS_PRIVTE_SRC}Tools/System/RTimekeeper.cpp
	${MDS_PUBLIC_SRC}Tools/System/RRandomizer.h		${MDS_PRIVTE_SRC}Tools/System/RRandomizer.cpp
	
	${MDS_PUBLIC_SRC}Tools/Assets/RAssetType.h		${MDS_PRIVTE_SRC}Tools/Assets/RAssetType.cpp
	${MDS_PUBLIC_SRC}Tools/Assets/RAsset.h			${MDS_PRIVTE_SRC}Tools/Assets/RAsset.cpp
	
	${MDS_PUBLIC_SRC}Tools/Pins/RID3D12RPin.h		${MDS_PRIVTE_SRC}Tools/Pins/RID3D12RPin.cpp
	${MDS_PUBLIC_SRC}Tools/Pins/RAssetPin.h			${MDS_PRIVTE_SRC}Tools/Pins/RAssetPin.cpp
	${MDS_PUBLIC_SRC}Tools/RCoordinate.h			${MDS_PRIVTE_SRC}Tools/RCoordinate.cpp
	${MDS_PUBLIC_SRC}Tools/RAllocator.h				${MDS_PRIVTE_SRC}Tools/RAllocator.cpp
	
	${MDS_PUBLIC_SRC}DirectXTK12/SimpleMath.h		${MDS_PRIVTE_SRC}DirectXTK12/SimpleMath.cpp			${MDS_PUBLIC_SRC}DirectXTK12/SimpleMath.inl
	
)

create_non_existing("${MDS_SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}" ${MDS_NAME})

add_library (${MDS_NAME} STATIC ${MDS_SOURCES})

# 'source_group' Ensure than Visual Studio Solution respect File Hierarchy in filters
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MDS_SOURCES})

# Exemple of a define
# option(THIS_IS_A_DEFINE "This a compilation flag define on by default" ON)
#
# if (USE_MYMATH)
#  target_compile_definitions(${MDS_NAME} PRIVATE "THIS_IS_A_DEFINE")
# endif()


# No differenciation between 'install(EXPORT)' and 'export()' for now
# (never been tested outside static include case)

target_include_directories(${MDS_NAME}
	PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/Source/Public/"
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}"/Source/Private/"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${MDS_NAME} PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(${MDS_NAME} PUBLIC "d3d12.lib" "dxgi.lib" "dxguid.lib")
# Normally '${CMAKE_CURRENT_SOURCE_DIR}' is implicit when a relative path is given
# But this is specifically not the case for the macro 'INSTALL_INTERFACE', making the explicit declaration needed
# 'INSTALL_INTERFACE' is used when the target is exported using 'install(EXPORT)'
# 'BUILD_INTERFACE' is used when the target is exported with 'export()' or when used by another target the same buildsystem
# see : 'https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html#genex:INSTALL_INTERFACE'

# target_include_directories(${MDS_NAME}
# 	PUBLIC
# 		$<INSTALL_INTERFACE:"${CMAKE_CURRENT_SOURCE_DIR}/Source/Private/${MDS_NAME}">
# 		$<BUILD_INTERFACE:$"{CMAKE_CURRENT_SOURCE_DIR}/Source/Public/${MDS_NAME}">
# 	PRIVATE
# 		${CMAKE_CURRENT_SOURCE_DIR}"/Source/Private/${MDS_NAME}"
# )
