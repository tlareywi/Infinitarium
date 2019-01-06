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
   
   void commit() override;
   
   void reserve( unsigned int sizeBytes ) override;
   
   void set( DataPackContainer& container ) override;
   void set( const void* const, unsigned int ) override;
   void set( const void* const, unsigned int, unsigned int ) override;
   
   id<MTLBuffer> getMTLBuffer();
   
private:
   id<MTLBuffer> managed{nullptr};
   id<MTLBuffer> GPU{nullptr};
   id<MTLDevice> device;
   id<MTLCommandQueue> commandQ;
};
