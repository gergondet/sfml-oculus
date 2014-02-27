varying vec4 vertex;

uniform vec2 LensCenter;
uniform vec4 HmdWarpParam;
uniform float Scale;

uniform sampler2D texture;

uniform mat4 modelviewprojection;

uniform bool useTexture=false;
uniform bool useDistortion=false;

uniform float x;
uniform float y;
uniform float w;
uniform float h;

vec2 hmdWarp(vec2 in01)
{
    vec2 theta = (in01 - LensCenter);
    float rSq = theta.x*theta.x + theta.y*theta.y;
    vec2 theta1 = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq + HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);
    return LensCenter + Scale*theta1;
}

void main(){
    vec4 texcoords;

    if(useTexture)
    {
        // calculate texture coordinats
        texcoords = modelviewprojection * vertex;
        texcoords.x = (texcoords.x / texcoords.w + 1.0) * 0.5;
        texcoords.y = (texcoords.y / texcoords.w + 1.0) * 0.5;

        // Crop texcoords from window to texture coordinates
        texcoords.x = (texcoords.x-x)/w;
        texcoords.y = (texcoords.y-y)/h;

        // get texture color for frame and model
        if(useDistortion)
        {
            vec2 tc = hmdWarp(texcoords.xy);
            gl_FragColor =  texture2D(texture, tc);
        }
        else
        {
            vec2 tc = texcoords.xy;
            gl_FragColor =  texture2D(texture, tc);
        }
    }
    else
    {
        gl_FragColor = vec4(0.2, 0.5, 1.0, 1.0);
    }
}
