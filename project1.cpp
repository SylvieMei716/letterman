// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68768FC9

// Date: Jan 17, 2024
// Programmer: Xinyu Mei
// This program will take in a dictionary, a begin word, an end word, and 
// the way to morph words.
// And its goal is to convert the beginning word to another 
// word, to another word, etc., eventually leading to the ending word, 
// making one change at a time.

#include <cstddef>
#include <vector>
#include <deque>
#include <iostream>
#include <algorithm>
#include <getopt.h>
#include <string>


struct Word {
    std::string word = "";
    int prevWordIdx = -1;
    bool isDiscovered = false;
}; 

class LetterManClass {
    private:
        std::string beginWord;
        int beginWordIdx = -1;
        
        std::string endWord;
        int endWordIdx = -1;

        std::string outputFormat;
        bool queueIsSpecified = false;
        bool stackIsSpecified = false;
        bool outputIsSpecified = false;
        bool changeIsSpecified = false;
        bool swapIsSpecified = false;
        bool lengthIsSpecified = false;
        bool beginIsSpecified = false;
        bool endIsSpecified = false;

    public:
        std::vector<Word> dictionary;
        bool beginWordFound = false;
        bool endWordFound = false;
        int numDiscovered = 0;
        
        std::deque<int> path;

        void getOptions(int argc, char** argv);
        bool wordShouldInDict(std::string &newWord);
        void checkBeginEnd(std::string &wordToPush, int &numWordsInDict);
        void readDict();
        bool areSimilarWords(Word &investingWord, Word &currentWord);
        bool search();
        std::string printModification(Word &wordToModify, Word &wordAfterModify);
        void output();

};

int main(int argc, char** argv) {
    LetterManClass letterman;

    letterman.getOptions(argc, argv);

    letterman.readDict();

    if (!letterman.beginWordFound) {
        std::cerr << "Beginning word does not exist in the dictionary\n";
        exit (1);
    }

    if (!letterman.endWordFound) {
        std::cerr << "Ending word does not exist in the dictionary\n";
        exit (1);
    }

    letterman.output();

    return 0;
}

void LetterManClass::getOptions(int argc, char** argv) {
    int option_index = 0, option = 0;
    
    // Don't display getopt error messages about options
    opterr = false;
    

    struct option longOpts[] = {{ "queue", no_argument, nullptr, 'q' },
                                { "stack", no_argument, nullptr, 's'},
                                { "begin", required_argument, nullptr, 'b'},
                                { "end", required_argument, nullptr, 'e'},
                                { "output", required_argument, nullptr, 'o'},
                                { "change", no_argument, nullptr, 'c'},
                                { "length", no_argument, nullptr, 'l'},
                                { "swap", no_argument, nullptr, 'p'},
                                { "help", no_argument, nullptr, 'h' },
                                { nullptr, 0, nullptr, '\0' }};
    
    while ((option = getopt_long(argc, argv, "qsb:e:o:clph", 
                                 longOpts, &option_index)) != -1) {
        switch (option) {
            case 'q':
                queueIsSpecified = true;
                break;

            case 's':
                stackIsSpecified = true;
                break;       

            case 'b':
                beginIsSpecified = true;
                beginWord = optarg;
                break;
                
            case 'e':
                endIsSpecified = true;
                endWord = optarg;
                break;

            case 'o':
                outputIsSpecified = true;
                outputFormat = optarg;
                break;
            
            case 'c':
                changeIsSpecified = true;
                break;
                
            case 'l':
                lengthIsSpecified = true;
                break;

            case 'p':
                swapIsSpecified = true;
                break;
            
            case 'h':
                std::cout << "This program reads a txt file that contains a dictionary,\n"
                          << "It then outputs the path from begin word to end word\n"
                          << "specified in the command line argumentsfollowing the\n"
                          << "rules specified in command line arguments.\n"
                          << "Usage: \'./project1\n\t[--queue | -q]\n"
                          <<                      "\t[--stack | -s]\n"
                          <<                      "\t[--begin | -b] <begin word>\n"
                          <<                      "\t[--end | -e] <end word>\n"
                          <<                      "\t[--output | -o] <W | M>\n"
                          <<                      "\t[--change | -c]\n"
                          <<                      "\t[--length | -l]\n"
                          <<                      "\t[--swap | -p]\n"
                          <<                      "\t[--help | -h]\n"
                          <<                      "\t< <TXT dictionary file>\'" << std::endl;    
                exit(0);
        }
    }
    
    if (!outputIsSpecified) {
        outputFormat = "W";
    }

    if (stackIsSpecified && queueIsSpecified) {
        std::cerr << "Conflicting or duplicate stack and queue specified\n";
        exit(1);
    }

    if (!(stackIsSpecified || queueIsSpecified)) {
        std::cerr << "Must specify one of stack or queue\n";
        exit(1);
    }

    if (!(changeIsSpecified || lengthIsSpecified || swapIsSpecified)) {
        std::cerr << "Must specify at least one" 
                  << "modification mode (change length swap)\n";
        exit(1);
    }

    if (!beginIsSpecified || beginWord.empty()) {
        std::cerr << "Beginning word not specified\n";
        exit(1);
    }

    if (!endIsSpecified || endWord.empty()) {
        std::cerr << "Ending word not specified\n";
        exit(1);
    }

    if (!lengthIsSpecified && (beginWord.size() != endWord.size())) {
        std::cerr << "The first and last words must have " 
                  << "the same length when length mode is off\n";
        exit(1);
    }

    if (outputFormat != "M" && outputFormat != "W") {
        std::cerr << "Invalid output mode specified, outputFormat: " << outputFormat << "\n";
        exit(1);
    }

} // getOptions()

