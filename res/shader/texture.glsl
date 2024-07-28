#type vertex
#version 450 core

layout (location =0) in vec3 a_Position;
layout (location =1) in vec4 a_Color;
layout (location =2) in vec2 a_TexCoord;
layout (location =3) in float a_TexIndex;
layout (location =4) in float a_TilingFactor;
layout (location =5) in int a_EntityId;


// constant in each scene(Cmaera location and Perspec)
layout(std140, binding=0) uniform Camera{
    mat4 u_ViewProjection;
};
// constant for each object, now each object need one draw call
// no need to pass it on draw each shape
/* Vulkan spec same as below in glsl
struct Transform{...fields...};
uniform Transform u_RendererUniforms;
*/
// layout(push_constant) uniform Transform{
//     mat4 Transform;
// } u_RendererUniforms;

struct VertexOutput{
    vec4 Color;
    vec2 TexCoord;
    float TexIndex;
    float TilingFactor;
};
layout( location =0) out VertexOutput Output;
layout( location =4) out flat int v_EntityId;

void main(){
    Output.Color = a_Color;
    Output.TexCoord = a_TexCoord;
    Output.TexIndex = a_TexIndex;
    Output.TilingFactor = a_TilingFactor;
    v_EntityId = a_EntityId;

#if OPENGL
#endif
    gl_Position = u_ViewProjection *
                //   u_RendererUniforms.Transform *
                  vec4(a_Position, 1.f);
}

//------------------------

#type fragment
#version 450 core

struct VertexOutput{
    vec4 Color;
    vec2 TexCoord;
    float TexIndex;
    float TilingFactor;
};
layout( location =0) in VertexOutput Input;
layout( location =4) in flat int v_EntityId;

layout(binding = 0) uniform sampler2D u_Textures[32];

// each in different color attachment
layout (location=0) out vec4 color;
layout (location=1) out int color2;//-1 + id +1




#define CASE(x) case x: the_texture = texture(u_Textures[x], Input.TexCoord * Input.TilingFactor); break;

// #define MULTI_MACRO_HELPER(last) CASE(last)
// #define MULTI_MACRO_HELPER(first,...) CASE(first) EXPAND_ARGS(__VA_ARGS__)
// #define EXPAND_ARGS(...) MULTI_MACRO_HELPER(__VA_ARGS__)
// #define MULTI_MACRO(...) EXPAND_ARGS(__VA_ARGS__)

void main(){
    vec4 the_texture;
    switch (int(Input.TexIndex)){
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

    color =  the_texture * Input.Color;
    color2 = v_EntityId;
}