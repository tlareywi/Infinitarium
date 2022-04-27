exec(open('./import_common.py').read())
import io
import numpy

X_EXT = 1920
Y_EXT = 1080

BLUR_DIV = 4

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

# DownSample --------------------------------------------------------------------------------------
downSampleCamera = engine.Camera()
downSampleCamera.setName('DownSampleCamera')
downSampleCamera.setRenderContext( context )
downSamplePass = engine.IRenderPass.create()
downSampleCamera.setRenderPass( downSamplePass )
downSampleTarget = engine.IRenderTarget.create( int(X_EXT / BLUR_DIV), int(Y_EXT / BLUR_DIV),
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Offscreen)
downSampleTarget.setName('DownSampleTarget')
downSampleTarget.setClearColor(0,0,0,0)
downSamplePass.addRenderTarget( downSampleTarget, engine.LoadOp.Clear )
scene.add( downSampleCamera )

downSampleQuad = engine.Sprite()
downSampleQuad.addSampler( mainRenderTarget )
downSampleQuad.setProgram( 'bounce' )
downSampleCamera.addChild( downSampleQuad )

# Horizontal Blur ---------------------------------------------------------------------------------
horzBlurCamera = engine.Camera()
horzBlurCamera.setName('HorzBlurCamera')
horzBlurCamera.setRenderContext( context )
horzBlurPass = engine.IRenderPass.create()
horzBlurCamera.setRenderPass( horzBlurPass )
horzBlurTarget = engine.IRenderTarget.create( int(X_EXT / BLUR_DIV), int(Y_EXT / BLUR_DIV),
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Offscreen)
horzBlurTarget.setName('HorzBlurTarget')
horzBlurTarget.setClearColor(0,0,0,0)
horzBlurPass.addRenderTarget( horzBlurTarget, engine.LoadOp.Clear )
scene.add( horzBlurCamera )

horzQuad = engine.Sprite()
horzQuad.addSampler( downSampleTarget )
horzQuad.setProgram( 'horzBlur' )
horzQuad.setUniform( 'radius', engine.Uniform(engine.UniformType(1.0), engine.UniformType(0.1), engine.UniformType(10.0)) )
horzQuad.setUniform( 'power', engine.Uniform(engine.UniformType(0.25), engine.UniformType(0.1), engine.UniformType(1.0)) )
horzBlurCamera.addChild( horzQuad )

# Vertical Blur -----------------------------------------------------------------------------------
vertBlurCamera = engine.Camera()
vertBlurCamera.setName('VertBlurCamera')
vertBlurCamera.setRenderContext( context )
vertBlurPass = engine.IRenderPass.create()
vertBlurCamera.setRenderPass( vertBlurPass )
vertBlurTarget = engine.IRenderTarget.create( int(X_EXT / BLUR_DIV), int(Y_EXT / BLUR_DIV),
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Offscreen)
vertBlurTarget.setName('VertBlurTarget')
vertBlurTarget.setClearColor(0,0,0,0)
vertBlurPass.addRenderTarget( vertBlurTarget, engine.LoadOp.Clear )
scene.add( vertBlurCamera )

vertQuad = engine.Sprite()
vertQuad.addSampler( horzBlurTarget )
vertQuad.setProgram( 'vertBlur' )
vertQuad.setUniform( 'radius', engine.Uniform(engine.UniformType(1.0), engine.UniformType(0.1), engine.UniformType(10.0)) )
vertQuad.setUniform( 'power', engine.Uniform(engine.UniformType(0.25), engine.UniformType(0.1), engine.UniformType(1.0)) )
vertBlurCamera.addChild( vertQuad )

# Radial Blur --------------------------------------------------------------------------------------
radialBlurCamera = engine.Camera()
radialBlurCamera.setName('RadialBlurCamera')
radialBlurCamera.setRenderContext( context )
radialBlurPass = engine.IRenderPass.create()
radialBlurCamera.setRenderPass( radialBlurPass )
radialBlurTarget = engine.IRenderTarget.create( int(X_EXT / BLUR_DIV), int(Y_EXT / BLUR_DIV),
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Offscreen)
radialBlurTarget.setName('RadialBlurTarget')
radialBlurTarget.setClearColor(0,0,0,0)
radialBlurPass.addRenderTarget( radialBlurTarget, engine.LoadOp.Clear )
scene.add( radialBlurCamera )

radialBlurQuad = engine.BloomSprite()
radialBlurQuad.addSampler( vertBlurTarget )
radialBlurQuad.setProgram( 'radialBlur' )
radialBlurQuad.setUniform( 'density', engine.Uniform(engine.UniformType(1.0), engine.UniformType(0.1), engine.UniformType(1.0)) )
radialBlurQuad.setUniform( 'weight', engine.Uniform(engine.UniformType(0.186), engine.UniformType(0.001), engine.UniformType(0.4)) )
radialBlurQuad.setUniform( 'decay', engine.Uniform(engine.UniformType(0.997684), engine.UniformType(0.7), engine.UniformType(1.1)) )
radialBlurQuad.setUniform( 'exposure', engine.Uniform(engine.UniformType(1.0), engine.UniformType(0.1), engine.UniformType(1.0)) )
radialBlurQuad.setUniform( 'saturation', engine.Uniform(engine.UniformType(2.253), engine.UniformType(1.0), engine.UniformType(5.0)) )

radialBlurCamera.addChild( radialBlurQuad )

# Composite pass -----------------------------------------------------------------------------------
compositeCamera = engine.Camera()
compositeCamera.setName('CompositeCamera')
compositeCamera.setRenderContext( context )
compositePass = engine.IRenderPass.create()
compositeCamera.setRenderPass( compositePass )
compositeTarget = engine.IRenderTarget.create( X_EXT, Y_EXT,
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Offscreen)
compositeTarget.setName('CompositeTarget')
compositeTarget.setClearColor(0,0,0,0)
compositePass.addRenderTarget( compositeTarget, engine.LoadOp.Clear )
scene.add( compositeCamera )

combineQuad = engine.Sprite()
combineQuad.addSampler( mainRenderTarget )
combineQuad.addSampler( radialBlurTarget )
combineQuad.setProgram( 'combine' ) 
compositeCamera.addChild( combineQuad )

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
quad.addSampler( compositeTarget )
quad.setProgram( 'FXAA' ) 
fbCamera.addChild( quad )

# Add GUI support (draw directly to swapchain target) ---------------------------------------------
initImGUI( scene, context, swapChainTarget )

os.chdir(homeDir)

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

sphere = engine.SpheroidEmitter(40, 40, 0.0, False) # meridians, parellels, oblateness (always unit size)

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