bool LetterManClass::wordShouldInDict(std::string &newWord) {
    std::string tempWord1 = beginWord;
    std::string tempWord2 = endWord;
    std::string tempWord3 = newWord;
    std::sort(tempWord1.begin(), tempWord1.end());
    std::sort(tempWord2.begin(), tempWord2.end());
    std::sort(tempWord3.begin(), tempWord3.end());

    if (newWord == beginWord || newWord == endWord) {
        return true;
    }
    else {
        if (!lengthIsSpecified) {
            if (newWord.size() != beginWord.size()) {
                return false;
            }
            else {
                if (!changeIsSpecified) {
                    if (tempWord1 != tempWord2) {
                        return false;
                    }
                    else {
                        return tempWord3 == tempWord1;
                    }
                }
            }
        }
    }
    
    return true;
} // wordShouldInDict(string newWord)

void LetterManClass::checkBeginEnd(std::string &wordToPush, int &numWordsInDict) {
    if (wordToPush == beginWord) {
        beginWordFound = true;
        beginWordIdx = numWordsInDict;
    }
    if (wordToPush == endWord) {
        endWordFound = true;
        endWordIdx = numWordsInDict;
    }
} // checkBeginEnd(string, int)

void LetterManClass::readDict() {
    char dictType;
    size_t numLines;
    std::cin >> dictType >> numLines;

    int numWordsInDict = 0;

    if (dictType == 'S') {
        dictionary.reserve(numLines);

        std::cin.ignore();

        Word newLine;

        for (size_t i = 0; i < numLines; i++) {
            std::cin >> newLine.word;

            if (newLine.word.substr(0,2) == "//") {
                getline(std::cin, newLine.word);
                i--;
            }

            else {
                checkBeginEnd(newLine.word, numWordsInDict);
                if (wordShouldInDict(newLine.word)) {
                    
                    dictionary.emplace_back(newLine);
                    numWordsInDict++;
                } // if newline word should be in dict
            } // new input line actually contains a word
        } // for (i)
    } // if dictType == 'S'

    else {
        // dictionary.reserve(numLines * 2);

        std::cin.ignore();

        Word newLine;

        std::string temp;

        const char signReversal = '&';
        const char signInsert = '[';
        const char signInsertEnd = ']';
        const char signSwap = '!';
        const char signDouble = '?';
        const std::string specialSign = "&[]!?";

        for (size_t i = 0; i < numLines; i++) {
            std::cin >> temp;

            if (temp.substr(0,2) == "//") {
                getline(std::cin, temp);
                i--;
            }

            else {
                size_t specialSignIdx = temp.find_first_of(specialSign);
                size_t insertSignEndIdx = temp.find_first_of(signInsertEnd);
                
                if (specialSignIdx == std::string::npos) {
                    checkBeginEnd(temp, numWordsInDict);
                    if (wordShouldInDict(temp)) {
                        
                        newLine.word = temp;
                        dictionary.emplace_back(newLine);
                        numWordsInDict++;
                    } // if newline word should be in dict
                } // if no special character is found in this line

                else {
                    if (temp[specialSignIdx] == signReversal) {
                        //push the original word
                        temp = temp.substr(0, specialSignIdx);
                        checkBeginEnd(temp, numWordsInDict);
                        if (wordShouldInDict(temp)) {
                            
                            newLine.word = temp;
                            dictionary.emplace_back(newLine);
                            numWordsInDict++;
                        } // if newline word should be in dict

                        //push the reversed word
                        std::reverse(temp.begin(), temp.end());
                        checkBeginEnd(temp, numWordsInDict);
                        if (wordShouldInDict(temp)) {
                            
                            newLine.word = temp;
                            dictionary.emplace_back(newLine);
                            numWordsInDict++;
                        } // if reversed newline word should be in dict
                    } // if reversal
                    

                    if (temp[specialSignIdx] == signInsert) {
                        std::string temp1 = temp;
                        
                        for (size_t j = specialSignIdx + 1; 
                                j < insertSignEndIdx; j++) {
                            
                            temp = 
                                temp1.substr(0, specialSignIdx) + 
                                temp1[j] + 
                                temp1.substr(insertSignEndIdx + 1, 
                                temp1.size() - insertSignEndIdx - 1);
                            checkBeginEnd(temp, numWordsInDict);
                            if (wordShouldInDict(temp)) {
                                
                                newLine.word = temp;
                                dictionary.emplace_back(newLine);
                                numWordsInDict++;
                            } // if inserted newline word should be in dict
                        } // for (j)
                    } // if insert
                    

                    if (temp[specialSignIdx] == signSwap) {
                        temp = temp.substr(0, specialSignIdx) + 
                                temp.substr(specialSignIdx + 1, 
                                temp.size() - specialSignIdx - 1);
                        checkBeginEnd(temp, numWordsInDict);
                        if (wordShouldInDict(temp)) {
                            
                            newLine.word = temp;
                            dictionary.emplace_back(newLine);
                            numWordsInDict++;
                        } // if reversed newline word should be in dict
                        std::string temp1 = temp;
                        temp[specialSignIdx - 1] = temp1[specialSignIdx - 2];
                        temp[specialSignIdx - 2] = temp1[specialSignIdx - 1];
                        checkBeginEnd(temp, numWordsInDict);
                        if (wordShouldInDict(temp)) {
                            
                            newLine.word = temp;
                            dictionary.emplace_back(newLine);
                            numWordsInDict++;
                        } // if swapped newline word should be in dict
                    } // if swap
                    

                    if (temp[specialSignIdx] == signDouble) {
                        std::string temp1 = temp.substr(0, specialSignIdx) + 
                                temp.substr(specialSignIdx + 1, 
                                temp.size() - specialSignIdx - 1);
                        checkBeginEnd(temp1, numWordsInDict);
                        if (wordShouldInDict(temp1)) {
                            
                            newLine.word = temp1;
                            dictionary.emplace_back(newLine);
                            numWordsInDict++;
                        } // if non-doubled newline word should be in dict
                        std::string temp2 = temp.substr(0, specialSignIdx) + 
                                       temp[specialSignIdx - 1] + 
                                       temp.substr(specialSignIdx + 1, 
                                       temp.size() - specialSignIdx - 1);
                        checkBeginEnd(temp2, numWordsInDict);
                        if (wordShouldInDict(temp2)) {
                            
                            newLine.word = temp2;
                            dictionary.emplace_back(newLine);
                            numWordsInDict++;
                        } // if reversed newline word should be in dict
                    } // if double
                } // special character processing
            } // new input line actually contains one or two words
        } // for(i)
    } // dictType == 'C'
} // readDict()

