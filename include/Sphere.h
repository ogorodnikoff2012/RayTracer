//
// Created by xenon on 20.08.17.
//

#ifndef RAYTRACER_SPHERE_H
#define RAYTRACER_SPHERE_H

#include "Shape.h"
#include "Point.h"
#include "Optional.h"
#include <cmath>
#include "types.h"

namespace geom3d {
    class Sphere : public Shape {
        Point o_;
        fp r_;
        Magick::Image cMap_;
        Vector nPole_, zMeridian_;
    public:
        Sphere(const Point &o, fp r) : o_(o), r_(r), cMap_("1x1", "white"), nPole_(0, -1, 0), zMeridian_(0, 0, -1) {}
        const util::Optional<Point> firstIntersection(const Ray &r) const {
            Vector a = r.dir();
            Vector b(r.start(), o_);
            Vector c = a * ((a * b) / (a * a)) - b;
            fp d = r_ * r_ - c.length_sqr();
            if (d < 0) {
                return util::Optional<Point>();
            } else {
                Point center = o_.moveTo(c);
                Vector offset = !a * sqrt(d);
                Point p1 = center.moveTo(-offset), p2 = center.moveTo(offset);
                return Vector(r.start(), p1) * a >= 0 ? p1 :
                       Vector(r.start(), p2) * a >= 0 ? p2 : util::Optional<Point>();
            }
        }
        const Vector getNormal(const Point &p) const {
            return !Vector(o_, p);
        }
        const Point center() const {
            return o_;
        }
        fp radius() const {
            return r_;
        }
        const Magick::Color getColor(const Point &p) const {
            Vector v(o_, p);
            Vector projection = v - nPole_ * ((nPole_ * v) / nPole_.length_sqr());
            fp longitude = std::acos(!projection * !zMeridian_);
            if (nPole_* (zMeridian_ % projection) < 0) {
                longitude = -longitude;
            }
            fp latitude = std::acos(!v * !nPole_);


            int w = cMap_.size().width(), h = cMap_.size().height();
            fp x = (longitude * M_1_PI + 1) * w / 2;
            fp y = latitude * M_1_PI * h;
//            fprintf(stderr, "%7.2f %7.2f\n", x, y);
            return cMap_.pixelColor(x < 0 ? 0 : x >= w ? w - 1 : (int) x,
                                    y < 0 ? 0 : y >= h ? h - 1 : (int) y);

        }
        Sphere &setColorMap(const Magick::Image &img) {
            cMap_ = img;
            fp perimeter = 2 * r_ * M_PI;
            cMap_.scale(Magick::Geometry(perimeter, perimeter / 2));
            return *this;
        }
        Sphere &setNorthPole(const Vector &v) {
            nPole_ = v;
            return *this;
        }
        Sphere &setZeroMeridian(const Vector &v) {
            zMeridian_ = v;
            return *this;
        }
        ~Sphere() = default;
    };
}

#endif //RAYTRACER_SPHERE_H
