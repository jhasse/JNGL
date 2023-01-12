#pragma once

#include "stream.hpp"

#include <memory>

namespace psemek::audio
{

	struct mixer
		: stream
	{
		virtual void add(stream_ptr stream) = 0;
		virtual void remove(stream*) = 0;
	};

	using mixer_ptr = std::shared_ptr<mixer>;

	mixer_ptr make_mixer();

}
