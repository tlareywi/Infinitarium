//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include <memory>

#include "RenderContext.hpp"
#include "RenderPass.hpp"

class IImGUI {
public:
	static std::shared_ptr<IImGUI> Create();
	virtual ~IImGUI() {}

	virtual void prepare(IRenderContext&) = 0;
	virtual void apply(IRenderPass&) = 0;
	
	void setDirty() {
		dirty = true;
	}

	virtual void update() = 0;
	virtual void render(IRenderPass&) = 0;

protected:
	IImGUI() {}
	bool dirty{true};
};