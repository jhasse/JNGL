// Copyright 2010-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "texture.hpp"

#include "jngl/Shader.hpp"
#include "jngl/Vertex.hpp"

#include <cassert>

namespace jngl {

ShaderProgram* Texture::textureShaderProgram = nullptr;
Shader* Texture::textureVertexShader = nullptr;
int Texture::shaderSpriteColorUniform = -1;
int Texture::modelviewUniform = -1;

Texture::Texture(const float preciseWidth, const float preciseHeight, const int width,
                 const int height, const GLubyte* const* const rowPointers, GLenum format,
                 const GLubyte* const data) : texture_(opengl::genAndBindTexture()) {
	assert(format == GL_RGB || format == GL_RGBA || format == GL_BGR);
	glTexImage2D(GL_TEXTURE_2D, 0, format == GL_RGBA ? GL_RGBA : GL_RGB, width, height, 0, format,
	             GL_UNSIGNED_BYTE, nullptr);
	vertexes = {
		0, 0,
		0, 0, // texture coordinates
		0, preciseHeight,
		0, 1, // texture coordinates
		preciseWidth, preciseHeight,
		1, 1, // texture coordinates
		preciseWidth, 0,
		1, 0 // texture coordinates
	};
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vertexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), vertexes.data(), GL_STATIC_DRAW);

	const GLint posAttrib = textureShaderProgram->getAttribLocation("position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	glEnableVertexAttribArray(posAttrib);

	const GLint texCoordAttrib = textureShaderProgram->getAttribLocation("inTexCoord");
	glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
	                      reinterpret_cast<void*>(2 * sizeof(float))); // NOLINT
	glEnableVertexAttribArray(texCoordAttrib);

	if (rowPointers) {
		assert(!data);
		for (int i = 0; i < height; ++i) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, width, 1, format, GL_UNSIGNED_BYTE,
			                rowPointers[i]);
		}
	}
	if (data) {
		assert(!rowPointers);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
	}
}

Texture::~Texture() {
	if (textureShaderProgram) {
		// if the textureShaderProgram is nullptr, this means unloadShader() has been called by
		// hideWindow() and the OpenGL context doesn't exist anymore. It's unnecessary to delete
		// OpenGL resources in that case. It might even lead to crashes when the OpenGL function
		// pointers have been unloaded (Windows).
		glDeleteTextures(1, &texture_);
		glDeleteBuffers(1, &vertexBuffer_);
		glDeleteVertexArrays(1, &vao);
	}
}

void Texture::bind() const {
	glBindVertexArray(vao);

	glBindTexture(GL_TEXTURE_2D, texture_);
}

void Texture::draw() const {
	bind();
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Texture::drawClipped(const float xstart, const float xend, const float ystart,
                          const float yend, const float red, const float green, const float blue,
                          const float alpha) const {
	std::vector<float> vertexes = this->vertexes;

	vertexes[8] *= (xend - xstart);
	vertexes[12] *= (xend - xstart);
	vertexes[5] *= (yend - ystart);
	vertexes[9] *= (yend - ystart);

	// Texture coordinates:
	vertexes[2] = vertexes[6] = xstart;
	vertexes[3] = vertexes[15] = ystart;
	vertexes[10] = vertexes[14] = xend;
	vertexes[7] = vertexes[11] = yend;

	glBindVertexArray(opengl::vaoStream);
	auto tmp = textureShaderProgram->use();
	glUniform4f(shaderSpriteColorUniform, red, green, blue, alpha);
	glUniformMatrix3fv(modelviewUniform, 1, GL_FALSE, opengl::modelview.data);
	glBindBuffer(GL_ARRAY_BUFFER, opengl::vboStream); // VAO does NOT save the VBO binding
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexes.size() * sizeof(float)),
	             vertexes.data(), GL_STREAM_DRAW);

	const GLint posAttrib = textureShaderProgram->getAttribLocation("position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	glEnableVertexAttribArray(posAttrib);

	const GLint texCoordAttrib = textureShaderProgram->getAttribLocation("inTexCoord");
	glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
	                      reinterpret_cast<void*>(2 * sizeof(float))); // NOLINT
	glEnableVertexAttribArray(texCoordAttrib);

	glBindTexture(GL_TEXTURE_2D, texture_);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Texture::drawMesh(const std::vector<Vertex>& vertexes) const {
	glBindVertexArray(opengl::vaoStream);
	glBindBuffer(GL_ARRAY_BUFFER, opengl::vboStream); // VAO does NOT save the VBO binding
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexes.size() * sizeof(vertexes[0])),
	             vertexes.data(), GL_STREAM_DRAW);

	const GLint posAttrib = textureShaderProgram->getAttribLocation("position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	glEnableVertexAttribArray(posAttrib);

	const GLint texCoordAttrib = textureShaderProgram->getAttribLocation("inTexCoord");
	glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
	                      reinterpret_cast<void*>(2 * sizeof(float))); // NOLINT
	glEnableVertexAttribArray(texCoordAttrib);

	glBindTexture(GL_TEXTURE_2D, texture_);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexes.size()));
}

GLuint Texture::getID() const {
	return texture_;
}

float Texture::getPreciseWidth() const {
	return vertexes[8];
}

float Texture::getPreciseHeight() const {
	return vertexes[5];
}

void Texture::unloadShader() {
	delete textureVertexShader;
	textureVertexShader = nullptr;
	delete textureShaderProgram;
	textureShaderProgram = nullptr;
}

void Texture::setBytes(const unsigned char* const bytes, const int width, const int height) const {
	glBindTexture(GL_TEXTURE_2D, texture_);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
}

} // namespace jngl
