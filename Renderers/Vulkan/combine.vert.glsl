//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved.
//

struct VertexOut {
    vec2 uv;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out VertexOut vertOut;

void main() {
    vec4 pos = vec4(position, 1.0);
    gl_Position = pos;

    vertOut.uv = uv;
}

