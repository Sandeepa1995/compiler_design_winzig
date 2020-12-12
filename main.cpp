#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include "Classes/Token.h"
#include "Classes/Node.h"
#include "scanner.h"
#include "parser.h"

/* Print the stack obtained from the parser
 * params:  node - Node being checked
 *          level - level of the node (in the tree)
 *          outStrings - Output Stings
 */
void printStack(Node node, int level, std::vector<std::string> *outStrings) {
    std::stringstream buffer;
    for (int i = 0; i < level; i++) {
        buffer << ". ";
    }

    if (node.getType() == 1) {
        buffer << "<identifier>" << "(" << node.numberOfChildren() + 1 << ")";
        outStrings->push_back(buffer.str());
        buffer.str("");
        for (int i = 0; i < level + 1; i++) {
            buffer << ". ";
        }
        buffer << node.getTerm() << "(" << node.numberOfChildren() << ")";
        outStrings->push_back(buffer.str());
        buffer.str("");

    } else if (node.getType() == 2) {
        buffer << "<integer>" << "(" << node.numberOfChildren() + 1 << ")";
        outStrings->push_back(buffer.str());
        buffer.str("");
        for (int i = 0; i < level + 1; i++) {
            buffer << ". ";
        }
        buffer << node.getTerm() << "(" << node.numberOfChildren() << ")";
        outStrings->push_back(buffer.str());
        buffer.str("");

    } else if (node.getType() == 3) {
        buffer << "<char>" << "(" << node.numberOfChildren() + 1 << ")";
        outStrings->push_back(buffer.str());
        buffer.str("");
        for (int i = 0; i < level + 1; i++) {
            buffer << ". ";
        }
        buffer << node.getTerm() << "(" << node.numberOfChildren() << ")";
        outStrings->push_back(buffer.str());
        buffer.str("");

    } else if (node.getType() == 4) {
        buffer << "<string>" << "(" << node.numberOfChildren() + 1 << ")";
        outStrings->push_back(buffer.str());
        buffer.str("");
        for (int i = 0; i < level + 1; i++) {
            buffer << ". ";
        }
        buffer << node.getTerm() << "(" << node.numberOfChildren() << ")";
        outStrings->push_back(buffer.str());
        buffer.str("");

    } else {
        buffer << node.getTerm() << "(" << node.numberOfChildren() << ")";
        outStrings->push_back(buffer.str());
        buffer.str("");
    }

    for (int i = node.numberOfChildren() - 1; i >= 0; i--) {
        printStack(node.getChildAt(i), level + 1, outStrings);
    }
}

int main(int argc, char **argv) {
    // Read the arguments
    bool gettingPath = false;
    bool addSpace = false;
    bool pathFound = false;
    std::string aSTPath;
    for (int argi = 0; argi < argc; argi++) {
        if (std::strncmp(argv[argi], "-ast", 4) == 0 || std::strncmp(argv[argi], "–ast", 4) == 0) {
            gettingPath = true;
        } else if (gettingPath) {
            if (addSpace) {
                aSTPath += " ";
            }
            aSTPath += argv[argi];
            addSpace = true;
            pathFound = true;
        }
    }

    if (pathFound){
        std::vector<std::string> errors;
        // Scanner
        std::vector<Token> tokenStream = scanner(aSTPath.c_str(), &errors);

        // Print errors found in the scanner
        if(errors.size() > 0){
            std::cout << errors.size() << " error/s found when scanning the code" << std::endl;
            for (int i = 0; i < errors.size(); i++){
                std::cout << i + 1 << ". " << errors.at(i) << std::endl;
            }
        } else if (tokenStream.empty()) {
            std::cout << "No tokens found in the input file. Check if the input file path is correct." << std::endl;
        }
        else {
            try{
                // Parser
                Node root = parser(&tokenStream, &errors);
                std::vector<std::string> outStrings;
                printStack(root, 0, &outStrings);
                for (int i = 0; i < outStrings.size(); i++) {
                    std::cout << outStrings.at(i) << std::endl;
                }
            } catch (...){
                if(errors.size() > 0){
                    // Print errors found in the parser
                    std::cout << errors.size() << " error/s found when parsing the code" << std::endl;
                    for (int i = 0; i < errors.size(); i++){
                        std::cout << i + 1 << ". " << errors.at(i) << std::endl;
                    }
                } else {
                    std::cout << "Unexpected error." << std::endl;
                }

            }
        }
    } else {
        std::cout << "Invalid arguments. An example is shown below." << std::endl;
        std::cout << "./winzigc -ast winzig_test_programs/winzig_01" << std::endl;
    }


    return 0;
}

