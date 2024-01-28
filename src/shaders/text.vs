#version 400 core

layout(location = 0) in vec2 pPosition;
layout(location = 1) in vec3 pColour;
layout(location = 2) in vec2 tc;
out vec2 texCoord;
out vec3 vColour;

uniform vec2 trans;

void main()
{

    gl_Position = vec4(pPosition.x + trans.x, pPosition.y - trans.y, -0.2, 1.0);

    vColour = pColour;
    texCoord = tc;
}