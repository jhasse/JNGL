// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/debug.hpp"
#include "SdlController.hpp"

namespace jngl {

void setKeyboardVisible(bool) {
}

std::vector<std::shared_ptr<Controller>> getConnectedControllers() {
	static std::vector<std::shared_ptr<SdlController>> controllers;
	int numJoysticks = SDL_NumJoysticks();
	std::vector<std::shared_ptr<Controller>> rtn;
	for (int i = 0; i < numJoysticks; ++i) {
		SDL_Joystick* handle = SDL_JoystickOpen(i);
		if (SDL_JoystickNumButtons(handle) == 0 ||
		    (SDL_JoystickNumAxes(handle) == 0 && SDL_JoystickNumBalls(handle) == 0 &&
		     SDL_JoystickNumHats(handle) == 0)) {
			// This could be the Motion Sensors of the DS4. Ignore it:
			SDL_JoystickClose(handle);
			continue;
		}
		bool found = false;
		for (const auto& controller : controllers) {
			if (controller->is(handle)) {
				rtn.push_back(controller);
				found = true;
				break;
			}
		}
		if (!found) {
			controllers.emplace_back(std::make_shared<SdlController>(handle));
			rtn.emplace_back(controllers.back());
			jngl::debug("Name: ");
			jngl::debugLn(SDL_JoystickNameForIndex(i));
			jngl::debug("Number of Axes: ");
			jngl::debugLn(SDL_JoystickNumAxes(handle));
			jngl::debug("Number of Buttons: ");
			jngl::debugLn(SDL_JoystickNumButtons(handle));
			jngl::debug("Number of Balls: ");
			jngl::debugLn(SDL_JoystickNumBalls(handle));
			jngl::debug("Number of Hats: ");
			jngl::debugLn(SDL_JoystickNumHats(handle));
		}
	}
	return rtn;
}

} // namespace jngl
