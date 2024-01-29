#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;

out vec3 color;

out vec3 Normal;
out vec3 crntPos;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 rot;
uniform mat4 liveModel;
uniform mat4 liveRot;
uniform mat4 globalModel;
uniform mat4 globalRot;
uniform mat4 globalTrans;

void main()
{
	crntPos = vec3(globalTrans * globalModel * liveModel * model * vec4 (aPos, 1.0f));
	gl_Position = camMatrix * vec4(crntPos, 1.0f);
	color = aColor;
	Normal = vec3(globalRot * liveRot * rot * vec4 (aNormal, 1.0f));
}