// Copyright 2019-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace jngl {

class Sound;
struct SoundParams;

class SoundFile {
public:
	explicit SoundFile(const std::string& filename);
	~SoundFile() = default;
	SoundFile(const SoundFile&) = delete;
	SoundFile& operator=(const SoundFile&) = delete;
	SoundFile(SoundFile&&) = default;
	SoundFile& operator=(SoundFile&&) = default;
	void Play();
	void Stop();
	bool IsPlaying();
	void loop();
	void SetPitch(float p);
	void setVolume(float v);

private:
	std::shared_ptr<Sound> sound_;
	std::unique_ptr<SoundParams> params;
	std::vector<char> buffer_;
};

} // namespace jngl
