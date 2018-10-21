cmake_minimum_required(VERSION 2.6)

project(ch8e)

set(files 
	memory.c
	registers.c
	random.c
	io.c
	clock.c
	interpreter.c
)

set(CMAKE_BUILD_TYPE debug)

add_executable(ch8e ${files} main.c)

target_link_libraries(ch8e PRIVATE ncurses)

