//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved.
//
//  Uncomment below for glslangValidator.exe  .\[name].[type].glsl - o .\[name].[type].spirv - V
//  #version 450


// Adapted from the work of Daniel Rakos
// http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/


struct FragmmentIn {
    vec2 uv;
};

struct Viewport {
    float w;
    float h;
};

layout(location = 0) in FragmmentIn fragment;
layout(location = 1) flat in Viewport viewport;

layout(binding = 3) uniform sampler2D source;

layout(location = 0) out vec4 outColor;

float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main() {
    vec3 tc;
    tc = texture(source, fragment.uv).rgb * weight[0];

    for (int i = 1; i < 3; i++) {
        tc += texture(source, fragment.uv + vec2(offset[i] / viewport.w, 0.0)).rgb * weight[i];
        tc += texture(source, fragment.uv - vec2(offset[i] / viewport.w, 0.0)).rgb * weight[i];
    }

    outColor = vec4(tc, 1.0);
}