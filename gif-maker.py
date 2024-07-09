from PIL import Image
import sys

# Commande : python gif-maker <taille> entrée-%d.jpeg sortie.gif

images = [Image.open(sys.argv[2].replace('%d', str(i + 1))) for i in range(int(sys.argv[1]))]
for i in range(30):
    images.append(images[-1])

images[0].save(sys.argv[3], save_all=True, append_images=images[1:], duration=40, loop=0)