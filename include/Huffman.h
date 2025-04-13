#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <unordered_map>
#include <memory>
#include <queue>
#include <vector>

struct HuffmanNode {
    char data;
    unsigned freq;
    std::shared_ptr<HuffmanNode> left, right;
    
    HuffmanNode(char data, unsigned freq) 
        : data(data), freq(freq), left(nullptr), right(nullptr) {}
    
    bool isLeaf() const { return !left && !right; }
};

class HuffmanCompressor {
public:
    void compress(const std::string& inputFile, const std::string& outputFile);
    void decompress(const std::string& inputFile, const std::string& outputFile);

private:
    std::unordered_map<char, unsigned> buildFrequencyTable(const std::string& data);
    std::shared_ptr<HuffmanNode> buildHuffmanTree(const std::unordered_map<char, unsigned>& freqTable);
    void generateCodes(const std::shared_ptr<HuffmanNode>& root, const std::string& str, 
                      std::unordered_map<char, std::string>& huffmanCode);
    std::string serializeTree(const std::shared_ptr<HuffmanNode>& root);
    std::shared_ptr<HuffmanNode> deserializeTree(std::string& data);
    
    void writeCompressedData(const std::string& outputFile, 
                           const std::string& treeStr, 
                           const std::string& encodedData);
    void readCompressedData(const std::string& inputFile, 
                          std::string& treeData, 
                          std::string& encodedData);
};

#endif // HUFFMAN_H