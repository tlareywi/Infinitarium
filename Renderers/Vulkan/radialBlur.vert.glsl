//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved.
//

struct VertexOut {
    vec2 uv;
};

struct VertexFlat {
    vec2 screenLightPos;
    float density;
    float weight;
    float decay;
    float exposure;
    float saturation;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out VertexOut vertOut;
layout(location = 1) out VertexFlat vertOutFlat;

void main() {
    vec4 pos = vec4(position, 1.0);
    gl_Position = pos;

    vertOut.uv = uv;

    vertOutFlat.density = uniforms.density;
    vertOutFlat.weight = uniforms.weight;
    vertOutFlat.decay = uniforms.decay;
    vertOutFlat.exposure = uniforms.exposure;
    vertOutFlat.saturation = uniforms.saturation;
    vertOutFlat.screenLightPos.x = uniforms.bloomCenter.x;
    vertOutFlat.screenLightPos.y = -uniforms.bloomCenter.y;
}