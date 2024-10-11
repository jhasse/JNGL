// Copyright 2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/Vec2.hpp"

#include <boost/ut.hpp>

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut; // NOLINT

	"Vec2"_test = [] {
		//
		//  o-->
		//
		jngl::Vec2 a{ 1., 0. };

		expect(eq(std::format("{}", a), std::string("[x=1, y=0]")));

		a.rotate(M_PI / 2);
		//
		//  o
		//  |
		//  v
		//
		expect(approx(a.x, 0., 1e-6));
		expect(approx(a.y, 1., 1e-6)); // JNGL uses negative y for up and positive for down.

		a.rotate(-M_PI / 4);
		//
		//  o
		//   ╲
		//    ⌟
		//
		expect(approx(a.x, sqrt(2) / 2., 1e-6));
		expect(approx(a.y, sqrt(2) / 2., 1e-6));
		a.rotate(0);
		expect(approx(a.x, sqrt(2) / 2., 1e-6));
		expect(approx(a.y, sqrt(2) / 2., 1e-6));
		a.rotate(M_PI * 8);
		expect(approx(a.x, sqrt(2) / 2., 1e-6));
		expect(approx(a.y, sqrt(2) / 2., 1e-6));
	};
};
} // namespace
