INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../wiic/)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../log/)

SET(WIICPP_SOURCES wiicpp.cpp ../log/dataset.cpp ../log/training.cpp)
ADD_LIBRARY(wiicpp SHARED ${WIICPP_SOURCES})
TARGET_LINK_LIBRARIES(wiicpp wiic)

INSTALL(TARGETS wiicpp DESTINATION /usr/local/lib)
INSTALL(FILES wiicpp.h DESTINATION /usr/local/include/wiic/)
INSTALL(FILES ../log/training.h DESTINATION /usr/local/include/wiic/)
INSTALL(FILES ../log/dataset.h DESTINATION /usr/local/include/wiic/)
INSTALL(FILES ../log/sample.h DESTINATION /usr/local/include/wiic/)
INSTALL(FILES ../log/accsample.h DESTINATION /usr/local/include/wiic/)
