//
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//

struct CartesianPosition {
   vec3 xyz;
};

struct ColorRGB {
   vec3 rgb;
};

struct Magnitude {
   float m;
};

layout(location = 0) in ColorRGB color;
layout(location = 1) in Magnitude V;
layout(location = 2) in CartesianPosition pos;

struct VertexOut {
   vec4 position;
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

in int gl_InstanceID;

const vec3 eye(0, 0, 0);

///
/// All shaders have ConstUniforms injected at compile time based on uniforms defined in scene file.
///

void main() {
   float3 position( pos[gl_InstanceID].xyz );
   vertOut.position = uniforms.modelViewProjectionMatrix * float4( position, 1.0 );
   
   float3 rgb = color[gl_InstanceID].rgb;
   vertOut.color = float4( rgb, 1.0 );
   
   float distParsecs = distance( position, eye );
   float appMag = 5.0 * log10(distParsecs/10.0) + V[gl_InstanceID].m;

   vertOut.brightness = pow(2.512, (appMag - uniforms.saturationMagnitude) / (uniforms.saturationMagnitude+0.00001) );
   //out.brightness = pow(2.512, (uniforms.limitingMagnitude - uniforms.saturationMagnitude) * (uniforms.saturationMagnitude - V[instance].m));
   
   vertOut.diskDensity = uniforms.diskDensity * uniforms.diskDensity;
   vertOut.haloDensity = uniforms.haloDensity * uniforms.haloDensity;
   
   float diskRadius = -log(uniforms.epsilon / (uniforms.diskBrightness * vertOut.brightness)) * 2.0 * vertOut.diskDensity;
   float blurRadius = -log(uniforms.epsilon / (uniforms.haloBrightness * vertOut.brightness)) * 2.0 * vertOut.haloDensity;
   
   vertOut.pointSize = 2.0 * sqrt(max(diskRadius, blurRadius));
   vertOut.diskBrightness = uniforms.diskBrightness;
   vertOut.haloBrightness = uniforms.haloBrightness;
   
   float2 ndcPosition = vertOut.position.xy / vertOut.position.w;
   vertOut.pointCenter = (ndcPosition * 0.5 + float2(0.5, 0.5)) * float2(uniforms.viewport);
   vertOut.pointCenter.y = uniforms.viewport.y - out.pointCenter.y; // Metal's window coords have flipped y-axis compared to OpenGL
   
   vertOut.id = gl_InstanceID;
   
   return vertOut;
}


