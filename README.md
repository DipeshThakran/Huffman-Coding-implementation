# 🗜️ Huffman Coding Compression

**A C++ implementation of Huffman coding for efficient file compression.**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Features
- File compression/decompression
- Handles text files of any size
- 48% compression ratio on large files
- Built with modern C++17

## Quick Start
```bash
# Compile
make

# Compress
./huffman -c input.txt output.huff

# Decompress
./huffman -d output.huff original.txt
