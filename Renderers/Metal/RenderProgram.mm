//
//  RenderProgram.m
//  IERenderer
//
//  Created by Trystan Larey-Williams on 12/31/18.
//

#include "RenderProgram.h"
#include "RenderState.h"
#include "RenderContext.h"
#include "UniformMap.h"

#include "../../config.h"

#include <sstream>
#include <fstream>

MetalRenderProgram::~MetalRenderProgram() {
   [program release];
}

void MetalRenderProgram::prepare( IRenderState& state ) {
   // Separating this out from compile allows function switching within a library
   // without recompiling the shader source.
   vertex = [program newFunctionWithName:@"vertexShader"];
   fragment = [program newFunctionWithName:@"fragmentShader"];
   
   MetalRenderState* metalState = dynamic_cast<MetalRenderState*>( &state );
   metalState->getPipelineDescriptor().vertexFunction = vertex;
   metalState->getPipelineDescriptor().fragmentFunction = fragment;
}

void MetalRenderProgram::apply( IRenderState& state ) {
   
}

void MetalRenderProgram::compile( const std::string& name, IRenderContext& context ) {
   MetalRenderContext* c = dynamic_cast<MetalRenderContext*>( &context );
   id<MTLDevice> device = c->getMTLDevice();
   std::string path {std::string(INSTALL_ROOT) + std::string("/share/Infinitarium/shaders/metal/") + name + ".metal"};
   
   std::ifstream infile(path);
   std::stringstream buffer;
   buffer << "#include <metal_stdlib>\nusing namespace metal;" << uniformBlock << infile.rdbuf();
   
   NSError* err {nullptr};
   program = [device newLibraryWithSource:[NSString stringWithUTF8String:buffer.str().c_str()] options:nullptr error:&err];
   
   infile.close();
   
   if( err ) {
      std::cout<<"Error compiling "<<path<<" "<<[err.localizedDescription UTF8String]<<std::endl;
   }
}

void MetalRenderProgram::injectUniformStruct( const std::vector<std::pair<std::string, UniformType>>& uniforms ) {
   std::stringstream ss;
   ss << "\n\nstruct ConstUniforms { \n";
   
   for( auto& i : uniforms )
      ss << i.second << " " << i.first << ";\n";
   
   ss << "};\n\n";
   
   uniformBlock = ss.str();
}

std::shared_ptr<IRenderProgram> CreateRenderProgram() {
   return std::make_shared<MetalRenderProgram>();
}
