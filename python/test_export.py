# Unit tests for boost serialization on engine objects.

import importlib
import sys

sys.path.append('/usr/local/lib')

import libInfinitariumEngine
importlib.reload(libInfinitariumEngine)

exportPath = './empty.ieb'

engine = libInfinitariumEngine

scene = engine.Scene()

camera = engine.Camera()
camera.setName('clear')
scene.add( camera )

context = engine.IRenderContext.create(0, 0, 1920, 1080, False)
camera.setRenderContext( context )

renderPass = engine.IRenderPass.create()
camera.setRenderPass( renderPass )

renderTarget = engine.IRenderTarget.create( 1920, 1080,
    engine.Format.BRGA8_sRGB, engine.Type.Color,
    engine.Resource.FrameBuffer)
renderTarget.setClear( True )
renderPass.addRenderTarget( renderTarget )

clearScreen = engine.ClearScreen()
camera.addRenderable( clearScreen )

print( 'Export ...' )
scene.save(exportPath)

print( 'Success!' )
