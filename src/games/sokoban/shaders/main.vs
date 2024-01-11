#version 400 core

layout(location = 0) in vec2 pPosition;
layout(location = 1) in vec2 tex;

uniform vec3 trans;

out vec2 texCoords;

void main()
{
    vec3 pos = vec3(pPosition.xy, -0.9) + trans;
    gl_Position = vec4(pos, 1.0);

    texCoords = tex;
}