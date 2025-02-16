#include "json_reader.h"
#include "json_builder.h"

using namespace std::literals;

// Получение базы запросов из JSON-документа
const json::Node& JsonReader::GetBaseRequests() const {
    auto it = input_.GetRoot().AsDict().find("base_requests");
    if (it == input_.GetRoot().AsDict().end()) 
        return dummy_;
    return it->second;
}

// Получение статистических запросов из JSON-документа
const json::Node& JsonReader::GetStatRequests() const {
    auto it = input_.GetRoot().AsDict().find("stat_requests");
    if (it == input_.GetRoot().AsDict().end()) 
        return dummy_;
    return it->second;
}

// Получение настроек рендеринга из JSON-документа
const json::Node& JsonReader::GetRenderSettings() const {
    auto it = input_.GetRoot().AsDict().find("render_settings");
    if (it == input_.GetRoot().AsDict().end()) 
        return dummy_;
    return it->second;
}

// Получение настроек маршрутизации из JSON-документа
const json::Node& JsonReader::GetRoutingSettings() const {
    auto it = input_.GetRoot().AsDict().find("routing_settings");
    if (it == input_.GetRoot().AsDict().end())
        return dummy_;
    return it->second;
}

// Обработка статистических запросов и вывод результатов
void JsonReader::ProcessRequests(const json::Node& stat_requests, RequestHandler& rh) const {
    json::Array result; // Массив для хранения результатов запросов
    for (const auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsDict();
        const auto& type = request_map.at("type").AsString();

        // В зависимости от типа запроса вызываем соответствующий метод
        if (type == "Stop") {
            result.push_back(PrintStop(request_map, rh).AsDict());
        } else if (type == "Bus") {
            result.push_back(PrintRoute(request_map, rh).AsDict());
        } else if (type == "Map") {
            result.push_back(PrintMap(request_map, rh).AsDict());
        } else if (type == "Route") {
            result.push_back(PrintRouting(request_map, rh).AsDict());
        }
    }

    // Вывод результата в поток
    json::Print(json::Document{result}, std::cout);
}

// Заполнение каталога остановками из JSON-документа
void JsonReader::FillCatalogue(transport::Catalogue& catalogue) {
    const json::Array& arr = GetBaseRequests().AsArray();
    FillStops(catalogue, arr);
    FillStopDistances(catalogue, arr);
    FillRoutes(catalogue, arr);
}

void JsonReader::FillStops(transport::Catalogue& catalogue, const json::Array& arr) const {
    for (const auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsDict();
        auto it = request_stops_map.find("type");
        if (it != request_stops_map.end() && it->second.AsString() == "Stop") {
            auto [stop_name, coordinates, stop_distances] = FillStop(request_stops_map);
            catalogue.AddStop(stop_name, coordinates);
        }
    }
}

void JsonReader::FillStopDistances(transport::Catalogue& catalogue, const json::Array& arr) const {
    for (const auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsDict();
        auto it = request_stops_map.find("type");
        if (it != request_stops_map.end() && it->second.AsString() == "Stop") {
            auto [stop_name, coordinates, stop_distances] = FillStop(request_stops_map);
            for (const auto& [to_name, dist] : stop_distances) {
                auto from = catalogue.FindStop(stop_name);
                auto to = catalogue.FindStop(to_name);
                catalogue.SetDistance(from, to, dist);
            }
        }
    }
}

void JsonReader::FillRoutes(transport::Catalogue& catalogue, const json::Array& arr) const {
    for (const auto& request_bus : arr) {
        const auto& request_bus_map = request_bus.AsDict();
        auto it = request_bus_map.find("type");
        if (it != request_bus_map.end() && it->second.AsString() == "Bus") {
            auto [bus_number, stops, circular_route] = FillRoute(request_bus_map, catalogue);
            catalogue.AddRoute(bus_number, stops, circular_route);
        }
    }
}

std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> JsonReader::FillStop(const json::Dict& request_map) const {
    std::string_view stop_name = request_map.find("name")->second.AsString();
    geo::Coordinates coordinates = { request_map.find("latitude")->second.AsDouble(), request_map.find("longitude")->second.AsDouble() };
    std::map<std::string_view, int> stop_distances;
    
    auto it = request_map.find("road_distances");
    if (it != request_map.end()) {
        const auto& distances = it->second.AsDict();
        for (const auto& [stop_name, dist] : distances) {
            stop_distances.emplace(stop_name, dist.AsInt());
        }
    }
    
    return std::make_tuple(stop_name, coordinates, stop_distances);
}

