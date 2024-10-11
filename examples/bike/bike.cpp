#include "bike.hpp"

#include "line.hpp"

#include <jngl.hpp>

void Bike::HandleCollision(const Line& line)
{
	for (auto& wheel : wheels_) {
		line.HandleCollision(wheel);
	}
}

void Bike::DoFrame()
{
	for (auto& wheel : wheels_) {
		wheel.Move();
	}
	jngl::Vec2 connection = wheels_[1].position_ - wheels_[0].position_;
	boost::qvm::normalize(connection);

	connection = { connection.y * 0.1, connection.x * 0.1 }; // Um 90 Grad drehen und 10% Intensität

	if (jngl::keyDown(jngl::key::Right)) {
		for (auto& wheel : wheels_) {
			wheel.speed_ += jngl::Vec2(0.4, 0);
		}
		wheels_[0].speed_ += connection; // Dann hoch damit!
		wheels_[1].speed_ -= connection; // Dann hoch damit!
	}
	if (jngl::keyDown(jngl::key::Left)) {
		for (auto& wheel : wheels_) {
			wheel.speed_ -= jngl::Vec2(0.4, 0);
		}
		wheels_[0].speed_ -= connection; // Dann hoch damit!
		wheels_[1].speed_ += connection; // Dann hoch damit!
	}
	std::vector<jngl::Vec2> correction(2);
	for (size_t i = 0; i < wheels_.size(); ++i) {
		jngl::Vec2 connection = wheels_[i].position_ - wheels_[1-i].position_;
		boost::qvm::normalize(connection);
		double projection = boost::qvm::dot(connection, wheels_[i].speed_);
		correction[i] = (connection * projection);
	}
	for (size_t i = 0; i < wheels_.size(); ++i) {
 		wheels_[i].speed_ -= correction[i];
		wheels_[i].speed_ += (correction[i] + correction[1-i]) / 2;
	}
	for (auto& wheel : wheels_) {
		wheel.position_ += wheel.speed_; // Move
	}
	connection = wheels_[1].position_ - wheels_[0].position_;
	boost::qvm::normalize(connection);
	wheels_[1].position_ =  wheels_[0].position_ + connection * 128;
}

Bike::Bike()
{
	wheels_.emplace_back(100, 100);
	wheels_.emplace_back(228, 100);
	wheels_[0].otherWheel_ = &wheels_[1];
	wheels_[1].otherWheel_ = &wheels_[0];
}

void Bike::draw() const {
	for (auto& wheel : wheels_) {
		wheel.draw();
	}
}
