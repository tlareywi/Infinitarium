# Unit tests for boost serialization on engine objects.

import importlib
import sys

sys.path.append('/usr/local/lib')

import libInfinitariumEngine
importlib.reload(libInfinitariumEngine)

exportPath = './test.ieb'

engine = libInfinitariumEngine

scene = engine.Scene()
#print( 'Export Scene ...' )
#scene.save(exportPath)

camera = engine.Camera()
camera.setName('stars')
scene.add( camera )
#print( 'Export Scene with Camera ...' )
#scene.save( exportPath )

renderPass = engine.IRenderPass.create()
camera.setRenderPass( renderPass )
#print( 'Export Scene with Camera + RenderPass ...' )
#scene.save( exportPath )

tychoCloud = engine.PointCloud()
camera.addRenderable( tychoCloud )

renderTarget = engine.IRenderTarget.create( 1920, 1080,
    engine.Format.BRGA8_sRGB, engine.Type.Color,
    engine.Resource.FrameBuffer)
renderPass.addRenderTarget( renderTarget )

print( 'Export ...' )
scene.save(exportPath)

print( 'Success!' )
