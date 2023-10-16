#type vertex
#version 330 core

layout (location =0) in vec3 a_Position;
layout (location =1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main(){
    v_TexCoord = a_TexCoord;
    gl_Position = u_ViewProjection *  u_Transform * vec4(a_Position, 1.f);
}

//------------------------

#type fragment
#version 330 core

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

out vec4 out_color;

void main(){
    out_color =  texture(u_Texture, v_TexCoord * 10.f);
    //    out_color = vec4(v_TexCoord, 0, 1);
}