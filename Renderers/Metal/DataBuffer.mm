//
//
//

#include "DataBuffer.h"
#include "RenderContext.h"

MetalDataBuffer::MetalDataBuffer( IRenderContext& context ) {
   MetalRenderContext* c = dynamic_cast<MetalRenderContext*>( &context );
   device = c->getMTLDevice();
   commandQ = c->getMTLCommandQ();
   GPU = nullptr;
}

MetalDataBuffer::~MetalDataBuffer() {
   [GPU release];
}

void MetalDataBuffer::set( DataPackContainer& container ) {
   std::visit( [this](auto& e) { // TODO: move visit to caller level and pass DataPack instead?
      [GPU release];
      GPU = [device newBufferWithLength:e.sizeBytes() options:MTLResourceStorageModePrivate];
      
      id<MTLBuffer> tmpBuffer = [device newBufferWithBytes:e.get() length:e.sizeBytes() options:MTLResourceStorageModeManaged];
      id<MTLCommandBuffer> cmdBuf = [commandQ commandBuffer];
      id<MTLBlitCommandEncoder> bltEncoder = [cmdBuf blitCommandEncoder];
      [bltEncoder copyFromBuffer:tmpBuffer sourceOffset:0 toBuffer:GPU destinationOffset:0 size:e.sizeBytes()];
      [bltEncoder endEncoding];
      [cmdBuf commit];
      
      [tmpBuffer release];
      [cmdBuf release];
      [bltEncoder release];
      
   }, container );
};

void MetalDataBuffer::reserve( unsigned int sizeBytes ) {
   if( !GPU || [GPU length] < sizeBytes ) {
      [GPU release];
      GPU = [device newBufferWithLength:sizeBytes options:MTLResourceStorageModePrivate];
   }
}

void MetalDataBuffer::set( const void* const data, unsigned int sizeBytes ) {
   reserve( sizeBytes );
   
   id<MTLBuffer> tmpBuffer = [device newBufferWithBytes:data length:sizeBytes options:MTLResourceStorageModeManaged];
   
   id<MTLCommandBuffer> cmdBuf = [commandQ commandBuffer];
   id<MTLBlitCommandEncoder> bltEncoder = [cmdBuf blitCommandEncoder];
   [bltEncoder copyFromBuffer:tmpBuffer sourceOffset:0 toBuffer:GPU destinationOffset:0 size:sizeBytes];
   [bltEncoder endEncoding];
   [cmdBuf commit];
   
   [tmpBuffer release];
   [cmdBuf release];
   [bltEncoder release];
}

id<MTLBuffer> MetalDataBuffer::getMTLBuffer() {
   return GPU;
}

extern "C" {
   std::shared_ptr<IDataBuffer> CreateDataBuffer( IRenderContext& context ) {
      std::shared_ptr<MetalDataBuffer> databuf = std::make_shared<MetalDataBuffer>(context);
      return databuf;
   }
}
