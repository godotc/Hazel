/**
 * @ Author: godot42
 * @ Create Time: 2024-08-13 00:16:31
 * @ Modified by: @godot42
 * @ Modified time: 2024-08-21 23:20:47
 * @ Description:
 */


// render2d circle shader

#type vertex
#version 450 core

layout (location = 0) in vec3  a_WorldPosition;
layout (location = 1) in vec3  a_LocalPosition;
layout (location = 2) in vec4  a_Color;
layout (location = 3) in float a_Thickness;
layout (location = 4) in float a_Fade;
layout (location = 5) in int   a_EntityId;


// constant in each scene(Cmaera location and Perspec)
layout(std140, binding=0) uniform Camera{
    mat4 u_ViewProjection;
};


struct VertexOutput
{
    vec3  LocalPostion;
    vec4  Color;
    float Thickness;
    float Fade;
};
layout( location = 0) out VertexOutput Output;
layout( location = 4) out flat int v_EntityId;

void main(){
    Output.LocalPostion = a_LocalPosition;
    Output.Color = a_Color;
    Output.Thickness = a_Thickness;
    Output.Fade = a_Fade;

    v_EntityId = a_EntityId;

    gl_Position = u_ViewProjection * vec4(a_WorldPosition, 1.f);
}

//------------------------

#type fragment
#version 450 core

struct VertexOutput
{
    vec3  LocalPostion;
    vec4  Color;
    float Thickness;
    float Fade;
};
layout( location =0) in VertexOutput Input;
layout( location =4) in flat int v_EntityId;

// each in different color attachment
layout (location=0) out vec4 o_Color;
layout (location=1) out int o_EntityId;//-1 + id +1

void main()
{
    float distance = 1.0 - length(Input.LocalPostion);
    float circle = smoothstep(0.0, Input.Fade, distance); // alpha?
    circle *= smoothstep(Input.Thickness + Input.Fade, Input.Thickness, distance);


    // drop enity id
    if(circle==0.0){
        discard;
    }

    o_Color = Input.Color;
    o_Color.a *= circle;

    o_EntityId = v_EntityId;
}
