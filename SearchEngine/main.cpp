#include <iostream>
#include "converter_json.h"
#include "inverted_index.h"
#include "search_server.h"

int main() {

    system("chcp 1251");

    try {
        std::cout << "=== Поисковый движок ===" << std::endl;
        std::cout << "Запуск приложения..." << std::endl;

        ConverterJSON converter;
        Config config = converter.getConfig();

        std::cout << "\nДвижок: " << config.name << std::endl;
        std::cout << "Версия: " << config.version << std::endl;
        std::cout << "Максимум результатов: " << config.max_responses << std::endl;
        std::cout << "Документов для индексации: " << config.files.size() << std::endl;

        auto documents = converter.getTextDocuments();

        std::cout << "\nИндексация документов..." << std::endl;
        InvertedIndex idx;
        idx.updateDocumentBase(documents);
        std::cout << "Индексация завершена" << std::endl;

        auto requests = converter.getRequests();
        std::cout << "\nЗагружено запросов: " << requests.size() << std::endl;

        std::cout << "\nВыполнение поиска..." << std::endl;
        SearchServer searchServer(idx);
        auto results = searchServer.search(requests);

        std::vector<std::vector<std::pair<int, float>>> answers;
        for (const auto& result : results) {
            std::vector<std::pair<int, float>> answer;
            for (const auto& rel : result) {
                answer.push_back({ static_cast<int>(rel.doc_id), rel.rank });
            }
            answers.push_back(answer);
        }

        converter.putAnswers(answers);
        std::cout << "Результаты сохранены в answers.json" << std::endl;

        std::cout << "\nПоиск завершён успешно!" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "\nОшибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}