bool LetterManClass::areSimilarWords(Word &investingWord, 
                                     Word &currentWord) {
    
    size_t invstWordSize = investingWord.word.size();
    size_t crntWordSize = currentWord.word.size();

    if ((invstWordSize > crntWordSize && invstWordSize - crntWordSize > 1) || 
        (crntWordSize > invstWordSize && crntWordSize - invstWordSize > 1)) {
        return false;
    } // if diff of size > 1, not similar
    
    else if (invstWordSize == crntWordSize) {
        if (!(changeIsSpecified || swapIsSpecified)) {
            return false;
        }

        else if (changeIsSpecified && swapIsSpecified) {
            int numDiffLetter = 0;
            bool isSwapped = false;

            for (size_t i = 0; i < invstWordSize; i++) {
                if (investingWord.word[i] != currentWord.word[i]) {
                    numDiffLetter++;
                    if (i < invstWordSize - 1 && 
                        investingWord.word[i] == currentWord.word[i + 1] &&
                        investingWord.word[i + 1] == currentWord.word[i]) {
                        isSwapped = true;
                    }
                }
            } // for (i)
            return (numDiffLetter == 1 && !isSwapped) || 
                   (numDiffLetter == 2 && isSwapped);
        } // we can both change and swap
        else if (changeIsSpecified && !swapIsSpecified) {
            int numDiffLetter = 0;
            for (size_t i = 0; i < invstWordSize; i++) {
                if (investingWord.word[i] != currentWord.word[i]) {
                    numDiffLetter++;
                }
            } // for (i)
            return numDiffLetter == 1;
        } // only change is allowed
        else {
            int numDiffLetter = 0;
            bool isSwapped = false;

            for (size_t i = 0; i < invstWordSize; i++) {
                if (investingWord.word[i] != currentWord.word[i]) {
                    numDiffLetter++;
                    if (i < invstWordSize - 1 && 
                        investingWord.word[i] == currentWord.word[i + 1] &&
                        investingWord.word[i + 1] == currentWord.word[i]) {
                        isSwapped = true;
                    }
                }
            } // for (i)
            return (numDiffLetter == 2 && isSwapped);
        } // only swap is allowed
    } // if same size

    else {
        if (!lengthIsSpecified) {
            return false;
        }
        else {
            if (invstWordSize > crntWordSize) {
                size_t i = 0;
                size_t j = 0;

                while (i < invstWordSize && j < crntWordSize) {
                    if (investingWord.word[i] == currentWord.word[j]) {
                        i++;
                        j++;
                    } // if same letter at some index, move forward
                    else {
                        i++;
                    } // else: diff letters at some index, longer word move forward
                } // while
                return j == crntWordSize;
            } // current word is longer, may insert a letter to prev one
            else {
                size_t i = 0;
                size_t j = 0;

                while (i < crntWordSize && j < invstWordSize) {
                    if (currentWord.word[i] == investingWord.word[j]) {
                        i++;
                        j++;
                    } // if same letter at some index, move forward
                    else {
                        i++;
                    } // else: diff letters at some index, longer word move forward
                } // while
                return j == invstWordSize;
            } // current word is shorter, may delete a letter from prev one
        }
    } // else: size differ by 1, can only insert or delete
    return true;
} // areSimilarWords()

