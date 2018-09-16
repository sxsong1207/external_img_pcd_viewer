add_executable(ImageViewer WIN32
    ivmain.cpp
    ivmainwindow.cpp
    qgraphicsgdalitem.cpp
    gdalview.cpp
    mygraphicshandleritem.cpp
    ivmainwindow.ui
    res.qrc)

target_link_libraries(ImageViewer ${GDAL_LIBRARY} Qt5::Core Qt5::Widgets Qt5::Gui)

if(WIN32)
target_link_libraries(ImageViewer Qt5::WinMain)
endif()
