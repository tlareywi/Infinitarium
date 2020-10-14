//
//  PyUtil.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 12/8/18.
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


