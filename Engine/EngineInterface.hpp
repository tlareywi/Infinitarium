//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

class IPythonInterpreter {
public:
   IPythonInterpreter() {};
   virtual ~IPythonInterpreter() {};

   virtual std::string eval( const std::string& ) = 0;
};

