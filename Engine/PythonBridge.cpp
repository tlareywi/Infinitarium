//
//  PythonBridge.cpp
//  Infinitaruim-Engine
//
//  Created by Trystan Larey-Williams on 10/6/18.
//

#include "DataPack.hpp"

#include <boost/python.hpp>
using namespace boost::python;

BOOST_PYTHON_MODULE(InfinitariumEngine)
{
   class_<DataPack_RGBAU8>("DataPack_RGBAU8", init<unsigned int>())
   ;
   class_<DataPack_RGBAF32>("DataPack_RGBAF32", init<unsigned int>())
   ;
   class_<DataPack_RGBU8>("DataPack_RGBU8", init<unsigned int>())
   ;
   class_<DataPack_RGBF32>("DataPack_RGBF32", init<unsigned int>())
   ;
}


