//
// Minimal passthrough shader
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
   return float4(0.0);
}


