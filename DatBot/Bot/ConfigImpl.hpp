#pragma once

#include "Config.hpp"

#include <boost/format.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <exception>
#include <functional>
#include <string>
#include <yaml-cpp/yaml.h>

namespace Bot::Config
{
template<typename Result, typename Key>
Result expectScalar(YAML::Node node, Key key)
{
	using boost::format, boost::str;
	using std::runtime_error;
	using YAML::Node;

	Node item = node[key];

	if (!item)
	{
		format fmt { "config: missing parameter %1%" };
		throw runtime_error { str(fmt % key) };
	}

	if (!item.IsScalar())
	{
		format fmt { "config: parameter %1% is not a single value" };
		throw runtime_error { str(fmt % key) };
	}

	return item.as<Result>();
}

template<typename Result, typename Key>
Result defaultScalar(YAML::Node node, Key key, Result defaultval)
{
	using boost::format, boost::str;
	using std::runtime_error;
	using YAML::Node;

	Node item = node[key];

	if (!item)
		return defaultval;

	if (!item.IsScalar())
	{
		format fmt { "config: parameter %1% is not a single value" };
		throw runtime_error { str(fmt % key) };
	}

	return item.as<Result>();
}

template<typename OutType, typename Key, typename OutputIterator>
void getSequence(YAML::Node node, Key key, OutputIterator out, bool required)
{
	using boost::format, boost::str, boost::transform;
	using std::runtime_error;
	using YAML::Node;

	Node item = node[key];

	if (required && !item)
	{
		format fmt { "config: missing parameter %1%" };
		throw runtime_error { str(fmt % key) };
	}

	if (!item.IsSequence())
	{
		format fmt { "config: parameter %1% is not a list" };
		throw runtime_error { str(fmt % key) };
	}

	transform(item, out, [](YAML::Node node) { return node.as<OutType>(); });
}
}
