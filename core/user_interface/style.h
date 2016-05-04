#ifndef STYLE_H
#define STYLE_H

/**
 * @file   style.h
 * @author Meir Goldenberg
 * @date   Wed May  4 20:02:06 2016
 *
 * @brief  Types for styles of visual representation of vertices and edges.
 *
 *
 */


/// Enumeration of possible shapes for visualizing a vertex.
enum class VertexShape {CIRCLE};

/// Enumeration of possible patterns for filling a visualized vertex.
enum class VertexFillPattern {SOLID};

/// Visualization properties for a single vertex.
struct VertexStyle {
    VertexShape shape = VertexShape();
    VertexFillPattern pattern = VertexFillPattern();
    static double sizeBase; ///< Size corresponding to sizeFactor = 1.0.
    double sizeFactor = 1.0; ///< Size (radius for circle) of the visual
                             ///representation of the vertex measured in
                             ///sizeBase units.
    Color fillColor = Color::WARM_GREY;
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
    Color color = Color::BROWN_GREY;
    static double widthBase;  ///< Width corresponding to widthFactor == 1.0.
    double widthFactor = 1.0; ///< Width of the edge (in widthBase units).
    bool arrow = false; ///< Whether the directed edge has an arrow to show the
                        ///edge's direction.
    int depth = 0;      ///< An edge with depth == 0 is drawn in foreground.
    static constexpr int maxDepth = 2; ///< Maximal depth
};
double EdgeStyle::widthBase = 5.0;

#endif