std::tuple<std::string_view, std::vector<const transport::Stop*>, bool> JsonReader::FillRoute(const json::Dict& request_map, transport::Catalogue& catalogue) const {
    std::string_view bus_number = request_map.find("name")->second.AsString();
    std::vector<const transport::Stop*> stops;
    
    auto it = request_map.find("stops");
    if (it != request_map.end()) {
        for (const auto& stop : it->second.AsArray()) {
            stops.push_back(catalogue.FindStop(stop.AsString()));
        }
    }
    
    bool circular_route = request_map.find("is_roundtrip")->second.AsBool();
    
    return std::make_tuple(bus_number, stops, circular_route);
}


// Заполнение настроек рендеринга карты из запроса
renderer::MapRenderer JsonReader::FillRenderSettings(const json::Node& settings) const {
    json::Dict request_map = settings.AsDict();
    renderer::RenderSettings render_settings;

    auto find_and_set_double = [&](const std::string& key, double& setting) {
        auto it = request_map.find(key);
        if (it != request_map.end()) {
            setting = it->second.AsDouble();
        }
    };

    auto find_and_set_int = [&](const std::string& key, int& setting) {
        auto it = request_map.find(key);
        if (it != request_map.end()) {
            setting = it->second.AsInt();
        }
    };

    auto find_and_set_offset = [&](const std::string& key, svg::Point& offset) {
        auto it = request_map.find(key);
        if (it != request_map.end()) {
            const json::Array& array = it->second.AsArray();
            offset = { array[0].AsDouble(), array[1].AsDouble() };
        }
    };

    find_and_set_double("width", render_settings.width);
    find_and_set_double("height", render_settings.height);
    find_and_set_double("padding", render_settings.padding);
    find_and_set_double("stop_radius", render_settings.stop_radius);
    find_and_set_double("line_width", render_settings.line_width);
    find_and_set_int("bus_label_font_size", render_settings.bus_label_font_size);
    find_and_set_offset("bus_label_offset", render_settings.bus_label_offset);
    find_and_set_int("stop_label_font_size", render_settings.stop_label_font_size);
    find_and_set_offset("stop_label_offset", render_settings.stop_label_offset);

    auto it = request_map.find("underlayer_color");
    if (it != request_map.end()) {
        render_settings.underlayer_color = ParseColor(it->second);
    }

    find_and_set_double("underlayer_width", render_settings.underlayer_width);

    it = request_map.find("color_palette");
    if (it != request_map.end()) {
        const json::Array& color_palette = it->second.AsArray();
        for (const auto& color_element : color_palette) {
            render_settings.color_palette.push_back(ParseColor(color_element));
        }
    }

    return render_settings;
}

// Заполнение настроек маршрутизации из JSON-данных
transport::Router JsonReader::FillRoutingSettings(const json::Node& settings) const {
    const auto& settings_dict = settings.AsDict();

    int bus_wait_time = 0;
    double bus_velocity = 0.0;

    if (auto it = settings_dict.find("bus_wait_time"s); it != settings_dict.end()) {
        bus_wait_time = it->second.AsInt();
    }

    if (auto it = settings_dict.find("bus_velocity"s); it != settings_dict.end()) {
        bus_velocity = it->second.AsDouble();
    }

    return transport::Router(bus_wait_time, bus_velocity);
}


// Вспомогательная функция для парсинга цвета
svg::Color JsonReader::ParseColor(const json::Node& color_node) const {
    if (color_node.IsString()) {
        return color_node.AsString();
    } else if (color_node.IsArray()) {
        const json::Array& color_array = color_node.AsArray();
        if (color_array.size() == 3) {
            return svg::Rgb(color_array[0].AsInt(), color_array[1].AsInt(), color_array[2].AsInt());
        } else if (color_array.size() == 4) {
            return svg::Rgba(color_array[0].AsInt(), color_array[1].AsInt(), color_array[2].AsInt(), color_array[3].AsDouble());
        }
    }
    throw std::logic_error("wrong color format");
}

// Определение функции для создания JSON-ответа с ошибкой
json::Node JsonReader::CreateErrorResponse(int id, const std::string& error_message) const {
    json::Builder builder;
    return builder.StartDict()
        .Key("request_id").Value(id)
        .Key("error_message").Value(error_message)
    .EndDict()
    .Build();
}

// Определение функции для создания JSON-ответа с данными маршрута
json::Node JsonReader::CreateRouteResponse(int id, const transport::BusStat& route_info) const {
    json::Builder builder;
    return builder.StartDict()
        .Key("request_id").Value(id)
        .Key("curvature").Value(route_info.curvature)
        .Key("route_length").Value(route_info.route_length)
        .Key("stop_count").Value(static_cast<int>(route_info.stops_count))
        .Key("unique_stop_count").Value(static_cast<int>(route_info.unique_stops_count))
    .EndDict()
    .Build();
}

