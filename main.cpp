#include <iostream>
#include "CException.h"
using namespace std;

int main(int argc, char *argv[]) {
    try {
        if (argc < 4 || argc > 6) {
            throw CExpension("Wrong amount of arguments");
        }
        for (int i = 1; i < argc; i++) {
            switch (i) {
                case 1:

            }
        }
    } catch (CExpension &expension) {
        cerr << expension.getError();
        if (expension.getFile()) {
            fclose(expension.getFile());
        }
        exit(1);
    }
    return 0;
}
