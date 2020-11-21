# To run this file in command line interpreter ...
# exec(open("./unit_test.py").read())

# You'll need the tycho2 catalog as one dat file along with the readme.
# These can be obtained from http://cdsarc.u-strasbg.fr/viz-bin/cat/I/259
# Individual catalog parts can be joined into one dat file via the command
# `zcat tyc2.dat.??.gz >tyc2.dat` (or gzcat for OSX)

exec(open('./import_common.py').read())

print('\nReading Tycho 2 Catalog ...')
t = Table.read("tyc2.dat",
readme="readme.tyc2",
format="ascii.cds")

t['BTmag'].fill_value = 0
t['VTmag'].fill_value = 0

scene = engine.Scene()

camera = engine.Camera()
camera.setName('tyc2Stars')
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

tychoCloud = engine.PointCloud()
position = engine.DataPack_FLOAT32(len(t)*3) # xyz
color = engine.DataPack_FLOAT32(len(t)*3) # rgb
apparentMagV = engine.DataPack_FLOAT32(len(t))

tychoCloud.setProgram( 'stars_no_plx' )
tychoCloud.setUniform( 'epsilon', engine.UniformType(0.0001) )
tychoCloud.setUniform( 'diskDensity', engine.UniformType(0.88) )
tychoCloud.setUniform( 'haloDensity', engine.UniformType(6.2) )
tychoCloud.setUniform( 'limitingMagnitude', engine.UniformType(14.0) )
tychoCloud.setUniform( 'saturationMagnitude', engine.UniformType(-2.0) )
tychoCloud.setUniform( 'diskBrightness', engine.UniformType(28.0) )
tychoCloud.setUniform( 'haloBrightness', engine.UniformType(1.0) )

print('Processing ...')

skipped = 0
numRecrods = 0
for record in t.filled():
    if( record['BTmag'] == 0 or record['VTmag'] == 0 ):
        skipped += 1
        continue
    pos = sphereToRectZUp( record['RAdeg'], record['DEdeg'], 1.0 )
    mag, bv = apparentMagColor( record['BTmag'], record['VTmag'] )
    rgb = parser.KelvinToRGB( 8540/(bv+0.865) )
    position.addVec3(pos[0], pos[1], pos[2])
    apparentMagV.add( mag )
    color.addVec3(rgb[0], rgb[1], rgb[2])
    numRecrods += 1

tychoCloud.setNumPoints( numRecrods )

print('\nWriting', numRecrods, 'records.', skipped, 'records skipped to due inomplete data.')

# TODO: can probably leverage more performance by making hard
# definitions on what position, mangitude, etc. need to be in terms
# of formatting.
tychoCloud.addVertexBuffer( engine.wrap(position), 'position' )
tychoCloud.addVertexBuffer( engine.wrap(apparentMagV), 'magnitude' )
tychoCloud.addVertexBuffer( engine.wrap(color), 'color' )
camera.addChild( tychoCloud )

exportPath = '../data/tyco2.ieb'
print('Exporting ' + exportPath)
scene.save(exportPath)
