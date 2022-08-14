//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "Scene.hpp"
#include "ObjectStore.hpp"
#include "Stats.hpp"

#include "../mechanics/Calendar.hpp"

#include <fstream>
#include <iostream>
#include <chrono>

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(Scene);

Scene::Scene() : _referenceTime{ 0 }, _tickTime{ 0 }, _simulationTime{std::chrono::system_clock::now()} {
}

Scene::~Scene() {
    clear();
}

void Scene::clear() {
   waitOnIdle();

   for (auto& ctx : renderContexts)
        ctx->unInit(); // Tear down non-static resources that the device still needs to be alive for.

   for( auto& camera : cameras )
      camera = nullptr;
   
   cameras.clear();
   renderContexts.clear();

   // Dealloc static instance tracking caches
   ITexture::clearRegisteredObjs();
   IRenderTarget::clearRegisteredObjs();
   IRenderPass::clearRegisteredObjs();
   ObjectStore::instance().clear();

   // Deleting the context(s) will destroy the graphics device. Do this after cleaning up other graphics resources.
   IRenderContext::clearRegisteredObjs();
}

void Scene::add( const std::shared_ptr<Camera>& camera ) {
   cameras.push_back( camera );
}

void Scene::prepareLoadScene( const std::string& filePath ) {
    loadPendingFilename = filePath;
    loadPending = true;
}

void Scene::load( const std::string& filename ) {

   std::ifstream ifs( filename, std::fstream::binary );
   if( !ifs.is_open() ) {
      std::cout<<"Unable to open "<<filename<<". Do you have read permissions?"<<std::endl;
	  return;
   }
   
   {
      boost::archive::binary_iarchive ia( ifs );
      ia >> boost::serialization::make_nvp( "Scene", *this );
      
      renderContexts.reserve(cameras.size());
      for( auto& camera : cameras ) {
          std::shared_ptr<IRenderContext> c{ camera->getContext() };

          if( std::any_of(renderContexts.begin(), renderContexts.end(), [&c](std::shared_ptr<IRenderContext>& ctx) {return ctx == c;}) )
              continue;
          
          c->init();
          renderContexts.push_back(c);
      }
   }
   
   ifs.close();
}

void Scene::setLocalScenePath( const std::string& path ) {
   localScenePath = path;
}

void Scene::save( const std::string& filename ) const {
   std::ofstream ofs( filename, std::ofstream::binary | std::ofstream::trunc );
   if( !ofs.is_open() ) {
      std::cout<<"Unable to open "<<filename<<". Do you have write permissions?"<<std::endl;
   }
   
   {
      boost::archive::binary_oarchive oa( ofs );
      oa << boost::serialization::make_nvp( "Scene", *this );
   }
   
   ofs.close();
}

void Scene::terminatePending() {
    shouldExit = true;
}

bool Scene::isTerminatePending() {
    return shouldExit;
}

void Scene::mainThread() {
    if (loadPending && canLoad) {
        std::lock_guard guard(loadSceneMutex);
        canLoad = false;
        loadPending = false;
        clear();
        load(loadPendingFilename);
    }
}

void Scene::update(const ReferenceTime& rt) {
   auto lastRefTime = _referenceTime;
   _referenceTime = rt;
   _tickTime = _referenceTime - lastRefTime;
   auto oldSimTime = _simulationTime;
   _simulationTime += (std::chrono::duration_cast<std::chrono::system_clock::duration>(_tickTime) * _timeMultiplier);
   _simulationTimeDelta = _simulationTime - oldSimTime;
   
   const std::time_t tt = std::chrono::system_clock::to_time_t(_simulationTime);
   std::tm tm = *std::localtime(&tt);
   std::stringstream timeStr;
   timeStr << std::put_time(&tm, "%m-%d-%Y %H:%M:%S");
   Stats::Instance().simulationDateTime = timeStr.str();
    
   Calendar cal;
   _JD = cal.timetToJD( tt );

   canLoad = false;
   std::lock_guard guard(loadSceneMutex);

   for (auto& context : renderContexts)
       context->beginFrame();

   for( auto& camera : cameras ) {
      UpdateParams ident( *camera, *this );
      camera->update( ident );
   }
}

void Scene::render() {
    std::lock_guard guard(loadSceneMutex);

    {
        RenderPassProxy stub;

        for (auto& camera : cameras)
            camera->render(stub);
    }

    for (auto& context : renderContexts)
        context->endFrame();

    canLoad = true;
}

void Scene::visit(const Visitor& v) {
    if (!v.apply(*this))
        return;

    for (auto& camera : cameras) {
        camera->visit(v);
    }
}

void Scene::visit(const Accumulator& v) {
    if (!v.push(*this))
        return;

    for (auto& camera : cameras) {
        camera->visit(v);
    }

    v.pop(*this);
}

void Scene::waitOnIdle() {
    // Wait till execution queue is complete on all render contexts. Use sparingly. 
    // A valid use is say 'on exit' when we need to wait for completion prior to tearing down resources. 
    for (auto& ctx : renderContexts)
        ctx->waitOnIdle();
}

template<class Archive> void Scene::serialize(Archive & ar, const unsigned int version) {
   std::cout<<"Serializing Scene"<<std::endl;
   boost::serialization::void_cast_register<Scene, SceneObject>();
   ar & boost::serialization::make_nvp("SceneObject", boost::serialization::base_object<SceneObject>(*this));
   ar & BOOST_SERIALIZATION_NVP(cameras);
}


