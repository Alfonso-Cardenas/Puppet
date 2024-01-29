#version 330 core

out vec4 fragColor;

in vec3 color;
uniform vec4 alpha;

void main(){
   fragColor = vec4(color,0.0f) + alpha;
}