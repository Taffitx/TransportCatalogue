#include "transport_catalogue.h" 
 
namespace transport { 
 
// Добавляет остановку с указанным названием и координатами  
void Catalogue::AddStop(std::string_view stop_name, const geo::Coordinates coordinates) { 
    all_stops_.push_back({ std::string(stop_name), coordinates, {} }); 
    stopname_to_stop_[all_stops_.back().name] = &all_stops_.back(); 
} 
     
// Добавляет новый маршрут в каталог 
void Catalogue::AddRoute(std::string_view bus_number, const std::vector<const Stop*>& stops, bool is_circle) {  
    all_buses_.push_back({ std::string(bus_number), stops, is_circle });  
      
    busname_to_bus_[all_buses_.back().number] = &all_buses_.back();  
  
    // Обновляем отображение остановок на маршрутах  
    for (const auto& route_stop : stops) {  
        for (auto& stop_ : all_stops_) {  
            if (stop_.name == route_stop->name) {  
                stop_.buses_by_stop.insert(std::string(bus_number));  
            }  
        }  
    }  
}  
 
// Находит маршрут по номеру маршрута 
const Bus* Catalogue::FindRoute(std::string_view bus_number) const { 
    auto it = busname_to_bus_.find(bus_number); 
    if (it == busname_to_bus_.end()) { 
        return nullptr; 
    } else { 
        return it->second; 
    } 
} 
 
// Находит остановку по имени 
const Stop* Catalogue::FindStop(std::string_view stop_name) const { 
    auto it = stopname_to_stop_.find(stop_name); 
    if (it == stopname_to_stop_.end()) { 
        return nullptr; 
    } else { 
        return it->second; 
    } 
} 
 
// Возвращает количество уникальных остановок для указанного маршрута  
size_t Catalogue::UniqueStopsCount(std::string_view bus_number) const { 
    auto it = busname_to_bus_.find(bus_number); 
    if (it == busname_to_bus_.end()) { 
        return 0; 
    } 
 
    std::unordered_set<std::string_view> unique_stops; 
    for (const auto& stop : it->second->stops) { 
        unique_stops.insert(stop->name); 
    } 
    return unique_stops.size(); 
} 
 
// Устанавливает расстояние между двумя остановками 
void Catalogue::SetDistance(const Stop* from, const Stop* to, const int distance) { 
    stop_distances_[{from, to}] = distance; 
} 
 
// Получает расстояние между двумя остановками 
int Catalogue::GetDistance(const Stop* from, const Stop* to) const { 
    auto it = stop_distances_.find({ from, to }); 
    if (it != stop_distances_.end()) { 
        return it->second; 
    } 
 
    it = stop_distances_.find({ to, from }); 
    if (it != stop_distances_.end()) { 
        return it->second; 
    } 
    return 0; 
} 
 
// Возвращает все маршруты, отсортированные по номеру маршрута 
const std::map<std::string_view, const Bus*> Catalogue::GetSortedAllBuses() const { 
    std::map<std::string_view, const Bus*> result; 
    for (const auto& bus : busname_to_bus_) { 
        result.emplace(bus); 
    } 
    return result; 
} 

// Возвращает все остановки, отсортированные по имени
const std::map<std::string_view, const Stop*> Catalogue::GetSortedAllStops() const {
    std::map<std::string_view, const Stop*> result;
    for (const auto& stop : stopname_to_stop_) {
        result.emplace(stop);
    }
    return result;
}
 
// Получение статистики по маршруту 
transport::BusStat Catalogue::GetRouteInfo(const Bus& bus) const { 
    BusStat bus_stat; 
    bus_stat.unique_stops_count = std::unordered_set<const Stop*>(bus.stops.begin(), bus.stops.end()).size(); 
 
    if (bus.is_circle) { 
        bus_stat.stops_count = bus.stops.size();   // Круговой маршрут 
    } else { 
        bus_stat.stops_count = bus.stops.size() * 2 - 1;   // Не круговой маршрут 
    } 
 
    int route_length = 0;             // Общая длина маршрута 
    double geographic_length = 0.0;   // Географическая длина маршрута 
 
    // Подсчет длины маршрута и географической длины 
    for (size_t i = 0; i < bus.stops.size() - 1; ++i) { 
        const Stop* from = bus.stops[i]; 
        const Stop* to = bus.stops[i + 1]; 
         
        // Для кругового маршрута 
        route_length += GetDistance(from, to); 
        geographic_length += geo::ComputeDistance(from->coordinates, to->coordinates); 
 
        // Для не кругового маршрута 
        if (!bus.is_circle) { 
            route_length += GetDistance(to, from); 
            geographic_length += geo::ComputeDistance(to->coordinates, from->coordinates); 
        } 
    } 
 
    bus_stat.route_length = route_length; 
    bus_stat.curvature = static_cast<double>(route_length) / geographic_length; 
 
    return bus_stat; 
} 
 
 
} // namespace transport 
