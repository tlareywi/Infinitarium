//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "Stats.hpp"

Stats::Stats() : fps{0} {

}

Stats& Stats::Instance() {
	static Stats instance;
	return instance;
}