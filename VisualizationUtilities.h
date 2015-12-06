#ifndef VISUALIZATION_UTILITIES
#define VISUALIZATION_UTILITIES

#include "Colors.h"

struct RGB {
    static double red(Color color) {
        return (double)(RGBValues[static_cast<int>(color)] >> 16)/255;
    }
    static double green(Color color) {
        return (double)((RGBValues[static_cast<int>(color)] >> 8) % 256)/255;
    }
    static double blue(Color color) {
        return (double)(RGBValues[static_cast<int>(color)] % 256)/255;
    }
};

enum class VertexShape {CIRCLE, SQUARE};
enum class VertexFillPattern {SOLID};

enum class EdgeShape {CIRCLE, SQUARE};
enum class EdgeDash {NONE};

struct VertexStyle {
    VertexShape shape;
    VertexFillPattern pattern;
    int size;
    Color fillColor;
    Color borderColor;
    int borderWidth;
    Color emphasisColor;
    int emphasisWidth;
};

VertexStyle defaultVertexStyle() {
    VertexStyle res;
    res.shape = VertexShape();
    res.pattern = VertexFillPattern();
    res.size = 10;
    res.fillColor = Color::WARM_GREY;
    res.borderColor = Color();
    res.borderWidth = 0;
    res.emphasisColor = Color();
    res.emphasisWidth = 0;
    return res;
}

struct EdgeStyle {
    bool operator==(const EdgeStyle &rhs) {
        if (shape != rhs.shape) return false;
        if (dash != rhs.dash) return false;
        if (color != rhs.color) return false;
        if (width != rhs.width) return false;
        return true;
    };
    EdgeShape shape;
    EdgeDash dash;
    Color color;
    int width;
};

EdgeStyle defaultEdgeStyle() {
    EdgeStyle res;
    res.shape = EdgeShape();
    res.dash = EdgeDash();
    res.color = Color::BROWN_GREY;
    res.width = 5;
    return res;
}

#endif
