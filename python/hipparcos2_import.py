# To run this file in command line interpreter ...
# exec(open("./unit_test.py").read())

# You'll need the tycho2 catalog as one dat file along with the readme.
# These can be obtained from http://cdsarc.u-strasbg.fr/viz-bin/cat/I/259
# Individual catalog parts can be joined into one dat file via the command
# `zcat tyc2.dat.??.gz >tyc2.dat`.

# Note, astropy has an issue interpreting the hip2 readme. It's fixed
# by replacing the last line of the hip2.readme file description with the
# following lines.
# 172-178  F7.2 ---      UW1     Upper-triangular weight matrix (G1)
# 179-185  F7.2 ---      UW2     Upper-triangular weight matrix (G1)
# 186-192  F7.2 ---      UW3     Upper-triangular weight matrix (G1)
# 193-199  F7.2 ---      UW4     Upper-triangular weight matrix (G1)
# 200-206  F7.2 ---      UW5     Upper-triangular weight matrix (G1)
# 207-213  F7.2 ---      UW6     Upper-triangular weight matrix (G1)
# 214-220  F7.2 ---      UW7     Upper-triangular weight matrix (G1)
# 221-227  F7.2 ---      UW8     Upper-triangular weight matrix (G1)
# 228-234  F7.2 ---      UW9     Upper-triangular weight matrix (G1)
# 235-241  F7.2 ---      UW10    Upper-triangular weight matrix (G1)
# 242-248  F7.2 ---      UW11    Upper-triangular weight matrix (G1)
# 249-255  F7.2 ---      UW12    Upper-triangular weight matrix (G1)
# 256-262  F7.2 ---      UW13    Upper-triangular weight matrix (G1)
# 263-269  F7.2 ---      UW14    Upper-triangular weight matrix (G1)
# 270-276  F7.2 ---      UW15    Upper-triangular weight matrix (G1)

exec(open('./import_common.py').read())

print('\nReading Hipparcos2 Catalog ...')
t = Table.read("/Volumes/MediaDrive/hip2/hip2.dat",
readme="/Volumes/MediaDrive/hip2/readme.hip2",
format="ascii.cds")

t['Plx'].fill_value = 0

scene = engine.Scene()

camera = engine.Camera()
camera.setName('hipStars')
scene.add( camera )

context = engine.IRenderContext.create(0, 0, 1920, 1080, False)
camera.setRenderContext( context )

renderPass = engine.IRenderPass.create()
camera.setRenderPass( renderPass )
camera.setMotionController( engine.Orbit() )

# Color target
renderTarget = engine.IRenderTarget.create( 1920, 1080,
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.FrameBuffer)
renderTarget.setClear( True )
renderTarget.setClearColor(0,0,0,1)
renderPass.addRenderTarget( renderTarget )

# Pick buffer
pickTarget = engine.IRenderTarget.create( 1920, 1080,
    engine.Format.RU32, engine.Type.Color,
    engine.Resource.Memory)
renderTarget.setClear( True )
renderTarget.setClearColor(0,0,0,0)
renderPass.addRenderTarget( pickTarget )

hip2Cloud = engine.PointCloud()
position = engine.DataPack_FLOAT32(len(t)*3) # xyz
color = engine.DataPack_FLOAT32(len(t)*3) # rgb
apparentMagV = engine.DataPack_FLOAT32(len(t))

hip2Cloud.setProgram( 'starsDefault' )
hip2Cloud.setUniform( 'epsilon', engine.UniformType(0.0001) )
hip2Cloud.setUniform( 'diskDensity', engine.UniformType(0.88) )
hip2Cloud.setUniform( 'haloDensity', engine.UniformType(6.2) )
hip2Cloud.setUniform( 'limitingMagnitude', engine.UniformType(14.0) )
hip2Cloud.setUniform( 'saturationMagnitude', engine.UniformType(-2.0) )
hip2Cloud.setUniform( 'diskBrightness', engine.UniformType(28.0) )
hip2Cloud.setUniform( 'haloBrightness', engine.UniformType(1.0) )

print('Processing ...')

skipped = 0
numRecrods = 0
for record in t.filled():
    if( record['Plx'] <= 0 ): # TODO: maybe negative plx can be corrected somehow? plx error field?
        skipped += 1
        continue
    distPC = distParsecs(record['Plx'])
    pos = sphereToRectZUpRads( record['RArad'], record['DErad'], distPC )
    bv = record['B-V']
    rgb = parser.KelvinToRGB( 8540/(bv+0.865) )
    position.addVec3(pos[0], pos[1], pos[2])
    magnitude = appMagToAbsMag(record['Hpmag'], distPC)
    apparentMagV.add( magnitude )
    color.addVec3(rgb[0], rgb[1], rgb[2])
    numRecrods += 1

hip2Cloud.setNumPoints( numRecrods )

print('\nWriting', numRecrods, 'records.', skipped, 'records skipped to due inomplete or non-sensical data.')

hip2Cloud.addVertexBuffer( position.container(), 'position' )
hip2Cloud.addVertexBuffer( apparentMagV.container(), 'magnitude' )
hip2Cloud.addVertexBuffer( color.container(), 'color' )
camera.addChild( hip2Cloud )

exportPath = '../data/hip2.ieb'
print('Exporting ' + exportPath)
scene.save(exportPath)
