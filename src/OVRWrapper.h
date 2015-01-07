#ifndef _H_OVRWRAPPER_H_
#define _H_OVRWRAPPER_H_

/* Utility class to wrap OVR call */

#ifdef WIN32
  #ifndef OVR_OS_WIN32
  #define OVR_OS_WIN32
  #endif
#else
  #ifndef OVR_OS_LINUX
  #define OVR_OS_LINUX
  #endif
#endif

#include <OVR_CAPI_GL.h>
#include <Kernel/OVR_Math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Eigen/Core>

#include <boost/noncopyable.hpp>

#include "OVRRenderTexture.h"

class OVRWrapper : public boost::noncopyable
{
public:
  OVRWrapper();

  ~OVRWrapper();

  void Init();

  bool IsDebug();

  glm::mat4 BeginRendering(int eyeIndex);

  void EndRendering(int eyeIndex);

  glm::vec2 GetResolution();

  float GetRenderScale();

  /* Returns the YPR values of head orientation */
  Eigen::Vector3d GetHMDOrientation();
private:
  ovrHmd hmd;
  bool hmd_debug;

  ovrSizei resolution;
  ovrSizei leftTextureSize;
  ovrSizei rightTextureSize;
  float renderScale;

  ovrFovPort eyeFov[2];
  ovrEyeRenderDesc eyeRenderDesc[2];
  ovrRecti eyeRenderViewport[2];

  OVRRenderTexture * rt[2];

  ovrGLConfig cfg;
  ovrGLTexture eyeTexture[2];

  ovrPosef headPose[2];
};

#endif
