add_executable(test_build_system test_build_system.cpp)
target_link_libraries(test_build_system ${OPENSCENEGRAPH_LIBRARIES} ${GDAL_LIBRARY} Qt5::Core Qt5::Widgets Qt5::Gui)

add_subdirectory(imageviewer)
add_subdirectory(cloudviewer)
add_subdirectory(cloudsender)
add_subdirectory(imageviewerdaemon)
