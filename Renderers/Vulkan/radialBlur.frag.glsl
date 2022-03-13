//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved.
//

// Based on GPU Gems 3, Chapter 13. Volumetric Light Scattering as a Post-Process 
// Kenny Mitchell Electronic Arts

struct FragmmentIn {
    vec2 uv;
};

struct FragmmentFlat {
    vec2 screenLightPos;
    float density;
    float weight;
    float decay;
    float exposure;
    float saturation;
};

layout(location = 0) in FragmmentIn fragment;
layout(location = 1) flat in FragmmentFlat fragmentFlat;

layout(binding = 3) uniform sampler2D source;

layout(location = 0) out vec4 outColor;

int NUM_SAMPLES = 120;

vec3 rgbToHsl(in vec3 c) {
    // http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hslToRgb(in vec3 c) {
    // http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    vec2 texCoord = fragment.uv;

    // Calculate vector from pixel to light source in screen space.
    const vec2 normalizedPos = (fragmentFlat.screenLightPos.xy * 0.5 + 0.5);
    vec2 deltaTexCoord = (texCoord - normalizedPos);
    // Divide by number of samples and scale by control factor.   
    deltaTexCoord *= 1.0f / NUM_SAMPLES * fragmentFlat.density;
    // Store initial sample.    
    vec3 color = texture(source, texCoord).rgb;
    // Set up illumination decay factor.    
    float illuminationDecay = 1.0f;
    // Evaluate summation from Equation 3 NUM_SAMPLES iterations.    
    for (int i = 0; i < NUM_SAMPLES; i++) {
        // Step sample location along ray.     
        texCoord -= deltaTexCoord;
        // Retrieve sample at new location.    
        vec3 frag = texture(source, texCoord).rgb;
        // Apply sample attenuation scale/decay factors.     
        frag *= illuminationDecay * fragmentFlat.weight;
        // Accumulate combined color.     
        color += frag;
        // Update exponential decay factor.     
        illuminationDecay *= fragmentFlat.decay;
    }

    color *= fragmentFlat.exposure;

    // Desaturate relative to distance from light source
    float len = length(texCoord - normalizedPos);
    vec3  hsl = rgbToHsl(color);
    hsl.y *= (2.0 - log(len+1.718281) * fragmentFlat.saturation); // saturation
    color = hslToRgb(hsl);

    // Output final color with a further scale control factor.    
    outColor = vec4(color, 1.0);
}