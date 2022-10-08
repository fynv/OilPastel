cmake_minimum_required (VERSION 3.0)

project(OilPastel)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

SET(QT_ROOT ${QT_ROOT} CACHE PATH "Qt path" FORCE)
SET(Qt6_DIR ${QT_ROOT}/lib/cmake/Qt6)
SET(Qt6Gui_DIR ${QT_ROOT}/lib/cmake/Qt6Gui)
SET(Qt6GuiTools_DIR ${QT_ROOT}/lib/cmake/Qt6GuiTools)
SET(Qt6CoreTools_DIR ${QT_ROOT}/lib/cmake/Qt6CoreTools)
SET(Qt6WidgetsTools_DIR ${QT_ROOT}/lib/cmake/Qt6WidgetsTools)

find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS GuiTools REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGL REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)

set(SOURCES
main.cpp
GLUtils.cpp
GLUtils.h
OilPastelWindow.cpp
OilPastelWindow.h
GLCanvas.cpp
GLCanvas.h
Image.cpp
Image.h
Operations.cpp
Operations.h
Stroke.cpp
Stroke.h
)

set(SOURCES_ROUTINES
routines/DrawTexture.cpp
routines/DrawTexture.h
routines/DrawThickness.cpp
routines/DrawThickness.h
routines/Blur.cpp
routines/Blur.h
routines/CompNorm.cpp
routines/CompNorm.h
routines/DrawLighted.cpp
routines/DrawLighted.h
routines/StrokeDrawTriangles.cpp
routines/StrokeDrawTriangles.h
routines/StrokeBlend.cpp
routines/StrokeBlend.h
routines/StrokeDrawCurve.cpp
routines/StrokeDrawCurve.h
routines/BlitImage.cpp
routines/BlitImage.h
)


set (INCLUDE_DIR
.
./thirdparty/glm/glm
)


set (LINK_LIBS 
Qt6::Gui
Qt6::Widgets
Qt6::OpenGL
Qt6::OpenGLWidgets
)


set (QT_DLLS
${QT_ROOT}/bin/Qt6Core.dll
${QT_ROOT}/bin/Qt6Gui.dll
${QT_ROOT}/bin/Qt6Widgets.dll
${QT_ROOT}/bin/Qt6OpenGL.dll
${QT_ROOT}/bin/Qt6OpenGLWidgets.dll
)

if (WIN32) 
set (DEFINES  ${DEFINES}
-D"_CRT_SECURE_NO_DEPRECATE"  
-D"_SCL_SECURE_NO_DEPRECATE" 
)
else()
add_definitions(-std=c++0x)
add_compile_options(-fPIC)
endif()

include_directories(${INCLUDE_DIR})
add_definitions(${DEFINES})
add_executable (OilPastel ${SOURCES} ${SOURCES_ROUTINES})

if (WIN32)
set_property(TARGET OilPastel PROPERTY WIN32_EXECUTABLE true)
endif()

target_link_libraries(OilPastel ${LINK_LIBS})

source_group("routines" FILES ${SOURCES_ROUTINES})

if (WIN32)
target_link_libraries(OilPastel opengl32.lib)
install(FILES ${QT_ROOT}/plugins/platforms/qwindows.dll DESTINATION ./platforms)
install(FILES ${QT_DLLS} DESTINATION .)
endif()

install(TARGETS OilPastel DESTINATION .)





