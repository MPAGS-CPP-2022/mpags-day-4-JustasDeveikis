#ifndef MPAGSCIPHER_PLAYFAIRCIPHER_HPP
#define MPAGSCIPHER_PLAYFAIRCIPHER_HPP

#include "CipherMode.hpp"

#include<string>
#include<vector>
#include <map>


class PlayfairCipher {
  public:
    explicit PlayfairCipher(const std::string& key);

    void setKey(const std::string& key);
    
    std::string applyCipher(const std::string& inputText, const CipherMode cipherMode) const;
  
  private:
    /// The alphabet - used to determine the cipher character given the plain character and the key
    const std::vector<char> alphabet_{
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

    /// The size of the alphabet
    const std::size_t alphabetSize_{alphabet_.size()};

    // The cipher key is a string
    std::string key_{""};

    using Str2IntMap = std::map<std::string, std::pair<int, int>>;
    Str2IntMap map;

};

#endif