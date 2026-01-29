#include "json_parser.hpp"
#include "text_analyzer.hpp"

#include <chrono>
#include <iostream>

int main()
{
    try
    {
        // Простой самотест парсера JSON (валидный)
        {
            std::string sample = R"({"text": "Hello world. Hello C++!"})";
            json::Parser p(sample);
            json::Value root = p.parse();
            auto blocks = extract_text_blocks(root);
            if (blocks.size() != 1)
            {
                std::cerr << "Самотест: неверное количество текстовых блоков\n";
                return 1;
            }
        }

        // Негативный тест: синтаксически испорченный JSON
        {
            std::string broken = R"({"text": "broken json" "missing_comma": true})";
            try
            {
                json::Parser p(broken);
                (void)p.parse();
                std::cerr << "Самотест: ожидалась ошибка парсинга для испорченного JSON\n";
                return 1;
            }
            catch (const json::ParseError&)
            {
                // ожидаемое поведение
            }
        }

        // Негативный тест: корректный JSON, но нет текста
        {
            std::string no_text = R"({"title": "no text here"})";
            json::Parser p(no_text);
            json::Value root = p.parse();
            auto blocks = extract_text_blocks(root);
            if (!blocks.empty())
            {
                std::cerr << "Самотест: ожидалось отсутствие текстовых блоков\n";
                return 1;
            }
        }

        // Самотест анализа текста
        {
            std::vector<std::string> blocks = { "Hello world. Hello C++!" };
            std::vector<std::string> stops = { "hello" };
            TextStats stats = analyze_text(blocks, stops);
            if (stats.total_words == 0 || stats.word_freq.empty())
            {
                std::cerr << "Самотест: анализ текста не дал результатов\n";
                return 1;
            }
        }

        // Простейший бенчмарк: анализ одного большого текста
        {
            std::string big_text(100000, 'a');
            std::vector<std::string> blocks = { big_text };
            std::vector<std::string> stops;

            auto start = std::chrono::high_resolution_clock::now();
            TextStats stats = analyze_text(blocks, stops);
            auto end = std::chrono::high_resolution_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            std::cout << "[bench] Анализ текста длиной " << big_text.size()
                      << " символов занял " << ms << " мс\n";
        }

        // Бенчмарк, имитирующий обработку большого количества файлов (до 100000)
        {
            std::string text = "word1 word2 word3 word4 word5.";
            std::vector<std::string> stops = { "word1", "word2" };

            const size_t iterations = 100000; // имитация 100000 файлов
            auto start = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < iterations; ++i)
            {
                std::vector<std::string> blocks = { text };
                (void)analyze_text(blocks, stops);
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            std::cout << "[bench] Имитация анализа " << iterations
                      << " небольших текстов заняла " << ms << " мс\n";
        }

        std::cout << "Самотесты успешно пройдены.\n";
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Исключение в тестах: " << e.what() << "\n";
        return 1;
    }
}


