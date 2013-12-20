varying vec4 vertex;

uniform sampler2D texture;

uniform mat4 modelviewprojection;

uniform bool useTexture=false;

uniform float x;
uniform float y;
uniform float w;
uniform float h;

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
        gl_FragColor =  texture2D(texture, texcoords.xy);
    }
    else
    {
        gl_FragColor = vec4(0.2, 0.5, 1.0, 1.0);
    }
}
