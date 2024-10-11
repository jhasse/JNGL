// Copyright 2017-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "SdlController.hpp"

#include "../../log.hpp"
#include "../../window.hpp"

#include <cmath>

namespace jngl {

SdlController::SdlController(SDL_Joystick* const handle, const int index)
: handle(handle), haptic(SDL_HapticOpenFromJoystick(handle)) {
	if (haptic) {
		if (SDL_HapticRumbleInit(haptic) < 0) {
			internal::error(SDL_GetError());
		}
	} else {
		internal::error(SDL_GetError());
	}
	switch (SDL_JoystickNumButtons(handle)) {
		case 11:
			model = Model::XBOX_WIRED;
			break;
		case 13:
			model = Model::DS4;
			break;
		case 15:
			model = Model::XBOX;
			break;
		default: {
			if (SDL_IsGameController(index)) {
				gameController = SDL_GameControllerOpen(index);
				if (gameController) {
					this->handle = nullptr;
					SDL_JoystickClose(handle);
				} else {
					internal::warn("SDL_GameControllerOpen failed, falling back to joystick.");
				}
			}
		}
	};
}

SdlController::~SdlController() {
	if (handle) {
		SDL_JoystickClose(handle);
	} else {
		SDL_GameControllerClose(gameController);
	}
}

float SdlController::stateWithoutDeadzone(controller::Button button) const {
	if (handle) {
		const bool xboxWired = model == Model::XBOX_WIRED;
		int axisIndex;
		switch (button) {
			case controller::LeftStickX: axisIndex = 0; break;
			case controller::LeftStickY: axisIndex = 1; break;
			case controller::RightStickX: axisIndex = xboxWired ? 2 : 3; break;
			case controller::RightStickY: axisIndex = xboxWired ? 3 : 4; break;
			case controller::LeftTrigger: axisIndex = xboxWired ? 5 : 2; break;
			case controller::RightTrigger: axisIndex = xboxWired ? 4 : 5; break;
			default: return down(button) ? 1 : 0;
		}
		float state = SDL_JoystickGetAxis(handle, axisIndex);
		if (state < 0) {
			state /= 32768;
		} else {
			state /= 32767;
		}
		if (button == controller::LeftStickY || button == controller::RightStickY) {
			state *= -1;
		}
		return state;
	}
	SDL_GameControllerAxis axis;
	switch (button) {
		case controller::LeftStickX: axis = SDL_CONTROLLER_AXIS_LEFTX; break;
		case controller::LeftStickY: axis = SDL_CONTROLLER_AXIS_LEFTY; break;
		case controller::RightStickX: axis = SDL_CONTROLLER_AXIS_RIGHTX; break;
		case controller::RightStickY: axis = SDL_CONTROLLER_AXIS_RIGHTY; break;
		case controller::LeftTrigger: axis = SDL_CONTROLLER_AXIS_TRIGGERLEFT; break;
		case controller::RightTrigger: axis = SDL_CONTROLLER_AXIS_TRIGGERRIGHT; break;
		default: return down(button) ? 1 : 0;
	}
	float state = SDL_GameControllerGetAxis(gameController, axis);
	if (state < 0) {
		state /= 32768;
	} else {
		state /= 32767;
	}
	if (button == controller::LeftStickY || button == controller::RightStickY) {
		state *= -1;
	}
	return state;
}

float SdlController::stateImpl(controller::Button button) const {
	float state = stateWithoutDeadzone(button);
	controller::Button otherAxis;
	switch(button) {
		case controller::LeftStickY:
			otherAxis = controller::LeftStickX;
			break;
		case controller::LeftStickX:
			otherAxis = controller::LeftStickY;
			break;
		case controller::RightStickY:
			otherAxis = controller::RightStickX;
			break;
		case controller::RightStickX:
			otherAxis = controller::RightStickY;
			break;
		default:
			return state; // no deadzone needed
	}
	auto otherState = stateWithoutDeadzone(otherAxis);
	if (model == Model::DS4) { // DualShock 4 returns sticks as square coordinates
		const double tmp = state * std::sqrt(1 - 0.5 * otherState * otherState);
		otherState *= std::sqrt(1 - 0.5 * state * state);
		state = static_cast<float>(tmp);
	}
	if (model != Model::XBOX_WIRED && model != Model::XBOX) {
		return state; // no deadzone needed
	}
	if (state * state + otherState * otherState < 0.1) { // inside deadzone circle?
		return 0;
	}
	return state;
}

bool SdlController::down(const controller::Button button) const {
	if (handle) {
		const bool xbox = (model == Model::XBOX || model == Model::XBOX_WIRED);
		if (xbox && (button == controller::LeftTrigger || button == controller::RightTrigger)) {
			return state(button) > 0;
		}
		int buttonIndex;
		switch (button) {
			case controller::A: buttonIndex = 0; break;
			case controller::B: buttonIndex = 1; break;
			case controller::X: buttonIndex = xbox ? 2 : 3; break;
			case controller::Y: buttonIndex = xbox ? 3 : 2; break;
			case controller::LeftButton: buttonIndex = 4; break;
			case controller::RightButton: buttonIndex = 5; break;
			case controller::LeftTrigger: buttonIndex = 6; break;
			case controller::RightTrigger: buttonIndex = 7; break;
			case controller::Start: buttonIndex = xbox ? 7 : 9; break;
			case controller::Back: buttonIndex = xbox ? 6 : 8; break;
			case controller::DpadUp: return (SDL_JoystickGetHat(handle, 0) & SDL_HAT_UP) != 0;
			case controller::DpadDown: return (SDL_JoystickGetHat(handle, 0) & SDL_HAT_DOWN) != 0;
			case controller::DpadLeft: return (SDL_JoystickGetHat(handle, 0) & SDL_HAT_LEFT) != 0;
			case controller::DpadRight: return (SDL_JoystickGetHat(handle, 0) & SDL_HAT_RIGHT) != 0;
			case controller::LeftStick: buttonIndex = xbox ? 9 : 11; break;
			case controller::RightStick: buttonIndex = xbox ? 10 : 12; break;
			default: return state(button) > 0.5f; // e.g. LeftStickX
		}
		return SDL_JoystickGetButton(handle, buttonIndex) != 0;
	}
	switch (button) {
	case controller::Button::A:
		return SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_A) != 0u;
	case controller::Button::B:
		return SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_B) != 0u;
	case controller::Button::X:
		return SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_X) != 0u;
	case controller::Button::Y:
		return SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_Y) != 0u;
	case controller::Button::LeftButton:
		return SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) !=
		       0u;
	case controller::Button::RightButton:
		return SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) !=
		       0u;
	case controller::Button::Start:
		return SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_START) != 0u;
	case controller::Button::Back:
		return SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_BACK) != 0u;
	case controller::Button::DpadUp:
		return SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_UP) != 0u;
	case controller::Button::DpadDown:
		return SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN) != 0u;
	case controller::Button::DpadLeft:
		return SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT) != 0u;
	case controller::Button::DpadRight:
		return SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) != 0u;
	case controller::Button::LeftStick:
		return SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_LEFTSTICK) != 0u;
	case controller::Button::RightStick:
		return SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_RIGHTSTICK) != 0u;
	default:
		return state(button) > 0.5f;
	}
}

void SdlController::rumble(const float vibration, const std::chrono::milliseconds ms) {
	if (haptic) {
		SDL_HapticRumblePlay(haptic, vibration, ms.count());
	}
}

bool SdlController::is(SDL_Joystick* const handle) const {
	if (this->handle) {
		return this->handle == handle;
	}
	return SDL_GameControllerGetJoystick(gameController) == handle;
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
				SDL_JoystickClose(handle);
				rtn.push_back(controller);
				found = true;
				break;
			}
		}
		if (!found) {
			controllers.emplace_back(std::make_shared<SdlController>(handle, i));
			rtn.emplace_back(controllers.back());
		}
	}
	return rtn;
}

void Window::updateControllerStates() {
}

} // namespace jngl
