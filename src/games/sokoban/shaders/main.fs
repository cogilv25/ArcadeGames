#version 400 core

in vec2 texCoords;
out vec4 fColour;

uniform sampler2D tex;

void main()
{
    fColour = texture(tex,texCoords);
} 