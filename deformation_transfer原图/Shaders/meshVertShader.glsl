#version 440 core

layout(location = 0) in vec3 vertices;
layout(location = 1) in vec3 normals;
uniform mat4 MVP;

out vec3 middle_colors;;
void main()
{
	gl_Position = MVP * vec4(vertices, 1.f);
	middle_colors = normals;
}