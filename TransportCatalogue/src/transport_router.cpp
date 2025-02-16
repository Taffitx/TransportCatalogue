#include "transport_router.h"

namespace transport {

// Строит граф маршрутизации на основе данных из каталога
void Router::BuildGraph(const Catalogue& catalogue) {
    std::map<std::string, graph::VertexId> stop_ids; 
    graph::DirectedWeightedGraph<double> stops_graph(catalogue.GetSortedAllStops().size() * 2); 


    AddStopEdges(stops_graph, catalogue, stop_ids);
    AddBusEdges(stops_graph, catalogue, stop_ids);

    stop_ids_ = std::move(stop_ids);                             // Обновляем соответствие между остановками и идентификаторами вершин
    graph_ = std::move(stops_graph);                             // Сохраняем построенный граф маршрутизации
    router_ = std::make_unique<graph::Router<double>>(graph_);   // Создаем объект маршрутизатора на основе построенного графа
}

// Добавляет рёбра для всех остановок в граф маршрутизации
void Router::AddStopEdges(graph::DirectedWeightedGraph<double>& graph, const Catalogue& catalogue, std::map<std::string, graph::VertexId>& stop_ids) {
    const auto& all_stops = catalogue.GetSortedAllStops(); 
    graph::VertexId vertex_id = 0; 

    for (const auto& [stop_name, stop_info] : all_stops) {
        stop_ids[stop_info->name] = vertex_id; 
        // Добавляем ребро для ожидания на остановке и посадки в автобус
        graph.AddEdge({
            stop_info->name,                    // Имя остановки
            0,                                  // Нулевой вес (нет пройденного расстояния)
            vertex_id,                          // Начальная вершина (ожидание)
            ++vertex_id,                        // Конечная вершина (посадка в автобус)
            static_cast<double>(bus_wait_time_) // Время ожидания на остановке
        });
        ++vertex_id; 
    }
}

// Добавляет рёбра для всех автобусных маршрутов в граф маршрутизации
void Router::AddBusEdges(graph::DirectedWeightedGraph<double>& graph, const Catalogue& catalogue, const std::map<std::string, graph::VertexId>& stop_ids) {
    const auto& all_buses = catalogue.GetSortedAllBuses(); 

    for (const auto& item : all_buses) {
        const auto& bus_info = item.second;   // Информация о текущем автобусном маршруте
        const auto& stops = bus_info->stops;  // Список остановок на маршруте
        size_t stops_count = stops.size();    // Количество остановок на маршруте

        for (size_t i = 0; i < stops_count; ++i) {
            for (size_t j = i + 1; j < stops_count; ++j) {
                const Stop* stop_from = stops[i]; // Начальная остановка
                const Stop* stop_to = stops[j];   // Конечная остановка
                int dist_sum = 0;                 // Сумма расстояний в прямом направлении
                int dist_sum_inverse = 0;         // Сумма расстояний в обратном направлении

                // Вычисляем расстояние между остановками
                for (size_t k = i + 1; k <= j; ++k) {
                    dist_sum += catalogue.GetDistance(stops[k - 1], stops[k]);
                    dist_sum_inverse += catalogue.GetDistance(stops[k], stops[k - 1]);
                }

                auto it_from = stop_ids.find(stop_from->name);
                auto it_to = stop_ids.find(stop_to->name);

                if (it_from != stop_ids.end() && it_to != stop_ids.end()) {
                    graph.AddEdge({
                        bus_info->number,    // Номер маршрута
                        j - i,               // Количество остановок между начальной и конечной
                        it_from->second + 1, // Начальная вершина (посадка в автобус)
                        it_to->second,       // Конечная вершина (ожидание на остановке)
                        static_cast<double>(dist_sum) / (bus_velocity_ * (100.0 / 6.0)) // Время в пути
                    });

                    // Если маршрут не кольцевой, добавляем обратное ребро
                    if (!bus_info->is_circle) {
                        graph.AddEdge({
                            bus_info->number,  // Номер маршрута
                            j - i,             // Количество остановок между начальной и конечной
                            it_to->second + 1, // Начальная вершина для обратного пути
                            it_from->second,   // Конечная вершина для обратного пути
                            static_cast<double>(dist_sum_inverse) / (bus_velocity_ * (100.0 / 6.0)) // Время в пути для обратного маршрута
                        });
                    }
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
    
    
// Находит оптимальный маршрут между двумя остановками и возвращает информацию о маршруте
const std::optional<graph::Router<double>::RouteInfo> Router::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const {
    std::string from_str(stop_from);
    std::string to_str(stop_to);

    auto it_from = stop_ids_.find(from_str);
    auto it_to = stop_ids_.find(to_str);

    // Проверка, что обе остановки существуют в stop_ids_
    if (it_from == stop_ids_.end() || it_to == stop_ids_.end()) {
        // Одна или обе остановки не найдены, маршрут не может быть построен
        return std::nullopt;
    }

    // Получение идентификаторов вершин из найденных элементов
    graph::VertexId vertex_from = it_from->second;
    graph::VertexId vertex_to = it_to->second;

    return router_->BuildRoute(vertex_from, vertex_to);
}
    
// Возвращает граф маршрутизации, который используется для поиска маршрутов
const graph::DirectedWeightedGraph<double>& Router::GetGraph() const {
	return graph_;
}

} // namespace transport