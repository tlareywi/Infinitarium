//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved.
//

struct VertexOut {
    vec2 uv;
};

struct VertexOutFlat {
    float vp_w;
    float vp_h;
    float radius;
    float power;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out VertexOut vertOut;
layout(location = 1) flat out VertexOutFlat vertOutFlat;

void main() {
    vec4 pos = vec4(position, 1.0);
    gl_Position = pos;

    vertOutFlat.radius = uniforms.radius;
    vertOutFlat.power = uniforms.power;
    vertOut.uv = uv;
    vertOutFlat.vp_w = float(uniforms.viewport.x);
    vertOutFlat.vp_h = float(uniforms.viewport.y);
}

