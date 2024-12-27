#type vertex
#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in float a_TexIndex;
layout (location = 4) in float a_TilingFactor;
layout (location = 5) in int a_EntityId;


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

struct VertexOutput
{
    vec4  Color;
    vec2  TexCoord;
    float TilingFactor;
};
layout( location = 0) out VertexOutput Output;

// flat: means no need to interpolate
layout( location = 3) out flat float v_TexIndex;
layout( location = 4) out flat int v_EntityId;

void main(){
    Output.Color = a_Color;
    Output.TexCoord = a_TexCoord;
    Output.TilingFactor = a_TilingFactor;

    v_TexIndex = a_TexIndex;
    v_EntityId = a_EntityId;

    gl_Position = u_ViewProjection *
                //   u_RendererUniforms.Transform *
                  vec4(a_Position, 1.f);
}

//------------------------

#type fragment
#version 450 core

struct VertexOutput
{
    vec4  Color;
    vec2  TexCoord;
    float TilingFactor;
};
layout( location =0) in VertexOutput Input;
layout( location =3) in flat float v_TexIndex;
layout( location =4) in flat int v_EntityId;

layout(binding = 0) uniform sampler2D u_Textures[32];

// each in different color attachment
layout (location=0) out vec4 o_Color;
layout (location=1) out int o_EntityId;//-1 + id +1




#define CASE(x) case x: the_texture = texture(u_Textures[x], Input.TexCoord * Input.TilingFactor); break;

// #define MULTI_MACRO_HELPER(last) CASE(last)
// #define MULTI_MACRO_HELPER(first,...) CASE(first) EXPAND_ARGS(__VA_ARGS__)
// #define EXPAND_ARGS(...) MULTI_MACRO_HELPER(__VA_ARGS__)
// #define MULTI_MACRO(...) EXPAND_ARGS(__VA_ARGS__)

void main(){
    o_Color = Input.Color; // I dont't know WTF must I use the Input.Color at here,
                        // Or it will be compile error by SPRIV "VertexOutput.Color is not declare in previous stage"

    vec4 the_texture;
    switch (int(v_TexIndex)){
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

    // do this to make the texture transparent
    // and make mouse picking not works when click on the transparent part of this texture 
    if(the_texture.a == 0.0){
        discard;
    }

    o_Color *=  the_texture;
    o_EntityId = v_EntityId;

}