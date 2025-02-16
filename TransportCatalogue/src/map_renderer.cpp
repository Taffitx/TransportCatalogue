#include "map_renderer.h"

namespace renderer {

// Проверка, является ли значение достаточно близким к нулю 
bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

// Получение линий маршрутов для отображения 
std::vector<svg::Polyline> MapRenderer::GetRouteLines(const std::map<std::string_view, const transport::Bus*>& buses, const SphereProjector& sp) const {
    std::vector<svg::Polyline> result;
    size_t color_num = 0; // Индекс для выбора цвета из палитры

    for (const auto& [bus_number, bus] : buses) {
        if (bus->stops.empty()) continue; // Пропустить автобус без остановок

        std::vector<const transport::Stop*> route_stops{bus->stops.begin(), bus->stops.end()};
        // Добавить обратный маршрут, если маршрут не круговой
        if (!bus->is_circle) {
            route_stops.insert(route_stops.end(), std::next(bus->stops.rbegin()), bus->stops.rend());
        }

        svg::Polyline line;
        for (const auto& stop : route_stops) {
            line.AddPoint(sp(stop->coordinates)); // Добавить точку маршрута в линию
        }

        // Настройка стиля линии
        line.SetStrokeColor(render_settings_.color_palette[color_num]);
        line.SetFillColor("none");
        line.SetStrokeWidth(render_settings_.line_width);
        line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        // Переключение цвета для следующего маршрута
        color_num = (color_num + 1) % render_settings_.color_palette.size();

        result.push_back(line);
    }

    return result;
}

// Получение меток автобусов для отображения 
std::vector<svg::Text> MapRenderer::GetBusLabels(const std::map<std::string_view, const transport::Bus*>& buses, const SphereProjector& sp) const {
    std::vector<svg::Text> result;
    size_t color_num = 0; // Индекс для выбора цвета из палитры

    for (const auto& [bus_number, bus] : buses) {
        if (bus->stops.empty()) continue; // Пропустить автобус без остановок

        // Настройка текста для метки автобуса
        svg::Text text;
        svg::Text underlayer;
        text.SetPosition(sp(bus->stops[0]->coordinates));
        text.SetOffset(render_settings_.bus_label_offset);
        text.SetFontSize(render_settings_.bus_label_font_size);
        text.SetFontFamily("Verdana");
        text.SetFontWeight("bold");
        text.SetData(bus->number);
        text.SetFillColor(render_settings_.color_palette[color_num]);

        // Настройка подложки для текста
        underlayer.SetPosition(sp(bus->stops[0]->coordinates));
        underlayer.SetOffset(render_settings_.bus_label_offset);
        underlayer.SetFontSize(render_settings_.bus_label_font_size);
        underlayer.SetFontFamily("Verdana");
        underlayer.SetFontWeight("bold");
        underlayer.SetData(bus->number);
        underlayer.SetFillColor(render_settings_.underlayer_color);
        underlayer.SetStrokeColor(render_settings_.underlayer_color);
        underlayer.SetStrokeWidth(render_settings_.underlayer_width);
        underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        result.push_back(underlayer);
        result.push_back(text);

        // Добавление дополнительной метки для концов круговых маршрутов
        if (!bus->is_circle && bus->stops[0] != bus->stops.back()) {
            svg::Text text2{text};
            svg::Text underlayer2{underlayer};
            text2.SetPosition(sp(bus->stops.back()->coordinates));
            underlayer2.SetPosition(sp(bus->stops.back()->coordinates));

            result.push_back(underlayer2);
            result.push_back(text2);
        }

        // Переключение цвета для следующего автобуса
        color_num = (color_num + 1) % render_settings_.color_palette.size();
    }

    return result;
}

// Получение символов для отображения остановок 
std::vector<svg::Circle> MapRenderer::GetStopSymbols(const std::map<std::string_view, const transport::Stop*>& stops, const SphereProjector& sp) const {
    std::vector<svg::Circle> result;

    for (const auto& [stop_name, stop] : stops) {
        svg::Circle symbol;
        symbol.SetCenter(sp(stop->coordinates));
        symbol.SetRadius(render_settings_.stop_radius);
        symbol.SetFillColor("white");

        result.push_back(symbol);
    }

    return result;
}

// Получение меток для отображения остановок 
std::vector<svg::Text> MapRenderer::GetStopLabels(const std::map<std::string_view, const transport::Stop*>& stops, const SphereProjector& sp) const {
    std::vector<svg::Text> result;

    for (const auto& [stop_name, stop] : stops) {
        svg::Text text;
        svg::Text underlayer;

        // Настройка текста для метки остановки
        text.SetPosition(sp(stop->coordinates));
        text.SetOffset(render_settings_.stop_label_offset);
        text.SetFontSize(render_settings_.stop_label_font_size);
        text.SetFontFamily("Verdana");
        text.SetData(stop->name);
        text.SetFillColor("black");

        // Настройка подложки для текста
        underlayer.SetPosition(sp(stop->coordinates));
        underlayer.SetOffset(render_settings_.stop_label_offset);
        underlayer.SetFontSize(render_settings_.stop_label_font_size);
        underlayer.SetFontFamily("Verdana");
        underlayer.SetData(stop->name);
        underlayer.SetFillColor(render_settings_.underlayer_color);
        underlayer.SetStrokeColor(render_settings_.underlayer_color);
        underlayer.SetStrokeWidth(render_settings_.underlayer_width);
        underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        result.push_back(underlayer);
        result.push_back(text);
    }

    return result;
}

// Вспомогательная функция для добавления линий маршрутов в SVG-документ
void MapRenderer::AddRouteLinesToSVG(svg::Document& doc, const std::map<std::string_view, const transport::Bus*>& buses, const SphereProjector& sp) const {
    for (const auto& line : GetRouteLines(buses, sp)) {
        doc.Add(line);
    }
}

// Вспомогательная функция для добавления меток автобусов в SVG-документ
void MapRenderer::AddBusLabelsToSVG(svg::Document& doc, const std::map<std::string_view, const transport::Bus*>& buses, const SphereProjector& sp) const {
    for (const auto& text : GetBusLabels(buses, sp)) {
        doc.Add(text);
    }
}

// Вспомогательная функция для добавления символов остановок в SVG-документ
void MapRenderer::AddStopSymbolsToSVG(svg::Document& doc, const std::map<std::string_view, const transport::Stop*>& stops, const SphereProjector& sp) const {
    for (const auto& circle : GetStopSymbols(stops, sp)) {
        doc.Add(circle);
    }
}

// Вспомогательная функция для добавления меток остановок в SVG-документ
void MapRenderer::AddStopLabelsToSVG(svg::Document& doc, const std::map<std::string_view, const transport::Stop*>& stops, const SphereProjector& sp) const {
    for (const auto& text : GetStopLabels(stops, sp)) {
        doc.Add(text);
    }
}

// Получение SVG-документа для отображения карты
svg::Document MapRenderer::GetSVG(const std::map<std::string_view, const transport::Bus*>& buses) const {
    svg::Document result;
    std::vector<geo::Coordinates> route_stops_coord;
    std::map<std::string_view, const transport::Stop*> all_stops;

    // Сбор всех остановок и координат маршрутов
    for (const auto& [bus_number, bus] : buses) {
        if (bus->stops.empty()) continue;
        for (const auto& stop : bus->stops) {
            route_stops_coord.push_back(stop->coordinates);
            all_stops[stop->name] = stop;
        }
    }

    // Создание проектора для преобразования координат
    SphereProjector sp(
        route_stops_coord.begin(), route_stops_coord.end(),
        render_settings_.width, render_settings_.height, render_settings_.padding
    );

    // Добавление элементов SVG в документ
    AddRouteLinesToSVG(result, buses, sp);
    AddBusLabelsToSVG(result, buses, sp);
    AddStopSymbolsToSVG(result, all_stops, sp);
    AddStopLabelsToSVG(result, all_stops, sp);

    return result;
}

} // namespace renderer