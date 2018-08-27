// Copyright 2010-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "jngl/ShaderProgram.hpp"
#include "opengl.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace jngl {

class Texture {
public:
	Texture(int width, int height,
	        GLubyte** rowPointers, // data as row pointers ...
	        GLenum format = GL_RGBA, GLubyte* data = nullptr /* ... or as one pointer */);
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	~Texture();
	void Bind() const;
	void draw(float red, float green, float blue, float alpha, const ShaderProgram* = nullptr) const;
	void drawClipped(float xstart, float xend, float ystart, float yend) const;
	GLuint getID() const;
	int getWidth() const;
	int getHeight() const;

private:
	static ShaderProgram* textureShaderProgram;
	static int shaderSpriteColorUniform;
	GLuint texture_;
	GLuint vertexBuffer_;
	GLuint vao;
	std::vector<opengl::CoordType> texCoords_;
	std::vector<GLfloat> vertexes_;
	int width;
	int height;
};

extern std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

} // namespace jngl
