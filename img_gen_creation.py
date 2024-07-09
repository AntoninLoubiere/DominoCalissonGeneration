from PIL import Image, ImageDraw
import sys

CROPPED = False
SIZE = int(input())
IMG_SIZE = SIZE
PIXEL_SIZE = min(1, 2000 // IMG_SIZE)
COLORS = [(0xFF, 0xD7, 0x00), (0xF0, 0x00, 0x00), (0x00, 0x00, 0xCE), (0x00, 0xD1, 0xD1)]
# COLORS = ["#eee", "#eee", "#111", "#111"]
# COLORS = ["#FFE6F3", "#FFD9ED", "#0E4725", "#124D29"]
# COLORS = ["#FFB3B3", "#F5A6A6", "#3B0000", "#450000"]
# COLORS = ["#B8C4E6", "#ADBCE6", "#020C26", "#031133"]

if not CROPPED:
    IMG_SIZE *= 2

img = Image.new("RGB", (IMG_SIZE * PIXEL_SIZE, IMG_SIZE * PIXEL_SIZE))
draw = ImageDraw.Draw(img)
def create_img():
    size = int(input())
    print(size)

    for j in range(3 * IMG_SIZE // 2  if CROPPED else IMG_SIZE):
        l = sys.stdin.readline()
        if j < IMG_SIZE // 2 and CROPPED:
            continue
        for i in range(IMG_SIZE // 2, 3 * IMG_SIZE // 2) if CROPPED else range(IMG_SIZE):
            # #F00000 : #FFD700
            # #0000CE : #00D1D1
            if l[i] == ' ':
                c = (0, 0, 0)
            elif l[i] == '^':
                c = COLORS[0] if (i + j + IMG_SIZE - size) % 2 == 1 else COLORS[1]
            elif l[i] == 'v':
                c = COLORS[0] if (i + j + IMG_SIZE - size) % 2 == 0 else COLORS[1]
            elif l[i] == '<':
                c = COLORS[2] if (i + j + IMG_SIZE - size) % 2 == 0 else COLORS[3]
            elif l[i] == '>':
                c = COLORS[2] if (i + j + IMG_SIZE - size) % 2 == 1 else COLORS[3]
            else:
                c = "#111"

            oi = i - IMG_SIZE // 2 if CROPPED else i
            oj = j - IMG_SIZE // 2 if CROPPED else j


            draw.rectangle((
                (PIXEL_SIZE * oi, PIXEL_SIZE * oj),
                (PIXEL_SIZE * (oi+1)-1, PIXEL_SIZE * (oj+1)-1)
            ), c)

    img.save(sys.argv[1].replace('%d', str(size)), "JPEG")

for i in range(SIZE):
    create_img()