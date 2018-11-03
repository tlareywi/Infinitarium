//
//  EventSampler.h
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 11/3/18.
//

#pragma once

#include <vector>
#include <mutex>

std::unique_ptr<class IEventSampler> CreateOSXEventSampler();

class IEventSampler {
public:
   enum State {
      DOWN,
      UP
   };
   
   enum Button {
      LEFT,
      MIDDLE,
      RIGHT
   };
   
   struct Key {
      unsigned char key;
      State state;
   };
   
   struct MouseMove {
      float dx;
      float dy;
      float dz; // Scroll
   };
   
   struct MouseDrag {
      Button button;
      float dx;
      float dy;
   };
   
   struct MouseButton {
      Button button;
      State state;
   };
   
   IEventSampler() {
      keys.reserve(128); // TODO: Hmm, think we need one queue actually. Order is potentially important.
      mmove.reserve(256);
      mdrag.reserve(256);
      mbutton.reserve(128);
   }
   virtual ~IEventSampler() {}
   
   static std::unique_ptr<IEventSampler>& Instance() {
      if( !instance )
         instance = CreateOSXEventSampler();
      
      return instance;
   }
   
   void push( const Key& k ) {
      std::lock_guard<std::mutex> lock(eventMutex);
      keys.push_back(k);
   }
   
   void push( const MouseMove& m ) {
      std::lock_guard<std::mutex> lock(eventMutex);
      mmove.push_back(m);
   }
   
   void push( const MouseDrag& m ) {
      std::lock_guard<std::mutex> lock(eventMutex);
      mdrag.push_back(m);
   }
   
   void push( const MouseButton& b ) {
      std::lock_guard<std::mutex> lock(eventMutex);
      mbutton.push_back(b);
   }
   
   void clear() {
      keys.clear();
      mmove.clear();
      mdrag.clear();
      mbutton.clear();
   }
   
private:
   friend class IMotionController;
   std::mutex eventMutex;
   std::vector<Key> keys;
   std::vector<MouseMove> mmove;
   std::vector<MouseDrag> mdrag;
   std::vector<MouseButton> mbutton;
   static std::unique_ptr<IEventSampler> instance;
};