#pragma once

#include <string>
#include <variant>
#include <vector>
#include <map>
#include <optional>
#include <stdexcept>

namespace json
{
    struct Value;

    using Object = std::map<std::string, Value>;
    using Array  = std::vector<Value>;

    struct Value
    {
        using Variant = std::variant<
            std::nullptr_t,
            bool,
            double,
            std::string,
            Array,
            Object
        >;

        Variant data;
    };

    class ParseError : public std::runtime_error
    {
    public:
        ParseError(const std::string& message, size_t position);
        size_t position() const noexcept { return m_position; }

    private:
        size_t m_position;
    };

    class Parser
    {
    public:
        explicit Parser(const std::string& text);
        Value parse();

    private:
        const std::string& m_text;
        size_t m_pos{0};

        void skip_whitespace();
        bool match(char c);
        char peek() const;
        char get();
        bool eof() const;

        Value parse_value();
        Value parse_null();
        Value parse_true();
        Value parse_false();
        Value parse_number();
        Value parse_string();
        Value parse_array();
        Value parse_object();
        std::string parse_raw_string();

        [[noreturn]] void error(const std::string& msg) const;
    };

} // namespace json


