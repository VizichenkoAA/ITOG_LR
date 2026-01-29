#include "cli.hpp"
#include "json_parser.hpp"
#include "text_analyzer.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

namespace
{
    std::string read_file(const std::string& path)
    {
        std::ifstream in(path, std::ios::binary);
        if (!in)
        {
            throw std::runtime_error("Не удалось открыть файл: " + path);
        }
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }

    void write_file_with_confirm(const std::string& path, const std::string& content)
    {
        namespace fs = std::filesystem;

        bool exists = fs::exists(path);
        if (exists)
        {
            std::cout << "Файл \"" << path << "\" уже существует. Перезаписать? [y/n]: ";
            char answer = 'n';
            if (!(std::cin >> answer))
            {
                throw std::runtime_error("Не удалось прочитать ответ пользователя при подтверждении перезаписи.");
            }
            if (answer != 'y' && answer != 'Y')
            {
                throw std::runtime_error("Перезапись файла отменена пользователем.");
            }
        }

        std::ofstream out(path, std::ios::binary);
        if (!out)
        {
            throw std::runtime_error("Не удалось открыть файл для записи: " + path);
        }
        out << content;
    }
}

int main(int argc, char** argv)
{
    try
    {
        CliOptions opts = parse_arguments(argc, argv);

        if (opts.show_help || !opts.input_path)
        {
            std::cout << make_help_text() << std::endl;
            return 0;
        }

        if (opts.report_type != "freq")
        {
            std::cerr << "Поддерживается только отчёт '--report freq'." << std::endl;
            return 1;
        }

        std::string input_json = read_file(*opts.input_path);

        auto t_start_parse = std::chrono::high_resolution_clock::now();
        json::Parser parser(input_json);
        json::Value root = parser.parse();
        auto t_end_parse = std::chrono::high_resolution_clock::now();

        std::vector<std::string> blocks = extract_text_blocks(root);
        if (blocks.empty())
        {
            std::cerr << "Во входном JSON не найден текст (\"text\" или массив параграфов)." << std::endl;
            return 1;
        }

        std::vector<std::string> stopwords;
        if (opts.stops_path)
        {
            std::string stops_json = read_file(*opts.stops_path);
            json::Parser sp(stops_json);
            json::Value sroot = sp.parse();
            stopwords = extract_stopwords(sroot);
        }

        auto t_start_analyze = std::chrono::high_resolution_clock::now();
        TextStats stats = analyze_text(blocks, stopwords);
        auto t_end_analyze = std::chrono::high_resolution_clock::now();

        std::string report = format_report(stats, opts.top_n);

        auto parse_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t_end_parse - t_start_parse).count();
        auto analyze_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t_end_analyze - t_start_analyze).count();

        std::ostringstream with_timing;
        with_timing << report << "\n";
        with_timing << "Время парсинга JSON: " << parse_ms << " мс\n";
        with_timing << "Время анализа текста: " << analyze_ms << " мс\n";

        if (opts.output_path)
        {
            write_file_with_confirm(*opts.output_path, with_timing.str());
            std::cout << "Отчёт сохранён в файл: " << *opts.output_path << std::endl;
        }
        else
        {
            std::cout << with_timing.str() << std::endl;
        }

        return 0;
    }
    catch (const json::ParseError& e)
    {
        std::cerr << "Ошибка парсинга JSON: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
}


