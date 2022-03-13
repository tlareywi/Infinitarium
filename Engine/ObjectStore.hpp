//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "DataBuffer.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <map>

/// <summary>
/// Manager for buffer data shared accross multiple Renderables. E.g. geometry data, etc.  
/// </summary>
class ObjectStore {
public:
	static ObjectStore& instance();
	
	void clear();
	
	void add( boost::uuids::uuid,  const std::shared_ptr<IDataBuffer>& );
	std::shared_ptr<IDataBuffer> get( boost::uuids::uuid );

private:
	ObjectStore() {};
	static std::unique_ptr<ObjectStore> _instance;
	std::map<boost::uuids::uuid, std::shared_ptr<IDataBuffer>> cache;
};