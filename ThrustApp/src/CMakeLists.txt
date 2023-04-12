cmake_minimum_required(VERSION 3.5)

project(ThrustApp LANGUAGES CXX)

# Git
FIND_PACKAGE( Git )

IF ( GIT_FOUND )

  MESSAGE("Git found: ${GIT_EXECUTABLE}")

	# including git version info
	# the commit's SHA1, and whether the building workspace was dirty or not
	execute_process(
		COMMAND  ${GIT_EXECUTABLE} rev-parse HEAD
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE GIT_COMMIT_ID
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

	execute_process(
		COMMAND  ${GIT_EXECUTABLE} rev-parse --short HEAD
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE GIT_COMMIT_ID_SHORT
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

	# the date of the commit
	execute_process(
		COMMAND ${GIT_EXECUTABLE} log -1 --format=%ad --date=local
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE GIT_COMMIT_DATE
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

	# author of the commit
	execute_process(
		COMMAND  ${GIT_EXECUTABLE} log -1 --pretty=format:%an
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE GIT_COMMIT_AUTHOR
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

	# the subject of the commit
	execute_process(
		COMMAND ${GIT_EXECUTABLE} log -1 --format=%s
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE GIT_COMMIT_SUBJECT_
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	

	string(REPLACE  "#" "<hash>" GIT_COMMIT_SUBJECT ${GIT_COMMIT_SUBJECT_} )
	
ENDIF ( GIT_FOUND )

add_definitions("-DGIT_COMMIT_ID=\"${GIT_COMMIT_ID}\"")
add_definitions("-DGIT_COMMIT_ID_SHORT=\"${GIT_COMMIT_ID_SHORT}\"")
add_definitions("-DGIT_COMMIT_DATE=\"${GIT_COMMIT_DATE}\"")
add_definitions("-DGIT_COMMIT_AUTHOR=\"${GIT_COMMIT_AUTHOR}\"")
add_definitions("-DGIT_COMMIT_SUBJECT=\"${GIT_COMMIT_SUBJECT}\"")


######################################################################
# print info

STRING ( TOUPPER "${CMAKE_BUILD_TYPE}" MY_CMAKE_BUILD_TYPE )
MESSAGE( STATUS )
MESSAGE( STATUS "Build Type = ${MY_CMAKE_BUILD_TYPE}")

MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "git id         = ${GIT_COMMIT_ID}" )
MESSAGE( STATUS "git id short   = ${GIT_COMMIT_ID_SHORT}" )
MESSAGE( STATUS "git date       = ${GIT_COMMIT_DATE}" )
MESSAGE( STATUS "git author     = ${GIT_COMMIT_AUTHOR}" )
MESSAGE( STATUS "git subject    = ${GIT_COMMIT_SUBJECT}" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "CMAKE version       = ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" )
MESSAGE( STATUS "CMAKE_SYSTEM        = ${CMAKE_SYSTEM}" )
MESSAGE( STATUS "CMAKE_GENERATOR     = ${CMAKE_GENERATOR}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE    = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "CMAKE_C_COMPILER    = ${CMAKE_C_COMPILER}" )
MESSAGE( STATUS "CMAKE_CXX_COMPILER  = ${CMAKE_CXX_COMPILER}" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "  flags :" )
MESSAGE( STATUS "CMAKE_C_FLAGS_${MY_CMAKE_BUILD_TYPE}          = ${CMAKE_C_FLAGS_${MY_CMAKE_BUILD_TYPE}}" )
MESSAGE( STATUS "CMAKE_CXX_FLAGS_${MY_CMAKE_BUILD_TYPE}        = ${CMAKE_CXX_FLAGS_${MY_CMAKE_BUILD_TYPE}}" )
MESSAGE( STATUS "CMAKE_EXE_LINKER_FLAGS_${MY_CMAKE_BUILD_TYPE} = ${CMAKE_EXE_LINKER_FLAGS_${MY_CMAKE_BUILD_TYPE}}" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "CMAKE_C_FLAGS          = ${CMAKE_C_FLAGS}")
MESSAGE( STATUS "CMAKE_CXX_FLAGS        = ${CMAKE_CXX_FLAGS}")
MESSAGE( STATUS "CMAKE_EXE_LINKER_FLAGS = ${CMAKE_EXE_LINKER_FLAGS}")
MESSAGE( STATUS "CMAKE_CXX_FLAGS_RELEASE = ${CMAKE_CXX_FLAGS_RELEASE}" )

MESSAGE( STATUS "-------------------------------------------------------------------------------" )
####################################################################



#C:\scratch\Qt\5.15.2\msvc2019_64\bin
set ( CMAKE_PREFIX_PATH "C:\\scratch\\Qt\\5.15.2\\msvc2019_64" )

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# find_package(QT NAMES Qt5 COMPONENTS Widgets SerialPort LinguistTools REQUIRED)
find_package(Qt5 COMPONENTS Widgets SerialPort LinguistTools REQUIRED)

set(TS_FILES 
	ThrustApp_en_US.ts 
	ThrustApp_pl_PL.ts
)

set(UI_FILES
	dlg_graphprobe.ui
	dlg_graphstyle.ui
	dlg_linestyle.ui
	form_calibration.ui
	form_connect.ui
	form_debugmsg.ui
	form_measurement.ui
	form_measuresettings.ui
	form_automeasurement.ui
	form_process.ui
	form_cfg_tool.ui
	form_cfg_convert.ui
	dlg_autocreate.ui
)

set(RESOURCE_FILES
)

set(PROJECT_SOURCES
	analyzer.cpp
	analyzer.h
	appsetup.h
	appsetup.cpp
	cf_gvector.h
	cf_typedef.h
	config.cpp
	config.hpp
	console.cpp
	console.h
	controller.cpp
	controller.h
	data_converter.cpp
	data_converter.h
	dlg_autocreate.cpp
	dlg_autocreate.h
	dlg_graphprobe.cpp
	dlg_graphprobe.h
	dlg_graphstyle.cpp
	dlg_graphstyle.h
	dlg_linestyle.cpp
	dlg_linestyle.h
	dlg_settings.cpp
	dlg_settings.h
	document.cpp
	document.h
	form_automeasurement.cpp
	form_automeasurement.h
	form_calibration.cpp
	form_calibration.h
	latex_generator.cpp
	latex_generator.h
	main.cpp
	mainwindow.cpp
	mainwindow.h
	measure_data.cpp
	measure_data.h
	measurement_setup.cpp
	measurement_setup.h
	result.cpp
	result.h
	sbuf.hpp
	stat_value.hpp
	statistic_value.h
	sysdecl.h
	thread_serial.cpp
	thread_serial.h
	thsp_handler.cpp
	thsp_handler.hpp
	thsp_packet.hpp
	thsp_protocol.h
	toolsetup.cpp
	toolsetup.h
	view_data.cpp
	view_data.h
	view_debug.cpp
	view_debug.h
	view_graph.cpp
	view_graph.h
	view_terminal.cpp
	view_terminal.h
	widget_control.cpp
	widget_control.h
	widget_graph.cpp
	widget_graph.h
	widget_output.cpp
	widget_output.h 	
	${UI_FILES}
	${RESOURCE_FILES}
#	${TS_FILES}
)

# add_definitions(-DQT_NO_DEBUG_OUTPUT)

add_executable(ThrustApp ${PROJECT_SOURCES} )

#    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})


target_link_libraries(ThrustApp PRIVATE Qt5::Widgets Qt5::SerialPort)
