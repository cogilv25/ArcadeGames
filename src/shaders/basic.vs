#version 400 core

layout(location = 0) in vec2 pPosition;
layout(location = 1) in vec4 pColour;

out vec4 vColour;

void main()
{
    gl_Position = vec4(pPosition.xy, 0.0, 1.0);

    vColour = pColour;
}