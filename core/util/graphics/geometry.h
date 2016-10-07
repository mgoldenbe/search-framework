#ifndef UTIL_GEOMETRY_H
#define UTIL_GEOMETRY_H

/// \namespace slb::core::util::gu
/// Geometry utilities.
namespace gu {

/// The point.
struct Point {
    double x; ///< The x-coordinate.
    double y; ///< The y-coordinate.
};

/// The circle.
struct Circle {
    Point center; ///< The center.
    double r; ///< The radius.
};

/// Computes the Euclidean distance between two points.
/// \param x1 The x-coordinate of the first point.
/// \param y1 The y-coordinate of the first point.
/// \param x2 The x-coordinate of the second point.
/// \param y2 The y-coordinate of the second point.
/// \return The Euclidean distance between the two points.
double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

/// Computes the Euclidean distance between two points.
/// \param p1 The first point.
/// \param p2 The second point.
/// \return The Euclidean distance between the two points.
double distance(const Point &p1, const Point &p2) {
    return distance(p1.x, p1.y, p2.x, p2.y);
}

/// Computes the point a certain proportion of distance from the first point to
/// the second one. So, if 0.5 is passed for the third parameter, the middle
/// between the two points is returned.
/// \param p1 The first point.
/// \param p2 The second point.
/// \param w The proportion.
/// \return The point a certain proportion of distance from the first point to
/// the second one.
Point pointOnLine(const Point &p1, const Point &p2, double w) {
    return {p1.x + (p2.x - p1.x) * w, p1.y + (p2.y - p1.y) * w};
}

/// Computes the angle in radians that a line from `from` to `to` makes with the
/// positive direction of the X-axis.
/// \param from The source point.
/// \param to The target point.
/// \return The angle in radians that a line from `from` to `to` makes with the
/// positive direction of the X-axis.
double angle(const Point &from, const Point &to) {
    return atan2(to.y - from.y, to.x - from.x);
}

/// Computes the point on the circle at a given angle with the positive direction
/// of the X-axis. Shifts the point inside the circle to a given distance.
/// \param c The circle.
/// \param angle The required angle with the positive direction
/// of the X-axis.
/// \param width The required shift.
\return The point on the circle at a given angle with the positive direction
/// of the X-axis and shifted inside the circle to a given distance.
/// \note \c width cannot be greater than diameter.
Point circlePoint(const Circle &c, double angle, double width = 0.0) {
    double f = sqrt(c.r * c.r - width * width / 4) / c.r;
    return {c.center.x + f * c.r * cos(angle),
            c.center.y + f * c.r * sin(angle)};
}
}
#endif
