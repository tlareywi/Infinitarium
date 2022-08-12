#include "Calendar.hpp"

#include <memory>

#include <libnova/julian_day.h>

Calendar::Calendar() {
    
}

double Calendar::timetToJD( const time_t& t ) const {
    ln_date date;
    time_t tc;
    memcpy( &tc, &t, sizeof(time_t) );
    ln_get_date_from_timet(&tc, &date);
    return ln_get_julian_day( &date );
}
