//
//  Copyright © 2020 Blue Canvas Studios LLC. All rights reserved.
//
//  Uncomment below for glslangValidator.exe  .\[name].[type].glsl - o .\[name].[type].spirv - V
/*  #version 450
    layout(binding = 0) uniform ConstUniforms {
        mat4 modelViewProjectionMatrix;
        uvec2 viewport;
    } uniforms;
*/

struct VertexOut {
    vec2 uv;
    float brightness;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out VertexOut vertOut;

void main() {
    gl_Position = uniforms.modelViewProjectionMatrix * vec4(position, 1.0);
    gl_Position.y = -gl_Position.y;

    vertOut.brightness = uniforms.brightness;
    vertOut.uv = uv;
}