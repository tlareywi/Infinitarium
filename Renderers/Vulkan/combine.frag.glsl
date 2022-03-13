//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved.
//

struct FragmmentIn {
    vec2 uv;
};

layout(location = 0) in FragmmentIn fragment;
layout(binding = 3) uniform sampler2D source;
layout(binding = 4) uniform sampler2D radialblur;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 srcColor = texture(source, fragment.uv);
    vec4 radialColor = texture(radialblur, fragment.uv);

    float srcMagnitude = dot(srcColor.rgb, vec3(1.0));

    if (srcMagnitude > 0.0)
        outColor.rgb = mix(srcColor.rgb, radialColor.rgb, 0.5);
    else
        outColor.rgb = radialColor.rgb;

    outColor.a = srcColor.a;
}

