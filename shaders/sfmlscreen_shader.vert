uniform mat4 transform;
attribute vec3 coord3d;
attribute vec2 texcoord;

varying vec2 f_texcoord;

void main(void)
{
  gl_Position = transform*vec4(coord3d, 1.0);
  f_texcoord = texcoord;
}
