//
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
// 
//  glslangValidator.exe  .\starsDefault.frag.glsl -o .\starsDefault.frag.spirv -V
//  #version 450

struct VertexIn {
	float pointSize;
	vec4 color;
	float brightness;
	float diskDensity;
	float haloDensity;
	float haloBrightness;
	float diskBrightness;
	vec2 pointCenter;
	uint id;
};

layout(location = 0) flat in VertexIn point;

layout(location = 0) out vec4 color;

void main() {
	vec2 offset = gl_FragCoord.xy - point.pointCenter;
	float len = dot(offset, offset);

	float disk = point.diskBrightness * point.brightness * exp(-len / (2.0 * point.diskDensity));
	float psf = point.haloBrightness * point.brightness * exp(-len / (2.0 * point.haloDensity));

	color = vec4(point.color.rgb * (disk + psf), 1.0);
}