#pragma once

#include <cmath>

namespace geo {

// Координаты
struct Coordinates {
    double lat;  // Широта
    double lng;  // Долгота
    
    // Оператор равенства для сравнения координат
    bool operator==(const Coordinates& other) const {
        return lat == other.lat && lng == other.lng;
    }
    
    // Оператор неравенства для сравнения координат
    bool operator!=(const Coordinates& other) const {
        return !(*this == other);
    }
};

// Функция для вычисления расстояния между двумя координатами
double ComputeDistance(Coordinates from, Coordinates to);

} // namespace geo