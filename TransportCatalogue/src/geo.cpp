#define _USE_MATH_DEFINES
#include "geo.h"

#include <cmath>

namespace geo {

// Функция для вычисления расстояния между двумя координатами
double ComputeDistance(Coordinates from, Coordinates to) {
    using namespace std;
    
    if (from == to) {
        return 0;
    }
    
    // Константа для перевода градусов в радианы
    static const double dr = M_PI / 180.;
    
    // Радиус Земли в метрах
    static const int earth_rd = 6371000;
    
    // Вычисляем расстояние с использованием формулы haversine
    return acos(sin(from.lat * dr) * sin(to.lat * dr) + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr)) * earth_rd;
}

} // namespace geo
