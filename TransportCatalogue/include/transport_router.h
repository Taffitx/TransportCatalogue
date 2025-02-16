#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>

namespace transport {

class Router {
public:
    Router() = default;
    
    // Конструктор с параметрами для установки времени ожидания автобуса и его скорости
	explicit Router(const int bus_wait_time, const double bus_velocity)
		: bus_wait_time_(bus_wait_time)
		, bus_velocity_(bus_velocity) {}

    // Конструктор копирования, который также строит граф маршрутизации на основе предоставленного каталога
	Router(const Router& settings, const Catalogue& catalogue) {
		bus_wait_time_ = settings.bus_wait_time_;
		bus_velocity_ = settings.bus_velocity_;
		BuildGraph(catalogue);
	}

    // Строит граф маршрутизации на основе данных из каталога
    void BuildGraph(const Catalogue& catalogue);
      
    // Находит оптимальный маршрут между двумя остановками и возвращает информацию о маршруте
    const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
    
    // Возвращает граф маршрутизации, который используется для поиска маршрутов
    const graph::DirectedWeightedGraph<double>& GetGraph() const;

    // Можете подсказать, что именно мне нужно сделать, поскольку метод GetGraph у меня используется в request_handler и json_reader (PrintRouting) я честно не совсем понимаю, что мне нужно сделать.
    // Провел весь день пытаясь устранить зависимость этого метода в других частях кода, но все безуспешно. Простите может за нелепый вопрос, а нельзя ли оставить этот метод или насколько сильно это влияет на работу программы?

private:
    // Время ожидания автобуса на остановке 
    int bus_wait_time_ = 0;
    // Средняя скорость автобуса 
    double bus_velocity_ = 0.0;     

    // Граф маршрутизации, представляющий собой ориентированный граф с весами
    graph::DirectedWeightedGraph<double> graph_; 
    // Отображение имен остановок на идентификаторы вершин в графе
    std::map<std::string, graph::VertexId> stop_ids_; 
    // Указатель на объект маршрутизатора, который использует граф для поиска маршрутов
    std::unique_ptr<graph::Router<double>> router_;    
    
    // Вспомогательный метод, добавляет рёбра для всех остановок в граф маршрутизации
    void AddBusEdges(graph::DirectedWeightedGraph<double>& graph, const Catalogue& catalogue, const std::map<std::string, graph::VertexId>& stop_ids);
    // Вспомогательный метод, добавляет рёбра для всех автобусных маршрутов в граф маршрутизации
    void AddStopEdges(graph::DirectedWeightedGraph<double>& graph, const Catalogue& catalogue, std::map<std::string, graph::VertexId>& stop_ids);
};
    
} // namespace transport