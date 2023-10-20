#type vertex
#version 330 core

layout (location =0) in vec3 a_Position;
layout (location =1) in vec4 a_Color;
layout (location =2) in vec2 a_TexCoord;

out vec4 v_Color;
out vec2 v_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main(){
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    // gl_Position = u_ViewProjection *  u_Transform * vec4(a_Position, 1.f);
    gl_Position = u_ViewProjection *  vec4(a_Position, 1.f);
}

//------------------------

#type fragment
#version 330 core

out vec4 v_Color;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform vec4 u_Color;
uniform float u_Tiling;

out vec4 out_color;

void main(){
    // out_color =  texture(u_Texture, v_TexCoord * u_Tiling) * u_Color;
    out_color = u_Color;
    // out_color = vec4(v_TexCoord, 0 ,1);
}