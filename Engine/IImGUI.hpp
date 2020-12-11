#pragma once

#include <memory>

#include "RenderContext.hpp"
#include "RenderPass.hpp"

class IImGUI {
public:
	static std::shared_ptr<IImGUI> Create();
	virtual ~IImGUI() {}

	virtual void prepare( IRenderContext& ) = 0;
	virtual void apply( IRenderPass& ) = 0;

	virtual void update() = 0;
	virtual void render( IRenderPass& ) = 0;

protected:
	IImGUI() {}
};