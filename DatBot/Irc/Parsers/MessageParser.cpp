#include "MessageParser.hpp"
#include "../Models/Message.hpp"

#include <string>
#include <cstdint>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_numeric.hpp>

namespace Irc { namespace Parsers {

using Irc::Models::Message;
using Iterator = std::string::const_iterator;
namespace qi = boost::spirit::qi;

std::string escapeTagValue(const std::string& value)
{
    std::string escaped;

    for (char c : value)
    {   // <http://ircv3.net/specs/core/message-tags-3.2.html>
        switch (c)
        {
        case ';':
            escaped.append("\\:");
            break;

        case ' ':
            escaped.append("\\s");
            break;

        case '\\':
            escaped.append("\\\\");
            break;

        case '\r':
            escaped.append("\\r");
            break;

        case '\n':
            escaped.append("\\n");
            break;

        default:
            escaped.push_back(c);
        }
    }

    return escaped;
}

static bool isLineBreak(char c)
{
    return c == '\r' || c == '\n';
}

static bool isSpCrLfCl(char c)
{
    return isLineBreak(c) || c == ' ' || c == ':';
}

static bool parseParam(Iterator& it, const Iterator& end, bool trailing, Message& message)
{   // [rfc2812]
    // middle     =  nospcrlfcl *( ":" / nospcrlfcl )
    // trailing   =  *( ":" / " " / nospcrlfcl )

    if (!trailing && (it == end || isSpCrLfCl(*it)))
        // middle parameters cannot be empty but trailing ones can. this might be a quirk in the RFC
        return false;

    Iterator start = it;
    while (it != end && (trailing || *it != ' ') && !isLineBreak(*it))
        ++it;

    message.params.push_back({ start, it });

    return true;
}

static bool parseParams(Iterator& it, const Iterator& end, Message& message)
{   // [rfc2812]
    // params =  *14( SPACE middle ) [ SPACE ":" trailing ]
    //        =/ 14( SPACE middle ) [ SPACE [ ":" ] trailing ]

    unsigned char paramCount;
    for (paramCount = 1; it != end && paramCount < 15; ++paramCount)
    {
        if (*it != ' ')
            break;  // no space, no parameter
        if (++it == end)
            return false;
        if (*it == ':')
            // ':' introduces a "trailing parameter that allows spaces
            return parseParam(++it, end, true, message);

        if (!parseParam(it, end, false, message))
            return false;   // expected a valid parameter after space
    }

    if (it != end && *it == ' ')
    {   // 15th parameter, rest of the message gets thrown in regardless of spaces
        if (*++it == ':')
            ++it;   // still consume ':' if there's one

        return parseParam(it, end, true, message);
    }

    return true;
}

static bool parseCommand(Iterator& it, const Iterator& end, Message& message)
{   // [rfc2812] command =  1*letter / 3digit
    if (it == end)
        return false;

    std::uint16_t numeric;
    qi::uint_parser<std::uint16_t, 10, 3, 3> numParser;
    if (qi::parse(it, end, numParser, numeric))
    {
        message.command = numeric;
    }
    else
    {
        Iterator start = it;
        while (it != end && std::isalpha(*it))
            ++it;

        if (it == start)
            return false;   // empty command, or number doesn't have 3 digits

        message.command = std::string { start, it };
    }

    return true;
}

static bool parsePrefix(Iterator& it, const Iterator& end, Message& message)
{   // [rfc2812] prefix =  servername / ( nickname [ [ "!" user ] "@" host ] )
    // deviation: we read everything until space. the address will be parsed later if needed.
    
    if (it == end || *it == ' ')
        return false;   // no prefix after ':'

    Iterator start = it;
    while (it != end && *it != ' ')
        ++it;

    message.prefix = { start, it };

    return true;
}

static bool parseTagValue(Iterator& it, const Iterator& end, std::string& value)
{   // [ircv3.2] <escaped value> ::= <sequence of any characters except NUL, CR, LF, semicolon (`;`) and SPACE>
    for (; it != end; ++it)
    {
        char c = *it;

        if (isLineBreak(c) || c == '\0' || c == ';' || c == ' ')
            break;

        if (c == '\\')
        {   // <http://ircv3.net/specs/core/message-tags-3.2.html> under "Escaping Values"
            char escape = *it++;
            switch (escape)
            {
            case ':':
                c = ';';
                break;

            case 's':
                c = ' ';
                break;

            case '\\':
                break;

            case 'r':
                c = '\r';
                break;

            case 'n':
                c = '\n';
                break;

            default:
                return false;   // invalid escape sequence
            }
        }

        value += c;
    }

    return true;
}

static bool parseTagKey(Iterator& it, const Iterator& end, std::string& key)
{   // [ircv3.2]
    // <key>    ::= [ <vendor> '/' ] <sequence of letters, digits, hyphens (`-`)>
    // <vendor> ::= <host>
    // deviation: in this case, we parse <host> as a sequence of letters,
    //   digits, hyphens, periods and colons. we don't check if it's well-formed.

    bool hasVendor = false;
    bool mustHaveVendor = false;

    Iterator start;
    for (start = it; it != end; ++it)
    {
        char c = *it;

        if (c != '-' && !std::isalnum(c))
        {
            if (c == '.' || c == ':')
            {
                if (!hasVendor)
                    mustHaveVendor = true;
                else
                    break;
            }
            else if (c == '/')
            {
                if (!hasVendor)
                    hasVendor = true;
                else
                    return false;   // slash may only appear once
            }
            else
                break;
        }
    }

    if (mustHaveVendor && !hasVendor)
        return false;   // invalid period or colon in key

    key = { start, it };
    return true;
}

static bool parseTag(Iterator& it, const Iterator& end, Message& message)
{   // [ircv3.2] <tag> ::= <key> ['=' <escaped value>]
    std::string key, value;

    if (it == end)
        return false;   // expected a tag, not end

    if (!parseTagKey(it, end, key))
        return false;

    if (it == end || (*it == '=' && !parseTagValue(++it, end, value)))
        return false;

    message.tags.insert({ key, value });
    return true;
}

static bool parseTags(Iterator& it, const Iterator& end, Message& message)
{   // [ircv3.2] <tags> ::= <tag> [';' <tag>]*
    if (it == end)
        return false;   // nothing after '@'

    if (!parseTag(it, end, message))    // first tag
        return false;   // invalid tag after '@'

    while (*it == ';')
        // more tags
        if (!parseTag(++it, end, message))
            return false;   // invalid tag after ';'

    return true;
}

bool parseMessage(const std::string& input, Message& message)
{
    Iterator it = input.cbegin(), end = input.cend();

     // [ircv3.2] <message> ::= ['@' <tags> <SPACE>] [':' <prefix> <SPACE> ] <command> <params> <crlf>
    if (it == end)
        return false;   // nothing to parse

    if (*it == '@')
    {
        if (!parseTags(++it, end, message))
            return false;   // no valid tags after '@'
        if (it == end || *it++ != ' ')
            return false;   // missing space after tags
    }

    if (*it == ':')
    {
        if (!parsePrefix(++it, end, message))
            return false;   // no valid prefix after ':'

        if (it == end || *it++ != ' ')
            return false;   // missing space after prefix
    }

    if (!parseCommand(it, end, message))
        return false;   // failed to parse command

    if (it != end && !parseParams(it, end, message))
        return false;   // failed to parse params

    if (!isLineBreak(*it++))
        return false;   // unexpected character

    return true;
}

}}
