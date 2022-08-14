exec(open('./import_common.py').read())
import io
import numpy

#
# Initialize DB table for local solar system
#
import sqlite3
dbConnection = sqlite3.connect('../data/infinitarium.db')
cursor = dbConnection.cursor()
cursor.execute('''DROP TABLE IF EXISTS SolarSystem''')
cursor.execute('''CREATE TABLE SolarSystem(id integer, name text)''')

X_EXT = 1920
Y_EXT = 1080

#
# Basic scene/rendering setup
#
scene = engine.Scene()
context = engine.IRenderContext.create(0, 0, X_EXT, Y_EXT, False, False)

motionController = engine.Orbit()

# Main Scene --------------------------------------------------------------------------------------
mainCamera = engine.Camera()
mainCamera.setName('MainCamera')
mainCamera.setRenderContext( context )
mainCamera.setMotionController( motionController )
mainRenderPass = engine.IRenderPass.create()
mainCamera.setRenderPass( mainRenderPass )
mainRenderTarget = engine.IRenderTarget.create( X_EXT, Y_EXT,
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Offscreen)
mainRenderTarget.setName('MainTarget')
mainRenderTarget.setClearColor(0,0,0,0)
mainRenderPass.addRenderTarget( mainRenderTarget, engine.LoadOp.Clear )
scene.add( mainCamera )

###
### Can insert, exec(open()), a factored-out post process pipline setup here.
###

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
quad.setProgram( 'FXAA' ) 
fbCamera.addChild( quad )

# Add GUI support (draw directly to swapchain target) ---------------------------------------------
initImGUI( scene, context, swapChainTarget )

os.chdir(homeDir)

# Read image and store as engine texture. ---------------------------------------------------------
from PIL import Image # Must have Pillow (pip3 install Pillow)
img = Image.open('../resources/textures/jupiter2_1k.jpg', mode='r')

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

# Create Coordinate System for Solar System -------------------------------------------------------
coordRoot = engine.UniversalPoint( 0, 0, 0, engine.Unit.Parsec ) # Center in parent coordinate system units
solRoot = engine.UniversalPoint( 0, 0, 0, engine.Unit.AstronomicalUnit ) # We want to initialize in the solar system
motionController.setHomeSystem( solRoot )
coordSystem = engine.CoordinateSystem( coordRoot, 50000, engine.Unit.AstronomicalUnit )
coordSystem.setQuery( 'SELECT name FROM SolarSystem WHERE name IS NOT NULL ORDER BY name ASC;' )
coordSystem.setLabel( 'Solar System' )

# Create spheroid and add to scene ----------------------------------------------------------------
insertBodyQuery = '''INSERT INTO SolarSystem (id, name) VALUES (?,?);'''

position = engine.Transform() 
position.translate( 0.0, 0.0, -4.0 )
position.setPositionCallback( engine.PositionCallbackId.Jupiter ) 

rotation = engine.Transform()
rotation.rotate( 90.0, 1.0, 0.0, 0.0 ) 
rotation.setRotation( 9.9250 ) # sidereal rotation period (hrs)

sphere = engine.Spheroid(40, 40, 0.0, False) # meridians, parellels, oblateness (always unit size)

data_tuple = ( 4, 'Jupiter' )
cursor.execute( insertBodyQuery, data_tuple )
sphere.setName( 'Jupiter' )

sphere.setProgram('solarBody')

sphere.setTexture( texture )

coordSystem.addChild( rotation )
rotation.addChild( position )
position.addChild( sphere )

mainCamera.addChild( coordSystem )

dbConnection.commit()
dbConnection.close()

# Write scene file --------------------------------------------------------------------------------
exportPath = exportPath + 'sol_base.ieb'
print('Exporting ' + exportPath)
scene.save(exportPath)

print( 'Success!' )

