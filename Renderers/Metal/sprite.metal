//
//  Copyright © 2019 Blue Canvas Studios LLC. All rights reserved.
//

struct VertexIn {
   packed_float3 pos;
   packed_float2 texCoord;
};

struct VertexOut {
   float4 position [[position]];
   float2 texCoord;
};

vertex VertexOut vertexShader(uint vertexID [[ vertex_id ]],
                              constant VertexIn* vert [[buffer(0)]],
                              constant ConstUniforms& uniforms [[buffer(1)]] ) {
   VertexOut out;
   out.position = float4( vert[vertexID].pos, 1.0 );
   out.position.xy = out.position.xy / (float2(uniforms.viewport) / 2.0) * 26.0;
   
   out.texCoord = vert[vertexID].texCoord;
   
   return out;
}

fragment float4 fragmentShader( VertexOut in [[stage_in]],
                                texture2d<half> colorTexture [[ texture(0) ]] ) {
   
   constexpr sampler textureSampler( mag_filter::linear, min_filter::linear );
   const half4 colorSample = colorTexture.sample( textureSampler, in.texCoord );
   return float4(colorSample);
}


