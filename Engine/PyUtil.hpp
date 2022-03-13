//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include <boost/circular_buffer.hpp>

class PyConsoleRedirect {
public:
   PyConsoleRedirect();
   
   void write( std::string const& str );
   std::string GetOutput();
   
private:
   static boost::circular_buffer<std::string> m_outputs;
};


