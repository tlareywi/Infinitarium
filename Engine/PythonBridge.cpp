//
//  PythonBridge.cpp
//  Infinitaruim-Engine
//
//  Created by Trystan Larey-Williams on 10/6/18.
//

#include "../config.h"
#include "DataPack.hpp"
#include "PointCloud.hpp"
#include "Scene.hpp"
#include "ConsoleInterface.hpp"
#include "PythonBridge.hpp"
#include "PyUtil.hpp"
#include "ApplicationWindow.hpp"
#include "Sprite.hpp"
#include "Spheroid.hpp"
#include "CoordinateSystem.hpp"
#include "ImGui.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

using namespace boost::python;

void (DataPack_FLOAT32::*addVec3f)(float,float,float) = &DataPack_FLOAT32::addVec;
void (DataPack_FLOAT32::*addVec4f)(float,float,float,float) = &DataPack_FLOAT32::addVec;
void (DataPack_UINT8::*addVec3u8)(uint8_t,uint8_t,uint8_t) = &DataPack_UINT8::addVec;
void (DataPack_UINT8::*addVec4u8)(uint8_t,uint8_t,uint8_t,uint8_t) = &DataPack_UINT8::addVec;
void (DataPack_UINT16::*addVec3u16)(uint16_t,uint16_t,uint16_t) = &DataPack_UINT16::addVec;
void (DataPack_UINT16::*addVec4u16)(uint16_t,uint16_t,uint16_t,uint16_t) = &DataPack_UINT16::addVec;
void (DataPack_UINT32::*addVec3u32)(uint32_t,uint32_t,uint32_t) = &DataPack_UINT32::addVec;
void (DataPack_UINT32::*addVec4u32)(uint32_t,uint32_t,uint32_t,uint32_t) = &DataPack_UINT32::addVec;

