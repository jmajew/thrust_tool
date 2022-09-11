cmake_minimum_required(VERSION 3.5)

project(ThrustApp LANGUAGES CXX)


#C:\scratch\Qt\5.15.2\msvc2019_64\bin
set ( CMAKE_PREFIX_PATH "C:\\scratch\\Qt\\5.12.2\\msvc2019_64" )

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
