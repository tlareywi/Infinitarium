exec(open('./import_common.py').read())
import io
import numpy

X_EXT = 1920
Y_EXT = 1080

#
# Basic scene/rendering setup
#
scene = engine.Scene()
context = engine.IRenderContext.create(0, 0, X_EXT, Y_EXT, False, False)

# Main Scene --------------------------------------------------------------------------------------
mainCamera = engine.Camera()
mainCamera.setName('MainCamera')
mainCamera.setRenderContext( context )
mainCamera.setMotionController( engine.Orbit() )
mainRenderPass = engine.IRenderPass.create()
mainCamera.setRenderPass( mainRenderPass )
mainRenderTarget = engine.IRenderTarget.create( X_EXT, Y_EXT,
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Offscreen)
mainRenderTarget.setName('MainTarget')
mainRenderTarget.setClearColor(0,0,0,0)
mainRenderPass.addRenderTarget( mainRenderTarget, engine.LoadOp.Clear )
scene.add( mainCamera )

# Horizontal Blur ---------------------------------------------------------------------------------
horzBlurCamera = engine.Camera()
horzBlurCamera.setName('HorzBlurCamera')
horzBlurCamera.setRenderContext( context )
horzBlurPass = engine.IRenderPass.create()
horzBlurCamera.setRenderPass( horzBlurPass )
horzBlurTarget = engine.IRenderTarget.create( int(X_EXT / 2), int(Y_EXT / 2),
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Offscreen)
horzBlurTarget.setName('HorzBlurTarget')
horzBlurTarget.setClearColor(0,0,0,0)
horzBlurPass.addRenderTarget( horzBlurTarget, engine.LoadOp.Clear )
scene.add( horzBlurCamera )

horzQuad = engine.Sprite()
horzQuad.addSampler( mainRenderTarget )
horzQuad.setProgram( 'horzBlur' )
horzBlurCamera.addChild( horzQuad )

# Vertical Blur -----------------------------------------------------------------------------------
vertBlurCamera = engine.Camera()
vertBlurCamera.setName('VertBlurCamera')
vertBlurCamera.setRenderContext( context )
vertBlurPass = engine.IRenderPass.create()
vertBlurCamera.setRenderPass( vertBlurPass )
vertBlurTarget = engine.IRenderTarget.create( int(X_EXT / 2), int(Y_EXT / 2),
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Offscreen)
vertBlurTarget.setName('VertBlurTarget')
vertBlurTarget.setClearColor(0,0,0,0)
vertBlurPass.addRenderTarget( vertBlurTarget, engine.LoadOp.Clear )
scene.add( vertBlurCamera )

vertQuad = engine.Sprite()
vertQuad.addSampler( horzBlurTarget )
vertQuad.setProgram( 'vertBlur' )
vertBlurCamera.addChild( vertQuad )

# Bloom pass --------------------------------------------------------------------------------------
#bloomCamera = engine.Camera()
#bloomCamera.setName('BloomCamera')
#bloomCamera.setRenderContext( context )
#bloomPass = engine.IRenderPass.create()
#bloomCamera.setRenderPass( bloomPass )
#bloomTarget = engine.IRenderTarget.create( X_EXT, Y_EXT,
#    engine.Format.BRGA8, engine.Type.Color,
#    engine.Resource.Offscreen)
#bloomPass.addRenderInput( mainRenderTarget )
#bloomPass.addRenderTarget( bloomTarget, engine.LoadOp.Undefined )
# TODO subpass to composite the bloom with original? Or just composite as part of the bloom shader? 
#scene.add( bloomCamera )

# Swapchain pass ----------------------------------------------------------------------------------
fbCamera = engine.Camera()
fbCamera.setName('Swapchain')
fbCamera.setRenderContext( context )
renderPass = engine.IRenderPass.create()
fbCamera.setRenderPass( renderPass )
swapChainTarget = engine.IRenderTarget.create( X_EXT, Y_EXT,
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Swapchain)
swapChainTarget.setClearColor(0,0,0,1)
renderPass.addRenderTarget( swapChainTarget, engine.LoadOp.Clear )
scene.add( fbCamera )

quad = engine.Sprite()
quad.addSampler( mainRenderTarget )
quad.addSampler( vertBlurTarget )
quad.setProgram( 'bounce' ) # Blt mainRenderTarget to framebufer
fbCamera.addChild( quad )

# Add GUI support (draw directly to swapchain target) ---------------------------------------------
initImGUI( scene, context, swapChainTarget )

# Read image and store as engine texture. ---------------------------------------------------------
from PIL import Image # Must have Pillow (pip3 install Pillow)
img = Image.open('../resources/textures/2k_sun.jpg', mode='r')

# Force alpha channel, set to opaque
a_channel = Image.new('L', img.size, 255)   # 'L' 8-bit pixels
img.putalpha(a_channel)

pixelData = numpy.asarray(img) # This will get us the color data only without the image header
linearData = pixelData.flatten()

imgData = engine.DataPack_UINT8(len(linearData))

imgData.getBuffer()[:] = linearData # Perform actual copy of data to engine side 

texture = engine.ITexture.create( img.width, img.height, engine.Format.RGBA8 )
texture.set( engine.wrap(imgData) )

linearData = None # Cleanup
pixelData = None
img = None

# Create spheroid and add to scene ----------------------------------------------------------------
transform = engine.Transform()
transform.rotate( 90.0, 1.0, 0.0, 0.0 ) 
transform.translate( 0.0, 0.0, -4.0 )

sphere = engine.Spheroid(40, 40, 0.0, False) # meridians, parellels, oblateness (always unit size)

sphere.setProgram('radiantBody')
sphere.setUniform( 'frequency', engine.Uniform(engine.UniformType(296.40), engine.UniformType(1.0), engine.UniformType(400.0)) )
sphere.setUniform( 'amplitude', engine.Uniform(engine.UniformType(0.000319), engine.UniformType(0.0001), engine.UniformType(0.001)) )
sphere.setUniform( 'speed', engine.Uniform(engine.UniformType(31.63), engine.UniformType(1.0), engine.UniformType(100.0)) )

sphere.setTexture( texture )

transform.addChild( sphere )
mainCamera.addChild( transform )

# Write scene file --------------------------------------------------------------------------------
exportPath = exportPath + 'radiantBody.ieb'
print('Exporting ' + exportPath)
scene.save(exportPath)

print( 'Success!' )

