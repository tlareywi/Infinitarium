//
//  Shaders.metal
//  MetalTest
//
//  Created by Trystan Larey-Williams on 5/28/18.
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//

// File for Metal kernel and shader functions

#include <metal_stdlib>
//#include <simd/simd.h>

// Including header shared between this Metal shader code and Swift/C code executing Metal API commands
//#import "ShaderTypes.h"

using namespace metal;

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
};

struct ConstUniforms {
   float4x4 modelViewProjectionMatrix;
};

constant float epsilon = 0.0000000001;
constant float diskDensity = 0.025;
constant float haloDensity = 400.0;

vertex VertexOut staticInstancedStarsVert( constant CartesianPosition* pos [[buffer(2)]],
                                      constant Magnitude* V [[buffer(1)]],
                                      constant ColorRGB* color [[buffer(0)]],
                                      constant ConstUniforms& uniforms [[buffer(3)]],
                                      uint instance [[instance_id]]
) {
   VertexOut out;
   out.position = uniforms.modelViewProjectionMatrix * float4( pos[instance].xyz, 1.0 );
   float3 rgb = color[instance].rgb;
   out.color = float4( rgb, 1.0 );

   out.brightness = pow( 10.0, (-14.18 - V[instance].m) / 2.54 ) * 6000000.0;
   
   float diskRadius = -log( epsilon / out.brightness ) * diskDensity;
   float blurRadius = pow( (out.brightness / epsilon - 1.0) / haloDensity, 1.0 / 2.0 );
   
   out.pointSize = max(diskRadius, blurRadius) * 2.0;
   out.pointSize = min(out.pointSize, 80.0);
   
   return out;
}

fragment float4 staticInstancedStarsFrag( VertexOut point [[stage_in]],
                                         float2 pointCoord [[point_coord]],
                                         constant ConstUniforms& uniforms [[buffer(3)]]
) {
   float lengthSquared = distance(float2(0.5f), pointCoord);
   
   float disk = point.brightness * exp(-lengthSquared / diskDensity);
   float psf = point.brightness / (haloDensity * pow(lengthSquared, 2.0) + 1.0);
   
   return float4( point.color.b, point.color.g, point.color.r, psf + disk);
}

//
// Minimal passthrough shaders
//
typedef struct
{
    float4 position [[position]];
} ColorInOut;

vertex ColorInOut vertexShader()
{
    ColorInOut out;

    float4 position = float4(0.0,0.0,0.0,1.0);
    out.position = position;

    return out;
}

fragment float4 fragmentShader(ColorInOut in [[stage_in]])
{
    return float4(1.0);
}
