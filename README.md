# Introduction
This 'star catalog viewer' is more or less a sandbox for me to fuss with modern graphics APIs such as Metal and Vulkan. It's not yet particularly usable, but it does some cool stuff such as ...
* OpenXR integration for stereoscopic headset rendering
    * Tested on Valve Index. Theoretically works on other stero headsets (not phone adaptors)
* Vulkan and Metal backends for cross-platform rendering (Windows, Linux, OSX)
* Astropy and Python integration
    * See the python folder for scripts that pull data from VizieR
* Highly customizable and distributable scene files
    * Catalog data and the graphics pipeline configuration itself can be modified and serialized out to a scene

#### Tycho2 Catalog Render
![Tycho2 Catalog](screen_captures/tycho2.png)
#### Initial ImGUI integration (Hipparcos catalog in background)
![ImGUI Integration](screen_captures/ImGUI.png)

# Current Efforts
* ImGUI integration to build out features needed to make this a tool some folks may actually want to use 
* General stabilization of existing feautres and cross-platform builds     

### Supported Platforms
Windows (Vulkan) and OSX (Metal). Vulkan work is currently, as of Dec. 2020, running ahead of Metal so only Windows/Vulkan will currently build on the mainline. OSX/Metal works on older tags. 

### Build Dependencies
I'll get around to documenting this once things stabilize a bit. It's a Cmake build system, so it will let you know what's missing ;) But you at least need the following.
* Boost 1.74 (or other boost versions that are on boost_python 3.8)
* Python 3.8
* GLFW (Windows)
* GLM
* OpenXR (Windows)
* VulkanSDK (Windows)
* RapidJSON

### Required Runtime Dependencies
brew install python

### Optional Runtime Dependencies  
pip3 install astropy  
pip3 install Pillow  

### Usage
A start-up scene file is hardcoded in main.cpp. Scenes are built using python on the command line using the scene files in the python folder. Some prebuilt scenes including Hipparcos and Tycho2 are included in the source tree under the data folder. More interactive ways to load and build scenes are forthcoming. 

## Tagged OSX releases only
Feaures below this line are on prior tagged OSX builds only. Soon on Windows. 

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
