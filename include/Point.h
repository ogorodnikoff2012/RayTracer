//
// Created by xenon on 20.08.17.
//

#ifndef RAYTRACER_POINT_H
#define RAYTRACER_POINT_H

#include "Vector.h"
#include "types.h"

namespace geom3d {
    class Point {
        fp x_, y_, z_;
    public:
        Point(fp x, fp y, fp z) : x_(x), y_(y), z_(z) {}
        Point() : Point(0, 0, 0) {}
        const Point moveTo(const Vector &v) const {
            return Point(x_ + v.x(), y_ + v.y(), z_ + v.z());
        }
        fp dist(const Point &p) const {
            return sqrt(dist_sqr(p));
        }
        fp dist_sqr(const Point &p) const {
            return Vector(*this, p).length_sqr();
        }
        fp x() const {
            return x_;
        }
        fp y() const {
            return y_;
        }
        fp z() const {
            return z_;
        }
    };
}


#endif //RAYTRACER_POINT_H
