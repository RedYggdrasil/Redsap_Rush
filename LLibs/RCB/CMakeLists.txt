
#Red Cpp Base module
set(RCB_NAME "RCB")

set (RCB_PUBLIC_SRC "Source/Public/${RCB_NAME}/")
set (RCB_PRIVTE_SRC "Source/Private/${RCB_NAME}/")


# Header file are not compiled into object (or rather are just copied into objects on include), so their inclusion into the project is optional
# Only their 'target_include_directories' is needed to be usable as #include
# List of included sources
set(RCB_SOURCES
	${RCB_PUBLIC_SRC}Defines/RCB.h					${RCB_PRIVTE_SRC}Defines/RCB.cpp
	${RCB_PUBLIC_SRC}Tools/RCBVectorMap.h			${RCB_PRIVTE_SRC}Tools/RCBVectorMap.cpp
)

create_non_existing("${RCB_SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}" ${RCB_NAME})

add_library (${RCB_NAME} STATIC ${RCB_SOURCES})

# 'source_group' Ensure than Visual Studio Solution respect File Hierarchy in filters
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RCB_SOURCES})

# Exemple of a define
# option(THIS_IS_A_DEFINE "This a compilation flag define on by default" ON)
#
# if (USE_MYMATH)
#  target_compile_definitions(${RCB_NAME} PRIVATE "THIS_IS_A_DEFINE")
# endif()


# No differenciation between 'install(EXPORT)' and 'export()' for now
# (never been tested outside static include case)

target_include_directories(${RCB_NAME}
	PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/Source/Public/"
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}"/Source/Private/"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${RCB_NAME} PROPERTY CXX_STANDARD 20)
endif()


#currently not using any library in RCB
#target_link_libraries(${RCB_NAME} PUBLIC "d3d12.lib" "dxgi.lib" "dxguid.lib")


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
