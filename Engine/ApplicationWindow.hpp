//
//  SimulationWindow.hpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/8/18.
//

#pragma once

#include <memory>

#include "RenderContext.hpp"

class IApplicationWindow {
public:
   virtual ~IApplicationWindow() {}
   IApplicationWindow( const IApplicationWindow& ) {}
   
   static std::shared_ptr<IApplicationWindow> Create();
   static std::shared_ptr<IApplicationWindow> Clone( const IApplicationWindow& );
   
   virtual void init( IRenderContext& ) = 0;
   
protected:
   IApplicationWindow() {}
};

class ApplicationWindowProxy : public IApplicationWindow {
public:
   ApplicationWindowProxy() {}
   ApplicationWindowProxy( const IApplicationWindow& obj ) : IApplicationWindow(obj) {}
   
   void init( IRenderContext& ) override {};

   template<class Archive> void serialize(Archive& ar);
   
private:
#if defined ENGINE_BUILD
   friend class boost::serialization::access;
#endif
};
