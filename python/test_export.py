# Unit tests for boost serialization on engine objects.

import importlib
import sys

sys.path.append('/usr/local/lib')

import libInfinitariumEngine
importlib.reload(libInfinitariumEngine)

exportPath = './test.ieb'

engine = libInfinitariumEngine

scene = engine.Scene()

camera = engine.Camera()
camera.setName('stars')
scene.add( camera )

context = engine.IRenderContext.create(0, 0, 1920, 1080, False)
camera.setRenderContext( context )

motionController = engine.Orbit()
camera.setMotionController( motionController )

renderPass = engine.IRenderPass.create()
camera.setRenderPass( renderPass )

tychoCloud = engine.PointCloud()
camera.addRenderable( tychoCloud )

renderTarget = engine.IRenderTarget.create( 1920, 1080,
    engine.Format.BRGA8_sRGB, engine.Type.Color,
    engine.Resource.FrameBuffer)
renderPass.addRenderTarget( renderTarget )

print( 'Export ...' )
scene.save(exportPath)

print( 'Success!' )
