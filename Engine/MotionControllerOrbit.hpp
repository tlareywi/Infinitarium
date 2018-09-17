//
//  MotionControllerOrbit.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#pragma once

#include <cstdint>

namespace Infinitarium {
   
   namespace MotionController {
      enum MouseButton {
         LEFT,
         MIDDLE,
         RIGHT
      };
      
      class Interface {
      public:
         virtual void onKeyDown( uint16_t code ) {};
         virtual void onKeyUp( uint16_t code ) {};
         virtual void onMouseDown( MouseButton btn ) {};
         virtual void onMouseDrag( double dx, double dy, MouseButton btn ) {};
         
      private:
         
      };
      
      class Orbit : public MotionController::Interface {
         void onKeyDown( uint16_t code ) override {};
         void onKeyUp( uint16_t code ) override {};
         void onMouseDown( MouseButton btn ) override {};
         void onMouseDrag( double dx, double dy, MouseButton btn ) override {};
      };
      
   } // END namespace MotionController
} // END namespace Infinitarium
