from PIL import Image, ImageDraw
import sys

CROPPED = False
SIZE = int(input())
if not CROPPED:
    SIZE *= 2
PIXEL_SIZE = max(1, 500 // SIZE)
img = Image.new("RGB", (SIZE * PIXEL_SIZE, SIZE * PIXEL_SIZE))
draw = ImageDraw.Draw(img)
COLORS = [(0xFF, 0xD7, 0x00), (0xF0, 0x00, 0x00), (0x00, 0x00, 0xCE), (0x00, 0xD1, 0xD1)]
# COLORS = ["#eee", "#eee", "#111", "#111"]
# COLORS = ["#FFE6F3", "#FFD9ED", "#0E4725", "#124D29"]
# COLORS = ["#B8C4E6", "#ADBCE6", "#020C26", "#031133"]

for j in range(3 * SIZE // 2  if CROPPED else SIZE):
    l = sys.stdin.readline()
    if j < SIZE // 2 and CROPPED:
        continue
    for i in range(SIZE // 2, 3 * SIZE // 2) if CROPPED else range(SIZE):
        # #F00000 : #FFD700
        # #0000CE : #00D1D1
        if l[i] == ' ':
            c = (0, 0, 0)
        elif l[i] == '^':
            c = COLORS[0] if (i + j) % 2 == 1 else COLORS[1]
        elif l[i] == 'v':
            c = COLORS[0] if (i + j) % 2 == 0 else COLORS[1]
        elif l[i] == '<':
            c = COLORS[2] if (i + j) % 2 == 0 else COLORS[3]
        elif l[i] == '>':
            c = COLORS[2] if (i + j) % 2 == 1 else COLORS[3]

        oi = i - SIZE // 2 if CROPPED else i
        oj = j - SIZE // 2 if CROPPED else j


        draw.rectangle((
            (PIXEL_SIZE * oi, PIXEL_SIZE * oj),
            (PIXEL_SIZE * (oi+1)-1, PIXEL_SIZE * (oj+1)-1)
        ), c)

img.save(sys.argv[1], "JPEG")