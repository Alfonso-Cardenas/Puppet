#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 acolor;

out vec3 color;

uniform mat4 camMatrix;
uniform mat4 trans;

void main(){
   vec3 curpos = vec3 (trans * vec4 (aPos, 1.0f));
   gl_Position = camMatrix * vec4 (curpos, 1.0f);
   color = acolor;
}