# 🔐 LSB Image Steganography using C

## 📌 Project Overview

LSB (Least Significant Bit) Image Steganography is a technique used to hide secret information inside an image without significantly changing its visual appearance.

This project is implemented using C programming. It uses the Least Significant Bit (LSB) technique to encode secret text data into a BMP image and retrieve the hidden information through decoding.

The project demonstrates image processing, bitwise operations, file handling, and data encoding and decoding concepts in C.

## 🚀 Features

- **Encoding:** Hide secret text data inside a BMP image.
- **Decoding:** Extract hidden information from an encoded BMP image.
- **Magic String:** Use a magic string (`#*`) to identify the presence of hidden data.
- **File Extension Handling:** Store and retrieve the original secret file extension.
- **Image Processing:** Modify the least significant bits of image data.
- **Data Preservation:** Maintain the visual appearance of the original image.

## 🛠️ Technologies Used

- C Programming
- Bitwise Operations
- File Handling
- Structures
- Pointers
- Arrays and Strings
- BMP Image Processing

## 📂 Project Structure

```text
LSB_Steganography/
│
├── main.c
├── encode.c
├── encode.h
├── decode.c
├── decode.h
├── types.h
├── common.h
├── stego.bmp
├── secret.txt
└── README.md
```

*Note: The filenames
⚙️ Working Principle

1. Encoding Process

The encoding process hides secret information inside a BMP image.

Steps involved:

Open the source BMP image.

Open the secret text file.

Validate whether the image has sufficient capacity.

Encode the magic string (#*).

Encode the secret file extension.

Encode the secret file size.

Encode the secret file data into the image using LSB manipulation.

Generate the output stego image.

2. Decoding Process

The decoding process retrieves the hidden information from the stego image.

Steps involved:

Open the encoded BMP image.

Extract and verify the magic string.

Retrieve the secret file extension.

Extract the secret file size.

Decode the hidden data.

Reconstruct the original secret file.

💻 Compilation and Execution

Compile

gcc *.c -o steganography

Encoding

./steganography -e secret.txt source.bmp

Decoding

./steganography -d stego.bmp

Note: Adjust the commands according to your actual command-line argument implementation.

🧠 Concepts Implemented

Least Significant Bit (LSB) manipulation

Bitwise operators

File handling

BMP image structure

Data encoding and decoding

Structures and pointers

Modular programming

Command-line argument handling

🎯 Learning Outcomes

Understanding image steganography techniques.

Practical implementation of bitwise operations in C.

Understanding binary file handling.

Working with BMP image data.

Developing encoding and decoding algorithms.

Improving modular C programming skills.

👨‍💻 Author

Shravan