CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(PROJECT_NAME sfml-oculus)

# C++0x is only used internally for convenience such as auto
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

# FIXME Set to appropriate directory if having trouble to find OVRSDK
FIND_PATH(OCULUS_SDK_DIR "LibOVR/Include/OVR.h" PATHS ${CMAKE_SOURCE_DIR}/../OculusSDK ${CMAKE_BINARY_DIR})
MESSAGE("Found Oculus SDK at ${OCULUS_SDK_DIR}")
# FIXME
INCLUDE_DIRECTORIES(${OCULUS_SDK_DIR}/LibOVR/Include)
LINK_DIRECTORIES(${OCULUS_SDK_DIR}/LibOVR/Lib/Linux/Release/i386)
LINK_DIRECTORIES(${OCULUS_SDK_DIR}/LibOVR/Lib/Linux/Release/x86_64)

SET(${PROJECT_NAME}_LIB_SRC
src/OVRWrapper.cpp
src/SFMLScreen.cpp
src/PostProcessing.cpp
src/OculusWindow.cpp
)

SET(${PROJECT_NAME}_EXAMPLE_SRC
src/PLYMesh.cpp
src/sfml-oculus-example.cpp
)

ADD_LIBRARY(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_LIB_SRC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} udev Xinerama ovr sfml-graphics sfml-window sfml-system GL boost_filesystem pthread boost_system X11)

ADD_EXECUTABLE(${PROJECT_NAME}-example ${${PROJECT_NAME}_EXAMPLE_SRC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}-example ${PROJECT_NAME})

SET(BG
${CMAKE_SOURCE_DIR}/bg/background.png
)

SET(FONTS
${CMAKE_SOURCE_DIR}/fonts/arial.ttf
)

SET(MODELS
${CMAKE_SOURCE_DIR}/models/can-00000.jpg
${CMAKE_SOURCE_DIR}/models/can-00001.jpg
${CMAKE_SOURCE_DIR}/models/can-00002.jpg
${CMAKE_SOURCE_DIR}/models/can-00003.jpg
${CMAKE_SOURCE_DIR}/models/can-00004.jpg
${CMAKE_SOURCE_DIR}/models/can-00005.jpg
${CMAKE_SOURCE_DIR}/models/can-00006.jpg
${CMAKE_SOURCE_DIR}/models/can.ply
)

SET(SHADERS
${CMAKE_SOURCE_DIR}/shaders/ply_shader.frag
${CMAKE_SOURCE_DIR}/shaders/ply_shader.vert
)

FILE(COPY ${BG} DESTINATION ${CMAKE_BINARY_DIR}/bg)
FILE(COPY ${FONTS} DESTINATION ${CMAKE_BINARY_DIR}/fonts)
FILE(COPY ${MODELS} DESTINATION ${CMAKE_BINARY_DIR}/models)
FILE(COPY ${SHADERS} DESTINATION ${CMAKE_BINARY_DIR}/shaders)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
INSTALL(FILES src/OculusWindow.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
