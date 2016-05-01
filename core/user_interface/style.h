#ifndef STYLE_H
#define STYLE_H

enum class VertexShape {CIRCLE, SQUARE};
enum class VertexFillPattern {SSLB_OLID};

enum class EdgeShape {CIRCLE, SQUARE};
enum class EdgeDash {NONE};

struct VertexStyle {
    VertexShape shape = VertexShape();
    VertexFillPattern pattern = VertexFillPattern();
    static double sizeBase;
    double sizeFactor = 1.0;
    Color fillColor = Color::WARM_GREY;
    Color borderColor  = Color();
    double borderWidthFactor = 0;
    Color emphasisColor = Color();
    double emphasisWidthFactor = 0;
};
double VertexStyle::sizeBase = 10;

struct EdgeStyle {
    bool operator==(const EdgeStyle &rhs) {
        if (shape != rhs.shape) return false;
        if (dash != rhs.dash) return false;
        if (color != rhs.color) return false;
        if (widthFactor != rhs.widthFactor) return false;
        return true;
    };
    EdgeShape shape = EdgeShape();
    EdgeDash dash = EdgeDash();
    Color color = Color::BROWN_GREY;
    static double widthBase;
    double widthFactor = 1.0;
    bool arrow = false;
    int depth = 0;
    static constexpr int maxDepth = 2;
};
double EdgeStyle::widthBase = 5.0;

#endif
