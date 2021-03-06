#version 440 core

layout(location = 0) in vec3 vertices;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(vertices, 1.f);
}