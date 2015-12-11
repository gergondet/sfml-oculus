#include "OVRWrapper.h"

#define GL_GLEXT_PROTOTYPES
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <iostream>

inline std::ostream & operator<<(std::ostream & os, const glm::mat4 & m)
{
  for(int i = 0; i < 4; ++i) {
    for(int j = 0; j < 4; ++j) {
      os << m[i][j] << ", ";
    }
    os << std::endl;
  }
  return os;
}

OVRWrapper::OVRWrapper()
: hmd(), hmd_debug(false), renderScale(0), rt{0,0}
{
  ovr_Initialize();
  hmd = ovrHmd_Create(0);
  if (!hmd)
  {
    std::cout << "No HMD plugged-in, using a virtual one." << std::endl;
    hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
    hmd_debug = true;
  }
  if(hmd)
  {
    std::cout << "---------------------------------" << std::endl;
    std::cout << "HMD & SENSOR DETECTED" << std::endl;
    std::cout << "---------------------------------" << std::endl;;

    resolution = hmd->Resolution;
    leftTextureSize = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left, hmd->DefaultEyeFov[0], 1.0f);
    rightTextureSize = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right, hmd->DefaultEyeFov[1], 1.0f);

    renderScale = static_cast<float>(leftTextureSize.w)/static_cast<float>(resolution.w/2);

    eyeFov[0] = hmd->DefaultEyeFov[0];
    eyeFov[1] = hmd->DefaultEyeFov[1];

    ovrHmd_RecenterPose(hmd);
  }
}

OVRWrapper::~OVRWrapper()
{
  if(hmd)
  {
    delete hmd;
  }
  if(rt[0])
  {
    delete rt[0];
  }
  if(rt[1])
  {
    delete rt[1];
  }
  /*FIXME This hangs-out at exit, find out why */
  //ovr_Shutdown();
}

void OVRWrapper::Init()
{
  rt[0] = new OVRRenderTexture( static_cast<float>(leftTextureSize.w),  static_cast<float>(leftTextureSize.h)  );
  rt[1] = new OVRRenderTexture( static_cast<float>(rightTextureSize.w), static_cast<float>(rightTextureSize.h) );

  cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
  #ifndef WIN32
  if(!hmd_debug)
  {
    cfg.OGL.Header.BackBufferSize = OVR::Sizei(resolution.h, resolution.w);
  }
  else
  {
    cfg.OGL.Header.BackBufferSize = OVR::Sizei(resolution.w, resolution.h);
  }
  #else
  cfg.OGL.Header.BackBufferSize = OVR::Sizei(resolution.w, resolution.h);
  #endif
  cfg.OGL.Header.Multisample = 1;
  #ifdef WIN32
  cfg.OGL.Window = 0;
  cfg.OGL.DC = 0;
  #else
  cfg.OGL.Disp = 0;
  #endif
  unsigned int distortionCaps = ovrDistortionCap_Vignette | ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive;
  #ifndef WIN32
  if(!hmd_debug)
  {
    distortionCaps |= ovrDistortionCap_LinuxDevFullscreen;
  }
  #endif
  bool configured = ovrHmd_ConfigureRendering(hmd, &cfg.Config,
                      distortionCaps,
                      eyeFov, eyeRenderDesc);
  if(configured)
  {
    std::cout << "HMD renderer configured" << std::endl;
  }
  else
  {
    std::cout << "HMD renderer failed to configure" << std::endl;
  }
  ovrHmd_SetEnabledCaps(hmd, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);
  bool attached = ovrHmd_AttachToWindow(hmd, 0, 0, 0);
  if (attached)
  {
    std::cout << "HMD renderer attached to window" << std::endl;
  }
  else
  {
    std::cout << "HMD renderer not attached to window (may be normal)" << std::endl;
  }

  configured = ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0);
  if(configured)
  {
    std::cout << "HMD tracking configured" << std::endl;
  }
  else
  {
    std::cout << "HMD tracking failed to configure" << std::endl;
  }


  eyeRenderViewport[0].Pos = OVR::Vector2i(0,0);
  eyeRenderViewport[0].Size = OVR::Sizei(leftTextureSize.w, leftTextureSize.h);
  eyeRenderViewport[1].Pos = OVR::Vector2i(0,0);
  eyeRenderViewport[1].Size = OVR::Sizei(rightTextureSize.w, rightTextureSize.h);

  eyeTexture[0].OGL.Header.API = ovrRenderAPI_OpenGL;
  eyeTexture[0].OGL.Header.TextureSize = leftTextureSize;
  eyeTexture[0].OGL.Header.RenderViewport = eyeRenderViewport[0];
  eyeTexture[0].OGL.TexId = rt[0]->getTextureID();

  eyeTexture[1].OGL.Header.API = ovrRenderAPI_OpenGL;
  eyeTexture[1].OGL.Header.TextureSize = rightTextureSize;
  eyeTexture[1].OGL.Header.RenderViewport = eyeRenderViewport[1];
  eyeTexture[1].OGL.TexId = rt[1]->getTextureID();

}

