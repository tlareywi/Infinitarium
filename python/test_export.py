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

tychoCloud = engine.PointCloud()
scene = engine.Scene()
scene.add( tychoCloud )
print( 'Export PointCloud ...' )
scene.save(exportPath)

print( 'Success!' )
