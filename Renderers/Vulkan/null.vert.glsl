//  Uncomment below for glslangValidator.exe  .\[name].[type].glsl - o .\[name].[type].spirv - V
/*  #version 450
    layout(binding = 0) uniform ConstUniforms {
        mat4 modelViewProjectionMatrix;
        uvec2 viewport;
    } uniforms;
*/

void main() {
    gl_Position = vec4(vec3(0.0), 1.0);
}