glm::mat4 OVRWrapper::BeginRendering(int eyeIndex)
{
  ovrHmd_DismissHSWDisplay(hmd);
  if(eyeIndex == 0)
  {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    /*ovrFrameTiming hmdFrameTiming = */ovrHmd_BeginFrame(hmd, 0);
  }
  rt[eyeIndex]->beginRendering();

  ovrEyeType eye = hmd->EyeRenderOrder[eyeIndex];
  headPose[eye] = ovrHmd_GetHmdPosePerEye(hmd, eye);

  //OVR::Quatf oriQ = OVR::Quatf(headPose[eye].Orientation);
  //OVR::Matrix4f ori = OVR::Matrix4f(oriQ.Inverted());
  //glm::mat4 orientation = glm::mat4(1.0);
  //for(size_t i = 0; i < 4; ++i) { for(size_t j = 0; j < 4; ++j) { orientation[i][j] = ori.M[i][j]; } }
  OVR::Matrix4f proj = ovrMatrix4f_Projection(eyeRenderDesc[eye].Fov, 0.01f, 10000.0f, true);
  glm::mat4 projection = glm::mat4(1.0);
  for(int i = 0; i < 4; ++i) { for(int j = 0; j < 4; ++j) { projection[i][j] = proj.M[j][i]; } }

  OVR::Matrix4f va = OVR::Matrix4f::Translation(eyeRenderDesc[eye].HmdToEyeViewOffset);
  glm::mat4 viewAdjust = glm::mat4(1.0);
  for(int i = 0; i < 4; ++i) { for(int j = 0; j < 4; ++j) { viewAdjust[i][j] = va.M[i][j]; } }

  return projection*viewAdjust;//*orientation;
}

void OVRWrapper::EndRendering(int eyeIndex)
{
  rt[eyeIndex]->endRendering();
  if(eyeIndex == 1)
  {
    ovrHmd_EndFrame(hmd, headPose, reinterpret_cast<const ovrTexture*>(eyeTexture));
  }
}

bool OVRWrapper::IsDebug()
{
#ifndef WIN32
  return hmd_debug;
#else
  return true;
#endif
}

glm::vec2 OVRWrapper::GetResolution()
{
#ifndef WIN32
  if(!hmd_debug)
  {
    return glm::vec2(resolution.h, resolution.w);
  }
  else
  {
    return glm::vec2(resolution.w, resolution.h);
  }
#else
  return glm::vec2(resolution.w, resolution.h);
#endif
}

float OVRWrapper::GetRenderScale()
{
  return renderScale;
}

Eigen::Vector3d OVRWrapper::GetHMDOrientation()
{
  Eigen::Vector3d res;
  ovrTrackingState ts = ovrHmd_GetTrackingState(hmd, ovr_GetTimeInSeconds());
  if(ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked))
  {
    OVR::Posef pose = ts.HeadPose.ThePose;
    float yaw; float pitch; float roll;
    pose.Rotation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);
    res(0) = yaw; res(1) = pitch; res(2) = roll;
  }
  return res;
}
