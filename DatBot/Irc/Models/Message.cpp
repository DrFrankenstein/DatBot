#include "Message.hpp"

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <variant>

using std::get, std::holds_alternative, std::ostream, std::ostringstream, std::setfill, std::setw, std::string,
    std::uint16_t;

namespace Irc::Models
{
bool Message::isNumeric() const
{
	return holds_alternative<uint16_t>(this->command);
}

bool Message::isCommand() const
{
	return !this->isNumeric();
}

bool Message::is(uint16_t numeric) const
{
	return this->isNumeric()
	    && get<uint16_t>(this->command) == numeric;
}

bool Message::is(const string& command) const
{
	return this->isCommand()
	    && get<string>(this->command) == command;
}

string Message::toString() const
{
	ostringstream stream;
	stream << *this;
	return stream.str();
}

}

using Irc::Models::Message;

namespace
{
void renderTag(ostream& out, const string& key, const string& value)
{
	out << key;
	out << '=';
	for (auto c : value)
	{
		switch (c)
		{
		case ';': out << "\\:"; break;
		case ' ': out << "\\s"; break;
		case '\\': out << "\\\\"; break;
		case '\r': out << "\\r"; break;
		case '\n': out << "\\n"; break;
		default: out << c;
		}
	}
}

void renderTags(ostream& out, Message::Tags tags)
{
	out << '@';

	for (auto& [key, value] : tags)
		renderTag(out, key, value);
	
	out << ' ';
}

void renderPrefix(ostream& out, const string& prefix)
{
	out << ':' << prefix << ' ';
}

void renderCommand(ostream& out, Message::Command command)
{
	if (holds_alternative<uint16_t>(command))
	{
		out << setfill('0') << setw(3);
		out << get<uint16_t>(command);
	}
	else
		out << get<string>(command);
}

void renderParam(ostream& out, const string& param)
{
	out << ' ';

	if (param.find(' ') != string::npos)
		out << ':';

	out << param;
}

void renderParams(ostream& out, Message::Params params)
{
	for (auto& param : params)
		renderParam(out, param);
}

}

ostream& operator<<(ostream& out, const Irc::Models::Message& message)
{
	if (!message.tags.empty())
		renderTags(out, message.tags);

	if (!message.prefix.empty())
		renderPrefix(out, message.prefix);

	renderCommand(out, message.command);

	if (!message.params.empty())
		renderParams(out, message.params);

	return out;
}
