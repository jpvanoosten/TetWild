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

target_link_libraries( TetWildPlugin 
		libTetWild
		CLI11::CLI11
		igl::cgal
)