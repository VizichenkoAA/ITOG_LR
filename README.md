## Итоговая лабораторная работа — Частотный анализ текста (C++)

Консольное приложение выполняет частотный анализ текста, хранящегося в пользовательском JSON.
Поддерживаются:

- загрузка и валидация JSON ({ "text": "..." } или массив параграфов),
- загрузка списка стоп-слов,
- подсчёт количества слов, предложений, уникальных слов,
- топ наиболее частотных слов (с флагом `--top`),
- перерасчёт статистики без стоп-слов,
- распределение слов по длине,
- формирование человекочитаемого отчёта (`--report freq`).

### Структура проекта

- `src\` — исходный код C++ (`.cpp`)
- `include\` — заголовочные файлы (`.h/.hpp`)
- `tests\` — самотесты и бенчмарки
- `data\` — примеры входных JSON (минимальные, ошибочные и «крупные»)
- `docs\` — Implementation Plan, основной отчёт, bench-отчёт, лог тестов и пример вывода
- `scripts\` — вспомогательные скрипты (генерация до 100000 JSON-файлов)

### Сборка (CMake)

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Будут собраны:

- `textfreq_cli.exe` — основное консольное приложение,
- `textfreq_tests.exe` — самотесты и бенчмарк.

### Запуск

Примеры:

```bash
Debug\textfreq_cli.exe --help

Debug\textfreq_cli.exe --input ../data/sample_text.json --stops ../data/stopwords.json --report freq --top 20
```

Результат выводится в консоль в виде форматированных таблиц и сводки.
Поддерживается вывод в файл через флаг:

```bash
Debug\textfreq_cli.exe --input ../data/sample_text.json ^
                --stops ../data/stopwords.json ^
                --report freq --top 20 ^
                --output ../data/report.txt
```

Более подробное описание формата JSON и сценария использования приведено в `docs/`.

### Данные и генерация больших наборов

- Минимальные примеры:
  - `data\sample_text.json` — один текст в поле `text`;
  - `data\paragraphs_array.json` — массив абзацев в поле `paragraph`;
  - `data\stopwords.json` — список стоп-слов.
- Ошибочные примеры для негативных тестов:
  - `data\invalid_missing_text.json`, `data\invalid_broken_json.json`, `data\invalid_wrong_type.json`.
- Крупный набор для бенчмарков:
  - скрипт `scripts\generate_json.py` создаёт до 100000 JSON-файлов в `data\generated\`.


