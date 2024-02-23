// Copyright 2012-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Functions for drawing shapes
/// @file
#pragma once

#include "Color.hpp"
#include "Vec2.hpp"

#include <cstdint>

namespace jngl {

class Mat3;

/// Sets the color which should be used to draw primitives
///
/// Doesn't change the alpha value currently set by setAlpha()
void setColor(jngl::Color rgb);

/// Sets the color and alpha which should be used to draw primitives
/// @param alpha [0...255]
void setColor(jngl::Color, unsigned char alpha);

void setColor(unsigned char red, unsigned char green, unsigned char blue);

void setColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);

/// Sets the alpha value which should be used to draw primitives (0 = fully transparent, 255 = fully
/// opaque)
void setAlpha(uint8_t alpha);

[[deprecated("Use setAlpha instead")]]
/// \deprecated Use setAlpha instead
void pushAlpha(unsigned char alpha);

[[deprecated("Use setAlpha instead")]]
/// \deprecated Use setAlpha instead
void popAlpha();

[[deprecated("Use drawRectangle instead")]]
/// \deprecated Use drawRectangle instead
void setLineWidth(float width);

/// Draws a line from start to end, the width can be set using setLineWidth
void drawLine(Vec2 start, Vec2 end);

[[deprecated("Use drawLine(Vec2, Vec2) instead")]]
/// \deprecated Use drawLine(Vec2, Vec2) instead
void drawLine(double xstart, double ystart, double xend, double yend);

/// Draws a line from (0, 0) to \a end
void drawLine(const Mat3& modelview, Vec2 end);

void drawEllipse(float xmid, float ymid, float width, float height, float startAngle = 0);

void drawEllipse(Vec2, float width, float height, float startAngle = 0);

void drawEllipse(Mat3 modelview, float width, float height, float startAngle = 0);

/// Angles in radian
void drawCircle(Vec2, float radius, float startAngle = 0);

void drawCircle(Mat3 modelview, float radius, float startAngle = 0);

void drawPoint(double x, double y);

/// Draws the triangle a -> b -> c
void drawTriangle(Vec2 a, Vec2 b, Vec2 c);

[[deprecated("Use drawTriangle(Vec2, Vec2, Vec2) instead")]]
/// \deprecated Use drawTriangle(Vec2, Vec2, Vec2) instead
void drawTriangle(double A_x, double A_y, double B_x, double B_y, double C_x, double C_y);

void drawRect(double xposition, double yposition, double width, double height);

/// Draws a rectangle at \a position
///
/// Use setColor(Color) to change the color and setAlpha(uint8_t) to change the translucency.
void drawRect(Vec2 position, Vec2 size);

/// Draws a rectangle spawning from (0, 0) to (size.x, size.y) with the specified color
///
/// Use setAlpha to set the opacity.
void drawRect(const Mat3& modelview, Vec2 size, Color);

template <class Vect> void drawRect(Vect pos, Vect size) {
	drawRect(pos.x, pos.y, size.x, size.y);
}

} // namespace jngl
