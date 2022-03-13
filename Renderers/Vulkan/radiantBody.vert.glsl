//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved.
//

struct VertexOut {
    vec2 uv;
};

struct FlatOuts {
    uint stride;
    float refTime;
    float frequency;
    float amplitude;
    float speed;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out VertexOut vertOut;
layout(location = 1) flat out FlatOuts vertFlats;

void main() {
    gl_Position = uniforms.modelViewProjectionMatrix * vec4(position, 1.0);
    gl_Position.y = -gl_Position.y;
    vertOut.uv = uv;

    vertFlats.refTime = uniforms.referenceTime;
    vertFlats.frequency = uniforms.frequency;
    vertFlats.amplitude = uniforms.amplitude;
    vertFlats.speed = uniforms.speed;
    vertFlats.stride = uint(uniforms.viewport.x);
}