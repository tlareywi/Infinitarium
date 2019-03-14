
import sys

# Read image and store as engine texture.
from PIL import Image # Must have Pillow (pip3 install Pillow)
img = Image.open(fh, mode='r')

texture = engine.DataPack_FLOAT32(len(t))
engine.ITexture.create( img.width, img.height, engine.Format.RGBA8)
