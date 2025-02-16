#include "json_reader.h"
#include "request_handler.h"

#include <fstream>

int main() {

    std::fstream input("input.json");
    if (!input) { 
        std::cerr << "Error: unable to open input file." << std::endl;
    }

    std::fstream output("output.xml");
    if (!output) {
        std::cerr << "Error: unable to open output file." << std::endl;
    }

    // Создание объекта JsonReader для загрузки и обработки JSON 
    JsonReader json_doc(input);
    input.close();

    // Создание объекта каталога для хранения информации о транспорте
    transport::Catalogue catalogue;
    // Заполнение каталога данными о остановках и маршрутах из JSON
    json_doc.FillCatalogue(catalogue);
    
    // Получение статистических запросов и настроек рендеринга из JSON-документа
    const auto& stat_requests = json_doc.GetStatRequests();
    const auto& render_settings = json_doc.GetRenderSettings().AsDict();
    
    // Создание настроек рендеринга карты на основе данных из JSON
    const auto& renderer = json_doc.FillRenderSettings(render_settings);
    
    // Получение настроек маршрутизации из JSON-документа
    const auto& routing_settings = json_doc.FillRoutingSettings(json_doc.GetRoutingSettings());
    
    // Создание объекта маршрутизатора с использованием настроек и каталога
    const transport::Router router = {routing_settings, catalogue};

    // Создание обработчика запросов с использованием каталога и настроек рендеринга
    RequestHandler rh(catalogue, renderer, router);

    // Обработка статистических запросов и вывод результатов
    json_doc.ProcessRequests(stat_requests, rh);

    // Вывод результата в XML-файл
    rh.RenderMap().Render(output);
    output.close();
    
    std::system("pause");

    return 0;
}
