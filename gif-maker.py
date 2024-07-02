from PIL import Image
import sys

images = [Image.open(sys.argv[1].replace('%d', str(i + 1))) for i in range(250)]
for i in range(30):
    images.append(images[-1])

images[0].save(sys.argv[2], save_all=True, append_images=images[1:], duration=40, loop=0)