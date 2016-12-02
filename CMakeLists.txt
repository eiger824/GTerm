cmake_minimum_required(VERSION 3.2)
project(gterm)
find_package(Qt4 REQUIRED)

INCLUDE(${QT_USE_FILE})

# add definitions
ADD_DEFINITIONS(
  ${QT_DEFINITIONS} 
)

include_directories(
	include
	src
)

set(HEADERS
  include/gui.hpp
  )

set(SOURCES
  src/gui.cpp
  )

qt4_wrap_cpp(HEADERS_MOC ${HEADERS})

add_executable(gterm
  src/main.cpp
  ${SOURCES}
  ${HEADERS_MOC}
  )

target_link_libraries(gterm
  Qt4::QtGui
  Qt4::QtXml
  )

