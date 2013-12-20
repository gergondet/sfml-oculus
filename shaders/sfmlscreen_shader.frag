uniform vec2 LensCenter;
uniform vec4 HmdWarpParam;
uniform vec2 Scale;
uniform vec2 ScaleInv;

varying vec2 f_texcoord;

uniform sampler2D texture;

uniform bool warpTexture=true;

vec2 hmdWarp(vec2 in01)
{
    //vec2 theta = (in01 - LensCenter);
    //float rSq = theta.x*theta.x + theta.y*theta.y;
    //vec2 theta1 = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq + HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);
    //return LensCenter + Scale*theta1;
    vec2 theta = (in01 - LensCenter)*ScaleInv;
    float rSq = theta.x*theta.x + theta.y*theta.y;
    vec2 theta1 = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq + HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);
    return LensCenter + Scale*theta1;
}

void main(void)
{
  vec2 tc; 
  if(warpTexture)
  {
    tc = hmdWarp(f_texcoord);
  }
  else
  {
    tc = f_texcoord;
  }
  gl_FragColor = texture2D(texture, tc);
}
