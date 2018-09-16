INCLUDE_DIRECTORIES(../imageviewer)
add_executable(imageviewer_daemon WIN32
    daemon_main.cpp
    imageviewerdaemondlg.ui
    imageviewerdaemondlg.cpp
    ../imageviewer/qgraphicsgdalitem.cpp
    ../imageviewer/gdalview.cpp)

target_link_libraries(imageviewer_daemon ${GDAL_LIBRARY} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)

if(WIN32)
target_link_libraries(imageviewer_daemon Qt5::WinMain)
endif()
add_executable(daemon_tester
     daemon_tester_main.cpp)

target_link_libraries(daemon_tester Qt5::Core Qt5::Network )
