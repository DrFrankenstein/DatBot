#pragma once

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
		throw runtime_error { str(format("config: missing parameter %1%") % key) };

	if (!item.IsScalar())
		throw runtime_error { str(format("config: parameter %1% is not a single value") % key) };

	return item.as<Result>();
}

template<typename Result, typename Key>
Result defaultScalar(YAML::Node node, Key key, Result default)
{
	using boost::format, boost::str;
	using std::runtime_error;
	using YAML::Node;

	Node item = node[key];

	if (!item)
		return default;

	if (!item.IsScalar())
		throw runtime_error { str(format("config: parameter %1% is not a single value") % key) };

	return item.as<Result>();
}

template<typename OutType, typename Key, typename OutputIterator>
void getSequence(YAML::Node node, Key key, OutputIterator out, bool required = false)
{
	using boost::format, boost::str, boost::transform;
	using std::runtime_error;
	using YAML::Node;

	Node item = node[key];

	if (required && !item)
		throw runtime_error { str(format("config: missing parameter %1%") % key) };
	else
		return;

	if (!item.IsSequence())
		throw runtime_error { str(format("config: parameter %1% is not a list") % key) };

	transform(item, out, [](YAML::Node node) { return node.as<OutType>(); });
}

}
