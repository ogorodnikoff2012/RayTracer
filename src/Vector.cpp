//
// Created by xenon on 20.08.17.
//

#include <Vector.h>
#include <Point.h>

geom3d::Vector::Vector(const geom3d::Point &a, const geom3d::Point &b) :
        Vector(b.x() - a.x(), b.y() - a.y(), b.z() - a.z()){

}