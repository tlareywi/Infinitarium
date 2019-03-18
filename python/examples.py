
exec(open('./import_common.py').read())

#
# Read image and store as engine texture.
#
from PIL import Image # Must have Pillow (pip3 install Pillow)
img = Image.open('../screen_captures/tycho2.png', mode='r')
bytes = img.tobytes()

imgData = engine.DataPack_UINT8(len(bytes))
imgData.getBuffer()[:] = bytes # Perform actual copy of data to engine side 
bytes = None

texture = engine.ITexture.create( img.width, img.height, engine.Format.RGBA8 )
texture.set( imgData.container() )

img = None

#
# 
#

