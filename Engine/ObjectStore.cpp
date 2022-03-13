//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "ObjectStore.hpp"

std::unique_ptr<ObjectStore> ObjectStore::_instance{nullptr};

ObjectStore& ObjectStore::instance() {
	if (!_instance) {
		_instance = std::unique_ptr<ObjectStore>(new ObjectStore());
	}

	return *_instance;
}

void ObjectStore::clear() {
	cache.clear();
	_instance = nullptr;
}

void ObjectStore::add(boost::uuids::uuid uuid, const std::shared_ptr<IDataBuffer>& buffer) {
	cache[uuid] = buffer;
}

std::shared_ptr<IDataBuffer> ObjectStore::get(boost::uuids::uuid uuid) {
	auto itr = cache.find(uuid);

	if (itr == cache.end())
		return nullptr;
	else
		return itr->second;
}