#ifndef UTIL_DRAWING_H
#define UTIL_DRAWING_H

///@file
///@brief Drawing utilities.
/// \author Meir Goldenberg

/// Functions for dealing with RGB color components.
namespace rgb {

namespace {
/// Computes the color component farthest from the given one.
/// \param The given color component.
/// \return The color component farthest from \c x.
double f(double x) {
    return (x < 0.5 ? 1 : 0);
    return 1.0 - x;
}
}

/// Computes the red component of the color.
/// \param color The color.
/// \return The red component of the color.
double red(Color color) {
    return (double)(RGBValues[static_cast<int>(color)] >> 16) / 255;
}

/// Computes the green component of the color.
/// \param color The color.
/// \return The green component of the color.
double green(Color color) {
    return (double)((RGBValues[static_cast<int>(color)] >> 8) % 256) / 255;
}

/// Computes the blue component of the color.
/// \param color The color.
/// \return The blue component of the color.
double blue(Color color) {
    return (double)(RGBValues[static_cast<int>(color)] % 256) / 255;
}

/// Computes the complement of the red component of the color.
/// \param color The color.
/// \return The complement of the red component of the color.
double cred(Color color) {
    return (color == Color::NOVAL) ? 0 : f(red(color));
}

/// Computes the complement of the green component of the color.
/// \param color The color.
/// \return The complement of the green component of the color.
double cgreen(Color color) {
    return (color == Color::NOVAL) ? 0 : f(green(color));
}

/// Computes the complement of the blue component of the color.
/// \param color The color.
/// \return The complement of the blue component of the color.
double cblue(Color color) {
    return (color == Color::NOVAL) ? 0 : f(blue(color));
}

} // namespace

/// Moves to the given point.
/// \param cr The drawing context.
/// \param p The point to move to.
void moveTo(cairo_t *cr, const gu::Point &p) { cairo_move_to(cr, p.x, p.y); }

/// Draws line to the given point.
/// \param cr The drawing context.
/// \param to The point to draw to.
/// \param strokeFlag If \c true, \c cairo_stroke is performed at the end.
void lineTo(cairo_t *cr, const gu::Point &to, bool strokeFlag = true) {
    cairo_line_to(cr, to.x, to.y);
    if (strokeFlag) cairo_stroke(cr);
}

/// Connects two points with a line.
/// \param cr The drawing context.
/// \param from The source point.
/// \param to The target point.
/// \param strokeFlag If \c true, \c cairo_stroke is performed at the end.
void line(cairo_t *cr, const gu::Point &from, const gu::Point &to,
          bool strokeFlag = true) {
    moveTo(cr, from);
    lineTo(cr, to, strokeFlag);
}

/// Connects two circles with a line. The line is a segment of the line that connects the centers with the part within the circles removed.
/// \param cr The drawing context.
/// \param from The source circle.
/// \param to The target circle.
void connectCircles(cairo_t *cr, const gu::Circle &from, const gu::Circle &to) {
    double myAngle = gu::angle(from.center, to.center);
    line(cr, gu::circlePoint(from, myAngle, cairo_get_line_width(cr)),
         gu::circlePoint(to, myAngle - M_PI, cairo_get_line_width(cr)));
}

/// Draws equilateral n-polygon inscribed into the circle with one of the vertices
/// at the specified angle of the circle.
/// \param cr The drawing context.
/// \param c The circle.
/// \param n The number of vertices of the polygon.
/// \param angle The angle in radians.
/// \param fillFlag If \c true, the polygon is filled.
void inscribePolygon(cairo_t *cr, const gu::Circle &c, int n, double angle,
                     bool fillFlag) {
    double step = 2 * M_PI / n;
    moveTo(cr, gu::circlePoint(c, angle));
    for (int i = 0; i != n - 1; ++i)
        lineTo(cr, gu::circlePoint(c, angle + (i + 1) * step), false);
    cairo_close_path(cr);
    (void)fillFlag;
    if (fillFlag) cairo_fill(cr);
    cairo_stroke(cr);
}

/// Draws text of given font size centered around the given point.
/// \param cr The drawing context.
/// \param s The text to be drawn.
/// \param size The font size.
/// \param p The center of the text.
void drawText(cairo_t *cr, const std::string s, int size, const gu::Point &p) {
    cairo_text_extents_t extents;
    double x, y;
    const char *cs = s.c_str();
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);

    cairo_set_font_size(cr, size);
    cairo_text_extents(cr, cs, &extents);
    x = p.x - (extents.width / 2 + extents.x_bearing);
    y = p.y - (extents.height / 2 + extents.y_bearing);

    cairo_move_to(cr, x, y);
    cairo_show_text(cr, cs);
    cairo_stroke(cr);
}

#endif
