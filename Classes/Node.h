//
// Created by damitha on 12/29/19.
//

#ifndef WINZIG_NODE_H
#define WINZIG_NODE_H

#include <string>
#include <vector>

class Node {
private:
    // Type of the Node
    int type;
    // Term of the Node
    std::string term;
    // Node's children
    std::vector<Node> children;
public:
    Node(int type1, std::string term1);

    int getType();
    void setType(int type1);

    std::string getTerm();
    void setTerm(std::string term1);

    std::vector<Node> getChildren();
    void addChiild(Node childNode);
    int numberOfChildren();
    Node getChildAt(int i);
};


#endif //WINZIG_NODE_H
