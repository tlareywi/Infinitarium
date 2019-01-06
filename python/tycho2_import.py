# To run this file in command line interpreter ...
# exec(open("./unit_test.py").read())

# You'll need the tycho2 catalog as one dat file along with the readme.
# These can be obtained from http://cdsarc.u-strasbg.fr/viz-bin/cat/I/259
# Individual catalog parts can be joined into one dat file via the command
# `zcat tyc2.dat.??.gz >tyc2.dat`.

exec(open('./BlackBodyParser.py').read())

import math
import importlib
import sys

sys.path.append('/usr/local/lib')

def degreesToRadians(x):
    return x * math.pi / 180.0;

def sphereToRectZUp(ra, dec, r):
    x = r * math.sin(degreesToRadians(90-dec)) * math.cos(degreesToRadians(ra))
    y = r * math.sin(degreesToRadians(90-dec)) * math.sin(degreesToRadians(ra))
    z = r * math.cos(degreesToRadians(90-dec))
    return[x,y,z]

# From https://heasarc.nasa.gov/W3Browse/all/tycho2.html
def apparentMagColor( bt, vt ):
     v = vt - 0.090 * (bt - vt)
     bv = 0.850 * (bt - vt)
     return v, bv

import libInfinitariumEngine
importlib.reload(libInfinitariumEngine)

from astropy.table import Table

engine = libInfinitariumEngine

print('\nReading Tycho 2 Catalog ...')
t = Table.read("tyc2.dat",
readme="readme.tyc2",
format="ascii.cds")

t['BTmag'].fill_value = 0
t['VTmag'].fill_value = 0

scene = engine.Scene()
tychoCloud = engine.PointCloud()
position = engine.DataPack_FLOAT32(len(t)*3) # xyz
color = engine.DataPack_FLOAT32(len(t)*3) # rgb
apparentMagV = engine.DataPack_FLOAT32(len(t))

tychoCloud.setProgram( 'starsDefault' )
tychoCloud.setUniform( 'epsilon', engine.UniformType(0.0000000001) )
tychoCloud.setUniform( 'diskDensity', engine.UniformType(0.025) )
tychoCloud.setUniform( 'haloDensity', engine.UniformType(400.0) )

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

print('\nWriting', numRecrods, 'records.', skipped, 'records skipped to due inomplete data.')

# TODO: can probably leverage more performance by making hard
# definitions on what position, mangitude, etc. need to be in terms
# of formatting.
tychoCloud.addVertexBuffer( position.container(), 'position' )
tychoCloud.addVertexBuffer( apparentMagV.container(), 'magnitude' )
tychoCloud.addVertexBuffer( color.container(), 'color' )
scene.add( tychoCloud )

exportPath = './tyco2.ieb'
print('Exporting ' + exportPath)
scene.save(exportPath)
