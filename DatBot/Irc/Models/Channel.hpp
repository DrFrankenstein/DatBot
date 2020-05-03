#pragma once

#include <string>

namespace Irc::Models
{
struct Channel
{
	std::string name;

	// ???: do we want Channel objects to be single-sources-of-truth
	// and live on the heap?

	//Channel(const Channel&) = delete;
	//Channel(const Channel&&) = delete;
	//Channel& operator=(const Channel&) = delete;
	//Channel& operator=(const Channel&&) = delete;
};

}
