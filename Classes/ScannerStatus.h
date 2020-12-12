//
// Created by damitha on 12/29/19.
//

#ifndef WINZIG_SCANNERSTATUS_H
#define WINZIG_SCANNERSTATUS_H

// To track the state of the scanner
class ScannerStatus {
private:
    // Type of the character
    int type;
    // Line the scanner is on
    int line;
    // Position in the line
    int position;
public:
    ScannerStatus(int type1, int line1, int position1);

    int getType();
    void setType(int type1);

    int getLine();
    void setLine(int line1);
    void incrementLine(int inc);

    int getPosition();
    void setPosition(int position1);
    void incrementPositon(int inc);
    void resetPosition();

};


#endif //WINZIG_SCANNERSTATUS_H
