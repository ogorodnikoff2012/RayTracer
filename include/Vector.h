//
// Created by xenon on 20.08.17.
//

#ifndef RAYTRACER_VECTOR_H
#define RAYTRACER_VECTOR_H

#include <cmath>
#include "types.h"

namespace geom3d {
    class Point;
    class Vector {
        fp x_, y_, z_;
    public:
        Vector(fp x, fp y, fp z) : x_(x), y_(y), z_(z) {}
        Vector() : Vector(0, 0, 0) {}
        Vector(const Point &a, const Point &b);
        const Vector operator +(const Vector &v) const {
            return Vector(x_ + v.x_, y_ + v.y_, z_ + v.z_);
        }
        const Vector operator -() const {
            return Vector(-x_, -y_, -z_);
        }
        const Vector operator -(const Vector &v) const {
            return Vector(x_ - v.x_, y_ - v.y_, z_ - v.z_);
        }
        const Vector operator *(fp d) const {
            return Vector(x_ * d, y_ * d, z_ * d);
        }
        const Vector operator /(fp d) const {
//            return operator*(1/d);
            return Vector(x_ / d, y_ / d, z_ / d);
        }
        const Vector operator !() const {
            return operator*(1 / length());
        }
        fp length_sqr() const {
            return x_ * x_ + y_ * y_ + z_ * z_;
        }
        fp length() const {
            return sqrt(length_sqr());
        }
        fp operator *(const Vector &v) const {
            return x_ * v.x_ + y_ * v.y_ + z_ * v.z_;
        }
        const Vector operator %(const Vector &v) const {
            return Vector(y_ * v.z_ - z_ * v.y_, z_ * v.x_ - x_ * v.z_, x_ * v.y_ - y_ * v.x_);
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
        Vector &normalize() {
            fp len = length();
            x_ /= len;
            y_ /= len;
            z_ /= len;
            return *this;
        }
    };
}

#endif //RAYTRACER_VECTOR_H
