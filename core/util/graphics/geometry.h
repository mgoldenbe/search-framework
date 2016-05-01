#ifndef UTIL_GEOMETRY_H
#define UTIL_GEOMETRY_H

namespace gu {

struct Point {
    double x, y;
};

struct Circle {
    Point center;
    double r;
};

double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

double distance(const Point &p1, const Point &p2) {
    return distance(p1.x, p1.y, p2.x, p2.y);
}

// Angle that a line from `from` to `to` makes with the positive direction of
// the X-axis
double angle(const Point &from, const Point &to) {
    return atan2(to.y - from.y, to.x - from.x);
}

// Computes a point on the circle at a given angle with the positive direction
// of the X-axis. Puts the point inside the circle to compensate so a segment of // the width `width` is fully inside the circle.
// Assumes that width is not greater than diameter.
Point circlePoint(const Circle &c, double angle, double width = 0.0) {
    double f = sqrt(c.r * c.r - width * width / 4) / c.r;
    return {c.center.x + f * c.r * cos(angle),
            c.center.y + f * c.r * sin(angle)};
}
}
#endif
