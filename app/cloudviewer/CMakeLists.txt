add_executable(CloudViewer WIN32
    cvmain.cpp
    qtosgwidget.cpp
    cvmainwindow.cpp
    test_thread.cpp
    udpsource.cpp
    cvmainwindow.ui)

target_link_libraries(CloudViewer
    ${OPENSCENEGRAPH_LIBRARIES}
    ${LIBLAS_LIBRARY}
    Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Network)

if(WIN32)
    target_link_libraries(CloudViewer Qt5::WinMain)
endif()
