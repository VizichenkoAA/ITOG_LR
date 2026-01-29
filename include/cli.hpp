#pragma once

#include <string>
#include <optional>

struct CliOptions
{
    bool show_help{false};

    std::optional<std::string> input_path;
    std::optional<std::string> stops_path;
    std::optional<std::string> output_path;

    std::string report_type{"freq"};
    size_t top_n{20};
};

CliOptions parse_arguments(int argc, char** argv);
std::string make_help_text();


