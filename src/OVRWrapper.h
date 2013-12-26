#ifndef _H_OVRWRAPPER_H_
#define _H_OVRWRAPPER_H_

/* Utility class to wrap OVR call */

#include <OVR.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <boost/noncopyable.hpp>

class OVRWrapper : public boost::noncopyable
{
public:
    OVRWrapper(float width, float height, bool enable_scaling = true);

    ~OVRWrapper();

    /* This function will set the current eye to Center/Left/Right respectively
     * and change the viewport accordingly */
    void setEye(OVR::Util::Render::StereoEye eye);

    glm::mat4 getViewAdjust();

    glm::mat4 getProjection();

    const OVR::Util::Render::DistortionConfig * getDistortionConfig() { return currentEye.pDistortion; }

    float getRenderScale() { return renderScale; }
private:
    OVR::DeviceManager * manager;
    OVR::HMDDevice * hmd;
    OVR::HMDInfo hmdInfo;
    OVR::SensorDevice * sensor;
    OVR::Util::Render::StereoConfig sConfig;
    OVR::Util::Render::StereoEyeParams currentEye;
    float renderScale;
};

#endif
