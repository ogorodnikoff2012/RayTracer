//
// Created by xenon on 21.08.17.
//

#include <Magick++.h>

using namespace Magick;
int main(int argc, char *argv[]) {
    InitializeMagick(*argv);
    Image img("1000x500", "white");
    FILE *f = fopen("pixels.log", "r");
    while (!feof(f)) {
        double x, y;
        fscanf(f, "%lf%lf", &x, &y);
        img.pixelColor((int) x, (int) y, "red");
    }
    fclose(f);
    img.write("pixels.png");
    return 0;
}
