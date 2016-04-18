#ifndef DRAWING_ROUTINES_H
#define DRAWING_ROUTINES_H

#include "GeometryUtilities.h"

void moveTo(cairo_t *cr, const gu::Point &p) {
    cairo_move_to(cr, p.x, p.y);
}

void lineTo(cairo_t *cr, const gu::Point &to, bool strokeFlag = true) {
    cairo_line_to(cr, to.x, to.y);
    if (strokeFlag) cairo_stroke(cr);
}

void line(cairo_t *cr, const gu::Point &from, const gu::Point &to,
          bool strokeFlag = true) {
    moveTo(cr, from);
    lineTo(cr, to, strokeFlag);
}

void connectCircles(cairo_t *cr, const gu::Circle &from, const gu::Circle &to) {
    double myAngle = gu::angle(from.center, to.center);
    line(cr, gu::circlePoint(from, myAngle, cairo_get_line_width(cr)),
         gu::circlePoint(to, myAngle - M_PI, cairo_get_line_width(cr)));
}

// draw equilateral n-polygon inscribed into the circle with one of the vertices
// at the specified angle of the circle
void inscribePolygon(cairo_t *cr, const gu::Circle &c, int n, double angle,
                     bool fillFlag) {
    double step = 2 * M_PI / n;
    moveTo(cr, gu::circlePoint(c, angle ));
    for (int i = 0; i != n - 1; ++i)
        lineTo(cr, gu::circlePoint(c, angle + (i + 1) * step), false);
    cairo_close_path (cr);
    (void)fillFlag;
    if (fillFlag) cairo_fill(cr);
    cairo_stroke(cr);
}

#endif
