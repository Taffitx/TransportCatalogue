#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <sstream>

class RequestHandler {
public:
    // Инициализирует обработчик запросов с помощью каталога транспорта и рендерера карты
    RequestHandler(const transport::Catalogue& catalogue, const renderer::MapRenderer& renderer, const transport::Router& router)
        : catalogue_(catalogue)  
        , renderer_(renderer) 
        , router_(router) {} 

    // Метод для получения статистики о маршруте по номеру автобуса
    std::optional<transport::BusStat> GetBusStat(const std::string_view bus_number) const;

    // Метод для получения набора номеров автобусов, проходящих через указанную остановку
    const std::set<std::string> GetBusesByStop(std::string_view stop_name) const;

    // Метод для проверки, существует ли автобус с указанным номером
    bool IsBusNumber(const std::string_view bus_number) const;

    // Метод для проверки, существует ли остановка с указанным именем
    bool IsStopName(const std::string_view stop_name) const;
    
    // Метод для получения оптимального маршрута между двумя остановками
    const std::optional<graph::Router<double>::RouteInfo> GetOptimalRoute(const std::string_view stop_from, const std::string_view stop_to) const;
   
    // Метод для получения графа маршрутизатора
    const graph::DirectedWeightedGraph<double>& GetRouterGraph() const;

    // Метод для рендеринга карты и получения SVG-документа
    svg::Document RenderMap() const;

private:
    const transport::Catalogue& catalogue_;  // Ссылка на объект каталога транспорта
    const renderer::MapRenderer& renderer_;  // Ссылка на объект рендерера карты
    const transport::Router& router_;        // Ссылка на объект маршрутизатора
};