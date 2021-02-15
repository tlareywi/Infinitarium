//
//  Copyright © 2020 Blue Canvas Studios LLC. All rights reserved.
//

//  Uncomment below for glslangValidator.exe  .\[name].[type].glsl - o .\[name].[type].spirv - V
//  #version 450

struct FragmmentIn {
    vec2 uv;
};

layout(location = 0) in FragmmentIn fragment;
layout(binding = 3) uniform sampler2D colorMap;

layout(location = 0) out vec4 color;

void main() {
    color = texture(colorMap, fragment.uv);
}



