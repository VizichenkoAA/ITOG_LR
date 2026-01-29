#pragma once

#include "json_parser.hpp"

#include <string>
#include <map>
#include <vector>

struct TextStats
{
    size_t total_words{0};
    size_t total_sentences{0};
    size_t unique_words{0};

    std::map<std::string, size_t> word_freq;
    std::map<std::string, size_t> word_freq_no_stops;
    std::map<size_t, size_t> length_distribution;
};

std::vector<std::string> extract_text_blocks(const json::Value& root);
std::vector<std::string> extract_stopwords(const json::Value& root);

TextStats analyze_text(const std::vector<std::string>& blocks,
                       const std::vector<std::string>& stopwords);

std::string format_report(const TextStats& stats, size_t top_n);


