#include "OVRWrapper.h"

OVRWrapper::OVRWrapper(float width, float height)
{
}

OVRWrapper::~OVRWrapper()
{
}

void OVRWrapper::setEye(OVR::Util::Render::StereoEye eye)
{
}

glm::mat4 OVRWrapper::getViewAdjust()
{
    glm::mat4 viewAdjust = glm::mat4(1.0);
    return viewAdjust;
}

glm::mat4 OVRWrapper::getProjection()
{
    glm::mat4 projection = glm::mat4(1.0);
    return projection;
}
