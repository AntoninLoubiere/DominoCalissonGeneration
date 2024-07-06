from PIL import Image, ImageDraw
from math import sqrt, ceil
import sys

SQRT3 = sqrt(3)
IMAGE_SIZE = 4000
MARGIN = 2
IMAGE_TOTAL = IMAGE_SIZE + 2 * MARGIN
size = int(input())
HEX_RADIUS = (SQRT3 * IMAGE_SIZE) / 4
WIDTH = ceil(2 * (HEX_RADIUS + MARGIN))
IMAGE_CENTERX = WIDTH / 2
IMAGE_CENTERY = IMAGE_SIZE / 2 + MARGIN

H_SPACING = IMAGE_SIZE / (4 * size)
W_SPACING = (SQRT3 * H_SPACING) / 2


def hex_to_card(t):
    return (IMAGE_CENTERX + 2 * t[1] * W_SPACING, IMAGE_CENTERY + (2 * t[0] + t[1]) * H_SPACING)

def tuple_hex_to_tuple_card(t):
    return (hex_to_card(t[0]), hex_to_card(t[1]))

def getSegCoord(i: int, j: int, d: int) -> tuple[tuple[int, int], tuple[int, int]]:
    return (((i + 1 if d == 2 else i) - size, j - size), ((i + 1 if d == 0 else i) - size, (j if d == 0 else j + 1) - size))

def drawEdge(i, j, d):
    draw.line(tuple_hex_to_tuple_card(getSegCoord(i, j, d)), fill="black", width=2)

def drawLosange(i, j, d):
    f, t = getSegCoord(i, j, d)
    fc = hex_to_card(f)
    tc = hex_to_card(t)

    vecx = tc[0] - fc[0]
    vecy = tc[1] - fc[1]
    draw.polygon((
        fc,
        (fc[0] + (vecx - vecy * SQRT3) / 2,fc[1] + (vecy + vecx * SQRT3) / 2),
        tc,
        (fc[0] + (vecx + vecy * SQRT3) / 2,fc[1] + (vecy - vecx * SQRT3) / 2)
    ), fill=("#2eb3ff", "#ff2e2e", "#ffe32e")[d])

img = Image.new("RGB", (WIDTH, IMAGE_TOTAL))
draw = ImageDraw.Draw(img)

draw.rectangle(((0, 0), (IMAGE_TOTAL, IMAGE_TOTAL)), fill="white")

OUTLINE = [
    (IMAGE_CENTERX, MARGIN),
    (IMAGE_CENTERX - HEX_RADIUS, IMAGE_SIZE / 4 + MARGIN),
    (IMAGE_CENTERX - HEX_RADIUS, (3 * IMAGE_SIZE) / 4 + MARGIN),
    (IMAGE_CENTERX, IMAGE_SIZE + MARGIN),
    (IMAGE_CENTERX + HEX_RADIUS, (3 * IMAGE_SIZE) / 4 + MARGIN),
    (IMAGE_CENTERX + HEX_RADIUS, IMAGE_SIZE / 4 + MARGIN),
]

EDGES = []

while (l := sys.stdin.readline()) != '':
    data = l.split()
    i = int(data[1])
    j = int(data[2])
    d = int(data[3])
    if data[0] == 'E':
        EDGES.append((i, j, d))
    if data[0] == 'L':
        drawLosange(i, j, d)

for e in EDGES:
    drawEdge(*e)

draw.polygon(OUTLINE, outline="black", width=5)
img.save(sys.argv[1])
