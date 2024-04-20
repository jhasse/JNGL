// Copyright 2023-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "../engine.hpp"
#include "../constants.hpp"
#include "../Stream.hpp"
#include "../../jngl/debug.hpp"

#include <oboe/Oboe.h>

#include <cassert>
#include <mutex>
#include <atomic>
#include <vector>
#include <cmath>

namespace jngl::audio {

struct engine::Impl {
	std::shared_ptr<Stream> output;

	Impl(std::shared_ptr<Stream> output);

	void start() {
		if (oboeStream) {
			assert(oboeStream->getState() == oboe::StreamState::Started);
			return;
		}
		const auto result = builder.openStream(oboeStream);
		if (result != oboe::Result::OK) {
			throw std::runtime_error(oboe::convertToText(result));
		}
		oboeStream->requestStart();
	}
	void stop() {
		if (oboeStream) {
			oboeStream->close();
			oboeStream.reset();
		}
	}

	class Callback : public oboe::AudioStreamCallback {
	public:
		Callback(Impl& self) : self(self) {
		}

	private:
		oboe::DataCallbackResult onAudioReady(oboe::AudioStream*, void* data,
		                                      int32_t len) override {
			float* dst = reinterpret_cast<float*>(data);
			self.output->read(dst, len * 2);
			return oboe::DataCallbackResult::Continue;
		}

		void onErrorAfterClose(oboe::AudioStream*, oboe::Result error) override {
			if (error == oboe::Result::ErrorDisconnected) {
				assert(self.oboeStream->getState() == oboe::StreamState::Closed);
				self.oboeStream.reset();
				self.start();
			}
		}

		Impl& self;
	};

private:
	Callback callback;
	oboe::AudioStreamBuilder builder;
	std::shared_ptr<oboe::AudioStream> oboeStream;
};

engine::Impl::Impl(std::shared_ptr<Stream> output) : output(std::move(output)), callback(*this) {
	builder.setDirection(oboe::Direction::Output);
	builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
	builder.setSharingMode(oboe::SharingMode::Exclusive);
	builder.setFormat(oboe::AudioFormat::Float);
	builder.setChannelCount(oboe::ChannelCount::Stereo);
	builder.setSampleRate(frequency);
	// desired.samples = 256;
	builder.setCallback(&callback);
	start();
}

engine::engine(std::shared_ptr<Stream> output) : impl(std::make_unique<Impl>(std::move(output))) {
}

engine::~engine() {
	impl->stop();
}

void engine::setPause(bool pause) {
	if (pause) {
		impl->stop();
	} else {
		impl->start();
	}
}

void engine::step() {
}

} // namespace jngl::audio
