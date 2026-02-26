# Поисковый движок

Консольное приложение для поиска по текстовым документам с ранжированием результатов.

## Стек технологий

- C++17
- CMake
- Google Test (опционально)
- Библиотека JSON (nlohmann/json)
- Visual Studio 2022

## Структура проекта
SearchEngine/
├── CMakeLists.txt (корневой)
├── SearchEngine/ # Основная программа
│ ├── CMakeLists.txt
│ ├── main.cpp
│ ├── converter_json.h/.cpp
│ ├── inverted_index.h/.cpp
│ ├── search_server.h/.cpp
│ └── json.hpp
├── SearchEngine_tests/ # Тесты (опционально)
│ ├── CMakeLists.txt
│ └── test_main.cpp
├── docs/ # Документы для поиска
│ ├── doc1.txt
│ ├── doc2.txt
│ └── doc3.txt
├── config.json
├── requests.json
└── README.md

text

## Сборка в Visual Studio 2022

1. Откройте проект: **Файл** → **Открыть** → **CMake** → выберите корневой `CMakeLists.txt`
2. Выберите конфигурацию: `x64-Debug` или `x64-Release`
3. Соберите проект: **Сборка** → **Собрать все** (Ctrl+Shift+B)
4. Запустите: **Отладка** → **Запуск без отладки** (Ctrl+F5)

## Конфигурационные файлы

### config.json
```json
{
    "config": {
        "name": "SearchEngine",
        "version": "0.1",
        "max_responses": 5
    },
    "files": [
        "docs/doc1.txt",
        "docs/doc2.txt",
        "docs/doc3.txt"
    ]
}
requests.json
json
{
    "requests": [
        "информационные технологии",
        "спорт победа",
        "политика экономика"
    ]
}
Функциональность
Многопоточная индексация документов

Поиск документов по всем словам запроса

Ранжирование результатов по релевантности

Ограничение количества результатов

Детальная обработка ошибок

Модульные тесты (Google Test) — опционально

Автор
Ионов Н.А.