BOOST_PYTHON_MODULE(libInfinitariumEngine)
{
   class_<DataPack_FLOAT32>("DataPack_FLOAT32", init<unsigned int>())
      .def("add", &DataPack_FLOAT32::add)
      .def("getBuffer", &DataPack_FLOAT32::getBuffer)
      .def("addVec3", addVec3f)
      .def("addVec4", addVec4f)
      .def("setChannelsPerVertex", &DataPack_FLOAT32::setChannelsPerVertex)
   ;
   class_<DataPack_UINT16>("DataPack_UINT16", init<unsigned int>())
      .def("add", &DataPack_UINT16::add)
      .def("getBuffer", &DataPack_UINT16::getBuffer)
      .def("addVec3", addVec3u16)
      .def("addVec4", addVec4u16)
      .def("setChannelsPerVertex", &DataPack_UINT16::setChannelsPerVertex)
   ;
   class_<DataPack_UINT32>("DataPack_UINT32", init<unsigned int>())
      .def("add", &DataPack_UINT32::add)
      .def("getBuffer", &DataPack_UINT32::getBuffer)
      .def("addVec3", addVec3u32)
      .def("addVec4", addVec4u32)
      .def("setChannelsPerVertex", &DataPack_UINT32::setChannelsPerVertex)
   ;
   class_<DataPack_UINT8>("DataPack_UINT8", init<unsigned int>())
      .def("add", &DataPack_UINT8::add)
      .def("getBuffer", &DataPack_UINT8::getBuffer)
      .def("addVec3", addVec3u8)
      .def("addVec4", addVec4u8)
      .def("setChannelsPerVertex", &DataPack_UINT8::setChannelsPerVertex)
   ;
   
   class_<DataPackContainer, boost::noncopyable>("DataPackContainer", no_init)
   ;

   def("wrap", wrapDataPack<float>, return_internal_reference<>() );
   def("wrap", wrapDataPack<uint32_t>, return_internal_reference<>() );
   def("wrap", wrapDataPack<uint16_t>, return_internal_reference<>() );
   def("wrap", wrapDataPack<uint8_t>, return_internal_reference<>() );
   
   // UniformType //////////////////////////////////////////////////////////////////////////////////////////////
   class_<UniformType>("UniformType", init<float>())
      .def(init<int>())
      .def(init<unsigned int>())
   ;
   
   class_<UniversalPoint>("UniversalPoint", init<double, double, double, UniversalPoint::Unit>())
   ;
   enum_<UniversalPoint::Unit>("Unit")
      .value("Meter", UniversalPoint::Unit::Meter)
      .value("Kilometer", UniversalPoint::Unit::Kilometer)
      .value("AstronomicalUnit", UniversalPoint::Unit::AstronomicalUnit)
      .value("Parsec", UniversalPoint::Unit::Parsec)
      .value("KiloParsec", UniversalPoint::Unit::KiloParsec)
      .value("MegaParsec", UniversalPoint::Unit::MegaParsec)
      .export_values()
   ;
   
   class_<BlendState>("BlendState", init<BlendState::Op, BlendState::Op, BlendState::Factor, BlendState::Factor, BlendState::Factor, BlendState::Factor>())
   ;
   enum_<BlendState::Op>("Op")
      .value("Add", BlendState::Op::Add)
      .value("Subtract", BlendState::Op::Subtract)
      .value("ReverseSubtract", BlendState::Op::ReverseSubtract)
      .value("Min", BlendState::Op::Min)
      .value("Max", BlendState::Op::Max)
   .export_values()
   ;
   enum_<BlendState::Factor>("Factor")
      .value("Zero", BlendState::Factor::Zero)
      .value("One", BlendState::Factor::One)
      .value("SourceColor", BlendState::Factor::SourceColor)
      .value("OneMinusSourceColor", BlendState::Factor::OneMinusSourceColor)
      .value("SourceAlpha", BlendState::Factor::SourceAlpha)
      .value("OneMinusSourceAlpha", BlendState::Factor::OneMinusSourceAlpha)
      .value("DestinationColor", BlendState::Factor::DestinationColor)
      .value("OneMinusDestinationColor", BlendState::Factor::OneMinusDestinationColor)
      .value("DestinationAlpha", BlendState::Factor::DestinationAlpha)
      .value("OneMinusDestinationAlpha", BlendState::Factor::OneMinusDestinationAlpha)
      .value("SourceAlphaSaturated", BlendState::Factor::SourceAlphaSaturated)
      .value("BlendColor", BlendState::Factor::BlendColor)
      .value("OneMinusBlendColor", BlendState::Factor::OneMinusBlendColor)
      .value("BlendAlpha", BlendState::Factor::BlendAlpha)
      .value("OneMinusBlendAlpha", BlendState::Factor::OneMinusBlendAlpha)
      .value("Source1Color", BlendState::Factor::Source1Color)
      .value("OneMinusSource1Color", BlendState::Factor::OneMinusSource1Color)
      .value("Source1Alpha", BlendState::Factor::Source1Alpha)
      .value("OneMinusSource1Alpha", BlendState::Factor::OneMinusSource1Alpha)
   .export_values()
   ;
   
   class_<CoordinateSystem>("CoordinateSystem", init<UniversalPoint, double, UniversalPoint::Unit>())
   ;
   
   // IMotionController ////////////////////////////////////////////////////////////////////////////////////////
   class_<IMotionController, boost::noncopyable>("IMotionController", no_init)
      .def("setHomeSystem", &IMotionController::pushHome);
   ;
   class_<Orbit, bases<IMotionController>>("Orbit", init<>())
   ;
   
   // IRenderContext ///////////////////////////////////////////////////////////////////////////////////////////
   class_<IRenderContext, boost::noncopyable>("IRenderContext", no_init)
      .def("create", &IRenderContext::Create)
   ;
   
    // IApplicationWindow //////////////////////////////////////////////////////////////////////////////////////
   class_<IApplicationWindow, boost::noncopyable>("IApplicationWindow", no_init)
      .def("create", &IApplicationWindow::Create)
   ;
   
    // IRenderPass ////////////////////////////////////////////////////////////////////////////////////////////
   class_<IRenderPass, boost::noncopyable>("IRenderPass", no_init)
      .def("create", &IRenderPass::Create)
      .def("addRenderTarget", &IRenderPass::addRenderTarget)
   ;
   
   // Textures ////////////////////////////////////////////////////////////////////////////////////////////////
   class_<ITexture, boost::noncopyable>("ITexture", no_init)
      .def("create", &ITexture::Create)
      .def("set", &ITexture::set)
   ;
   class_<IRenderTarget, bases<ITexture>, boost::noncopyable>("IRenderTarget", no_init)
      .def("create", &IRenderTarget::Create)
      .def("setClear", &IRenderTarget::setClear)
      .def("setClearColor", &IRenderTarget::setClearColor)
      .def("setBlendState", &IRenderTarget::setBlendState)
   ;
   enum_<ITexture::Format>("Format")
      .value("BRGA8", ITexture::Format::BRGA8)
      .value("BRGA8_sRGB", ITexture::Format::BRGA8_sRGB)
      .value("RGBA8_sRGB", ITexture::Format::RGBA8_sRGB)
      .value("RU32", ITexture::Format::RU32)
      .value("RGBA8", ITexture::Format::RGBA8)
      .value("RF32", ITexture::Format::RF32)
      .export_values()
   ;
   enum_<IRenderTarget::Type>("Type")
      .value("Color", IRenderTarget::Type::Color)
      .value("Depth", IRenderTarget::Type::Depth)
      .value("Stencil", IRenderTarget::Type::Stencil)
      .export_values()
   ;
   enum_<IRenderTarget::Resource>("Resource")
      .value("Swapchain", IRenderTarget::Resource::Swapchain)
      .value("Offscreen", IRenderTarget::Resource::Offscreen)
      .export_values()
   ;
   
   // Scene-graph objects ///////////////////////////////////////////////////////////////////////////////////
   class_<SceneObject>("SceneObject", init<>())
      .def("addChild", &SceneObject::addChild)
   ;
   class_<Transform, bases<SceneObject>>("Transform", init<>())
      .def("translate", &Transform::translate)
      .def("rotate", &Transform::rotate)
      .def("scale", &Transform::scale)
   ;
   
   // IRenderable ///////////////////////////////////////////////////////////////////////////////////////////
   class_<IRenderable, bases<SceneObject>, boost::noncopyable>("IRenderable", no_init)
      .def("propList", &IRenderable::propList)
      .def("listUniforms", &IRenderable::listUniforms)
      .def("setProgram", &IRenderable::setProgram)
      .def("setUniform", &IRenderable::setUniform)
      .def("setDirty", &IRenderable::setDirty)
      .def("removeUniform", &IRenderable::removeUniform)
      .def("manipulateUniform", &IRenderable::manipulateUniform)
      .def("setTexture", &IRenderable::setTexture)
   ;
   class_<ClearScreen, bases<IRenderable>>("ClearScreen", init<>())
   ;
   class_<ImGUI, bases<IRenderable>>("ImGUI", init<>())
   ;
   class_<PointCloud, bases<IRenderable>>("PointCloud", init<>())
      .def("addVertexBuffer", &PointCloud::addVertexBuffer)
      .def("setNumPoints", &PointCloud::setNumPoints)
   ;
   class_<Sprite, bases<IRenderable>>("Sprite", init<>())
   ;
   class_<Spheroid, bases<IRenderable>>("Spheroid", init<unsigned int, unsigned int, float, bool>())
   ;
   
   // Scene ////////////////////////////////////////////////////////////////////////////////////////////////
   class_<Scene, boost::noncopyable>("Scene", init<>())
      .def("load", &Scene::load)
      .def("loadLocal", &Scene::loadLocal)
      .def("save", &Scene::save)
      .def("add", &Scene::add)
      .def("clear", &Scene::clear)
      .def("propList", &Scene::propList)
   ;
   
   // Camera ///////////////////////////////////////////////////////////////////////////////////////////////
   class_<Camera, bases<Transform>, boost::noncopyable>("Camera", init<>())
      .def("setName", &Camera::setName)
      .def("setRenderPass", &Camera::setRenderPass)
      .def("setMotionController", &Camera::setMotionController)
      .def("setRenderContext", &Camera::setRenderContext)
      .def("propList", &Camera::propList)
   ;
   
   register_ptr_to_python<std::shared_ptr<IRenderPass>>();
   register_ptr_to_python<std::shared_ptr<SceneObject>>();
   register_ptr_to_python<std::shared_ptr<Transform>>();
   register_ptr_to_python<std::shared_ptr<IRenderTarget>>();
   register_ptr_to_python<std::shared_ptr<Camera>>();
   register_ptr_to_python<std::shared_ptr<IRenderable>>();
   register_ptr_to_python<std::shared_ptr<Scene>>();
   register_ptr_to_python<std::shared_ptr<IMotionController>>();
   register_ptr_to_python<std::shared_ptr<IRenderContext>>();
   register_ptr_to_python<std::shared_ptr<ITexture>>();
   register_ptr_to_python<std::shared_ptr<IApplicationWindow>>();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PyConsoleRedirect
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
boost::circular_buffer<std::string> PyConsoleRedirect::m_outputs;
   
PyConsoleRedirect::PyConsoleRedirect() {
   m_outputs.set_capacity(100);
}
   
void PyConsoleRedirect::write( std::string const& str ) {
   m_outputs.push_back(str);
}

std::string PyConsoleRedirect::GetOutput() {
   std::stringstream ss;
      
   for( auto& item : m_outputs )
      ss << item;
   
   m_outputs.clear();
   std::string out( ss.str() );
   boost::replace_all(out, "\n", "\\n");
   boost::replace_all(out, "\"", "\\\"");
   
   return out;
}

static PyConsoleRedirect python_stdio_redirector;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PythonInterpreter
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

PythonInterpreter::PythonInterpreter() {
   Py_Initialize();
   main_module = import("__main__");
   main_namespace = main_module.attr("__dict__");
   
   main_namespace["PyConsoleRedirect"] = class_<PyConsoleRedirect>("PyConsoleRedirect", init<>())
      .def("write", &PyConsoleRedirect::write);
   
   import("sys").attr("stderr") = python_stdio_redirector;
   import("sys").attr("stdout") = python_stdio_redirector;
   
   std::string enginePath {INSTALL_ROOT};
   enginePath += "/lib";
   eval("import sys");
   eval(std::string("sys.path.append('") + enginePath + "')");
   std::cout << eval("import libInfinitariumEngine") << std::endl;
   eval("engine = libInfinitariumEngine");
}

PythonInterpreter::~PythonInterpreter() {
   Py_Finalize();
}
   
std::string PythonInterpreter::eval( const std::string& expr ) {
   std::string retVal {""};
   std::string inStr {expr};
   
   // Replace html 'smart quotes'
   boost::replace_all(inStr, "‘","'");
   boost::replace_all(inStr, "’","'");
   boost::replace_all(inStr, "“","\"");
   boost::replace_all(inStr, "”","\"");
   
   try {
      object result = exec( str(inStr), main_namespace );
      
      retVal = python_stdio_redirector.GetOutput();
      
      // May eventually want expression evaluation support, i.e. 1+1, which currently won't work.
  /*    boost::python::extract<double> num(result);
      if( num.check() ) {
         double val = extract<double>(result);
         retVal += boost::lexical_cast<std::string>(val);
      }
      
      boost::python::extract<std::string> str(result);
      if( str.check() ) {
         retVal += extract<std::string>(result);
      } */
   }
   catch( boost::python::error_already_set const& ) {
      PyObject *ptype, *pvalue, *ptraceback;
      PyErr_Fetch(&ptype, &pvalue, &ptraceback);
      
      if( pvalue ) {
         handle<> h_val(pvalue);
         str a(h_val);
         extract<std::string> returned(a);
         if( returned.check() )
            retVal +=  ": " + returned();
         else
            retVal += std::string(": Unparseable Python error: ");
      }
      
      if( ptraceback ) {
         handle<> hTraceback(ptraceback);
         object tb( import("traceback") );
         object fmt_tb(tb.attr("format_tb"));
         // Call format_tb to get a list of traceback strings
         object tb_list(fmt_tb(hTraceback));
         // Join the traceback strings into a single string
         object tb_str(str("\n\\").join(tb_list));
         // Extract the string, check the extraction, and fallback in necessary
         extract<std::string> returned(tb_str);
         if(returned.check())
            retVal += ": " + returned();
         else
            retVal += std::string(": Unparseable Python traceback");
      }
      
      boost::replace_all(retVal, "\n", "\\n");
      boost::replace_all(retVal, "\"", "\\\"");
   }
   catch( ... ) {
      retVal = "PythonInterpreter::eval encountered unexpected exception.";
   }
   
   return retVal;
}

void PythonInterpreter::setScene( std::shared_ptr<Scene>& scene ) {
	try {
		boost::python::import("__main__").attr("activeScene") = scene;
	}
	catch ( ... ) {
		boost::python::handle_exception();
	}
}


