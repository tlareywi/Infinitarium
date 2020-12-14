#include "Stats.hpp"

Stats::Stats() : fps{0} {

}

Stats& Stats::Instance() {
	static Stats instance;
	return instance;
}