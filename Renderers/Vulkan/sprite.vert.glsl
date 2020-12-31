//
//  Copyright © 2020 Blue Canvas Studios LLC. All rights reserved.
//
//  Uncomment below for glslangValidator.exe  .\[name].[type].glsl - o .\[name].[type].spirv - V
/*  #version 450
    layout(binding = 0) uniform ConstUniforms {
        uvec2 viewport;
    } uniforms;
*/

struct VertexOut {
    vec2 uv;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out VertexOut vertOut;

void main() {
    vec4 pos = vec4(position, 1.0);
    pos.xy = pos.xy / (vec2(uniforms.viewport) / 2.0) * 26.0;
    gl_Position = pos;
    gl_Position.y = -gl_Position.y;

    vertOut.uv = uv;
}




