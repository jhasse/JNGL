// Copyright 2012-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "shapes.hpp"

#include "../main.hpp"
#include "../spriteimpl.hpp"
#include "matrix.hpp"
#include "screen.hpp"

namespace jngl {

uint8_t colorRed = 0, colorGreen = 0, colorBlue = 0, colorAlpha = 255;

void setColor(const jngl::Color rgb) {
	setColor(rgb.getRed(), rgb.getGreen(), rgb.getBlue());
}

void setColor(const jngl::Color color, const unsigned char alpha) {
	setColor(color.getRed(), color.getGreen(), color.getBlue(), alpha);
}

void setColor(unsigned char red, unsigned char green, unsigned char blue) {
	colorRed = red;
	colorGreen = green;
	colorBlue = blue;
}

void setColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
	colorRed = red;
	colorGreen = green;
	colorBlue = blue;
	colorAlpha = alpha;
}

void setAlpha(const uint8_t alpha) {
	colorAlpha = alpha;
}

std::stack<unsigned char> alphas;

void pushAlpha(unsigned char alpha) {
	alphas.push(colorAlpha);
	setAlpha(colorAlpha * alpha / 255);
}

void popAlpha() {
	setAlpha(alphas.top());
	alphas.pop();
}

void drawEllipse(float xmid, float ymid, float width, float height, float startAngle) {
	drawEllipse(modelview().translate({xmid, ymid}), width, height, startAngle);
}

void drawEllipse(const Vec2 position, const float width, const float height,
                 const float startAngle) {
	drawEllipse(modelview().translate(position), width, height, startAngle);
}

void drawEllipse(Mat3 modelview, float width, float height, float startAngle) {
	glBindVertexArray(opengl::vaoStream);
	auto tmp = useSimpleShaderProgram(modelview.scale(
	    static_cast<float>(getScaleFactor()), static_cast<float>(getScaleFactor())));
	std::vector<float> vertexes;
	vertexes.push_back(0.f);
	vertexes.push_back(0.f);
	for (float t = startAngle; t < 2.f * M_PI; t += 0.1f) {
		vertexes.push_back(width * std::sin(t));
		vertexes.push_back(-height * std::cos(t));
	}
	vertexes.push_back(0.f);
	vertexes.push_back(-height);
	glBindBuffer(GL_ARRAY_BUFFER, opengl::vboStream); // VAO does NOT save the VBO binding
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexes.size() * sizeof(float)),
	             vertexes.data(), GL_STREAM_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(vertexes.size() / 2));
}

void drawCircle(const Vec2 position, const float radius, const float startAngle) {
	drawEllipse(modelview().translate(position), radius, radius, startAngle);
}

void drawCircle(Mat3 modelview, const float radius, const float startAngle) {
	drawEllipse(modelview, radius, radius, startAngle);
}

} // namespace jngl
