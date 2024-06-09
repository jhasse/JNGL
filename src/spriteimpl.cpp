// Copyright 2007-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "spriteimpl.hpp"

#include "jngl/Alpha.hpp"
#include "jngl/ImageData.hpp"
#include "jngl/message.hpp"
#include "jngl/screen.hpp"
#include "texture.hpp"
#include "windowptr.hpp"

namespace jngl {

Rgba gSpriteColor{ 1, 1, 1, 1 };

void setSpriteColor(unsigned char red, unsigned char green, unsigned char blue) {
	gSpriteColor.setRgb(Rgb::u8(red, green, blue));
}

void setSpriteColor(Rgb color) {
	gSpriteColor.setRgb(color);
}

std::stack<float> spriteAlphas;

void pushSpriteAlpha(unsigned char alpha) {
	spriteAlphas.push(gSpriteColor.getAlpha());
	gSpriteColor.setAlpha(gSpriteColor.getAlpha() * Alpha::u8(alpha).getAlpha());
}

void popSpriteAlpha() {
	gSpriteColor.setAlpha(spriteAlphas.top());
	spriteAlphas.pop();
}

void setSpriteColor(unsigned char red, unsigned char green, unsigned char blue,
                    unsigned char alpha) {
	gSpriteColor = Rgba::u8(red, green, blue, alpha);
}

void setSpriteAlpha(unsigned char alpha) {
	gSpriteColor.setAlpha(Alpha::u8(alpha));
}

std::unordered_map<std::string_view, std::shared_ptr<Sprite>> sprites_;

// halfLoad is used, if we only want to find out the width or height of an image. Load won't throw
// an exception then
Sprite& GetSprite(std::string_view filename, const Sprite::LoadType loadType) {
	auto it = sprites_.find(filename);
	if (it == sprites_.end()) { // texture hasn't been loaded yet?
		if (loadType != Sprite::LoadType::HALF) {
			pWindow.ThrowIfNull();
		}
		return *sprites_.emplace(filename, std::make_shared<Sprite>(filename, loadType))
		            .first->second;
	}
	return *(it->second);
}

void draw(std::string_view filename, double x, double y) {
	auto& s = GetSprite(filename);
	s.setPos(x, y);
	s.draw();
}

void drawClipped(const std::string& filename, const double xposition, const double yposition,
                 const float xstart, const float xend, const float ystart, const float yend) {
	auto& s = GetSprite(filename);
	s.setPos(xposition + xstart * s.getWidth(), yposition + ystart * s.getHeight());
	s.drawClipped({ xstart, ystart }, { xend, yend });
}

Finally loadSprite(const std::string& filename) {
	auto& loader = GetSprite(filename, Sprite::LoadType::THREADED).loader;
	if (loader) {
		auto rtn = std::move(*loader);
		loader = nullptr;
		return rtn;
	}
	return Finally(nullptr);
}

Sprite::Loader::Loader(std::string filename) noexcept : filename(std::move(filename)) {
	if (sprites_.count(this->filename) == 0) {
		imageDataFuture = std::async(std::launch::async, [this]() {
			auto tmp = ImageData::load(this->filename, getScaleFactor());
			tmp->pixels(); // TODO: Not needed when Sprite loading and jngl::load will be reworked
			return tmp;
		});
	}
}

Sprite::Loader::~Loader() noexcept {
	try {
		shared();
	} catch(std::exception& e) {
		errorMessage(e.what());
	}
}

std::shared_ptr<Sprite> Sprite::Loader::shared() const {
	if (auto it = sprites_.find(filename); it != sprites_.end()) {
		return it->second;
	}
	auto imageData = imageDataFuture.get();
	double scale = imageData->getImageWidth() == imageData->getWidth() ? getScaleFactor() : 1;
	return sprites_.try_emplace(filename, std::make_shared<Sprite>(*imageData, scale, filename))
	    .first->second;
}

Sprite::Loader::operator bool() const {
	if (sprites_.count(filename) > 0) {
		return true;
	}
	return imageDataFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

Sprite* Sprite::Loader::operator->() const {
	return shared().get();
}

void unload(const std::string& filename) {
	auto it = sprites_.find(filename);
	if (it != sprites_.end()) {
		sprites_.erase(it);
	}
	auto it2 = textures.find(filename);
	if (it2 != textures.end()) {
		textures.erase(it2);
	}
}

void unloadAll() {
	sprites_.clear();
	textures.clear();
	Texture::unloadShader();
}

int getWidth(const std::string& filename) {
	const int width = int(std::lround(GetSprite(filename, Sprite::LoadType::HALF).getWidth()));
	if (!pWindow) {
		unload(filename);
	}
	return width;
}

int getHeight(const std::string& filename) {
	const int height = int(std::lround(GetSprite(filename, Sprite::LoadType::HALF).getHeight()));
	if (!pWindow) {
		unload(filename);
	}
	return height;
}

} // namespace jngl
