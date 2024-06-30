from PIL import Image

SIZE = 1000
img = Image.new("RGB", (SIZE, SIZE))

with open('wg.txt', 'r') as fir:
    for j in range(SIZE):
        l = fir.readline()
        for i in range(SIZE):
            # #F00000 : #FFD700
            # #0000CE : #00D1D1
            if l[i] == ' ':
                c = (0, 0, 0)
            elif l[i] == '^':
                c = (0xFF, 0xD7, 0x00) if (i + j) % 2 == 1 else (0xF0, 0x00, 0x00)
            elif l[i] == 'v':
                c = (0xFF, 0xD7, 0x00) if (i + j) % 2 == 0 else (0xF0, 0x00, 0x00)
            elif l[i] == '<':
                c = (0x00, 0x00, 0xCE) if (i + j) % 2 == 0 else (0x00, 0xD1, 0xD1)
            elif l[i] == '>':
                c = (0x00, 0x00, 0xCE) if (i + j) % 2 == 1 else (0x00, 0xD1, 0xD1)

            img.putpixel((i, j), c)

img.save('imgwg.jpeg', "JPEG")