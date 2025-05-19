#!/usr/bin/env python3

# Usage: provide a font bitmap image (in PNG) with a black background and white pixels.
#   Call the script like this: ./fontgen.py IMAGE.PNG CELL_WIDTH CELL_HEIGHT N_HORIZONTAL_CELLS
#   Example: ./fontgen.py ibm.png 8 16 16

import argparse
from PIL import Image
import os

def bitmap_to_cheader(image_path, cell_width, cell_height, horizontal_cells, initial_char=32):
    """
    Converts a bitmap font image to a C header file with binary data.

    Args:
        image_path: Path to the bitmap image (PNG format)
        cell_width: Width of each character cell in pixels
        cell_height: Height of each character cell in pixels
        horizontal_cells: Number of cells horizontally in the image
        initial_char: ASCII code of the first character (default: 32, space)
    """
    # Ensure cell width is 8 or less
    if cell_width > 8:
        raise ValueError("Maximum horizontal cell width is 8 pixels")

    # Open and load the image
    img = Image.open(image_path)
    img = img.convert('1')  # Convert to 1-bit (black and white) image

    # Get image dimensions
    width, height = img.size

    # Calculate the number of vertical cells
    vertical_cells = height // cell_height
    total_cells = horizontal_cells * vertical_cells

    # Create output header file
    base_name = os.path.splitext(os.path.basename(image_path))[0]
    header_file = f"{base_name}_font.h"

    with open(header_file, 'w') as f:
        # Write header guards and includes
        f.write(f"#ifndef {base_name.upper()}_FONT_H\n")
        f.write(f"#define {base_name.upper()}_FONT_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write("#include <pico/platform.h>\n\n")

        # Write font metadata
        f.write(f"static const uint8_t {base_name.lower()}_font_height = {cell_height};\n")
        f.write(f"static const uint8_t {base_name.lower()}_font_first_char = {initial_char};\n")
        f.write(f"static const uint8_t {base_name.lower()}_font_last_char = {initial_char + total_cells - 1};\n\n")

        # Write font data array declaration
        f.write(f"// Font data for characters from ASCII {initial_char} to {initial_char + total_cells - 1}\n")
        f.write(f"static const uint8_t __in_flash() {base_name}_font_data[][{cell_height}] = {{\n")

        # Process each character cell
        char_index = 0
        for y in range(vertical_cells):
            for x in range(horizontal_cells):
                if char_index >= total_cells:
                    break

                # Calculate character value
                char_value = initial_char + char_index

                # Write character header
                f.write(f"    // Character: {chr(char_value) if 32 <= char_value <= 126 else '?'} (ASCII: {char_value})\n")
                f.write("    {\n")

                # Process each row in the cell
                for row in range(cell_height):
                    byte_value = 0
                    row_visual = ["." for _ in range(cell_width)]

                    # Process each column in the cell
                    for col in range(cell_width):
                        # Get pixel at position (inverted since black is background, white is font)
                        pixel_x = x * cell_width + col
                        pixel_y = y * cell_height + row

                        # Check if pixel is within image bounds
                        if pixel_x < width and pixel_y < height:
                            # Get pixel value (255 for white, 0 for black)
                            pixel = img.getpixel((pixel_x, pixel_y))

                            # White pixels (255) are set to 1 in the bitmap
                            if pixel == 255:
                                # Set corresponding bit in byte
                                byte_value |= (1 << (7 - col if cell_width == 8 else cell_width - 1 - col))
                                row_visual[col] = "X"

                    # Write byte value with visual comment
                    visual_comment = "".join(row_visual)
                    f.write(f"        (uint8_t) 0x{byte_value:08b}, // {visual_comment}\n")

                f.write("    },\n")
                char_index += 1

        # Close the array
        f.write("};\n\n")

        # Close header guard
        f.write(f"#endif // {base_name.upper()}_FONT_H\n")

    print(f"Font data has been written to {header_file}")
    return header_file

def main():
    parser = argparse.ArgumentParser(description='Convert a bitmap font to a C header file')
    parser.add_argument('image_file', help='Path to the bitmap image file (PNG format)')
    parser.add_argument('cell_width', type=int, help='Width of each character cell in pixels (max 8)')
    parser.add_argument('cell_height', type=int, help='Height of each character cell in pixels')
    parser.add_argument('horizontal_cells', type=int, help='Number of cells horizontally in the image')
    parser.add_argument('--initial_char', type=int, default=32,
                        help='ASCII code of the first character (default: 32, space)')

    args = parser.parse_args()

    try:
        bitmap_to_cheader(
            args.image_file,
            args.cell_width,
            args.cell_height,
            args.horizontal_cells,
            args.initial_char
        )
    except Exception as e:
        print(f"Error: {e}")
        return 1

    return 0

if __name__ == "__main__":
    exit(main())