//
//
//

#include "DataBuffer.h"
#include "RenderContext.h"
#include "RenderPass.h"

MetalDataBuffer::MetalDataBuffer( IRenderContext& context ) {
   MetalRenderContext* c = dynamic_cast<MetalRenderContext*>( &context );
   device = c->getMTLDevice();
   commandQ = c->getMTLCommandQ();
}

MetalDataBuffer::~MetalDataBuffer() {
   [GPU release];
   [managed release];
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
   if( !managed || [managed length] < sizeBytes ) {
      [managed release];
      managed = [device newBufferWithLength:sizeBytes options:MTLResourceStorageModeManaged];
   }
   
   if( !GPU || [GPU length] < sizeBytes ) {
      [GPU release];
      GPU = [device newBufferWithLength:sizeBytes options:MTLResourceStorageModePrivate];
   }
}

void MetalDataBuffer::set( const void* const data, unsigned int sizeBytes ) {
   reserve( sizeBytes );
   memcpy( managed.contents, data, sizeBytes );
}

void MetalDataBuffer::set( const void* const data, unsigned int offset, unsigned int sizeBytes ) {
   // No sanity checking in this version. May be called many times in tight loop.
   memcpy( (uint8*)(managed.contents) + offset, data, sizeBytes );
}

void MetalDataBuffer::commit() {
   [managed didModifyRange:NSMakeRange(0, [managed length])];
   
   id<MTLCommandBuffer> cmdBuf = [commandQ commandBuffer];
   id<MTLBlitCommandEncoder> bltEncoder = [cmdBuf blitCommandEncoder];
   [bltEncoder copyFromBuffer:managed sourceOffset:0 toBuffer:GPU destinationOffset:0 size:[managed length]];
   [bltEncoder endEncoding];
   [cmdBuf commit];
   
   [cmdBuf release];
   [bltEncoder release];
}

void MetalDataBuffer::copy( IRenderTarget& rt, const glm::uvec4& rect ) {
   MetalRenderTarget* mtlrt = dynamic_cast<MetalRenderTarget*>(&rt);
   if( !mtlrt ) return;
   id<MTLTexture> source = mtlrt->getMetalTexture();
   
   MTLOrigin origin = MTLOriginMake(rect.x, source.height - rect.y, 0);
   MTLSize size = MTLSizeMake(rect.z, rect.w, 1);
   NSUInteger bpp = mtlrt->getBytesPerPixel();
   NSUInteger rectBytes = size.height * size.width * bpp;
   
   reserve( rectBytes );
   id<MTLCommandBuffer> cmdBuf = [commandQ commandBuffer];
   id<MTLBlitCommandEncoder> bltEncoder = [cmdBuf blitCommandEncoder];
   [bltEncoder copyFromTexture:source
                   sourceSlice:0
                   sourceLevel:0
                   sourceOrigin:origin
                   sourceSize:size
                   toBuffer:managed
                   destinationOffset:0
                   destinationBytesPerRow:(size.width*bpp)
                   destinationBytesPerImage:rectBytes];
   [bltEncoder synchronizeResource:managed];
   [bltEncoder endEncoding];
   [cmdBuf commit];
   [cmdBuf waitUntilCompleted];
   
   [cmdBuf release];
   [bltEncoder release];
}

void MetalDataBuffer::getData( void* buf ) {
   memcpy( buf, managed.contents, managed.length );
}

id<MTLBuffer> MetalDataBuffer::getMTLBuffer() {
   return GPU;
}

std::shared_ptr<IDataBuffer> CreateDataBuffer( IRenderContext& context ) {
   std::shared_ptr<MetalDataBuffer> databuf = std::make_shared<MetalDataBuffer>(context);
   return databuf;
}
