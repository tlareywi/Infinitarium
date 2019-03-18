# Introduction
So far, this is basically a star catalog viewer very much in an alpha/experimental state. It features an integrated python3 console facilitating the use of popular astonomy packages like AstroPy. It's possible to download star catalogs directly from a remote source such as the VizieR archives and process them into a renderable scene all within the application.  

It is also possible, and often more convinient, to process catalogs via a standalone python script that writes out a scene file consumable by the Infinitarium viewer.        

### Supported Platforms
Only OSX Mojave with XCode 10 has been tested and is currently supported. This project started as a way to learn the Metal graphics API (Apple specific). However, the code is well designed to decouple the graphics API and future plans include Windows and Linux support with a Vulkan graphics backend.

### Prerequsites
Install homebrew; /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

### Build Dependencies
brew install glm  
brew install boost  
brew install boost-python3  
brew install rapidjson

### Required Runtime Dependencies
brew install python

### Optional Runtime Dependencies  
pip3 install astropy  
pip3 install Pillow  

### Screen Captures

#### Tycho2 Catalog
![Tycho2 Catalog](screen_captures/tycho2.png)

### Usage
Running the execuable will load the default empty scene with a python console. From here you can do anything you could in Python and Astropy including loading star catalogs from VizieR. It's often more convinient though to process star catalogs via a standalone python script. See examples in the python folder. The included pre-built scenes (see below) are built using the scripts python/hipparcos2_import.py and python/tycho2_import.py.

#### Loading pre-built scenes
In the application's python console run activeScene.loadLocal('[scenefile]'). There are currently two scenes distrubuted, hip2.ieb and tyco2.ieb. 

#### Exploring the scenegraph
Cameras and renderable objects in the scene support the method propList() which prints the public methods and members of the object to the console. This serves as a form of interactive documentation, albiet a little obtuse at present.  

#### Manipulating shader uniforms
Shader uniforms can be manipulated in real-time during the simulation. An example is provided below. Invoking a monipulator will add a small slider control to the console window which is used to change the values of the uniform. Many manipulators can be active simulatainiously.  
  
camera = activeScene.getCamera( 0 )  
stars = camera.getRenderable( 0 )  
stars.listUniforms()  
stars.manipulateUniform('[name]')  

#### Saving changes
activeScene.save('[filename]') will serialize out the current scene including all shader uniforms, camera configuration, etc. to the given filename.  
