from PIL import Image
import sys

img = Image.open(sys.argv[1]).convert('L')

UPSCALE = 1
SIZE = min(img.size) * UPSCALE

POIDS_CLAIR = 1
POIDS_SOMBRE = 1
SCALE_FACTOR = 1.5

print(SIZE)

for y in range(SIZE - 1):
    for x in range(SIZE):
        print(POIDS_CLAIR * ((img.getpixel((x / UPSCALE, y / UPSCALE)) + img.getpixel((x / UPSCALE, (y + 1) / UPSCALE))) / 2 + 1) ** SCALE_FACTOR)

for y in range(SIZE):
    for x in range(SIZE - 1):
        print(POIDS_SOMBRE * (256 - (img.getpixel((x / UPSCALE, y / UPSCALE)) + img.getpixel(((x + 1) / UPSCALE, y / UPSCALE))) / 2) ** SCALE_FACTOR)