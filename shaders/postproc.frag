uniform sampler2D fbo_texture;
varying vec2 f_texcoord;

/* Parameters for distortion */
uniform vec2 LensCenter;
uniform vec4 HmdWarpParam;
uniform vec2 Scale;
uniform vec2 ScaleInv;

uniform bool warpTexture;

vec2 hmdWarp(vec2 in01)
{
    vec2 theta = (in01 - 0.5 - LensCenter)*ScaleInv;
    float rSq = theta.x*theta.x + theta.y*theta.y;
    vec2 theta1 = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq + HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);
    return 0.5 + LensCenter + Scale*theta1;
}

 
void main(void) {
  if(warpTexture)
  {
    vec2 tc = hmdWarp(f_texcoord);
    if(!all(equal(clamp(tc, vec2(0,0), vec2(1,1)), tc)))
    {
        gl_FragColor = vec4(0);
    }
    else
    {
        gl_FragColor = texture2D(fbo_texture, tc);
    }
  }
  else
  {
    gl_FragColor = texture2D(fbo_texture, f_texcoord);
  }
}
