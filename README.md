# Introduction
This 'star catalog viewer' is more or less a sandbox for me to fuss with modern graphics APIs such as Metal and Vulkan. It's not yet particularly usable, but it does some cool stuff such as ...
* ImGUI integration
* OpenXR integration for stereoscopic headset rendering
    * Tested on Valve Index. Theoretically works on other stero headsets (not phone adaptors)
* Vulkan backend under continuous development. Metal abondoned. OSX support via Molten planned. 
* Astropy and Python integration
    * See the python folder for scripts that pull data from VizieR
* Highly customizable and distributable scene files
    * Catalog data and the graphics pipeline configuration itself can be modified and serialized out to a scene

#### Tycho2 Catalog Render
![Tycho2 Catalog](screen_captures/tycho2.png)
#### Initial ImGUI integration (Hipparcos catalog in background)
![ImGUI Integration](screen_captures/ImGUI.png)

# Current Efforts
* More features; bloom/blur effects, 3D stars, navigation, more elaborate scenes, etc.
* General stabilization of existing feautres and cross-platform builds     

### Supported Platforms
Windows and Linux (Vulkan). OSX (Molten). Windows is currently under active development and will be the most stable and likely to build off the mainline.

### Build Dependencies
I'll get around to documenting this once things stabilize a bit. It's a Cmake build system, so it will let you know what's missing ;) But you at least need the following.
* Boost 1.78 (or other boost versions that are on boost_python 3.10)
* Python 3.10.x (where the x matches the whater version boost_python was validated against)
* GLFW
* GLM
* VulkanSDK
* OpenXR (optional)

### Required Runtime Dependencies
Python 3.10.x
Boost Serialization
Boost_Python

### Runtime Dependencies for building scenes (python)
pip3 install astropy  
pip3 install pillow  

### Usage
Startup scene has a minimal ImGui interface that allows loading of sample scenes such as Hipparcos and Tyco2 catalogs. The Scenegraph window allows exploration of the current scene nodes and interactive manipulation of shader uniforms. Various aspects of rendering such as star appearance can be adjusted this way. 
