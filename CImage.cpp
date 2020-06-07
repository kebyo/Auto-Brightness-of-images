#include "CImage.h"

CImage::CImage(SInput config) {
    FILE *f = fopen(config.inputName, "rb");
    file = f;
    if (fscanf(f, "P%i%i%i%i\n", &this->version, &this->width, &this->height, &max_val) != 4) {
        throw CExpension("Wrong amount data in file", f);
    }
    size = width * height;
    unsigned char *buffer;
    if (version == 6) {
        buffer = new unsigned char[size * 3];
        pix = new RGB[size];
        fread(buffer, sizeof(unsigned char), size * 3, f);
        for (int i = 0, j = 0; i < size * 3; i += 3, j++) {
            pix[j].red = (double) buffer[i];
            pix[j].green = (double) buffer[i + 1];
            pix[j].blue = (double) buffer[i + 2];
        }
    } else if (version == 5) {
        buffer = new unsigned char[size];
        pix = new RGB[size];
        fread(buffer, sizeof(unsigned char), size, f);
        for (int i = 0; i < size; i++) {
            pix[i].red = (double) buffer[i];
            pix[i].green = (double) buffer[i];
            pix[i].blue = (double) buffer[i];
        }
    } else {
        throw CExpension("Wrong version of file", f);
    }
    colorSpace = (config.mode == 0 || config.mode == 2 || config.mode == 4);//False is YCbCr601
    delete[] buffer;
    fclose(f);
}

void CImage::writeToFile(char *fileName) {
    FILE *f = fopen(fileName, "wb");
    if (!f) {
        throw CExpension("Input file didn't open");
    }
    fprintf(f, "P%i\n%i %i\n%i\n", version, width, height, max_val);
    unsigned char *buffer;
    if (version == 5) {
        buffer = new unsigned char[size];
        for (int i = 0; i < size; i++) {
            buffer[i] = (unsigned char) pix[i].red;
        }
        fwrite(buffer, sizeof(unsigned char), size, f);
    } else {
        buffer = new unsigned char[size * 3];
        for (int i = 0, j = 0; i < size * 3; i += 3, j++) {
            buffer[i] = (unsigned char) pix[j].red;
            buffer[i + 1] = (unsigned char) pix[j].green;
            buffer[i + 2] = (unsigned char) pix[j].blue;
        }
        fwrite(buffer, sizeof(unsigned char), size * 3, f);
    }
    delete[] buffer;
    fclose(f);
}

void CImage::settingUpOfBrightntess(SInput config) {
    if (config.mode % 2 != 0) {
        RGBtoYCbCr601();
    }
    if (config.mode == 0 || config.mode == 1) {
        userMode(config.offset, config.multiplier);
    } else {
        autoMode(config);
    }
    if (config.mode % 2 != 0) {
        YCbCr601toRGB();
    }
}

void CImage::userMode(double offset, double multilier) {
    if (version == 5 && colorSpace) {
        for (int i = 0; i < size; i++) {
            int newValue = cut((pix[i].red - offset) * multilier);
            pix[i].red = newValue;
            pix[i].green = newValue;
            pix[i].blue = newValue;
        }
    } else {
        if (colorSpace) {
            for (int i = 0; i < size; i++) {
                int newRed = cut((pix[i].red - offset) * multilier);
                pix[i].red = newRed;
                int newGreen = cut((pix[i].green - offset) * multilier);
                pix[i].green = newGreen;
                int newBlue = cut((pix[i].blue - offset) * multilier);
                pix[i].blue = newBlue;
            }
        } else {
            for (int i = 0; i < size; i++) {
                int newValue = cut((pix[i].red - offset) * multilier);
                pix[i].red = newValue;
            }
        }
    }
}

