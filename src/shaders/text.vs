#version 400 core

layout(location = 0) in vec2 pPosition;
layout(location = 1) in vec3 pColour;
layout(location = 2) in vec2 tc;
out vec2 texCoord;
out vec3 vColour;

void main()
{
    gl_Position = vec4(pPosition.xy, -0.1, 1.0);

    vColour = pColour;
    texCoord = tc;
}