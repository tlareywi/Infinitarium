# Unit tests for boost serialization on engine objects.

import importlib
import sys

sys.path.append('/usr/local/lib')

import libInfinitariumEngine
importlib.reload(libInfinitariumEngine)

exportPath = './test.ieb'

engine = libInfinitariumEngine

scene = engine.Scene()
print( 'Export Scene ...' )
scene.save(exportPath)

camera = engine.Camera()
scene.add( camera )
print( 'Export Scene with Camera ...' )
scene.save( exportPath )

renderPass = engine.IRenderPass.create()
camera.setRenderPass( renderPass )
print( 'Export Scene with Camera + RenderPass ...' )
scene.save( exportPath )

tychoCloud = engine.PointCloud()
camera.addRenderable( tychoCloud )
print( 'Export PointCloud ...' )
scene.save(exportPath)

print( 'Success!' )