void CImage::autoMode(SInput config) {
    SMinMax tmp;
    tmp = getDarkestAndBrightest(config.mode == 4 || config.mode == 5);
    std::cout << tmp.min << " " << 255.0 / (tmp.max - tmp.min) << "\n";
    if (version == 5 && colorSpace) {
        for (int i = 0; i < size; i++) {
            int newValue = cut((pix[i].red - tmp.min) * 255.0 / (tmp.max - tmp.min));
            pix[i].red = newValue;
            pix[i].green = newValue;
            pix[i].blue = newValue;
        }
    } else {
        if (colorSpace) {
            for (int i = 0; i < size; i++) {
                int newRed = cut((pix[i].red - tmp.min) * 255.0 / (tmp.max - tmp.min));
                pix[i].red = newRed;
                int newGreen = cut((pix[i].green - tmp.min) * 255.0 / (tmp.max - tmp.min));
                pix[i].green = newGreen;
                int newBlue = cut((pix[i].blue - tmp.min) * 255.0 / (tmp.max - tmp.min));
                pix[i].blue = newBlue;
            }
        } else {
            for (int i = 0; i < size; i++) {
                int newValue = cut((pix[i].red - tmp.min) * 255.0 / (tmp.max - tmp.min));
                pix[i].red = newValue;
            }
        }
    }
}

SMinMax CImage::getDarkestAndBrightest(bool is0039) {
    vector<int> p(256, 0);
    SMinMax tmp;
    tmp.min = 0;
    tmp.max = 255;
    for (int i = 0; i < size; i++) {
        p[pix[i].red]++;
        if (version == 6 && colorSpace) {
            p[pix[i].green]++;
            p[pix[i].blue]++;
        }
    }
    if (is0039) {
        int j = size * 0.0039;
        j = version == 6 && colorSpace ? j * 3 : j;
        int i = 0;
        int bright = 255, dark = 0;
        while (i < j) {
            bright = getFirstB(bright, p);
            p[bright]--;
            dark = getFirstD(dark, p);
            p[dark]--;
            i++;
        }
    }
    bool flagB = false, flagD = false;
    for (int i = 0; i < p.size(); i++) {
        if (p[i] > 0 && !flagD) {
            tmp.min = i;
            flagD = true;
        }
        if (p[255 - i] > 0 && !flagB) {
            tmp.max = 255 - i;
            flagB = true;
        }
    }
    return tmp;
}

int CImage::getFirstD(int i, vector<int> &p) {
    while (p[i] == 0) {
        i++;
    }
    return i;
}

int CImage::getFirstB(int i, vector<int> &p) {
    while (p[i] == 0) {
        i--;
    }
    return i;
}

double CImage::cut(double x) {
    x = x > 255.0 ? 255.0 : x;
    x = x < 0.0 ? 0.0 : x;
    return x;
}

void CImage::YCbCr601toRGB() {
    double kr = 0.299, kb = 0.114;
    for (int i = 0; i < size; i++) {
        double y = pix[i].red / 255.0;
        double pb = pix[i].green / 255.0 - 0.5;
        double pr = pix[i].blue / 255.0 - 0.5;
        double r, g, b;
        b = 2 * pb * (1 - kb) + y;
        r = 2 * pr * (1 - kr) + y;
        g = (y - kr * r - kb * b) / (1 - kr - kb);
        r = r < 0 ? 0 : r;
        r = r > 1 ? 1 : r;
        b = b < 0 ? 0 : b;
        b = b > 1 ? 1 : b;
        g = g < 0 ? 0 : g;
        g = g > 1 ? 1 : g;
        pix[i].red = r * 255.0;
        pix[i].green = g * 255.0;
        pix[i].blue = b * 255.0;
    }
}

void CImage::RGBtoYCbCr601() {
    double kr = 0.299, kb = 0.114;
    for (int i = 0; i < size; i++) {
        double r = pix[i].red / 255.0;
        double g = pix[i].green / 255.0;
        double b = pix[i].blue / 255.0;
        double y = kr * r + (1 - kr - kb) * g + kb * b;
        double pb = 0.5 * (b - y) / (1.0 - kb);
        double pr = 0.5 * (r - y) / (1.0 - kr);
        pix[i].red = y * 255.0;
        pix[i].green = (pb + 0.5) * 255.0;
        pix[i].blue = (pr + 0.5) * 255.0;
    }
}