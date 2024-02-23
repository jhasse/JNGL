// Copyright 2012-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Job class and related functions
/// @file
#pragma once

#include <functional>
#include <memory>

namespace jngl {

/// Background job which stays part of the main loop independent of the active jngl::Work
///
/// This can be used for an achievement system for example. Override this class and use
/// jngl::addJob to register it.
class Job {
public:
	/// Advance the game logic
	///
	/// This function gets called 60 times per second or whatever has been set by
	/// setStepsPerSecond().
	virtual void step() = 0;

	/// Draw the game state
	///
	/// As it isn't garuanteed that this function is called as often as Job::step, you shouldn't
	/// change any game state in it.
	virtual void draw() const = 0;

	/// Does nothing
	Job() = default;

	/// Copy constructor
	Job(const Job&) = default;

	/// Copy assignment
	Job& operator=(const Job&) = default;

	/// Move constructor
	Job(Job&&) = default;

	/// Move assignment
	Job& operator=(Job&&) = default;

	/// Does nothing
	virtual ~Job();
};

/// Add a new Job which will be always be stepped and drawn by App::mainLoop()
void addJob(std::shared_ptr<Job> job);

/// The same as addJob(std::shared_ptr<Job>) but creates the Job for you
template <class T, class... Args>
void addJob(Args&&... args) {
	addJob(std::make_shared<T>(std::forward<Args>(args)...));
}

/// Removes the passed Job after all Jobs have been stepped
///
/// If the Job isn't found, nothing happens.
void removeJob(Job*);

/// Returns the first Job for which \a predicate returned true
std::shared_ptr<Job> getJob(const std::function<bool(Job&)>& predicate);

/// Returns the first Job that is a T
template <class T> std::shared_ptr<T> getJob() {
	return std::dynamic_pointer_cast<T>(
	    getJob([](Job& job) { return dynamic_cast<T*>(&job) != nullptr; }));
}

} // namespace jngl
