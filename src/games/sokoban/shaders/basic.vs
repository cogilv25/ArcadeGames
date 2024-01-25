#version 400 core

layout(location = 0) in vec2 pPosition;
layout(location = 1) in vec3 pColour;

uniform vec3 trans;

uniform mat4 mat;

out vec3 vColour;

void main()
{
    vec4 pos = vec4(pPosition.x, pPosition.y, 0.0, 1.0) * mat;
    gl_Position = vec4(pos.xyz + trans, pos.w);

    vColour = pColour;
}