bool LetterManClass::search() {
    std::deque<int> searchContainer;
    int currentWordIdx;
    
    // step 0: set initial currentWord as beginWord, send it to searchContainer
    dictionary[beginWordIdx].isDiscovered = true;
    numDiscovered++;
    searchContainer.push_back(beginWordIdx);

    while (!dictionary[endWordIdx].isDiscovered) {
        // step 1: remove next word from searchContainer, set to currentWord
        if (searchContainer.size() == 0) {
            return false;
        }

        if (stackIsSpecified) {
            currentWordIdx = searchContainer.back();
            searchContainer.pop_back();
        } // if stack
        else {
            currentWordIdx = searchContainer.front();
            searchContainer.pop_front();
        } // else: queue

        // step 2: taking a loop of the dictionary, change features and add to sc if similar
        for (size_t i = 0; i < dictionary.size(); i++) {
            if (!dictionary[i].isDiscovered && areSimilarWords(dictionary[i], dictionary[currentWordIdx])) {
                dictionary[i].isDiscovered = true;
                numDiscovered++;
                dictionary[i].prevWordIdx = currentWordIdx;

                searchContainer.push_back(static_cast<int>(i));

                if (dictionary[endWordIdx].isDiscovered) {
                    break;
                }
            } // if investigated word is not discovered and similar word to prev
        } // iterating the dictionary

        // step 3: check whether endWord is discovered using while loop
    } // while searchContainer not empty
    
    // no solution
    if (!dictionary[endWordIdx].isDiscovered) {
        return false;
    } // if endWord never be discovered

    // prepare for outputs
    else {
        Word temp = dictionary[endWordIdx];
        path.push_back(endWordIdx);
        while (temp.prevWordIdx != -1) {
            path.push_back(temp.prevWordIdx);
            temp = dictionary[temp.prevWordIdx];
        } // while not backtradking to beginWord
    } // else: found a path to endWord

    return true;
} // search()

