#include "json_parser.hpp"

#include <cctype>
#include <stdexcept>
#include <sstream>

namespace json
{
    ParseError::ParseError(const std::string& message, size_t position)
        : std::runtime_error(message)
        , m_position(position)
    {
    }

    Parser::Parser(const std::string& text)
        : m_text(text)
    {
    }

    Value Parser::parse()
    {
        skip_whitespace();
        Value v = parse_value();
        skip_whitespace();
        if (!eof())
        {
            error("Unexpected characters after JSON value");
        }
        return v;
    }

    void Parser::skip_whitespace()
    {
        while (!eof() && std::isspace(static_cast<unsigned char>(m_text[m_pos])))
            ++m_pos;
    }

    bool Parser::match(char c)
    {
        if (!eof() && m_text[m_pos] == c)
        {
            ++m_pos;
            return true;
        }
        return false;
    }

    char Parser::peek() const
    {
        return eof() ? '\0' : m_text[m_pos];
    }

    char Parser::get()
    {
        if (eof())
            error("Unexpected end of input");
        return m_text[m_pos++];
    }

    bool Parser::eof() const
    {
        return m_pos >= m_text.size();
    }

    Value Parser::parse_value()
    {
        char c = peek();
        if (c == 'n') return parse_null();
        if (c == 't') return parse_true();
        if (c == 'f') return parse_false();
        if (c == '"') return parse_string();
        if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parse_number();
        if (c == '[') return parse_array();
        if (c == '{') return parse_object();
        error("Unexpected character while parsing value");
    }

    Value Parser::parse_null()
    {
        const std::string expected = "null";
        for (char ch : expected)
        {
            if (get() != ch)
                error("Invalid literal 'null'");
        }
        return Value{std::nullptr_t{}};
    }

    Value Parser::parse_true()
    {
        const std::string expected = "true";
        for (char ch : expected)
        {
            if (get() != ch)
                error("Invalid literal 'true'");
        }
        return Value{true};
    }

    Value Parser::parse_false()
    {
        const std::string expected = "false";
        for (char ch : expected)
        {
            if (get() != ch)
                error("Invalid literal 'false'");
        }
        return Value{false};
    }

    Value Parser::parse_number()
    {
        size_t start = m_pos;
        if (match('-')) {}
        if (!eof() && peek() == '0')
        {
            get();
        }
        else
        {
            if (!std::isdigit(static_cast<unsigned char>(peek())))
                error("Invalid number");
            while (!eof() && std::isdigit(static_cast<unsigned char>(peek())))
                get();
        }

        if (!eof() && peek() == '.')
        {
            get();
            if (!std::isdigit(static_cast<unsigned char>(peek())))
                error("Invalid number after decimal point");
            while (!eof() && std::isdigit(static_cast<unsigned char>(peek())))
                get();
        }

        if (!eof() && (peek() == 'e' || peek() == 'E'))
        {
            get();
            if (peek() == '+' || peek() == '-')
                get();
            if (!std::isdigit(static_cast<unsigned char>(peek())))
                error("Invalid exponent");
            while (!eof() && std::isdigit(static_cast<unsigned char>(peek())))
                get();
        }

        std::string number_str = m_text.substr(start, m_pos - start);
        double value{};
        try
        {
            value = std::stod(number_str);
        }
        catch (...)
        {
            error("Failed to convert number");
        }
        return Value{value};
    }

    std::string Parser::parse_raw_string()
    {
        if (!match('"'))
            error("Expected '\"' at beginning of string");

        std::string result;
        while (!eof())
        {
            char c = get();
            if (c == '"')
                break;
            if (c == '\\')
            {
                if (eof())
                    error("Unfinished escape sequence");
                char esc = get();
                switch (esc)
                {
                case '"': result.push_back('"'); break;
                case '\\': result.push_back('\\'); break;
                case '/': result.push_back('/'); break;
                case 'b': result.push_back('\b'); break;
                case 'f': result.push_back('\f'); break;
                case 'n': result.push_back('\n'); break;
                case 'r': result.push_back('\r'); break;
                case 't': result.push_back('\t'); break;
                default:
                    error("Unsupported escape sequence");
                }
            }
            else
            {
                result.push_back(c);
            }
        }
        return result;
    }

    Value Parser::parse_string()
    {
        return Value{parse_raw_string()};
    }

    Value Parser::parse_array()
    {
        if (!match('['))
            error("Expected '['");
        skip_whitespace();
        Array arr;
        if (match(']'))
            return Value{arr};
        while (true)
        {
            skip_whitespace();
            arr.push_back(parse_value());
            skip_whitespace();
            if (match(']'))
                break;
            if (!match(','))
                error("Expected ',' or ']'");
        }
        return Value{arr};
    }

    Value Parser::parse_object()
    {
        if (!match('{'))
            error("Expected '{'");
        skip_whitespace();
        Object obj;
        if (match('}'))
            return Value{obj};
        while (true)
        {
            skip_whitespace();
            if (peek() != '"')
                error("Expected string key");
            std::string key = parse_raw_string();
            skip_whitespace();
            if (!match(':'))
                error("Expected ':' after key");
            skip_whitespace();
            Value val = parse_value();
            obj.emplace(std::move(key), std::move(val));
            skip_whitespace();
            if (match('}'))
                break;
            if (!match(','))
                error("Expected ',' or '}'");
        }
        return Value{obj};
    }

    [[noreturn]] void Parser::error(const std::string& msg) const
    {
        std::ostringstream oss;
        oss << "JSON parse error at position " << m_pos << ": " << msg;
        throw ParseError(oss.str(), m_pos);
    }

} // namespace json


