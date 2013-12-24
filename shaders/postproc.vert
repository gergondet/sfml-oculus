uniform mat4 view;
attribute vec2 coord2d;

varying vec2 f_texcoord;

void main(void)
{
  gl_Position = view*vec4(coord2d, 0.0, 1.0);
  f_texcoord = (coord2d+1.0)/2.0;
}

