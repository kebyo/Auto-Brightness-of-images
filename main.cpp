#include <iostream>
#include "CException.h"
#include "CImage.h"

using namespace std;
SInput input;

int main(int argc, char *argv[]) {
    try {
        if (argc < 4 || argc > 6) {
            throw CExpension("Wrong amount of arguments");
        }
        for (int i = 1; i < argc; i++) {
            switch (i) {
                case 1:
                    input.inputName = argv[i];
                    break;
                case 2:
                    input.outputName = argv[i];
                    break;
                case 3:
                    input.mode = atoi(argv[i]);
                    if (input.mode < 0 || input.mode > 5) {
                        throw CExpension("Wrong mode");
                    }
                    break;
                case 4:
                    input.offset = atoi(argv[i]);
                    if (input.offset < -255 || input.offset > 255) {
                        throw CExpension("Wrong offset");
                    }
                    break;
                case 5:
                    input.multiplier = atof(argv[i]);
                    if (input.multiplier < 1.0 / 255.0 || input.multiplier > 255.0) {
                        throw CExpension("Wrong multiplier");
                    }
            }
        }
        if ((input.mode == 0 || input.mode == 1) && argc != 6) {
            throw CExpension("Wrong amount of arguments");
        }
        CImage image(input);
        image.settingUpOfBrightntess(input);
        image.writeToFile(input.outputName);
    } catch (CExpension &expension) {
        cerr << expension.getError();
        if (expension.getFile()) {
            fclose(expension.getFile());
        }
        exit(1);
    }
    return 0;
}

int CImage::getMax() {
    for (int i = 0; i <)
}
