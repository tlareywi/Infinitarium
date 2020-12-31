   # To run this file in command line interpreter ...
# exec(open("./unit_test.py").read())

# You'll need the Hipparcos (new reduction) catalog as one dat file along with the readme.
# These can be obtained from http://cdsarc.u-strasbg.fr/viz-bin/cat/I/311

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
t = Table.read("E:/HipTyc/hip2.dat",
        readme="E:/HipTyc/readme.hip2",
        format="ascii.cds")

t['Plx'].fill_value = 0

scene = engine.Scene()
scene.setName('Hipparcos2 Catalog')

camera = engine.Camera()
camera.setName('Hip2 Camera')
scene.add( camera )

context = engine.IRenderContext.create(0, 0, 1920, 1080, False, False)
camera.setRenderContext( context )

origin = engine.UniversalPoint( 0, 0, 0, engine.Unit.Parsec )

# Main RenderPass
renderPass = engine.IRenderPass.create()
camera.setRenderPass( renderPass )
motionController = engine.Orbit() 
motionController.setHomeSystem( origin )
camera.setMotionController( motionController )

# Color target
renderTarget = engine.IRenderTarget.create( 1920, 1080,
    engine.Format.BRGA8, engine.Type.Color,
    engine.Resource.Swapchain)
renderTarget.setClearColor(0,0,0,1)
renderTarget.setBlendState( engine.BlendState(engine.Op.Add, engine.Op.Add, engine.Factor.One, engine.Factor.One, engine.Factor.SourceAlpha, engine.Factor.OneMinusSourceAlpha) )
renderPass.addRenderTarget( renderTarget, engine.LoadOp.Clear )

# Pick buffer
pickTarget = engine.IRenderTarget.create( 1920, 1080,
    engine.Format.RF32, engine.Type.Color,
    engine.Resource.Offscreen)
pickTarget.setClearColor(0,0,0,0)
renderPass.addRenderTarget( pickTarget, engine.LoadOp.Clear )

hip2Cloud = engine.PointCloud()
hip2Cloud.setName('Hip2 PointCloud')
position = engine.DataPack_FLOAT32(len(t)*3) # xyz
color = engine.DataPack_FLOAT32(len(t)*3) # rgb
apparentMagV = engine.DataPack_FLOAT32(len(t))

hip2Cloud.setProgram( 'starsDefault' ) # Shader program for star catalogs with dinstance info / Plx
hip2Cloud.setUniform( 'epsilon', engine.Uniform(engine.UniformType(0.0001), engine.UniformType(0.00001), engine.UniformType(0.001)) )
hip2Cloud.setUniform( 'diskDensity', engine.Uniform(engine.UniformType(0.88), engine.UniformType(0.1), engine.UniformType(2.0)) )
hip2Cloud.setUniform( 'haloDensity', engine.Uniform(engine.UniformType(6.2), engine.UniformType(0.1), engine.UniformType(10.0)) )
hip2Cloud.setUniform( 'limitingMagnitude', engine.Uniform(engine.UniformType(14.0), engine.UniformType(-4.0), engine.UniformType(18.0)) )
hip2Cloud.setUniform( 'saturationMagnitude', engine.Uniform(engine.UniformType(-2.0), engine.UniformType(-4.0), engine.UniformType(4.0)) )
hip2Cloud.setUniform( 'diskBrightness', engine.Uniform(engine.UniformType(28.0), engine.UniformType(0.0), engine.UniformType(50.0)) )
hip2Cloud.setUniform( 'haloBrightness', engine.Uniform(engine.UniformType(1.0), engine.UniformType(0.0), engine.UniformType(50.0)) )

print('Processing ...')

skipped = 0
numRecrods = 0
maxDist = 0
for record in t.filled():
    if( record['Plx'] <= 0 ): # TODO: maybe negative plx can be corrected somehow? plx error field?
        skipped += 1
        continue
    distPC = distParsecs(record['Plx'])
    if( distPC > maxDist ):
        maxDist = distPC
    pos = sphereToRectZUpRads( record['RArad'], record['DErad'], distPC )
    bv = record['B-V']
    rgb = parser.KelvinToRGB( 8540/(bv+0.865) )
    position.addVec3(pos[0], pos[1], pos[2])
    magnitude = appMagToAbsMag(record['Hpmag'], distPC)
    apparentMagV.add( magnitude )
    color.addVec3(rgb[0], rgb[1], rgb[2])
    numRecrods += 1

hip2Cloud.setNumPoints( numRecrods )

print('\nWriting', numRecrods, 'records.', skipped, 'records skipped to due incomplete or non-sensical data.')

hip2Cloud.addVertexBuffer( engine.wrap(position), 'position' )
hip2Cloud.addVertexBuffer( engine.wrap(apparentMagV), 'magnitude' )
hip2Cloud.addVertexBuffer( engine.wrap(color), 'color' )
camera.addChild( hip2Cloud )

initImGUI( scene, context, renderTarget )

exportPath = exportPath + 'hip2.ieb'
print('Exporting ' + exportPath)
scene.save(exportPath)
