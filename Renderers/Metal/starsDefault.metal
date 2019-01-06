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
   float diskDensity;
   float haloDensity;
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
   out.position = uniforms.modelViewProjectionMatrix * float4( pos[instance].xyz, 1.0 );
   float3 rgb = color[instance].rgb;
   out.color = float4( rgb, 1.0 );

   out.brightness = pow( 10.0, (-14.18 - V[instance].m) / 2.54 ) * 6000000.0;
   
   float diskRadius = -log( uniforms.epsilon / out.brightness ) * uniforms.diskDensity;
   float blurRadius = pow( (out.brightness / uniforms.epsilon - 1.0) / uniforms.haloDensity, 1.0 / 2.0 );
   
   out.pointSize = max(diskRadius, blurRadius) * 2.0;
   out.pointSize = min(out.pointSize, 80.0);
   out.diskDensity = uniforms.diskDensity;
   out.haloDensity = uniforms.haloDensity;
   
   return out;
}

fragment float4 fragmentShader( VertexOut point [[stage_in]],
                                         float2 pointCoord [[point_coord]]) {
   float lengthSquared = distance(float2(0.5f), pointCoord);
   
   float disk = point.brightness * exp(-lengthSquared / point.diskDensity);
   float psf = point.brightness / (point.haloDensity * pow(lengthSquared, 2.0) + 1.0);
   
   return float4( point.color.b, point.color.g, point.color.r, psf + disk);
}

