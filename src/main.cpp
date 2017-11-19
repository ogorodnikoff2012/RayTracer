#include <iostream>
#include <random>
#include <Magick++.h>
#include <Sphere.h>
#include <thread>

// using namespace Magick;
using Magick::Color;
using Magick::Quantum;
using Magick::InitializeMagick;
using Magick::Image;
using Magick::Geometry;
using Magick::Pixels;
using Magick::PixelPacket;
using namespace geom3d;

// std::mt19937 rnd(218);
std::minstd_rand rnd(218);
Image bg;

constexpr int WIDTH = 1920, HEIGHT = 1080;
#ifndef DEBUG
constexpr int X_THREADS = 16, Y_THREADS = 18;
#else
constexpr int X_THREADS = 1, Y_THREADS = 1;
#endif
constexpr int BLOCK_WIDTH = WIDTH / X_THREADS, BLOCK_HEIGHT = HEIGHT / Y_THREADS;
constexpr int SOFT_SHADOW_RAYS_CNT = 1000;
constexpr double SOFT_SHADOW_BLUR = 0.02;

static double min(double a, double b) {
    return a < b ? a : b;
}

static double max(double a, double b) {
    return a < b ? b : a;
}

static double very_fast(double x) {
    return 1 - exp(-x * x);
}

Color colorHSV(double H, double S, double V) {
    int Hi = (int) floor(H / 60);
    Hi = (Hi % 6 + 6) % 6;
    double Vmin = (100 - S) * V / 100;
    double a = (V - Vmin) * fmod(H, 60) / 60;
    double Vinc = Vmin + a;
    double Vdec = V - a;

    double R = 0, G = 0, B = 0;
    switch (Hi) {
        case 0:
            R = V;
            G = Vinc;
            B = Vmin;
            break;
        case 1:
            R = Vdec;
            G = V;
            B = Vmin;
            break;
        case 2:
            R = Vmin;
            G = V;
            B = Vinc;
            break;
        case 3:
            R = Vmin;
            G = Vdec;
            B = V;
            break;
        case 4:
            R = Vinc;
            G = Vmin;
            B = V;
            break;
        case 5:
            R = V;
            G = Vmin;
            B = Vdec;
            break;
    }
    return Color((Quantum) (R / 100 * QuantumRange), (Quantum) (G / 100 * QuantumRange), (Quantum) (B / 100 * QuantumRange));
}

Color background(int x, int y) {
    double nx = x - WIDTH / 2;
    double ny = HEIGHT / 2 - y;
    double theta = atan2(ny, nx);
    if (theta < 0) {
        theta += 2 * M_PI;
    }
    double dist = sqrt(nx * nx + ny * ny);
    double H = theta * 180 * M_1_PI;
    double S = min(100 * very_fast(dist / 100), 100);
    double V = 100;
    return colorHSV(H, S, V);
}

double lightIntensity(const Ray &light, const Point &p, const std::vector<Shape *> &shapes, int idx) {
    // An object is found. Searching for shadows
    bool inShadow = false;
    for (int i = 0; i < shapes.size() && !inShadow; ++i) {
        if (i == idx) continue;
        auto shadowIntersect = shapes[i]->firstIntersection(light);
        inShadow = shadowIntersect && light.dir() * Vector(p, *shadowIntersect) < 0;
    }
    if (inShadow) {
        return 0;
    } else {
        double angleCos = -!light.dir() * shapes[idx]->getNormal(p);
        return max(0, min(1, angleCos));
    }
}

const Vector blur(const Vector &v, double radius) {
    std::vector<Vector> ortho;
    ortho.push_back(v % Vector(1, 0, 0));
    ortho.push_back(v % Vector(0, 1, 0));
    Vector a = !(ortho[0].length_sqr() > 0 ? ortho[0] : ortho[1]);
    Vector b = !(v % a);
    std::uniform_real_distribution<> dis(0, 1);
    double phi = dis(rnd) * 2 * M_PI;
    double r = dis(rnd) * radius;
    return v + a * cos(phi) * r + b * sin(phi) * r;
}

