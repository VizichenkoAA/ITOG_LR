#include "text_analyzer.hpp"

#include <algorithm>
#include <cctype>
#include <set>
#include <sstream>

namespace
{
    std::string to_lower(const std::string& s)
    {
        std::string res;
        res.reserve(s.size());
        for (unsigned char c : s)
        {
            res.push_back(static_cast<char>(std::tolower(c)));
        }
        return res;
    }

    bool is_word_char(char c)
    {
        return std::isalpha(static_cast<unsigned char>(c)) || std::isdigit(static_cast<unsigned char>(c)) || c == '\'';
    }

    bool is_sentence_end(char c)
    {
        return c == '.' || c == '!' || c == '?';
    }

    void collect_words_and_sentences(const std::vector<std::string>& blocks,
                                     std::vector<std::string>& words,
                                     size_t& sentence_count)
    {
        for (const auto& block : blocks)
        {
            bool in_word = false;
            std::string current;
            for (char ch : block)
            {
                if (is_word_char(ch))
                {
                    in_word = true;
                    current.push_back(ch);
                }
                else
                {
                    if (in_word)
                    {
                        words.push_back(to_lower(current));
                        current.clear();
                        in_word = false;
                    }
                    if (is_sentence_end(ch))
                    {
                        ++sentence_count;
                    }
                }
            }
            if (in_word && !current.empty())
            {
                words.push_back(to_lower(current));
            }
        }
    }
}

std::vector<std::string> extract_text_blocks(const json::Value& root)
{
    std::vector<std::string> blocks;
    if (std::holds_alternative<json::Object>(root.data))
    {
        const auto& obj = std::get<json::Object>(root.data);
        auto it = obj.find("text");
        if (it != obj.end() && std::holds_alternative<std::string>(it->second.data))
        {
            blocks.push_back(std::get<std::string>(it->second.data));
        }
    }
    else if (std::holds_alternative<json::Array>(root.data))
    {
        const auto& arr = std::get<json::Array>(root.data);
        for (const auto& v : arr)
        {
            if (std::holds_alternative<std::string>(v.data))
            {
                blocks.push_back(std::get<std::string>(v.data));
            }
            else if (std::holds_alternative<json::Object>(v.data))
            {
                const auto& obj = std::get<json::Object>(v.data);
                auto it = obj.find("paragraph");
                if (it != obj.end() && std::holds_alternative<std::string>(it->second.data))
                {
                    blocks.push_back(std::get<std::string>(it->second.data));
                }
            }
        }
    }
    return blocks;
}

std::vector<std::string> extract_stopwords(const json::Value& root)
{
    std::vector<std::string> stops;
    if (std::holds_alternative<json::Array>(root.data))
    {
        const auto& arr = std::get<json::Array>(root.data);
        for (const auto& v : arr)
        {
            if (std::holds_alternative<std::string>(v.data))
            {
                stops.push_back(to_lower(std::get<std::string>(v.data)));
            }
            else if (std::holds_alternative<json::Object>(v.data))
            {
                const auto& obj = std::get<json::Object>(v.data);
                auto it = obj.find("stop");
                if (it != obj.end() && std::holds_alternative<std::string>(it->second.data))
                {
                    stops.push_back(to_lower(std::get<std::string>(it->second.data)));
                }
            }
        }
    }
    return stops;
}

TextStats analyze_text(const std::vector<std::string>& blocks,
                       const std::vector<std::string>& stopwords)
{
    TextStats stats;

    std::vector<std::string> words;
    collect_words_and_sentences(blocks, words, stats.total_sentences);
    stats.total_words = words.size();

    std::set<std::string> stopset(stopwords.begin(), stopwords.end());
    std::set<std::string> uniq;

    for (const auto& w : words)
    {
        ++stats.word_freq[w];
        if (!stopset.count(w))
        {
            ++stats.word_freq_no_stops[w];
        }
        ++stats.length_distribution[w.size()];
        uniq.insert(w);
    }

    stats.unique_words = uniq.size();
    return stats;
}

std::string format_report(const TextStats& stats, size_t top_n)
{
    std::ostringstream out;

    out << "=== Частотный анализ текста ===\n\n";
    out << "Всего слов: " << stats.total_words << "\n";
    out << "Всего предложений: " << stats.total_sentences << "\n";
    out << "Уникальных слов: " << stats.unique_words << "\n\n";

    std::vector<std::pair<std::string, size_t>> freq(stats.word_freq.begin(), stats.word_freq.end());
    std::sort(freq.begin(), freq.end(),
              [](const auto& a, const auto& b) {
                  if (a.second != b.second)
                      return a.second > b.second;
                  return a.first < b.first;
              });

    out << "Топ " << top_n << " слов (все слова):\n";
    out << "----------------------------------------\n";
    out << "Слово                 | Частота\n";
    out << "----------------------------------------\n";
    size_t printed = 0;
    for (const auto& p : freq)
    {
        if (printed >= top_n) break;
        out << p.first << std::string(22 - std::min<size_t>(p.first.size(), 22), ' ')
            << "| " << p.second << "\n";
        ++printed;
    }
    out << "----------------------------------------\n\n";

    std::vector<std::pair<std::string, size_t>> freq_ns(stats.word_freq_no_stops.begin(),
                                                        stats.word_freq_no_stops.end());
    std::sort(freq_ns.begin(), freq_ns.end(),
              [](const auto& a, const auto& b) {
                  if (a.second != b.second)
                      return a.second > b.second;
                  return a.first < b.first;
              });

    out << "Топ " << top_n << " слов (без стоп-слов):\n";
    out << "----------------------------------------\n";
    out << "Слово                 | Частота\n";
    out << "----------------------------------------\n";
    printed = 0;
    for (const auto& p : freq_ns)
    {
        if (printed >= top_n) break;
        out << p.first << std::string(22 - std::min<size_t>(p.first.size(), 22), ' ')
            << "| " << p.second << "\n";
        ++printed;
    }
    out << "----------------------------------------\n\n";

    out << "Распределение по длине слов:\n";
    out << "-----------------------------\n";
    out << "Длина | Кол-во слов\n";
    out << "-----------------------------\n";
    for (const auto& p : stats.length_distribution)
    {
        out << p.first << "     | " << p.second << "\n";
    }
    out << "-----------------------------\n";

    return out.str();
}


