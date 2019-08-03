# cmake_minimum_required(VERSION 2.8)
# include(../../cmake/PiUtilities.cmake)#include the utilities cmake funcitons
get_filename_component(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CUR_MODULE_INCLUDE_DIRS "")
set(CUR_MODULE_INCLUDES "")
set(CUR_MODULE_SRCS "")

#if you module depend other libraries, add the libraries directories to  CUR_MODULE_LINK_DIRS
#library names to the CUR_MODULE_LINKS
set(CUR_MODULE_LINKS "")
set(CUR_MODULE_LINK_DIRS "")

#find the "*.h/*.hpp" in the include directory
FindAllHeaderDirs("${CMAKE_CURRENT_SOURCE_DIR}/include" CUR_MODULE_INCLUDES CUR_MODULE_INCLUDE_DIRS)
# message(${CUR_MODULE_INCLUDE_DIRS})
# set(CURMODULE_INCLUDE_DIRS ${CURMODULE_INCLUDE_DIRS} ${CUR_MODULE_INCLUDE_DIRS} CACHE INTERNAL "")
# FindAllHeaderDirs("${CMAKE_CURRENT_SOURCE_DIR}/../Shm/include" Shm_INCLUDES Shm_INCLUDES_dir)
# set(CURMODULE_INCLUDE_DIRS ${CURMODULE_INCLUDE_DIRS} ${Shm_INCLUDES_dir} CACHE INTERNAL "")
FindAllCpps("${CMAKE_CURRENT_SOURCE_DIR}/src" CUR_MODULE_SRCS)#find the "*.c/*.cpp" in the src directory


# message("The files found in the ${CURMODULE_INCLUDE_DIRS} module : ")
# message("The files found in the ${CURRENT_DIR_NAME} module : ")
# message("${CUR_MODULE_INCLUDE_DIRS}")
# show_strlist("${CUR_MODULE_INCLUDES}")
# show_strlist("${CUR_MODULE_SRCS}")


#Add the depedance library : add the library header file and  the dynamic library
#	findExtLibHeaders("lua.hpp" "/usr/include/lua5.1" LUA_INCLUDE_DIR LUA_INCLUDES)
#	findExtLibLinks("lua5.1" "/usr/lib/x86_64-linux-gnu" LUA_LINK_DIR LUA_LIBRARY)
#
#	set(CUR_MODULE_INCLUDE_DIRS ${CUR_MODULE_INCLUDE_DIRS} ${LUA_INCLUDE_DIR} CACHE INTERNAL "")
#	set(CUR_MODULE_LINKS ${CUR_MODULE_LINKS} ${LUA_LIBRARY})
#	set(CUR_MODULE_LINK_DIRS ${CUR_MODULE_LINK_DIRS} ${LUA_LINK_DIR})


#add the dependent libraries and directories to PI_ALL_LINKS PI_ALL_LINK_DIRS
#set(PI_ALL_LINK_DIRS ${PI_ALL_LINK_DIRS} ${CUR_MODULE_LINK_DIRS} CACHE INTERNAL "")
#set(PI_ALL_LINKS ${PI_ALL_LINKS} ${CUR_MODULE_LINKS} CACHE INTERNAL "")

#add the "*.h/*.hpp" "*.c/*.cpp" to the global vars
# set(PI_ALL_INCLUDE_DIRS ${PI_ALL_INCLUDE_DIRS} ${CUR_MODULE_INCLUDE_DIRS} CACHE INTERNAL "")
set(PI_ALL_INCLUDES ${PI_ALL_INCLUDES} ${CUR_MODULE_INCLUDES} CACHE INTERNAL "")
set(PI_ALL_SRCS ${PI_ALL_SRCS} ${CUR_MODULE_SRCS} CACHE INTERNAL "")

if(BUILD_TEST)
	set(TEST_INCLUDE_DIRS "")
	set(TEST_INCLUDES "")
	set(TEST_SRCS "")

	FindAllHeaderDirs("${CMAKE_CURRENT_SOURCE_DIR}/test" TEST_INCLUDES TEST_INCLUDE_DIRS)
	FindAllCpps("${CMAKE_CURRENT_SOURCE_DIR}/test" TEST_SRCS)#find the "*.c/*.cpp" in the src directory

	#add the "*.h/*.hpp" "*.c/*.cpp" to the global vars
	set(PI_TEST_INCLUDE_DIRS ${PI_TEST_INCLUDE_DIRS} ${TEST_INCLUDE_DIRS} CACHE INTERNAL "")
	set(PI_TEST_SRCS ${PI_TEST_SRCS} ${TEST_SRCS} ${TEST_INCLUDES} CACHE INTERNAL "")

	CLEAR_VARS(TEST_INCLUDE_DIRS TEST_INCLUDES TEST_SRCS)
endif()

# set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
# set(ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# include_directories(${CURMODULE_INCLUDE_DIRS})

add_library(${CURRENT_DIR_NAME} SHARED ${CUR_MODULE_SRCS})
SET_TARGET_PROPERTIES (${CURRENT_DIR_NAME} PROPERTIES OUTPUT_NAME "${CURRENT_DIR_NAME}")
SET_TARGET_PROPERTIES (${CURRENT_DIR_NAME} PROPERTIES VERSION 1.2 SOVERSION 1)
install(
  FILES ${CUR_MODULE_INCLUDES}
  DESTINATION ${Include_Install_Dir_Root}/${CURRENT_DIR_NAME}
)
# install(
#   FILES ${LIBRARY_OUTPUT_PATH}/${CURRENT_DIR_NAME}
#   DESTINATION ${Lib_Install_Dir}
# )

install(TARGETS ${CURRENT_DIR_NAME} DESTINATION ${Lib_Install_Dir})
#set(Include_Install_Dirs ${Include_Install_Dirs} ${Include_Install_Dir_Root}/${CURRENT_DIR_NAME})

CLEAR_VARS(CUR_MODULE_INCLUDE_DIRS CUR_MODULE_INCLUDES CUR_MODULE_SRCS CUR_MODULE_LINKS CUR_MODULE_LINK_DIRS)