void render(PixelPacket *img, const Point *camera, const std::vector<Shape *> *shapes, const Point *light,
        int x0, int x1, int y0, int y1, double *percent) {
    int S = (x1 - x0) * (y1 - y0);
    int covered = 0;
    for (int x = x0; x < x1; ++x) {
        for (int y = y0; y < y1; ++y) {
            Ray r(*camera, Point(x, y, 0));
            Point intersection;
            int idx = -1;
            for (int i = 0; i < shapes->size(); ++i) {
                auto intersect = (*shapes)[i]->firstIntersection(r);
                if (intersect) {
                    if (idx < 0 || intersect->dist_sqr(*camera) < intersection.dist_sqr(*camera)) {
                        idx = i;
                        intersection = *intersect;
                    }
                }
            }

            if (idx >= 0) {
                double intensity = 0;
                for (int i = 0; i < SOFT_SHADOW_RAYS_CNT; ++i) {
                    Vector lightDir(*light, intersection);
                    lightDir.normalize();
                    Ray r(*light, blur(lightDir, SOFT_SHADOW_BLUR));
                    intensity += lightIntensity(r, intersection, *shapes, idx);
                }
                intensity /= SOFT_SHADOW_RAYS_CNT;
                Color c = (*shapes)[idx]->getColor(intersection);
                img[y * WIDTH + x] = Color((Quantum) (c.redQuantum() * intensity),
                                           (Quantum) (c.greenQuantum() * intensity),
                                           (Quantum) (c.blueQuantum() * intensity));
            } else {
                img[y * WIDTH + x] = bg.pixelColor(x, y); // background(x, y);
            }

            ++covered;
            *percent = 100. * covered / S;
        }
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void statePrinter(double percent[X_THREADS][Y_THREADS]) {
    while (true) {
        for (int y = 0; y < Y_THREADS; ++y) {
            for (int x = 0; x < X_THREADS; ++x) {
                printf("%6.2f%% ", percent[x][y]);
            }
            printf("\n");
        }
        printf("\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
#pragma clang diagnostic pop

typename std::enable_if<(WIDTH % X_THREADS == 0 && HEIGHT % Y_THREADS == 0), int>::type
    main(int argc, char *argv[]) {
    InitializeMagick(*argv);
    Image my_img(Geometry(WIDTH, HEIGHT), "white");
    my_img.modifyImage();

    Pixels view(my_img);
    PixelPacket *pixels = view.get(0, 0, WIDTH, HEIGHT);

    int S = WIDTH * HEIGHT;

    Point camera(WIDTH / 2, HEIGHT / 2, -WIDTH);
    Point light(WIDTH, HEIGHT, -sqrt((double) S));
    double blurRadius = 0.002;

    std::vector<Shape *> shapes;
    Sphere *earth = new Sphere(Point(WIDTH / 2, HEIGHT / 2, 0), 300);
    Sphere *moon = new Sphere(light.moveTo(Vector(light, earth->center()) * 0.6), 50);
    shapes.push_back(static_cast<Shape *>(earth));
    shapes.push_back(static_cast<Shape *>(moon));

    Image earthCMap("earthmap1k.jpg");
    Image moonCMap("moonmap4k.jpg");
    bg.read("starsky.jpg");

    Vector zMeridian = Vector(0, -1, 0) % Vector(light, earth->center());
    Vector nPole = Vector(light, earth->center()) % zMeridian;
    earth->setColorMap(earthCMap).setNorthPole(nPole).setZeroMeridian(zMeridian);
    moon->setColorMap(moonCMap).setNorthPole(nPole).setZeroMeridian(zMeridian);

    double percent[X_THREADS][Y_THREADS];
    std::thread renderers[X_THREADS][Y_THREADS];

    for(int x = 0; x < X_THREADS; ++x) {
        for (int y = 0; y < Y_THREADS; ++y) {
            percent[x][y] = 0;
            renderers[x][y] = std::thread(render, pixels, &camera, &shapes, &light,
                                          x * BLOCK_WIDTH, (x + 1) * BLOCK_WIDTH,
                                          y * BLOCK_HEIGHT, (y + 1) * BLOCK_HEIGHT, &percent[x][y]);
        }
    }

    std::thread t(statePrinter, percent);
    t.detach();

    for (int x = 0; x < X_THREADS; ++x) {
        for (int y = 0; y < Y_THREADS; ++y) {
            renderers[x][y].join();
        }
    }

    printf("\nSaving\n");
    for (int i = 0; i < shapes.size(); ++i) {
        delete shapes[i];
    }

    view.sync();

    my_img.write("rainbow.png");
    printf("Done\n");
    return 0;
}