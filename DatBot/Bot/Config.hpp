#pragma once

#include <yaml-cpp/yaml.h>

namespace Bot::Config
{
template<typename Result, typename Key>
Result expectScalar(YAML::Node node, Key key);

template<typename Result, typename Key>
Result defaultScalar(YAML::Node node, Key key, Result defaultval);

template<typename OutType, typename Key, typename OutputIterator>
void getSequence(YAML::Node node, Key key, OutputIterator out, bool required = false);
}

#include "ConfigImpl.hpp"
