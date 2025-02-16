#pragma once

#include "geo.h"

#include <string>
#include <vector>
#include <set>
#include <unordered_map>

namespace transport {

// Остановка
struct Stop {
    std::string name;                     // Название остановки
    geo::Coordinates coordinates;         // Координаты остановки
    std::set<std::string> buses_by_stop;  // Набор номеров автобусов, проходящих через остановку
};

// Автобусный маршрут
struct Bus {
    std::string number;              // Номер маршрута
    std::vector<const Stop*> stops;  // Список указателей на остановки маршрута
    bool is_circle;                  // Является ли маршрут круговым
};

// Статистика по автобусному маршруту
struct BusStat {
    size_t stops_count;         // Общее количество остановок
    size_t unique_stops_count;  // Количество уникальных остановок
    double route_length;        // Длина маршрута
    double curvature;           // Кривизна маршрута (отношение фактической длины к географической)
};

} // namespace transport
