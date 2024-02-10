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

import csv;

import sqlite3
dbConnection = sqlite3.connect('../data/infinitarium.db')
cursor = dbConnection.cursor()
cursor.execute('''DROP TABLE IF EXISTS HipStars''')
cursor.execute('''CREATE TABLE HipStars(hipid integer, name text, ra real, dec real)''')

dataRoot = '/Users/trystan/StarCatalogs/HipTyc/'

if platform.system() == 'Windows':
    dataRoot = 'E:/HipTyc/'

print('\nReading Hipparcos2 Catalog ...')
t = Table.read(dataRoot + "hip2.dat",
        readme=dataRoot + "readme.hip2",
        format="ascii.cds")

print('Building Scene ...')

t['Plx'].fill_value = 0

scene = engine.Scene()
scene.setName('Hipparcos2 Catalog')

camera = engine.Camera()
camera.setName('Hip2 Camera')
scene.add( camera )

context = engine.IRenderContext.create(0, 0, 1920, 1080, False, False)
camera.setRenderContext( context )

origin = engine.UniversalPoint( 0, 0, 0, engine.Unit.Parsec )
stars3D = engine.CoordinateSystem( origin, 1.0e9, engine.Unit.Parsec )

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

hip2Cloud.setQuery( 'SELECT name FROM HipStars WHERE name IS NOT NULL ORDER BY name ASC;' )
hip2Cloud.setLabel( 'Common Stars' )

print('Reading common names ...')

# Parse star common names file
common_names = {}
fh = open( homeDir + '/common_star_names.txt', "r")
for line in fh.read().splitlines():
    fields = line.split('|')
    if len(fields) > 1:
        common_names[fields[0]] = fields[1]
fh.close()

print('Processing ...')

insertStarQuery = """INSERT INTO HipStars (hipid, name, ra, dec) VALUES (?, ?, ?, ?);"""

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
    # Insert record into application DB for representation in the navigation UI
    hipId = record['HIP']
    data_tuple = ( hipId, common_names.get(str(hipId)), record['RArad'], record['DErad'] )
    cursor.execute( insertStarQuery, data_tuple )

dbConnection.commit()
dbConnection.close()
hip2Cloud.setNumPoints( numRecrods )

print('\nWriting', numRecrods, 'records.', skipped, 'records skipped to due incomplete or non-sensical data.')

hip2Cloud.addVertexBuffer( engine.wrap(position), 'position' )
hip2Cloud.addVertexBuffer( engine.wrap(apparentMagV), 'magnitude' )
hip2Cloud.addVertexBuffer( engine.wrap(color), 'color' )
camera.addChild( stars3D )
stars3D.addChild( hip2Cloud )

initImGUI( scene, context, renderTarget )

exportPath = homeDir + '/' + exportPath + 'hip2.ieb'
print('Exporting ' + exportPath)
scene.save(exportPath)
print('Success!')
