exec(open('./import_common.py').read())
import io
import numpy

#
# Basic scene/rendering setup
#
scene = engine.Scene()

camera = engine.Camera()
camera.setName('sprite test')
scene.add( camera )

context = engine.IRenderContext.create(0, 0, 1920, 1080, False, False)
camera.setRenderContext( context )

renderPass = engine.IRenderPass.create()
camera.setRenderPass( renderPass )
camera.setMotionController( engine.Orbit() )

renderTarget = engine.IRenderTarget.create( 1920, 1080,
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Swapchain)
renderTarget.setClearColor(0,0,0,1)
renderPass.addRenderTarget( renderTarget, engine.LoadOp.Clear )

#
# Read image and store as engine texture.
#
from PIL import Image # Must have Pillow (`pip3 install Pillow`, or on Windows `python -m pip install Pillow`)
img = Image.open('../resources/crosshair.png', mode='r')
output = io.BytesIO()
img.save(output, format='BMP')
out = output.getbuffer()
imgData = engine.DataPack_UINT8(len(out))

imgData.getBuffer()[:] = out # Perform actual copy of data to engine side 
bytes = None

texture = engine.ITexture.create( img.width, img.height, engine.Format.RGBA8 )
texture.set( engine.wrap(imgData) )

img = None

#
# Create sprite and add to scene.
#
crosshair = engine.Sprite()
crosshair.setTexture( texture )
camera.addChild( crosshair )

#
# Write scene file.
#
exportPath = exportPath + 'sprite.ieb'
print('Exporting ' + exportPath)
scene.save(exportPath)

