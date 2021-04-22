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
from PIL import Image # Must have Pillow (pip3 install Pillow)
img = Image.open('../resources/textures/jupiter2_1k.jpg', mode='r')

# Force alpha channel, set to opaque
a_channel = Image.new('L', img.size, 255)   # 'L' 8-bit pixels
img.putalpha(a_channel)

pixelData = numpy.asarray(img) # This will get us the color data only without the image header
linearData = pixelData.flatten()

imgData = engine.DataPack_UINT8(len(linearData))

imgData.getBuffer()[:] = linearData # Perform actual copy of data to engine side 

texture = engine.ITexture.create( img.width, img.height, engine.Format.RGBA8_sRGB )
texture.set( engine.wrap(imgData) )

linearData = None # Cleanup
pixelData = None
img = None

#
# Create spheroid and add to scene. Sphereoid is z-axis aligned for VSOP-87 but we can rotate for y up.
#
transform = engine.Transform()
transform.rotate( 90.0, 1.0, 0.0, 0.0 ) 
transform.rotate( 180.0, 0.0, 1.0, 0.0 )
transform.translate( 0.0, 0.0, -4.0 )

sphere = engine.Spheroid(40, 40, 0.0, False) # meridians, parellels, oblateness (always unit size)
sphere.setProgram('solarBody')
sphere.setTexture( texture )

transform.addChild( sphere )
camera.addChild( transform )

# Add GUI support
initImGUI( scene, context, renderTarget )

#
# Write scene file.
#
exportPath = exportPath + 'sphere.ieb'
print('Exporting ' + exportPath)
scene.save(exportPath)

