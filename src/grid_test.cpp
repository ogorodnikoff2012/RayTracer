//
// Created by xenon on 22.08.17.
//

#include <Magick++.h>

using namespace Magick;

constexpr int WIDTH = 1000, HEIGHT = 500;
constexpr int LINE_WIDTH = 3;
constexpr int CELL_SIZE = 20;

int main(int argc, char *argv[]) {
    InitializeMagick(argv[0]);
    Image img(Geometry(WIDTH, HEIGHT), "white");
    img.modifyImage();
    Pixels view(img);
    PixelPacket *pixels = view.get(0, 0, WIDTH, HEIGHT);
    Color red("red");

    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            if (x % CELL_SIZE < LINE_WIDTH ||
                    y % CELL_SIZE < LINE_WIDTH) {
                pixels[x + y * WIDTH] = red;
            }
        }
    }

    view.sync();
    img.display();
    img.write("grid.png");
    return 0;
}
