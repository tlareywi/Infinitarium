//
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//
//  glslangValidator.exe  .\starsDefault.vert.glsl -o .\starsDefault.vert.spirv -V
//  #version 450

 #extension GL_EXT_debug_printf : enable

struct ColorData
{
    float r;
    float g;
    float b;
};

struct MagnitudeData
{
    float m;
};

struct PositionData
{
    float x;
    float y;
    float z;
};

layout(std430, binding = 1) buffer ColorRGB
{
   ColorData color[];
};

layout(std430, binding = 2) buffer Magnitude
{
   MagnitudeData V[];
};

layout(std430, binding = 3) buffer CartesianPosition
{
   PositionData pos[];
};

/*layout(binding = 0) uniform ConstUniforms {
   mat4 modelViewProjectionMatrix;
   mat4 modelViewMatrix;
   uvec2 viewport;
   float epsilon;
   float diskDensity;
   float haloDensity;
   float limitingMagnitude;
   float saturationMagnitude;
   float diskBrightness;
   float haloBrightness;
} uniforms; */

struct VertexOut {
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

layout(location = 0) flat out VertexOut vertOut;

///
/// All shaders have ConstUniforms injected at compile time based on uniforms defined in scene file.
///

void main() {
   const vec3 eye = vec3(0, 0, 0);
   vec3 position = vec3( pos[gl_InstanceIndex].x, pos[gl_InstanceIndex].y, pos[gl_InstanceIndex].z );
   gl_Position = uniforms.modelViewProjectionMatrix * vec4( position, 1.0 );
   gl_Position.y = -gl_Position.y;
   
   vec3 rgb = vec3(color[gl_InstanceIndex].r, color[gl_InstanceIndex].g, color[gl_InstanceIndex].b);
   vertOut.color = vec4( rgb, 1.0 );
   
   float distParsecs = distance( position, eye );
   float appMag = 5.0 * (log(distParsecs/10.0) / log(10.0)) + V[gl_InstanceIndex].m;

   // vertOut.brightness = pow(2.512, (appMag - uniforms.saturationMagnitude) / (uniforms.saturationMagnitude+0.00001) );
   vertOut.brightness = pow(2.512, (appMag - -2.0) / (-2.0+0.00001) );
   
   vertOut.diskDensity = uniforms.diskDensity * uniforms.diskDensity;
   vertOut.haloDensity = uniforms.haloDensity * uniforms.haloDensity;
   
   float diskRadius = -log(uniforms.epsilon / (uniforms.diskBrightness * vertOut.brightness)) * 2.0 * vertOut.diskDensity;
   float blurRadius = -log(uniforms.epsilon / (uniforms.haloBrightness * vertOut.brightness)) * 2.0 * vertOut.haloDensity;
   
   gl_PointSize = vertOut.pointSize = 2.0 * sqrt(max(diskRadius, blurRadius));
   vertOut.diskBrightness = uniforms.diskBrightness;
   vertOut.haloBrightness = uniforms.haloBrightness;
   
   vec2 ndcPosition = gl_Position.xy / gl_Position.w;
   vertOut.pointCenter = (ndcPosition * 0.5 + vec2(0.5, 0.5)) * vec2(uniforms.viewport);

   //debugPrintfEXT("pointCenter:%f pointCenter:%f\n", vertOut.pointCenter.x, vertOut.pointCenter.y );
   
   vertOut.id = gl_InstanceIndex;
}


