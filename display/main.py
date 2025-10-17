from PIL import Image

NUM_BUCKETS = 64
BUCKET_SIZE = 8
xlim = 128
ylim = 32

# Initialize 64 empty buckets

buckets = []


with Image.open("clockface _blank.bmp") as img:
    # Ensure it's in 1-bit mode
    img = img.convert('1')
    roundtrip = 8

    for bucketSize in range(4):
        for x in range(128):
            str = ''
            for y in reversed(range(8)):
                pixel = img.getpixel((x, y+(roundtrip*bucketSize)))
                str += ('1' if pixel == 255 else '0')
            buckets.append(str)

hexval = []

for bucket in buckets:
    hexval.append(hex(int(bucket, 2)))

print (hexval)
    


# with Image.open("clockface.bmp") as img:
#     # Ensure it's in 1-bit mode
#     img = img.convert('1')

#     # Get width and height
#     width, height = img.size

#     print(f"Bitmap size: {width}x{height}\n")

#     # Loop through pixels
#     for y in range(height):
#         for x in range(width):
#             pixel = img.getpixel((x, y))
#             print('1' if pixel == 255 else '0', end='')
#             # buckets[y]
#         print()  # Newline after each row