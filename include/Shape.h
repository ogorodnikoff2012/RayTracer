//
// Created by xenon on 20.08.17.
//

#ifndef RAYTRACER_SHAPE_H
#define RAYTRACER_SHAPE_H

#include "Ray.h"
#include <Optional.h>

namespace geom3d {
    class Shape {
    public:
        const virtual util::Optional<Point> firstIntersection(const Ray &r) const = 0;
        const virtual Vector getNormal(const Point &p) const = 0;
        const virtual Magick::Color getColor(const Point &p) const = 0;
        virtual ~Shape() {};
    };
}


#endif //RAYTRACER_SHAPE_H
