//
// Created by xenon on 20.08.17.
//

#ifndef RAYTRACER_LINE_H
#define RAYTRACER_LINE_H

#include "Vector.h"
#include "Point.h"
#include "types.h"

namespace geom3d {
    class Line {
        Point a_, b_;
    public:
        Line(const Point &a, const Point &b) : a_(a), b_(b) {}
        fp dist(const Point &p) const {
            Vector v(a, p), w(a, b);
            return (v * w) / w.length();
        }
    };
}


#endif //RAYTRACER_LINE_H
