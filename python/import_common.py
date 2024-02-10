exec(open('./BlackBodyParser.py').read())
print( '\nBlack body mappings complete' )

import math
import importlib
import sys
import ctypes
import platform
import os

homeDir = os.getcwd()

if platform.system() == 'Windows':
    exportPath = '../data/Windows/'
    sys.path.append('''C:\\Program Files (x86)\\Infinitarium\\bin''')
    os.add_dll_directory('C:\\Program Files (x86)\\Infinitarium\\bin')
elif platform.system() == 'Linux':
    sys.path.append('/usr/local/lib')
    exportPath = '../data/Linux/'  
else: # Apple/OSX
    sys.path.append('/usr/local/lib')
    exportPath = '../data/OSX/'

def degreesToRadians(x):
    return x * math.pi / 180.0

def sphereToRectZUp(ra, dec, r):
    x = r * math.sin(degreesToRadians(90-dec)) * math.cos(degreesToRadians(ra))
    y = r * math.sin(degreesToRadians(90-dec)) * math.sin(degreesToRadians(ra))
    z = r * math.cos(degreesToRadians(90-dec))
    return[x,y,z]

def sphereToRectZUpRads(ra, dec, r):
    x = r * math.sin(1.5708-dec) * math.cos(ra)
    y = r * math.sin(1.5708-dec) * math.sin(ra)
    z = r * math.cos(1.5708-dec)
    return[x,y,z]

# From https://heasarc.nasa.gov/W3Browse/all/tycho2.html
def apparentMagColor( bt, vt ):
     v = vt - 0.090 * (bt - vt)
     bv = 0.850 * (bt - vt)
     return v, bv

def distParsecs( plx ):
    if plx <= 0:
        return 0
    dist = 1.0 / (plx * 0.001)
    return dist

def pcToLy( pc ):
    return pc * 3.26156

def appMagToAbsMag( appMag, distPc ):
    return appMag - 5.0 * (math.log(distPc, 10) - 1.0)

import libInfinitariumEngine
importlib.reload(libInfinitariumEngine)

from astropy.table import Table

engine = libInfinitariumEngine

def initImGUI( scene, context, renderTarget ):
    # Setup ImGUI Camera
    gui = engine.Camera()
    gui.setName('ImGui Camera')
    scene.add( gui )
    # Add ImGui drawable
    imgui = engine.ImGUI()
    imgui.setName('GUI Renderable')
    gui.addChild( imgui )
    # ImGUI RenderPass
    guiPass = engine.IRenderPass.create()
    gui.setRenderPass( guiPass )
    gui.setRenderContext( context )
    guiPass.addRenderTarget( renderTarget, engine.LoadOp.Load )
    return