// Формирует JSON-ответ с информацией о маршруте на основе запроса
const json::Node JsonReader::PrintRoute(const json::Dict& request_map, RequestHandler& rh) const {
    auto id_it = request_map.find("id");
    auto name_it = request_map.find("name");

    int id = (id_it != request_map.end()) ? id_it->second.AsInt() : 0;
    const std::string& route_number = (name_it != request_map.end()) ? name_it->second.AsString() : "";

    if (!rh.IsBusNumber(route_number)) {
        return CreateErrorResponse(id, "not found");
    } else {
        const auto& route_info = rh.GetBusStat(route_number);
        return CreateRouteResponse(id, *route_info);
    }
}

// Формирует JSON-ответ с информацией о остановке на основе запроса
const json::Node JsonReader::PrintStop(const json::Dict& request_map, RequestHandler& rh) const {
    auto id_it = request_map.find("id");
    auto name_it = request_map.find("name");

    int id = (id_it != request_map.end()) ? id_it->second.AsInt() : 0;
    const std::string& stop_name = (name_it != request_map.end()) ? name_it->second.AsString() : "";

    json::Builder builder;
    if (!rh.IsStopName(stop_name)) {
        return CreateErrorResponse(id, "not found");
    } else {
        json::Array buses;
        for (const auto& bus : rh.GetBusesByStop(stop_name)) {
            buses.push_back(bus);
        }
        return builder.StartDict()
            .Key("request_id").Value(id)
            .Key("buses").Value(buses)
        .EndDict()
        .Build();
    }
}

// Формирует JSON-ответ с картой на основе запроса
const json::Node JsonReader::PrintMap(const json::Dict& request_map, RequestHandler& rh) const {
    auto id_it = request_map.find("id");
    int id = (id_it != request_map.end()) ? id_it->second.AsInt() : 0;

    std::ostringstream strm;
    svg::Document map = rh.RenderMap();
    map.Render(strm);

    return json::Builder{}
        .StartDict()
            .Key("request_id").Value(id)
            .Key("map").Value(strm.str())
        .EndDict()
        .Build();
}

// Формирует JSON-ответ с информацией о маршруте между двумя остановками на основе запроса
const json::Node JsonReader::PrintRouting(const json::Dict& request_map, RequestHandler& rh) const {
    json::Node result;

    auto id_it = request_map.find("id"s);
    auto from_it = request_map.find("from"s);
    auto to_it = request_map.find("to"s);

    if (id_it == request_map.end() || from_it == request_map.end() || to_it == request_map.end()) {
        result = json::Builder{}
            .StartDict()
                .Key("error_message"s).Value("Invalid request format"s)
            .EndDict()
        .Build();
        return result;
    }

    const int id = id_it->second.AsInt();
    const std::string_view stop_from = from_it->second.AsString();
    const std::string_view stop_to = to_it->second.AsString();
    const auto& routing = rh.GetOptimalRoute(stop_from, stop_to);

    if (!routing) {
        result = json::Builder{}
            .StartDict()
                .Key("request_id"s).Value(id)
                .Key("error_message"s).Value("not found"s)
            .EndDict()
        .Build();
    }
    else {
        json::Array items;
        double total_time = 0.0;
        items.reserve(routing.value().edges.size());
        for (auto& edge_id : routing.value().edges) {
            const graph::Edge<double> edge = rh.GetRouterGraph().GetEdge(edge_id);
            if (edge.quality == 0) {
                items.emplace_back(json::Node(json::Builder{}
                    .StartDict()
                        .Key("stop_name"s).Value(edge.name)
                        .Key("time"s).Value(edge.weight)
                        .Key("type"s).Value("Wait"s)
                    .EndDict()
                .Build()));

                total_time += edge.weight;
            }
            else {
                items.emplace_back(json::Node(json::Builder{}
                    .StartDict()
                        .Key("bus"s).Value(edge.name)
                        .Key("span_count"s).Value(static_cast<int>(edge.quality))
                        .Key("time"s).Value(edge.weight)
                        .Key("type"s).Value("Bus"s)
                    .EndDict()
                .Build()));

                total_time += edge.weight;
            }
        }

        result = json::Builder{}
            .StartDict()
                .Key("request_id"s).Value(id)
                .Key("total_time"s).Value(total_time)
                .Key("items"s).Value(items)
            .EndDict()
        .Build();
    }

    return result;
}

