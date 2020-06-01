//
// Created by ssiie on 01.06.2020.
//

#ifndef UNTITLED5_CEXCEPTION_H
#define UNTITLED5_CEXCEPTION_H


#include <string>

using namespace std;

class CExpension {
public:
    CExpension(string error);

    CExpension(string error, FILE *file);

    string getError();

    FILE *getFile();

private:
    string error_;
    FILE *file_ = nullptr;
};


#endif //UNTITLED5_CEXCEPTION_H
