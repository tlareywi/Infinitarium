//
//  Application.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 11/18/18.
//

#pragma once

#include "../../Engine/Application.hpp"

class OSXApplication : public IApplication {
public:
   OSXApplication();
   static std::shared_ptr<IApplication> Instance();
   
   void run() override;
   void stop() override;
   void* platformInstance() override {
      return nullptr;
   }
   
   void addManipulator( const std::string&, float, float, float ) override;
   
private:
   static std::shared_ptr<IApplication> instance;
};



