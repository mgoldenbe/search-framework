#ifndef VISUALIZATION_UTILITIES
#define VISUALIZATION_UTILITIES

#include "Colors.h"

struct RGB {
    static int red(Color color) {
        return RGBValues[static_cast<int>(color)] >> 16;
    }
    static int green(Color color) {
        return (RGBValues[static_cast<int>(color)] >> 8) % 256;
    }
    static int blue(Color color) {
        return RGBValues[static_cast<int>(color)] % 256;
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
    res.fillColor = Color::COOL_GREY;
    res.borderColor = Color();
    res.borderWidth = 0;
    res.emphasisColor = Color();
    res.emphasisWidth = 0;
    return res;
}

struct EdgeStyle {
    EdgeShape shape;
    EdgeDash dash;
    Color color;
    int width;
};

EdgeStyle defaultEdgeStyle() {
    EdgeStyle res;
    res.shape = EdgeShape();
    res.dash = EdgeDash();
    res.color = Color::COOL_GREY;
    res.width = 5;
    return res;
}

#endif
