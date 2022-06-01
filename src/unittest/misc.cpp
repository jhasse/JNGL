// Copyright 2018-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/Finally.hpp"
#include "../jngl/other.hpp"
#include "../jngl/sprite.hpp"

#include <boost/test/unit_test.hpp>
#include <filesystem>

BOOST_AUTO_TEST_CASE(FinallyTest) {
	bool called = false;
	{
		jngl::Finally _([&called]() {
			BOOST_CHECK(!called);
			called = true;
		});
		BOOST_CHECK(!called);
	}
	BOOST_CHECK(called);
	{ // don't crash
		jngl::Finally _(nullptr);
	}
	called = false;
	{
		jngl::Finally f1([&called]() {
			BOOST_CHECK(!called);
			called = true;
		});
		jngl::Finally f2 = std::move(f1);
		jngl::Finally f3(std::move(f2));
		{
			jngl::Finally f4(nullptr);
			f4 = std::move(f3);
			f3 = std::move(f4);
		}
		BOOST_CHECK(!called);
	}
	BOOST_CHECK(called);
}

BOOST_AUTO_TEST_CASE(halfLoadTest) {
	std::error_code err;
	std::filesystem::current_path("data", err);
	if (err) {
		std::filesystem::current_path("../data", err); // move out of build/bin folder
		if (err) {
			std::filesystem::current_path("../../data", err); // move out of build/Debug folder
			if (err) {
				std::filesystem::current_path("../../../data",
				                              err); // move out of out\build\x64-Debug
			}
		}
	}
	BOOST_CHECK_EQUAL(jngl::getWidth("jngl"), 600);
	BOOST_CHECK_EQUAL(jngl::getHeight("jngl"), 300);
	BOOST_CHECK_THROW(jngl::load("jngl"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(getBinaryPath) {
	BOOST_CHECK(!jngl::getBinaryPath().empty());
}

BOOST_AUTO_TEST_CASE(readAsset) {
	BOOST_CHECK(!jngl::readAsset("non existing file"));
	BOOST_CHECK_THROW(jngl::readAsset("/some/absolute/path"), std::runtime_error);
}
