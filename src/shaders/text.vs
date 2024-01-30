#version 400 core

layout(location = 0) in vec2 vPos;
layout(location = 1) in vec2 texCoordVert;

out vec2 texCoordFrag;
uniform vec3 position;

void main()
{
    gl_Position = vec4(vPos.xy + position.xy, position.z, 1.0);
    texCoordFrag = texCoordVert;
}