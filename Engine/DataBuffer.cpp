//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "DataBuffer.hpp"
#include "Module.hpp"

std::shared_ptr<IDataBuffer> IDataBuffer::Create( IRenderContext& context ) {
   return ModuleFactory<RendererFactory>::Instance()->createDataBuffer( context );
}
