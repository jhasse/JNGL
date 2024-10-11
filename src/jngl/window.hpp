// Copyright 2012-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// Functions related to the main window.
/// @file

#pragma once

#include "Pixels.hpp"

#include <array>
#include <functional>
#include <string>
#include <vector>

namespace jngl {

/// Creates the main window
///
/// If you haven't set a display name for your app using AppParameters::displayName, \a title will
/// set for the display name.
void showWindow(const std::string& title, int width, int height, bool fullscreen = false,
                std::pair<int, int> minAspectRatio = { 4, 3 },
                std::pair<int, int> maxAspectRatio = { 16, 9 });

/// Cleans up the window and unloads everything
///
/// Will delete all instances of Singleton before starting to unload everything.
void hideWindow();

/// Call this function once when the window is hidden
///
/// The function will be called at the next hideWindow() call, if there is a currently active
/// window.
///
/// Use this function for any cleanup tasks when you game exits. Note that on Android, the process
/// doesn't exit necessarely and the main function can be reentered - so C functions like atexit
/// or destructors of global objects won't work.
void atExit(std::function<void()>);

/// Returns the width of the window in actual pixels (i.e. ignoring jngl::getScaleFactor)
int getWindowWidth();

/// Returns the height of the window in actual pixels (i.e. ignoring jngl::getScaleFactor)
int getWindowHeight();

/// Returns {width, height} of the window in actual pixels
std::array<Pixels, 2> getWindowSize();

/// Returns the width of the main display in actual pixels
int getDesktopWidth();

/// Returns the height of the main display in actual pixels
int getDesktopHeight();

/// If the window is displayed fullscreen (always true on mobile devices)
bool getFullscreen();

/// Toggle fullscreen window mode
void setFullscreen(bool);

/// Sets the main window title
void setTitle(const std::string& title);

enum class Cursor : uint8_t {
    ARROW,
    I,
};
void setCursor(Cursor);

/// Read red, green and blue values of the whole window frame buffer
std::vector<float> readPixels();

} // namespace jngl
