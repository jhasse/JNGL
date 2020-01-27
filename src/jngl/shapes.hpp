// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include "Color.hpp"
#include "Vec2.hpp"

namespace jngl {

/// Sets the color which should be used to draw primitives
///
/// Doesn't change the alpha value currently set by setAlpha()
void setColor(jngl::Color rgb);

void setColor(jngl::Color, unsigned char alpha);

void setColor(unsigned char red, unsigned char green, unsigned char blue);

void setColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);

/// Sets the alpha value which should be used to draw primitives (0 = fully transparent, 255 = fully
/// opaque)
void setAlpha(unsigned char alpha);

void pushAlpha(unsigned char alpha);

void popAlpha();

void setLineWidth(float width);

void drawLine(Vec2 start, Vec2 end);

void drawLine(double xstart, double ystart, double xend, double yend);

void drawEllipse(float xmid, float ymid, float width, float height, float startAngle = 0);

void drawEllipse(Vec2, float width, float height, float startAngle = 0);

void drawCircle(Vec2, float radius, float startAngle = 0);

void drawPoint(double x, double y);

void drawTriangle(Vec2 a, Vec2 b, Vec2 c);

void drawTriangle(double A_x, double A_y, double B_x, double B_y, double C_x, double C_y);

void drawRect(double xposition, double yposition, double width, double height);

void drawRect(Vec2 position, Vec2 size);

template <class Vect> void drawRect(Vect pos, Vect size) {
	drawRect(pos.x, pos.y, size.x, size.y);
}

} // namespace jngl
