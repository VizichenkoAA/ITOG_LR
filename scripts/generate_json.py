import json
import os
import random
import string


def random_word(min_len=3, max_len=10):
    letters = string.ascii_lowercase
    length = random.randint(min_len, max_len)
    return "".join(random.choice(letters) for _ in range(length))


def random_sentence(min_words=5, max_words=15):
    words = [random_word() for _ in range(random.randint(min_words, max_words))]
    return " ".join(words).capitalize() + random.choice([".", "!", "?"])


def random_paragraph(min_sent=2, max_sent=5):
    return " ".join(random_sentence() for _ in range(random.randint(min_sent, max_sent)))


def main():
    base_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = os.path.join(base_dir, "..", "data", "generated")
    os.makedirs(data_dir, exist_ok=True)

    total_files = 100000
    error_ratio = 0.01  # 1% заведомо ошибочных файлов

    for i in range(total_files):
        path = os.path.join(data_dir, f"text_{i:05d}.json")

        # Некоторая доля файлов — ошибочные
        if random.random() < error_ratio:
            # Нарушаем либо синтаксис, либо структуру
            if random.random() < 0.5:
                content = '{ "text": "broken json without closing brace" '
            else:
                obj = {"title": "no text field here"}
                content = json.dumps(obj, ensure_ascii=False)
        else:
            # Корректный файл с ключом "text"
            paragraph = random_paragraph()
            obj = {"text": paragraph}
            content = json.dumps(obj, ensure_ascii=False)

        with open(path, "w", encoding="utf-8") as f:
            f.write(content)

    print(f"Сгенерировано {total_files} JSON-файлов в {data_dir}")


if __name__ == "__main__":
    main()


