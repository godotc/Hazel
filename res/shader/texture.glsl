#type vertex
#version 330 core

layout (location =0) in vec3 a_Position;
layout (location =1) in vec4 a_Color;
layout (location =2) in vec2 a_TexCoord;
layout (location =3) in float a_TexIndex;
layout (location =4) in float a_TilingFactor;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexIndex;
out float v_TilingFactor;


uniform mat4 u_ViewProjection;
// uniform mat4 u_Transform;

void main(){
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
    v_TilingFactor = a_TilingFactor;
    // gl_Position = u_ViewProjection *  u_Transform * vec4(a_Position, 1.f);
    gl_Position = u_ViewProjection *  vec4(a_Position, 1.f);
}

//------------------------

#type fragment
#version 330 core

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;
in float v_TilingFactor;

uniform vec4 u_Color;
uniform sampler2D u_Textures[32];

out vec4 color;



#define CASE(x)  case x: the_texture = texture(u_Textures[x] , v_TexCoord * v_TilingFactor); break;
// #define CASE(x)  case x: color = texture(u_Textures[x] , v_TexCoord * v_TilingFactor) * u_Color; break;

// #define MULTI_MACRO_HELPER(last) CASE(last)
// #define MULTI_MACRO_HELPER(first,...) CASE(first) EXPAND_ARGS(__VA_ARGS__)
// #define EXPAND_ARGS(...) MULTI_MACRO_HELPER(__VA_ARGS__)
// #define MULTI_MACRO(...) EXPAND_ARGS(__VA_ARGS__)

void main(){
    vec4 the_texture;
    switch(int(v_TexIndex)){
    //     // MULTI_MACRO(0,1,2,3,4,5,6);
        CASE(0); 
        CASE(1);
        CASE(2);
        CASE(3);
        CASE(4);
        CASE(5);
        CASE(6);
        CASE(7);
        CASE(8);
        CASE(9);
        CASE(10);
        CASE(11);
        CASE(12);
        CASE(13);
        CASE(14);
        CASE(15);
        CASE(16);
        CASE(17);
        CASE(18);
        CASE(19);
        CASE(20);
        CASE(21);
        CASE(22);
        CASE(23);
        CASE(24);
        CASE(25);
        CASE(26);
        CASE(27);
        CASE(28);
        CASE(29);
        CASE(30);
        CASE(31);
        // CASE(32);
    }

    color =  the_texture * u_Color;
    color = vec4(v_TexCoord, 0, 1);
}