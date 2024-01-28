#version 400 core

layout(location = 0) in vec2 pPosition;
layout(location = 1) in vec2 tex;

uniform vec2 trans;

uniform float layerPosition;

uniform mat4 mat;

out vec2 texCoords;

void main()
{
    vec4 pos = vec4(pPosition.xy, layerPosition, 1.0) * mat;
    gl_Position = vec4(pos.xy + trans, pos.zw);

    texCoords = tex;
}