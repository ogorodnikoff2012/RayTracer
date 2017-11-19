//
// Created by xenon on 20.08.17.
//

#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include "Vector.h"
#include "Point.h"

namespace geom3d {
    class Ray {
        Point p_;
        Vector v_;
    public:
        Ray(const Point &p, const Vector &v) : p_(p), v_(v) {}
        Ray(const Point &a, const Point &b) : Ray(a, Vector(a, b)) {}
        const Point start() const {
            return p_;
        }
        const Vector dir() const {
            return v_;
        }
    };
}


#endif //RAYTRACER_RAY_H
