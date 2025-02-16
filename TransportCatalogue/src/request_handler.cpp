#include "request_handler.h"

std::optional<transport::BusStat> RequestHandler::GetBusStat(const std::string_view bus_number) const {
    // Находим маршрут по номеру
    const transport::Bus* bus = catalogue_.FindRoute(bus_number);

    // Если маршрут не найден, выбрасываем исключение
    if (!bus) throw std::invalid_argument("bus not found");
    
    return catalogue_.GetRouteInfo(*bus);
}

const std::set<std::string> RequestHandler::GetBusesByStop(std::string_view stop_name) const { 
    // Находим остановку по имени и возвращаем набор маршрутов, проходящих через эту остановку 
    return catalogue_.FindStop(stop_name)->buses_by_stop; 
} 
 
bool RequestHandler::IsBusNumber(const std::string_view bus_number) const { 
    // Проверяем, существует ли маршрут с указанным номером 
    return catalogue_.FindRoute(bus_number); 
} 
 
bool RequestHandler::IsStopName(const std::string_view stop_name) const { 
    // Проверяем, существует ли остановка с указанным именем 
    return catalogue_.FindStop(stop_name); 
} 

const std::optional<graph::Router<double>::RouteInfo> RequestHandler::GetOptimalRoute(const std::string_view stop_from, const std::string_view stop_to) const {
    // Возвращаем информацию о маршруте, если он существует
    return router_.FindRoute(stop_from, stop_to);
}

const graph::DirectedWeightedGraph<double>& RequestHandler::GetRouterGraph() const {
    // Возвращаем ссылку на граф, используемый маршрутизатором
    return router_.GetGraph();
}
 
svg::Document RequestHandler::RenderMap() const { 
    // Получаем SVG-документ карты, используя все отсортированные маршруты 
    return renderer_.GetSVG(catalogue_.GetSortedAllBuses()); 
}