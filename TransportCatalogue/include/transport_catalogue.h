#pragma once

#include "geo.h"
#include "domain.h"

#include <iostream>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <optional>
#include <unordered_set>
#include <set>
#include <map>

namespace transport {

class Catalogue {
public:
    struct StopDistancesHasher {
        size_t operator()(const std::pair<const Stop*, const Stop*>& points) const {
            size_t hash_first = std::hash<const void*>{}(points.first);
            size_t hash_second = std::hash<const void*>{}(points.second);
            return hash_first + hash_second * 37;
        }
    };

    // Добавляет остановку в каталог
    void AddStop(std::string_view stop_name, const geo::Coordinates coordinates);

    // Добавляет маршрут в каталог
    void AddRoute(std::string_view bus_number, const std::vector<const Stop*>& stops, bool is_circle);
    
    // Находит маршрут по номеру маршрута
    const Bus* FindRoute(std::string_view bus_number) const;

    // Находит остановку по её имени
    const Stop* FindStop(std::string_view stop_name) const;

    // Устанавливает расстояние между двумя остановками
    void SetDistance(const Stop* from, const Stop* to, const int distance);

    // Получает расстояние между двумя остановками
    int GetDistance(const Stop* from, const Stop* to) const;

    // Возвращает все маршруты, отсортированные по номеру
    const std::map<std::string_view, const Bus*> GetSortedAllBuses() const;
    
    // Возвращает все остановки, отсортированные по имени
    const std::map<std::string_view, const Stop*> GetSortedAllStops() const;

    // Получение статистики по маршруту
    BusStat GetRouteInfo(const Bus& route) const;

private:
    // Возвращает количество уникальных остановок для указанного маршрута
    size_t UniqueStopsCount(std::string_view bus_number) const;

    // Хранит все маршруты в очереди
    std::deque<Bus> all_buses_;

    // Хранит все остановки в очереди
    std::deque<Stop> all_stops_;

    // Отображение имени маршрута на указатель на объект маршрута
    std::unordered_map<std::string_view, const Bus*> busname_to_bus_;

    // Отображение имени остановки на указатель на объект остановки
    std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;

    // Отображение пары остановок на расстояние между ними 
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopDistancesHasher> stop_distances_;
};

} // namespace transport
