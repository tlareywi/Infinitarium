//
//  Header.h
//  Infinitarium
//
//  Created by Trystan (Home) on 11/28/18.
//

#pragma once

#include <string>

class IConsole {
public:
   virtual ~IConsole() {}
   virtual void info() const = 0;
   virtual void values() const = 0;
   virtual void manipulate( const std::string& ) = 0;
};
