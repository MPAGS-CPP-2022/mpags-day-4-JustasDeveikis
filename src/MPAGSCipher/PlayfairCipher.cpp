#include "PlayfairCipher.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

PlayfairCipher::PlayfairCipher(const std::string& key)
{
    PlayfairCipher::setKey(key);
}

void PlayfairCipher::setKey(const std::string& key)
{
    // Store the original key
    key_ = key;

    // Append the alphabet
    key_ += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::transform(key_.begin(), key_.end(), key_.begin(), ::toupper);

    // Remove non-alpha characters
    auto rem_iter = std::remove_if(key_.begin(), key_.end(),
                                   [](char c) { return !std::isalpha(c); });
    key_.erase(rem_iter, key_.end());

    // Change  J -> I
    std::transform(key_.begin(), key_.end(), key_.begin(), [](char c) {
        if (c == 'J')
            return 'I';
        return c;
    });

    //Remove duplicated letters
    std::string letters_found{""};
    auto detect_duplicates = [&](char c) {
        if (letters_found.find(c) == std::string::npos) {
            letters_found += c;
            return false;
        } else {
            return true;
        }
    };
    auto rem_iter2 =
        std::remove_if(key_.begin(), key_.end(), detect_duplicates);
    key_.erase(rem_iter2, key_.end());

    // Store the playfair cihper key map
    map = PlayfairCipher::map;

    for (int i = 0; i < int(key_.length()); i++) {
        std::string k{key_[i]};
        map[k] = std::make_pair(i % 5, i / 5);
    }
}

std::string PlayfairCipher::applyCipher(const std::string& inputText,
                                        const CipherMode cipherMode) const
{
    std::string outputText{inputText};
    switch (cipherMode) {
        case CipherMode::Encrypt: {
            // Change J -> I
            std::transform(outputText.begin(), outputText.end(),
                           outputText.begin(), [](char c) {
                               if (c == 'J')
                                   return 'I';
                               return c;
                           });

            // If repeated chars in a digraph add an X or Q if XX
            for (int i = 1; i < int(outputText.length()) - 1; i++) {
                char c{outputText[i]};
                if (c == outputText[i - 1]) {
                    if (c == 'X') {
                        outputText[i] = 'Q';
                    } else {
                        outputText[i] = 'X';
                    }
                }
            }

            // If the size of input is odd, add a trailing Z
            if (int(outputText.length()) % 2 == 1)
                outputText += 'Z';

            // Loop over the input in Digraphs
            for (int i = 0; i < int(outputText.length()); i += 2) {
                std::string str1{outputText[i]}, str2{outputText[i + 1]};

                auto iter1 = map.find(str1);
                auto iter2 = map.find(str2);

                /* Coordinates: second.first  -> column 
                                second.second -> row 
                */

                // - First rule: if letters are on the same row, replace with letter to the right
                if ((*iter1).second.second ==
                        (*iter2)
                            .second
                            .second /* first letter row == second letter row*/
                    &&
                    abs((*iter1).second.first - (*iter2).second.first) ==
                        1 /* check if digraph letters are located in neighbouring columns */) {
                    // Obtain the coordinates of each letter from map for a digraph
                    int newCol1{(*iter1).second.first + 1};
                    int newCol2{(*iter2).second.first + 1};

                    if (newCol2 > 4)
                        newCol2 = newCol2 % 4;

                    for (auto m : map) {
                        if (m.second.first == newCol1)
                            outputText.replace(i, 1, m.first);
                        if (m.second.first == newCol2) 
                            outputText.replace(i + 1, 1, m.first);
                    }
                }

                // - Second rule: if letters are on the same column, replace with letters directly below
                else if (
                    (*iter1).second.first ==
                        (*iter2)
                            .second
                            .first /* first letter column == second letter column*/
                    &&
                    abs((*iter1).second.second - (*iter2).second.second) ==
                        1 /* check if digraph letters are located in neighbouring rows */) {
                    // Obtain the coordinates of each letter from map for a digraph
                    int newRow1{(*iter1).second.second + 1};
                    int newRow2{(*iter2).second.second + 1};

                    if (newRow2 > 4)
                        newRow2 = newRow2 % 4;

                    for (auto m : map) {
                        if (m.second.second == newRow1) 
                            outputText.replace(i, 1, m.first);
                        if (m.second.second == newRow2)
                            outputText.replace(i + 1, 1, m.first);
                    }
                }

                // - Third rule: if letters form a rectangle, replace with ones from corner on the same row
                else {
                    int col3{(*iter2).second.first},
                        row3{(*iter1).second.second};
                    int col4{(*iter1).second.first},
                        row4{(*iter2).second.second};

                    // Find the replacement letter
                    for (auto m : map) {
                        if (m.second.first == col3 && m.second.second == row3)
                            // Replace the letters with corners of the square
                            outputText.replace(i, 1, m.first);
                        if (m.second.first == col4 && m.second.second == row4)
                            outputText.replace(i + 1, 1, m.first);
                    }
                }
            }
            break;
        }

        case CipherMode::Decrypt: {
            // Loop over the input in Digraphs
            for (int i = 0; i < int(outputText.length()); i += 2) {
                std::string str1{outputText[i]}, str2{outputText[i + 1]};

                auto iter1 = map.find(str1);
                auto iter2 = map.find(str2);

                // - First rule: if letters are on the same row, replace with letter to the left
                if ((*iter1).second.second ==
                        (*iter2)
                            .second
                            .second /* first letter row == second letter row*/
                    &&
                    abs((*iter1).second.first - (*iter2).second.first) ==
                        1 /* check if digraph letters are located in neighbouring columns */) {
                    // Obtain the coordinates of each letter from map for a digraph
                    int newCol1{(*iter1).second.first - 1};
                    int newCol2{((*iter2).second.first - 1)};

                    if (newCol2 < 0)
                        newCol2 = 4;

                    for (auto m : map) {
                        if (m.second.first == newCol1)
                            outputText.replace(i, 1, m.first);
                        if (m.second.first == newCol2) 
                            outputText.replace(i + 1, 1, m.first);
                    }
                }

                // - Second rule: if letters are on the same column, replace with letters directly above
                else if (
                    (*iter1).second.first ==
                        (*iter2)
                            .second
                            .first /* first letter column == second letter column*/
                    &&
                    abs((*iter1).second.second - (*iter2).second.second) ==
                        1 /* check if digraph letters are located in neighbouring rows */) {
                    // Obtain the coordinates of each letter from map for a digraph
                    int newRow1{(*iter1).second.second - 1};
                    int newRow2{(*iter2).second.second - 1};

                    if (newRow2 < 0)
                        newRow2 = 4;

                    for (auto m : map) {
                        if (m.second.second == newRow1) 
                            outputText.replace(i, 1, m.first);
                        if (m.second.second == newRow2)
                            outputText.replace(i + 1, 1, m.first);
                    }
                }

                // - Third rule: if letters form a rectangle, replace with ones from corner on the same row (same as for encryption)
                else {
                    int col3{(*iter2).second.first},
                        row3{(*iter1).second.second};
                    int col4{(*iter1).second.first},
                        row4{(*iter2).second.second};

                    // Find the replacement letter
                    for (auto m : map) {
                        if (m.second.first == col3 && m.second.second == row3)
                            // Replace the letters with corners of the square
                            outputText.replace(i, 1, m.first);
                        if (m.second.first == col4 && m.second.second == row4)
                            outputText.replace(i + 1, 1, m.first);
                    }
                }
            }
            break;
        }
    }
    return outputText;
}