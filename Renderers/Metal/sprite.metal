//
//  Copyright © 2019 Blue Canvas Studios LLC. All rights reserved.
//

struct VertexIn {
   float2 pos;
   float2 texCoord;
};

struct VertexOut {
   float4 position [[position]];
   float4 color;
};

///
/// All shaders have ConstUniforms injected at compile time based on uniforms defined in scene file.
///

vertex VertexOut vertexShader( constant VertexIn* vert [[buffer(2)]] ) {
   VertexOut out;
   float3 position( pos[instance].xyz );
   out.position = uniforms.modelViewProjectionMatrix * float4( vert.pos, 1.0 );
   
   return out;
}

fragment float4 fragmentShader( VertexOut point [[stage_in]] ) {
   float2 offset = point.position.xy - point.pointCenter;
   float len = dot(offset, offset);
   
   float disk = point.diskBrightness * point.brightness * exp(-len / (2.0 * point.diskDensity));
   float psf = point.haloBrightness * point.brightness * exp(-len / (2.0 * point.haloDensity));
   
   return float4( point.color.rgb * (disk+psf), 1.0 );
}


