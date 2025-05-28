#include "../include/Huffman.h"
#include <fstream>
#include <sstream>
#include <bitset>
#include <algorithm>
#include <iostream>
#include <memory>
#include <queue>
#include <unordered_map>

using namespace std;

unordered_map<char, unsigned> HuffmanCompressor::buildFrequencyTable(const string& data) {
    unordered_map<char, unsigned> freqTable;
    for (char ch : data) {
        freqTable[ch]++;
    }
    return freqTable;
}

shared_ptr<HuffmanNode> HuffmanCompressor::buildHuffmanTree(const unordered_map<char, unsigned>& freqTable) {
    auto compare = [](const shared_ptr<HuffmanNode>& a, const shared_ptr<HuffmanNode>& b) {
        return a->freq > b->freq;
    };
    priority_queue<shared_ptr<HuffmanNode>, vector<shared_ptr<HuffmanNode>>, decltype(compare)> minHeap(compare);
    
    for (const auto& pair : freqTable) {
        minHeap.push(make_shared<HuffmanNode>(pair.first, pair.second));
    }
    
    while (minHeap.size() > 1) {
        auto left = minHeap.top(); minHeap.pop();
        auto right = minHeap.top(); minHeap.pop();
        
        auto newNode = make_shared<HuffmanNode>('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        minHeap.push(newNode);
    }
    
    return minHeap.empty() ? nullptr : minHeap.top();
}

void HuffmanCompressor::generateCodes(const shared_ptr<HuffmanNode>& root, const string& str, 
                                    unordered_map<char, string>& huffmanCode) {
    if (!root) return;
    
    if (root->isLeaf()) {
        huffmanCode[root->data] = str.empty() ? "0" : str;
    }
    
    generateCodes(root->left, str + "0", huffmanCode);
    generateCodes(root->right, str + "1", huffmanCode);
}

string HuffmanCompressor::serializeTree(const shared_ptr<HuffmanNode>& root) {
    if (!root) return "";
    
    if (root->isLeaf()) {
        return "1" + string(1, root->data);
    }
    
    return "0" + serializeTree(root->left) + serializeTree(root->right);
}

shared_ptr<HuffmanNode> HuffmanCompressor::deserializeTree(string& data) {
    if (data.empty()) return nullptr;
    
    char type = data[0];
    data.erase(0, 1);
    
    if (type == '1') {
        if (data.empty()) return nullptr;
        char ch = data[0];
        data.erase(0, 1);
        return make_shared<HuffmanNode>(ch, 0);
    }
    
    auto left = deserializeTree(data);
    auto right = deserializeTree(data);
    if (!left || !right) return nullptr;
    
    auto node = make_shared<HuffmanNode>('\0', 0);
    node->left = left;
    node->right = right;
    return node;
}

void HuffmanCompressor::writeCompressedData(const string& outputFile, 
                                          const string& treeStr, 
                                          const string& encodedData) {
    ofstream out(outputFile, ios::binary);
    if (!out) {
        cerr << "Error: Could not open output file " << outputFile << endl;
        return;
    }
    
    // Write tree size and data
    unsigned treeSize = treeStr.size();
    out.write(reinterpret_cast<const char*>(&treeSize), sizeof(treeSize));
    out.write(treeStr.c_str(), treeSize);
    
    // Calculate and write padding
    unsigned padding = (8 - (encodedData.size() % 8)) % 8;
    out.write(reinterpret_cast<const char*>(&padding), sizeof(padding));
    
    // Write encoded data
    string compressed;
    for (size_t i = 0; i < encodedData.size(); i += 8) {
        string byteStr = encodedData.substr(i, min((size_t)8, encodedData.size() - i));
        if (byteStr.size() < 8) {
            byteStr.append(padding, '0');
        }
        bitset<8> bits(byteStr);
        compressed += static_cast<char>(bits.to_ulong());
    }
    
    unsigned dataSize = compressed.size();
    out.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
    out.write(compressed.c_str(), dataSize);
    
    out.close();
}

void HuffmanCompressor::readCompressedData(const string& inputFile, 
                                         string& treeData, 
                                         string& encodedData) {
    ifstream in(inputFile, ios::binary);
    if (!in) {
        cerr << "Error: Could not open input file " << inputFile << endl;
        return;
    }
    
    // Read tree size and data
    unsigned treeSize;
    in.read(reinterpret_cast<char*>(&treeSize), sizeof(treeSize));
    if (treeSize > 0) {
        treeData.resize(treeSize);
        in.read(&treeData[0], treeSize);
    }
    
    // Read padding
    unsigned padding;
    in.read(reinterpret_cast<char*>(&padding), sizeof(padding));
    
    // Read encoded data
    unsigned dataSize;
    in.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
    string compressed(dataSize, '\0');
    in.read(&compressed[0], dataSize);
    
    // Convert to binary string
    for (char byte : compressed) {
        encodedData += bitset<8>(byte).to_string();
    }
    
    // Remove padding bits
    if (padding > 0 && !encodedData.empty()) {
        encodedData.erase(encodedData.size() - padding);
    }
    
    in.close();
}

void HuffmanCompressor::compress(const string& inputFile, const string& outputFile) {
    // Read input file
    ifstream in(inputFile);
    if (!in) {
        cerr << "Error: Could not open input file " << inputFile << endl;
        return;
    }
    
    stringstream buffer;
    buffer << in.rdbuf();
    string text = buffer.str();
    in.close();
    
    if (text.empty()) {
        cerr << "Error: Input file is empty\n";
        return;
    }
    
    // Build Huffman tree and codes
    auto freqTable = buildFrequencyTable(text);
    auto root = buildHuffmanTree(freqTable);
    if (!root) {
        cerr << "Error: Failed to build Huffman tree\n";
        return;
    }
    
    unordered_map<char, string> huffmanCode;
    generateCodes(root, "", huffmanCode);
    
    // Encode text
    string encoded;
    for (char ch : text) {
        encoded += huffmanCode[ch];
    }
    
    // Serialize tree
    string treeStr = serializeTree(root);
    
    // Write compressed file
    writeCompressedData(outputFile, treeStr, encoded);
}

void HuffmanCompressor::decompress(const string& inputFile, const string& outputFile) {
    string treeData, encodedData;
    readCompressedData(inputFile, treeData, encodedData);
    
    if (treeData.empty() || encodedData.empty()) {
        cerr << "Error: Invalid compressed file format\n";
        return;
    }
    
    auto root = deserializeTree(treeData);
    if (!root) {
        cerr << "Error: Failed to reconstruct Huffman tree\n";
        return;
    }
    
    // Decode text
    string decoded;
    auto current = root;
    for (char bit : encodedData) {
        if (bit == '0') {
            current = current->left;
        } else {
            current = current->right;
        }
        
        if (!current) {
            cerr << "Error: Invalid Huffman code encountered\n";
            return;
        }
        
        if (current->isLeaf()) {
            decoded += current->data;
            current = root;
        }
    }
    
    // Write decompressed file
    ofstream out(outputFile);
    if (!out) {
        cerr << "Error: Could not create output file " << outputFile << endl;
        return;
    }
    
    out << decoded;
    out.close();
}