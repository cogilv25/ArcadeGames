#version 400 core

in vec2 texCoordFrag;
out vec4 fColour;

uniform sampler2D tex;
uniform vec4 colour;

void main()
{
    fColour = colour;
    fColour.a *= texture(tex, texCoordFrag).r;
} 