//
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//

typedef struct {
   packed_float3 xyz;
} CartesianPosition;

typedef struct {
   packed_float3 rgb;
} ColorRGB;

typedef struct {
   float m;
} Magnitude;

struct VertexOut {
   float4 position [[position]];
   float pointSize [[point_size]];
   float4 color [[flat]];
   float brightness [[flat]];
   float diskDensity [[flat]];
   float haloDensity [[flat]];
   float haloBrightness [[flat]];
   float diskBrightness [[flat]];
   float2 pointCenter [[flat]];
};

///
/// All shaders have ConstUniforms injected at compile time based on uniforms defined in scene file.
///

vertex VertexOut vertexShader( constant CartesianPosition* pos [[buffer(2)]],
                              constant Magnitude* V [[buffer(1)]],
                              constant ColorRGB* color [[buffer(0)]],
                              constant ConstUniforms& uniforms [[buffer(3)]],
                              uint instance [[instance_id]]) {
   VertexOut out;
   float3 position( pos[instance].xyz );
   out.position = uniforms.modelViewProjectionMatrix * float4( position, 1.0 );
   
   float3 rgb = color[instance].rgb;
   out.color = float4( rgb, 1.0 );
   
   out.brightness = pow(2.512, (V[instance].m - uniforms.saturationMagnitude) / (uniforms.saturationMagnitude+0.00001) );
   
   out.diskDensity = uniforms.diskDensity * uniforms.diskDensity;
   out.haloDensity = uniforms.haloDensity * uniforms.haloDensity;
   
   float diskRadius = -log(uniforms.epsilon / (uniforms.diskBrightness * out.brightness)) * 2.0 * out.diskDensity;
   float blurRadius = -log(uniforms.epsilon / (uniforms.haloBrightness * out.brightness)) * 2.0 * out.haloDensity;
   
   out.pointSize = 2.0 * sqrt(max(diskRadius, blurRadius));
   out.diskBrightness = uniforms.diskBrightness;
   out.haloBrightness = uniforms.haloBrightness;
   
   float2 ndcPosition = out.position.xy / out.position.w;
   out.pointCenter = (ndcPosition * 0.5 + float2(0.5, 0.5)) * float2(uniforms.viewport);
   out.pointCenter.y = 1080 -  out.pointCenter.y; // Metal's window coords have flipped y-axis compared to OpenGL
   
   return out;
}

fragment float4 fragmentShader( VertexOut point [[stage_in]] ) {
   float2 offset = point.position.xy - point.pointCenter;
   float len = dot(offset, offset);
   
   float disk = point.diskBrightness * point.brightness * exp(-len / (2.0 * point.diskDensity));
   float psf = point.haloBrightness * point.brightness * exp(-len / (2.0 * point.haloDensity));
   
   return float4( point.color.rgb * (disk+psf), 1.0 );
}



