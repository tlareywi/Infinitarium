#pragma once

#include <memory>

class Stats {
public:
	static Stats& Instance();
	unsigned int fps;

private:
	Stats();
};