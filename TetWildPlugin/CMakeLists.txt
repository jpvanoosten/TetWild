cmake_minimum_required(VERSION 3.3)

set( INC
	pch.h
)

set( SRC
	dllmain.cpp
	pch.cpp
	TetWildPlugin.cpp
)

add_library(TetWildPlugin SHARED ${INC} ${SRC})

target_compile_definitions( TetWildPlugin
	INTERFACE TetWildPlugin_IMPORTS
)

target_link_libraries( TetWildPlugin 
		libTetWild
)
igl_copy_cgal_dll(TetWildPlugin)

