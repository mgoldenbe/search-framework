#ifndef STYLE_H
#define STYLE_H

/// \file style.h
/// \brief Types for styles of visual representation of vertices and edges.
/// \author Meir Goldenberg

/// Enumeration of possible shapes for visualizing a vertex.
enum class VertexShape {CIRCLE};

/// Enumeration of possible patterns for filling a visualized vertex.
enum class VertexFillPattern {SOLID};

/// Visualization properties for a single vertex.
struct VertexStyle {
    /// The shape used for visual representation of the vertex.
    VertexShape shape = VertexShape();

    /// The fill pattern used for visual representation of the vertex.
    VertexFillPattern pattern = VertexFillPattern();

    static double sizeBase; ///< Size corresponding to sizeFactor = 1.0.

    /// Size (radius for circle) of the visual representation of the vertex
    /// measured in sizeBase units.
    double sizeFactor = 1.0;

    /// The fill color used for visual representation of the vertex.
    Color fillColor = Color::WARM_GREY;

    /// The border color used for visual representation of the vertex.
    Color borderColor  = Color();

    double borderWidthFactor = 0;  ///< Width of the border line of the vertex
                                   ///(in sizeBase units).

    Color emphasisColor = Color(); ///< A marker inside the vertex. Used to
                                   ///distinguish special vertices (e.g. start
                                   ///and goal).

    double emphasisSizeFactor = 0; ///< Size (radius for circle) used to
                                   ///emphasize the vertex measured in sizeBase
                                   ///units.
};
double VertexStyle::sizeBase = 10;

/// Visualization properties for a single directed edge.
struct EdgeStyle {
    /// Comparison of two edge styles
    bool operator==(const EdgeStyle &rhs) {
        if (color != rhs.color) return false;
        if (widthFactor != rhs.widthFactor) return false;
        return true;
    };
    Color color = defaultColor; ///< Edge color.
    double widthFactor = 1.0; ///< Width of the edge (in widthBase units).
    bool arrow = false; ///< Whether the directed edge has an arrow to show the
                        ///edge's direction.

    /// Color of an edge before an algorithm does something with it.
    static constexpr Color defaultColor = Color::BROWN_GREY;
    static double widthBase;  ///< Width corresponding to widthFactor == 1.0.
};
double EdgeStyle::widthBase = 5.0;

#endif
