#version 400 core

in vec3 vColour;
out vec4 fColour;

void main()
{
    fColour = vec4(vColour,1.0f);
} 