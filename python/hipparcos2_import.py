# To run this file in command line interpreter ...
# exec(open("./unit_test.py").read())

# You'll need the tycho2 catalog as one dat file along with the readme.
# These can be obtained from http://cdsarc.u-strasbg.fr/viz-bin/cat/I/259
# Individual catalog parts can be joined into one dat file via the command
# `zcat tyc2.dat.??.gz >tyc2.dat`.

print('\nReading Hipparcos2 Catalog ...')
t = Table.read("/Volumes/MediaDrive/hip2/hip2.dat",
readme="/Volumes/MediaDrive/hip2/readme.hip2",
format="ascii.cds")

t['Plx'].fill_value = 0

scene = engine.Scene()
hip2Cloud = engine.PointCloud()
position = engine.DataPack_FLOAT32(len(t)*3) # xyz
color = engine.DataPack_FLOAT32(len(t)*3) # rgb
apparentMagV = engine.DataPack_FLOAT32(len(t))

hip2Cloud.setProgram( 'starsDefault' )
hip2Cloud.setUniform( 'epsilon', engine.UniformType(0.0000000001) )
hip2Cloud.setUniform( 'diskDensity', engine.UniformType(0.025) )
hip2Cloud.setUniform( 'haloDensity', engine.UniformType(400.0) )

print('Processing ...')

skipped = 0
numRecrods = 0
for record in t.filled():
    if( record['Plx'] == 0 ):
        skipped += 1
        continue
    distLY = pcToLy( distParsecs(record['Plx']) )
    pos = sphereToRectZUpRads( record['RArad'], record['DErad'], distLY )
    bv = record['B-V']
    rgb = parser.KelvinToRGB( 8540/(bv+0.865) )
    position.addVec3(pos[0], pos[1], pos[2])
    apparentMagV.add( record['Hpmag'] )
    color.addVec3(rgb[0], rgb[1], rgb[2])
    numRecrods += 1

print('\nWriting', numRecrods, 'records.', skipped, 'records skipped to due inomplete data.')

hip2Cloud.addVertexBuffer( position.container(), 'position' )
hip2Cloud.addVertexBuffer( apparentMagV.container(), 'magnitude' )
hip2Cloud.addVertexBuffer( color.container(), 'color' )
scene.add( hip2Cloud )

exportPath = './hip2.ieb'
print('Exporting ' + exportPath)
scene.save(exportPath)
