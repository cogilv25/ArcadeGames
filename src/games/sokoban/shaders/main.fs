#version 400 core

in vec2 texCoords;
out vec4 fColour;

uniform sampler2D tex;

void main()
{
    //fColour = vec4(0.0f,0.0f,0.0f,1.0f);
    fColour = texture(tex,texCoords);
} 