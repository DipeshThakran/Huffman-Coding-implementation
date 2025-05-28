#include "../include/Huffman.h"
#include <iostream>

using namespace std;

void printUsage() {
    cout << "Huffman Coding Compression Tool\n";
    cout << "Usage:\n";
    cout << "  Compress:   ./huffman -c [input.txt] [output.bin]\n";
    cout << "  Decompress: ./huffman -d [input.bin] [output.txt]\n";
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printUsage();
        return 1;
    }
    
    string mode = argv[1];
    string inputFile = argv[2];
    string outputFile = argv[3];
    
    HuffmanCompressor compressor;
    
    if (mode == "-c") {
        cout << "Compressing " << inputFile << " to " << outputFile << "...\n";
        compressor.compress(inputFile, outputFile);
        cout << "Compression completed.\n";
    } 
    else if (mode == "-d") {
        cout << "Decompressing " << inputFile << " to " << outputFile << "...\n";
        compressor.decompress(inputFile, outputFile);
        cout << "Decompression completed.\n";
    } 
    else {
        cerr << "Error: Invalid mode specified\n";
        printUsage();
        return 1;
    }
    
    return 0;
}