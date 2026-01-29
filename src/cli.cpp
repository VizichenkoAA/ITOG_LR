#include "cli.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

CliOptions parse_arguments(int argc, char** argv)
{
    CliOptions opts;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h")
        {
            opts.show_help = true;
        }
        else if (arg == "--input" && i + 1 < argc)
        {
            opts.input_path = argv[++i];
        }
        else if (arg == "--stops" && i + 1 < argc)
        {
            opts.stops_path = argv[++i];
        }
        else if (arg == "--output" && i + 1 < argc)
        {
            opts.output_path = argv[++i];
        }
        else if (arg == "--report" && i + 1 < argc)
        {
            opts.report_type = argv[++i];
        }
        else if (arg == "--top" && i + 1 < argc)
        {
            opts.top_n = static_cast<size_t>(std::stoul(argv[++i]));
        }
        else
        {
            std::ostringstream oss;
            oss << "Неизвестный или некорректный аргумент: " << arg
                << "\nИспользуйте --help для справки.";
            throw std::runtime_error(oss.str());
        }
    }

    return opts;
}

std::string make_help_text()
{
    std::ostringstream out;
    out << "Частотный анализ текста (итоговая лабораторная)\n\n";
    out << "Использование:\n";
    out << "  textfreq_cli --input <file.json> --stops <stops.json> --report freq [--top N] [--output out.txt]\n\n";
    out << "Параметры:\n";
    out << "  --help, -h        Показать эту справку.\n";
    out << "  --input PATH      Входной JSON с текстом ({\"text\":\"...\"} или массив параграфов).\n";
    out << "  --stops PATH      JSON со списком стоп-слов (массив строк или объектов {\"stop\":\"...\"}).\n";
    out << "  --report TYPE     Тип отчёта. На данный момент поддерживается только 'freq'.\n";
    out << "  --top N           Количество слов в топе по частоте (по умолчанию 20).\n";
    out << "  --output PATH     Путь к файлу для сохранения отчёта (если не указан, вывод в консоль).\n\n";
    out << "Примеры:\n";
    out << "  textfreq_cli --input data/sample_text.json --stops data/stopwords.json --report freq --top 20\n";
    out << "  textfreq_cli --input data/large_text.json --stops data/stopwords.json --report freq --top 50 --output report.txt\n";
    return out.str();
}


