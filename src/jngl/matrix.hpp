// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

namespace jngl {

/// Multiplies the global ModelView matrix with a rotation matrix
void rotate(double degree);

/// Multiplies the global ModelView matrix with a translation matrix
void translate(double x, double y);

/// Multiplies the global ModelView matrix with a translation matrix
///
/// Equivalent to calling `jngl::translate(v.x, v.y);`.
template <class Vect> void translate(Vect v) {
	translate(v.x, v.y);
}

/// Multiplies the global ModelView matrix by a scaling matrix
///
/// Equivalent to calling `jngl::scale(factor, factor)`.
void scale(double factor);

/// Multiplies the global ModelView matrix by a scaling matrix
void scale(double xfactor, double yfactor);

/// Pushes the current ModelView matrix on a global stack
void pushMatrix();

/// Replaces the current ModelView matrix with the top element of the global stack
void popMatrix();

/// Resets the global ModelView matrix to the identity matrix
void reset();

} // namespace jngl
