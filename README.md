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
