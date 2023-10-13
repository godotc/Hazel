//Flat Color Shader

#type vertex
#version 330 core

layout (location =0) in vec3 a_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform mat4 u_Model;

void main(){
    gl_Position = u_ViewProjection *  u_Transform * u_Model * vec4(a_Position, 1.f);
}

//------------------------

#type fragment
#version 330 core

out vec4 out_color;

uniform vec4 u_Color;


void main(){
    out_color =  u_Color;
}