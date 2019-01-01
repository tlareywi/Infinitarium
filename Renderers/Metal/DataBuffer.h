#pragma once

#include "../../Engine/DataBuffer.hpp"

#import <Metal/Metal.h>

///
/// brief Metal implementation of DataBuffer
///
class MetalDataBuffer : public IDataBuffer {
public:
   MetalDataBuffer( IRenderContext& context );
   virtual ~MetalDataBuffer();
   
   void set( DataPackContainer& container ) override;
   
   void reserve( unsigned int sizeBytes ) override;
   
   void set( const void* const data, unsigned int sizeBytes ) override;
   
   id<MTLBuffer> getMTLBuffer();
   
private:
   id<MTLBuffer> GPU{nullptr};
   id<MTLDevice> device;
   id<MTLCommandQueue> commandQ;
};
