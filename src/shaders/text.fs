#version 400 core

in vec3 vColour;
in vec2 texCoord;
out vec4 fColour;

uniform sampler2D tex;

void main()
{
    float c = texture(tex, texCoord).r;
    fColour = vec4(vColour.rgb, c);
} 