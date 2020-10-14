//
//  Copyright © 2019 Blue Canvas Studios LLC. All rights reserved.
//

struct VertexIn {
   packed_float3 vert;
   packed_float3 normal;
   packed_float2 texCoord;
};

struct VertexOut {
   float4 position [[position]];
   float2 texCoord;
};

vertex VertexOut vertexShader( uint vertexID [[ vertex_id ]],
                              constant VertexIn* vert [[buffer(0)]],
                              constant ConstUniforms& uniforms [[buffer(1)]] ) {
   VertexOut out;
   out.position = uniforms.modelViewProjectionMatrix * float4( vert[vertexID].vert, 1.0 );
   out.texCoord = vert[vertexID].texCoord;
   
   return out;
}

fragment float4 fragmentShader( VertexOut in [[stage_in]],
                               texture2d<float, access::sample> colorTexture [[ texture(0) ]] ) {
   
   constexpr sampler textureSampler( mag_filter::linear, min_filter::linear );
   const float4 colorSample = colorTexture.sample( textureSampler, in.texCoord );
   // The fact that we need to do * 0.75 is concerning. Hardware sRGB conversion problem? Colors are blown out without this.
   // Color profile for text image is sRGB IEC61966-2.1 (jupiter2_1k).
   return float4(colorSample.bgr * 0.75, colorSample.a);
}