std::string LetterManClass::printModification(Word &wordToModify, Word &wordAfterModify) {
    size_t sizeBeforeMdf = wordToModify.word.size();
    size_t sizeAfterMdf = wordAfterModify.word.size();

    std::string outputStr;

    if (sizeBeforeMdf == sizeAfterMdf) {
        for (size_t i = 0; i < sizeBeforeMdf; i++) {
            if (wordToModify.word[i] != wordAfterModify.word[i]) {
                if (i == sizeAfterMdf - 1) {
                    outputStr = "c," + std::to_string(i) + 
                                "," + wordAfterModify.word[i] + "\n";
                    return outputStr;
                } // if modification happened at the last letter
                else {
                    if (wordToModify.word[i + 1] != wordAfterModify.word[i + 1]) {
                        outputStr = "s," + std::to_string(i) + "\n";
                        return outputStr;
                    } // if two ajacent letters differ, swapped happened
                    else {
                        outputStr = "c," + std::to_string(i) + 
                                    "," + wordAfterModify.word[i] + "\n";
                        return outputStr;
                    } // else: change happened
                } // else: modification not happened at the last letter
            } // if modification happened at index i
        } // for (i)
    } // if size not changed, change or swap was done

    else {
        if (sizeAfterMdf > sizeBeforeMdf) {
            // insert was done
            for (size_t i = 0; i < sizeBeforeMdf; i++) {
                if (wordToModify.word[i] != wordAfterModify.word[i]) {
                    outputStr = "i," + std::to_string(i) + 
                                "," + wordAfterModify.word[i] + "\n";
                    return outputStr;
                } // if insert happened at index i
            } // for (i)
            outputStr = "i," + std::to_string(sizeAfterMdf - 1) + 
                        "," + wordAfterModify.word[sizeAfterMdf - 1] + "\n";
            return outputStr;
        } // if insert was done
        else {
            for (size_t i = 0; i < sizeAfterMdf; ++i) {
                if (wordAfterModify.word[i] != wordToModify.word[i]) {
                    outputStr = "d," + std::to_string(i) + "\n";
                    return outputStr;
                } // if delete happened at index i
            } // for (i)
            outputStr = "d," + std::to_string(sizeAfterMdf) + "\n";
            return outputStr;
        } // else: delete was done
    } // else: size changed, insert or delete was done
    return std::string();
} // printModification()

void LetterManClass::output() {
    if (search()) {
        size_t sizePath = path.size();
        if (outputFormat == "W") {
            std::cout << "Words in morph: " << sizePath << "\n";
            while (sizePath > 0) {
                std::cout << dictionary[path.back()].word << "\n";
                path.pop_back();
                sizePath--;
            }
        } // output by words
        
        if (outputFormat == "M") {
            std::cout << "Words in morph: " << sizePath << "\n";
            std::cout << beginWord << "\n"; // print beginWord
            while (sizePath > 1) {
                Word wordToModify = dictionary[path.back()];
                path.pop_back();
                sizePath--;
                Word wordAfterModify = dictionary[path.back()];
                std::cout << printModification(wordToModify, wordAfterModify);
            }
        } // output by morph
    } // if found solution

    else {
        std::cout << "No solution, " 
                  << std::to_string(numDiscovered) 
                  << " words discovered.\n";
    } // else: no solution
} // output()