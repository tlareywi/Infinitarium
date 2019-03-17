
exec(open('./import_common.py').read())

# Read image and store as engine texture.
from PIL import Image # Must have Pillow (pip3 install Pillow)
img = Image.open('/Users/tlareywi/Source/Infinitarium/screen_captures/tycho2.png', mode='r')
bytes = img.tobytes()
imgData = engine.DataPack_UINT8(len(bytes))
i = 0
while i < len(bytes):
    imgData.addVec4(bytes[i], bytes[i+1], bytes[i+2], bytes[i+3])
    i += 4
texture = engine.ITexture.create( img.width, img.height, engine.Format.RGBA8)
texture.set( imgData )

#
