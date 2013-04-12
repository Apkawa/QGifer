CMAKE_MINIMUM_REQUIRED (VERSION 2.6)


#------------------------------------------------------
#-------------------- GENERAL -------------------------
#------------------------------------------------------

PROJECT(QGifer)
SET(TARGET_NAME qgifer)

SET(CMAKE_INSTALL_PREFIX "/usr/")

SET(QGIFER_VERSION_MAJOR 0)
SET(QGIFER_VERSION_MINOR 1)
SET(QGIFER_VERSION_PATCH 0)

OPTION(QUIET_MODE "quiet mode" OFF)
OPTION(RELEASE_MODE "release mode" ON)

IF(UNIX)
	SET(BIN_INSTALL_DIR "bin")
	# SET(DATA_INSTALL_DIR "share/qgifer")
	SET(ICON_INSTALL_DIR "share/icons")
	SET(DESKTOP_INSTALL_DIR "share/applications")
	SET(MENU_INSTALL_DIR "share/menu")
	SET(DOC_INSTALL_DIR "share/doc/qgifer")
ENDIF(UNIX)


SET(W32_EXEC )
IF(WIN32)
	SET(W32_EXEC WIN32)
	SET(OpenCV_DIR "C:\\opencv\\build")
	SET(GIF_LIBRARY "C:\\Program Files\\GnuWin32\\lib")
	SET(GIF_INCLUDE_DIR "C:\\Program Files\\GnuWin32\\include")
ENDIF(WIN32)

FIND_PACKAGE( OpenCV REQUIRED core highgui )
FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(GIF REQUIRED)

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wno-reorder -Wno-sign-compare" )

IF( QUIET_MODE )
    MESSAGE("Quiet mode is ON, no debug output will be shown")
    ADD_DEFINITIONS(-DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT)
ENDIF( QUIET_MODE )

IF( RELEASE_MODE )
    MESSAGE("Release mode is ON")
    ADD_DEFINITIONS(-DQT_NO_DEBUG)
    SET(CMAKE_BUILD_TYPE Release)
ELSE( RELEASE_MODE )
      MESSAGE("Release mode is OFF")
    SET(CMAKE_BUILD_TYPE Debug)
ENDIF( RELEASE_MODE )

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)



#------------------------------------------------------
#-------------------- APP BUILD -----------------------
#------------------------------------------------------

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/version.h"
  )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} src ui )

SET (HEADERS 
src/gifcreator.h
)

SET (QT_HEADERS 
src/frameplayer.h 
src/previewwidget.h 
src/mainwindow.h
src/gifwidget.h
src/qgifcreator.h
src/palettewidget.h
src/qmultislider.h
src/optimizerdialog.h
src/converterdialog.h
src/aboutdialog.h
)

SET (SOURCES 
src/frameplayer.cpp 
src/main.cpp 
src/previewwidget.cpp
src/mainwindow.cpp 
src/gifwidget.cpp
src/gifcreator.cpp
src/qgifcreator.cpp
src/palettewidget.cpp
src/qmultislider.cpp
src/optimizerdialog.cpp
src/converterdialog.cpp
src/aboutdialog.cpp
)

SET (RESOURCES 
res.qrc
)

SET (FORMS 
ui/frameplayer.ui 
ui/mainwindow.ui
ui/gifwidget.ui
ui/optimizerdialog.ui
ui/converterdialog.ui
ui/aboutdialog.ui
)

QT4_WRAP_CPP(HEADERS_MOC ${QT_HEADERS})
QT4_WRAP_UI(FORMS_HEADERS ${FORMS})
QT4_ADD_RESOURCES(RESOURCES_RCC ${RESOURCES})

IF(WIN32)
	SET( OPENCV_LIBS "-lopencv_core244 -lopencv_highgui244" )
ELSE(WIN32)
	SET( OPENCV_LIBS "-lopencv_core -lopencv_highgui" )
ENDIF(WIN32)

SET( GIF_LIBS "-lgif" )	

set(RES_FILES "")
if(MINGW)
  set(RES_FILES "icon.rc")
  set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)
  SET(CMAKE_RC_COMPILE_OBJECT
    "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)

ADD_EXECUTABLE(${TARGET_NAME} ${W32_EXEC} ${SOURCES} ${HEADERS_MOC} ${FORMS_HEADERS} ${RESOURCES_RCC} ${RES_FILES})
TARGET_LINK_LIBRARIES(${TARGET_NAME} ${QT_LIBRARIES} ${OPENCV_LIBS} ${GIF_LIBS})

IF(UNIX)
	INSTALL(TARGETS ${TARGET_NAME} DESTINATION ${BIN_INSTALL_DIR})
	INSTALL(FILES "doc/menu/qgifer" DESTINATION ${MENU_INSTALL_DIR})
	INSTALL(FILES "doc/qgifer.desktop" DESTINATION ${DESKTOP_INSTALL_DIR})
	INSTALL(FILES "doc/qgifer.xpm" DESTINATION ${ICON_INSTALL_DIR})
	INSTALL(FILES "doc/README" "doc/LICENSE" "doc/COPYRIGHT" "doc/ChangeLog" 
	DESTINATION ${DOC_INSTALL_DIR})
ENDIF(UNIX)



#------------------------------------------------------
#-------------------- CPACK ---------------------------
#------------------------------------------------------

SET(PACKAGE_VERSION 1)
SET(PACKAGE_NAME qgifer)
 
IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
INCLUDE(InstallRequiredSystemLibraries)
 
SET(CPACK_SET_DESTDIR "on")
SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A video-based gif creator.")

SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "A video-based gif creator.")

SET(CPACK_PACKAGE_VENDOR "Chodak")
SET(CPACK_PACKAGE_CONTACT "Lukasz Chodyla")
SET(CPACK_PACKAGE_VERSION_MAJOR "${PACKAGE_VERSION}")
# SET(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
# SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
SET(CPACK_PACKAGE_FILE_NAME "${PACKAGE_NAME}_${QGIFER_VERSION_MAJOR}.${QGIFER_VERSION_MINOR}.${QGIFER_VERSION_PATCH}-${PACKAGE_VERSION}_${CMAKE_SYSTEM_PROCESSOR}")
#SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${PACKAGE_NAME}_${QGIFER_VERSION_MAJOR}.${QGIFER_VERSION_MINOR}.${QGIFER_VERSION_PATCH}")
 
#set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON) #za pierwszym razem, pozniej dodano | dla nowszych opencv
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.1.3), libc6 (>= 2.3.6-6~), libgcc1 (>= 1:4.1.1), libgif4 (>= 4.1.4), libopencv-core2.3 | libopencv-core2.4, libopencv-highgui2.3 | libopencv-highgui2.4, libqtcore4 (>= 4:4.7.0~beta1), libqtgui4 (>= 4:4.6.1), libstdc++6 (>= 4.1.1)")
 
SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
SET(CPACK_DEBIAN_PACKAGE_SECTION "video")
SET(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
 
SET(CPACK_COMPONENTS_ALL Libraries ApplicationData)
INCLUDE(CPack)
 
ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")



