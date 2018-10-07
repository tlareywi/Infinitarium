//
//  OISBridge.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#include "OISBridge.hpp"

#include <iostream>

#if defined OIS_APPLE_PLATFORM
void checkMacEvents()
{
   //TODO - Check for window resize events, and then adjust the members of mousestate
   EventRef event            = NULL;
   EventTargetRef targetWindow = GetEventDispatcherTarget();
   
   if(ReceiveNextEvent(0, NULL, kEventDurationNoWait, true, &event) == noErr)
   {
      SendEventToEventTarget(event, targetWindow);
      std::cout << "Event : " << GetEventKind(event) << "\n";
      ReleaseEvent(event);
   }
}
#endif

