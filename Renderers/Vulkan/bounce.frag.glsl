//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved.
//

struct FragmmentIn {
    vec2 uv;
};

layout(location = 0) in FragmmentIn fragment;
layout(binding = 3) uniform sampler2D source;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(source, fragment.uv);
}

