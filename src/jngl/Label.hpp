// Copyright 2020-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Label class
/// @file
#pragma once

#include "Color.hpp"
#include "TextLine.hpp"
#include "Widget.hpp"
#include "text.hpp"

namespace jngl {

/// Simple Widget displaying a line of text
class Label : public Widget {
public:
	explicit Label(const std::string& str, Font&, Color, Vec2 position);

	void drawSelf(jngl::Mat3) const override;

	/// Set font color alpha from 0...1
	void setAlpha(float alpha);

	/// How to align the label's text, default is CENTER
	void setAlign(Alignment);

	/// Returns the width of the underlying jngl::TextLine
	float getWidth() const;

protected:
	float alpha = 1.f;

private:
	jngl::TextLine text;
	jngl::Color color;
};

} // namespace jngl
