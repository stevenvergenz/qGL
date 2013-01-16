#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 texCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 TexCoord;
out vec4 Normal;
out vec4 Position;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	
	TexCoord = vec2(texCoord);
	Normal = view * model * vec4(normal, 0.0f);
	Position = view * model * vec4(position, 1.0f);
}
