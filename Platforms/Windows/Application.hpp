#pragma once

#include "../../Engine/Application.hpp"

class WinApplication : public IApplication {
public:
	static std::shared_ptr<IApplication> Instance();
	WinApplication();
	virtual ~WinApplication() {}

	void run() override;
	void stop() override;
	void addManipulator(const std::string& id, float, float, float) override;

private:
	static std::shared_ptr<IApplication> instance;
};