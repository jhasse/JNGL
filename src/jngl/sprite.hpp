// Copyright 2012-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Sprite class and related functions
/// \file
#pragma once

#include "Color.hpp"
#include "Drawable.hpp"
#include "ShaderProgram.hpp"
#include "Vec2.hpp"

#include <future>
#include <string_view>
#include <vector>

namespace jngl {

class ImageData;
class Mat3;
class Texture;
struct Vertex;

/// Higher-level representation of an image
class Sprite : public Drawable {
public:
	enum class LoadType {
		NORMAL,
		HALF,
		THREADED,
	};

	Sprite(const unsigned char* bytes, size_t width, size_t height);

	/// \deprecated Use Loader instead
	explicit Sprite(std::string_view filename, LoadType loadType = LoadType::NORMAL);

	/// Does nothing
	void step() override;

	/// Draws the Sprite, centered by default
	void draw() const override;

	/// Use this class to load a Sprite asynchronously
	///
	/// Example:
	/// \code
	/// class MyGame : public jngl::Work {
	///     void step() override {}
	///     void draw() const override {
	///         if (mySprite) {
	///             mySprite->draw();
	///         } else {
	///             jngl::print("loading...", jngl::Vec2(0, 0));
	///         }
	///     }
	///
	///     jngl::Sprite::Loader mySprite{ "foo.webp" };
	/// };
	/// \endcode
	class Loader {
	public:
		/// Starts a thread to load \a filename and returns instantly
		///
		/// Note that if the file couldn't be found this will not throw. Instead the exception will
		/// be thrown on first use by shared() or operator->().
		explicit Loader(std::string filename) noexcept;

		/// Blocks until the Sprite has been loaded
		///
		/// While you won't have access to std::shared_ptr returned by shared(), the Sprite will
		/// still be loaded into JNGL's cache and the next time you create a Sprite from this
		/// filename, you'll get the same std::shared_ptr.
		///
		/// If shared() or operator->() haven't been called yet and the file wasn't found the
		/// destructor won't throw but use errorMessage(const std::string&).
		~Loader() noexcept;

		/// Blocks until the Sprite has been loaded and returns a non-nullptr std::shared_ptr
		///
		/// \throws std::runtime_error on decoding errors or file not found
		std::shared_ptr<Sprite> shared() const;

		/// Returns whether the sprite has been loaded, does NOT block
		///
		/// After this returned true all other methods won't block any more.
		operator bool() const; // NOLINT

		/// Blocks until the Sprite has been loaded
		///
		/// \throws std::runtime_error on decoding errors or file not found
		Sprite* operator->() const;

	private:
		mutable std::future<std::unique_ptr<ImageData>> imageDataFuture;
		std::string filename;
	};

	/// Draws the image centered using \a modelview
	///
	/// \param shaderProgram Passing `nullptr` uses the default.
	void draw(Mat3 modelview, const ShaderProgram* = nullptr) const;

	void draw(const ShaderProgram* shaderProgram) const;

	/// Draws the image scaled by `xfactor` and `yfactor`
	///
	/// \param xfactor Scale width by this factor
	/// \param yfactor Scale height by this factor
	/// \param shaderProgram Passing `nullptr` uses the default.
	///
	/// `drawScaled(1, 1)` would draw it normally. You can pass negative values to flip the image.
	/// For example `drawScaled(-1, 1)` would draw the image horizontally flipped.
	///
	/// \deprecated Scale the modelview matrix instead using jngl::Mat3::scale
	[[deprecated("Scale the modelview matrix instead using jngl::Mat3::scale")]]
	void drawScaled(float xfactor, float yfactor,
	                const ShaderProgram* shaderProgram = nullptr) const;

	/// \deprecated Use new drawClipped(Vec2, Vec2) method instead
	[[deprecated("Use new drawClipped(Vec2, Vec2) method instead")]] void
	drawClipped(float xstart, float xend, float ystart, float yend) const;

	/// Draw a cutout of the sprite. drawClipped({0, 0}, {1, 1}) would draw it normally.
	void drawClipped(Vec2 start, Vec2 end) const;

	/// Draws a list of triangles with the sprite's texture on it using the global modelview from
	/// jngl::modelview()
	void drawMesh(const std::vector<Vertex>& vertexes, const ShaderProgram* = nullptr) const;

	/// Draws a list of triangles with the sprite's texture on it, ignores the Sprite's position
	void drawMesh(Mat3 modelview, const std::vector<Vertex>& vertexes,
	              const ShaderProgram* = nullptr) const;

	void setBytes(const unsigned char*);

	/// Returns a reference to JNGL's default vertex shader used to draw textures
	static const Shader& vertexShader();

	/// Function which actually loads the sprite
	std::shared_ptr<Finally> loader;

private:
	static void cleanUpRowPointers(std::vector<unsigned char*>& buf);
	void loadTexture(int scaledWidth, int scaledHeight, std::string_view filename, bool halfLoad,
	                 unsigned int format, const unsigned char* const* rowPointers,
	                 const unsigned char* data = nullptr);
	Finally LoadPNG(std::string_view filename, FILE* fp, bool halfLoad);
	struct BMPHeader {
		unsigned int dataOffset;
		unsigned int headerSize;
		int width;
		int height;
		unsigned short planes;
		unsigned short bpp;
		unsigned int compression;
		unsigned int dataSize;
	};
	Finally LoadBMP(std::string_view filename, FILE* fp, bool halfLoad);
#ifndef NOWEBP
	Finally LoadWebP(std::string_view filename, FILE* file, bool halfLoad);
#endif

	std::shared_ptr<Texture> texture;
};

void draw(std::string_view filename, double x, double y);

template <class Vect> void draw(const std::string& filename, Vect pos) {
	draw(filename, pos.x, pos.y);
}

/// Starts a thread to load \a filename and returns a Finally which will join it
///
/// \param filename Name of an image file (extension is optional) or a .ogg sound file.
Finally load(const std::string& filename);

void unload(const std::string& filename);

void unloadAll();

void drawClipped(const std::string& filename, double xposition, double yposition, float xstart,
                 float xend, float ystart, float yend);

void setSpriteColor(unsigned char red, unsigned char green, unsigned char blue,
                    unsigned char alpha);

void setSpriteColor(unsigned char red, unsigned char green, unsigned char blue);

void setSpriteColor(Color);

void setSpriteAlpha(unsigned char alpha);

void pushSpriteAlpha(unsigned char alpha = 255);

void popSpriteAlpha();

int getWidth(const std::string& filename);

int getHeight(const std::string& filename);

#if __cplusplus >= 201703L
[[nodiscard]]
#endif
Finally
disableBlending();

} // namespace jngl
