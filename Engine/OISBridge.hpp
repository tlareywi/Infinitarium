//
//  OISBridge.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 9/16/18.
//

#pragma once

#include "ois/OISInputManager.h"
#include "ois/OISException.h"
#include "ois/OISKeyboard.h"
#include "ois/OISMouse.h"
#include "ois/OISJoyStick.h"
#include "ois/OISEvents.h"

#if defined OIS_APPLE_PLATFORM
#include <Carbon/Carbon.h>
#import <Cocoa/Cocoa.h>
#endif


