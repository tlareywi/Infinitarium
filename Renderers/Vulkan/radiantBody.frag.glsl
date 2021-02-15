//
//  Copyright © 2020 Blue Canvas Studios LLC. All rights reserved.
//

//  Uncomment below for glslangValidator.exe  .\[name].[type].glsl - o .\[name].[type].spirv - V
//#version 450

struct FragmmentIn {
    vec2 uv;
};


struct FragmmentFlats {
    uint stride;
    float refTime;
    float frequency;
    float amplitude;
    float speed;
};

struct PickData
{
    float brightness;
    uint  objId;
};

struct PostData
{
    float intensity;
};

layout(std430, binding = 1) buffer Pick
{
    PickData pick[];
};
layout(std430, binding = 2) buffer PostProcess
{
    PostData post[];
};
layout(binding = 3) uniform sampler2D colorMap;

layout(location = 0) in FragmmentIn fragment;
layout(location = 1) flat in FragmmentFlats flats;

layout(location = 0) out vec4 color;

void main() {
    float distortion = sin(fragment.uv.y * flats.frequency + flats.refTime * flats.speed) * flats.amplitude;
    color = texture(colorMap, vec2(fragment.uv.x + distortion, fragment.uv.y));
    color = vec4(color.rgb, color.a);

    uint indx = uint(gl_FragCoord.y * flats.stride + gl_FragCoord.x);
    pick[indx].objId = 1; // TODO temporary. Currently collides with star id
    post[indx].intensity = 1;
}