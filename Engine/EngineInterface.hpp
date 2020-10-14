//
//  PythonInterp.h
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 11/18/18.
//

#pragma once

class IPythonInterpreter {
public:
   IPythonInterpreter() {};
   virtual ~IPythonInterpreter() {};

   virtual std::string eval( const std::string& ) = 0;
};

