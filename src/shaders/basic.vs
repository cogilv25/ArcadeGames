#version 400 core

layout(location = 0) in vec2 pPosition;
layout(location = 1) in vec3 pColour;

uniform vec3 trans;

out vec3 vColour;

void main()
{
    vec3 pos = vec3(pPosition.xy, 0.0) + trans;
    gl_Position = vec4(pos, 1.0);

    vColour = pColour;
}