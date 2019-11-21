exec(open('./import_common.py').read())
import io
import numpy

#
# Basic scene/rendering setup
#
scene = engine.Scene()

camera = engine.Camera()
camera.setName('spheroid test')
scene.add( camera )

context = engine.IRenderContext.create(0, 0, 1920, 1080, False)
camera.setRenderContext( context )

renderPass = engine.IRenderPass.create()
camera.setRenderPass( renderPass )
camera.setMotionController( engine.Orbit() )

renderTarget = engine.IRenderTarget.create( 1920, 1080,
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.FrameBuffer)
renderTarget.setClear( True )
renderTarget.setClearColor(0,0,0,1)
renderPass.addRenderTarget( renderTarget )

#
# Read image and store as engine texture.
#
from PIL import Image # Must have Pillow (pip3 install Pillow)
img = Image.open('../resources/textures/jupiter2_1k.jpg', mode='r')

# Force alpha channel, set to opaque
a_channel = Image.new('L', img.size, 255)   # 'L' 8-bit pixels
img.putalpha(a_channel)

output = io.BytesIO()
img.save(output, format='BMP')
out = output.getbuffer()

imgData = engine.DataPack_UINT8(len(out))

imgData.getBuffer()[:] = out # Perform actual copy of data to engine side 
bytes = None

texture = engine.ITexture.create( img.width, img.height, engine.Format.RGBA8_sRGB )
texture.set( imgData.container() )

img = None

#
# Create spheroid and add to scene.
#
transform = engine.Transform()
transform.rotate( 90.0, -1.0, 0.0, 0.0 ) 
transform.translate( 0.0, 0.0, 4.0 )

sphere = engine.Spheroid(40, 40, 0.0, False) # meridians, parellels, oblateness (always unit size)
sphere.setProgram('star3D')
sphere.setTexture( texture )

transform.addChild( sphere )
camera.addChild( transform )

#
# Write scene file.
#
exportPath = '../data/sphere.ieb'
print('Exporting ' + exportPath)
scene.save(exportPath)

