// Copyright 2018-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "opengl.hpp"

#include "App.hpp"

#include <boost/qvm_lite.hpp>
#include <stdexcept>

namespace opengl {

jngl::Mat3 modelview;
jngl::Mat4 projection;
GLuint vaoStream;
GLuint vboStream;

void translate(float x, float y) {
	modelview *= boost::qvm::translation_mat(boost::qvm::vec<float, 2>{{ x, y }});
}

void scale(const float x, const float y) {
	modelview *= boost::qvm::diag_mat(boost::qvm::vec<float, 3>{{ x, y, 1 }});
}

GLuint genAndBindTexture() {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
	                jngl::App::isPixelArt() ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return texture;
}

} // namespace opengl
