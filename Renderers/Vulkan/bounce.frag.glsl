//
// Bounce offscreen render target to another render target
//

struct FragmmentIn {
    vec2 uv;
};

layout(location = 0) in FragmmentIn fragment;
layout(binding = 3) uniform sampler2D source;
layout(binding = 4) uniform sampler2D blur;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(blur, fragment.uv);
}

