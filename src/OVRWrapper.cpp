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

inline std::ostream & operator<<(std::ostream & os, const OVR::Util::Render::StereoEye & eye)
{
    switch(eye)
    {
        case OVR::Util::Render::StereoEye_Center:
            os << "center eye";
            break;
        case OVR::Util::Render::StereoEye_Left:
            os << "left eye";
            break;
        case OVR::Util::Render::StereoEye_Right:
            os << "right eye";
            break;
    }
    return os;
}

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

OVRWrapper::OVRWrapper(float width, float height, bool enable_scaling)
: manager(0), hmd(0), hmdInfo(), sensor(0), sfusion(0), sConfig(), currentEye(), renderScale(0)
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
    if(sensor)
    {
        sfusion = new OVR::SensorFusion(sensor); 
    }
    sConfig.SetFullViewport(OVR::Util::Render::Viewport(0,0, width, height));
    sConfig.SetStereoMode(OVR::Util::Render::Stereo_LeftRight_Multipass);
    sConfig.SetDistortionFitPointVP(-1.0f, 0.0f);
    renderScale = enable_scaling ? sConfig.GetDistortionScale() : 1;
}

OVRWrapper::~OVRWrapper()
{
    delete sfusion;
    delete sensor;
    delete hmd;
    /*FIXME This hangs-out at exit, find out why */
//    OVR::System::Destroy();
}

void OVRWrapper::setEye(OVR::Util::Render::StereoEye eye)
{
    currentEye = sConfig.GetEyeRenderParams(eye);
    //OVR::Util::Render::Viewport vp = currentEye.VP;
    //glViewport(renderScale*vp.x, renderScale*vp.y, renderScale*vp.w, renderScale*vp.h);
    //std::cout << "Viewport to " << renderScale*vp.x << " " << renderScale*vp.y << " " << renderScale*vp.w << " " << renderScale*vp.h << std::endl;
    //glEnable(GL_SCISSOR_TEST);
    //glScissor(renderScale*vp.x, renderScale*vp.y, renderScale*vp.w, renderScale*vp.h);
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
    OVR::Matrix4f proj_in = currentEye.Projection.Transposed();
    glm::mat4 projection = glm::mat4(1.0);
    for(size_t i = 0; i < 4; ++i) {
    for(size_t j = 0; j < 4; ++j) {
        projection[i][j] = proj_in.M[i][j];
    } }
    return projection;
}

Eigen::Vector3d OVRWrapper::GetHMDOrientation()
{
    Eigen::Vector3d res;
    if(sensor)
    {
        OVR::Quatf hmdOrientation = sfusion->GetOrientation();
        float yaw; float pitch; float roll;
        hmdOrientation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);
        res(0) = yaw; res(1) = pitch; res(2) = roll;
    }
    return res;
}
