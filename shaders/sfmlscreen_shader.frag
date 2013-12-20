uniform vec2 LensCenter;
uniform vec4 HmdWarpParam;
uniform float Scale;

varying vec2 f_texcoord;

uniform sampler2D texture;

vec2 hmdWarp(vec2 in01)
{
    vec2 theta = (in01 - LensCenter);
    float rSq = theta.x*theta.x + theta.y*theta.y;
    vec2 theta1 = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq + HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);
    return LensCenter + Scale*theta1;
}

void main(void)
{
//  vec2 flipped_texcoord = vec2(f_texcoord.x, 1.0 - f_texcoord.y);
  vec2 tc = hmdWarp(f_texcoord);
  gl_FragColor = texture2D(texture, tc);
}
