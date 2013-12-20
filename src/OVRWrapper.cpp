#include "OVRWrapper.h"

#define GL_GLEXT_PROTOTYPES
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <iostream>

inline std::ostream & operator<<(std::ostream & os, const OVR::Matrix4f & m)
{
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            os << m.M[i][j] << ", ";
        }
        os << std::endl;
    }
    return os;
}

OVRWrapper::OVRWrapper(float width, float height)
: manager(0), hmd(0), hmdInfo(), sensor(0), sConfig(), currentEye(), renderScale(0)
{
    OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));

    manager = OVR::DeviceManager::Create();
    hmd = manager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();
    if (hmd)
    {
        sensor = hmd->GetSensor();
        if(hmd->GetDeviceInfo(&hmdInfo))
        {
            sConfig.SetHMDInfo(hmdInfo);
        }
    }
    else
    {
        sensor = manager->EnumerateDevices<OVR::SensorDevice>().CreateDevice();
    }
    if(hmd == 0 && sensor == 0)
    {
        std::cout << "---------------------------------" << std::endl;
        std::cout << "NO HMD DETECTED" << std::endl;
        std::cout << "NO SENSOR DETECTED" << std::endl;
        std::cout << "---------------------------------" << std::endl;;
    }
    else if(hmd == 0)
    {
        std::cout << "---------------------------------" << std::endl;
        std::cout << "NO HMD DETECTED" << std::endl;
        std::cout << "---------------------------------" << std::endl;;
    }
    else if(sensor == 0)
    {
        std::cout << "---------------------------------" << std::endl;
        std::cout << "NO SENSOR DETECTED" << std::endl;
        std::cout << "---------------------------------" << std::endl;;
    }
    else
    {
        std::cout << "---------------------------------" << std::endl;
        std::cout << "HMD & SENSOR DETECTED" << std::endl;
        std::cout << "---------------------------------" << std::endl;;
    }
    sConfig.SetFullViewport(OVR::Util::Render::Viewport(0,0, width, height));
    sConfig.SetStereoMode(OVR::Util::Render::Stereo_LeftRight_Multipass);
    sConfig.SetDistortionFitPointVP(-1.0f, 0.0f);
    renderScale = sConfig.GetDistortionScale();
}

OVRWrapper::~OVRWrapper()
{
    delete sensor;
    delete hmd;
    /*FIXME This hangs-out at exit, find out why */
//    OVR::System::Destroy();
}

void OVRWrapper::setEye(OVR::Util::Render::StereoEye eye)
{
    currentEye = sConfig.GetEyeRenderParams(eye);
    OVR::Util::Render::Viewport vp = currentEye.VP;
    glViewport(vp.x, vp.y, vp.w, vp.h);
    glEnable(GL_SCISSOR_TEST);
    glScissor(vp.x, vp.y, vp.w, vp.h);
}

glm::mat4 OVRWrapper::getViewAdjust()
{
    glm::mat4 viewAdjust = glm::mat4(1.0);
    for(size_t i = 0; i < 4; ++i) {
    for(size_t j = 0; j < 4; ++j) {
        viewAdjust[i][j] = currentEye.ViewAdjust.M[i][j];
    } }
    return viewAdjust;
}

glm::mat4 OVRWrapper::getProjection()
{
    glm::mat4 projection = glm::mat4(1.0);
    return projection;
}
