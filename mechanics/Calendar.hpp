#pragma once

#include "../config.h"
#include <ctime>

class MECHANICS_EXPORT Calendar {
public:
    Calendar();
    
    double timetToJD( const time_t& ) const;
private:
    
};
