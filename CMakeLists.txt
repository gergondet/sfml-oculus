CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(PROJECT_NAME sfml-oculus)

SET(OCULUS_SDK_DIR /home/gergondet/devel-src/oculus/OculusSDK)
INCLUDE_DIRECTORIES(${OCULUS_SDK_DIR}/LibOVR/Include)
LINK_DIRECTORIES(${OCULUS_SDK_DIR}/LibOVR/Lib/Linux/Release/i386)

SET(${PROJECT_NAME}_SRC
src/OVRWrapper.cpp
src/SFMLScreen.cpp
src/PLYMesh.cpp
src/main.cpp
)

ADD_EXECUTABLE(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} udev Xinerama ovr sfml-graphics sfml-window sfml-system GL boost_filesystem)

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
${CMAKE_SOURCE_DIR}/shaders/sfmlscreen_shader.frag
${CMAKE_SOURCE_DIR}/shaders/sfmlscreen_shader.vert
)


FILE(COPY ${FONTS} DESTINATION ${CMAKE_BINARY_DIR}/fonts)
FILE(COPY ${MODELS} DESTINATION ${CMAKE_BINARY_DIR}/models)
FILE(COPY ${SHADERS} DESTINATION ${CMAKE_BINARY_DIR}/shaders)
