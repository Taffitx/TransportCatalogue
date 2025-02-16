#pragma once

#include "svg.h"
#include "geo.h"
#include "json.h"
#include "domain.h"

#include <algorithm>
#include <optional>
#include <map>
#include <string_view>
#include <vector>

namespace renderer {

// Константа для сравнения чисел с плавающей точкой
inline const double EPSILON = 1e-6;

// Функция для проверки, является ли значение близким к нулю
bool IsZero(double value);

class SphereProjector {
public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding) {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](const auto& lhs, const auto& rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](const auto& lhs, const auto& rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding_) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding_) / (max_lat_ - min_lat);
        }

        // Устанавливаем коэффициент масштабирования
        if (width_zoom && height_zoom) {
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_;         // Отступ от краёв изображения
    double min_lon_ = 0;     // Минимальная долгота
    double max_lat_ = 0;     // Максимальная широта
    double zoom_coeff_ = 0;  // Коэффициент масштабирования
};

// Структура для хранения настроек рендеринга
struct RenderSettings {
    double width = 0;                                // Ширина изображения
    double height = 0;                               // Высота изображения
    
    double padding = 0;                              // Отступ от краёв изображения
    
    double stop_radius = 0;                          // Радиус круга для остановок
    
    double line_width = 0;                           // Ширина линий маршрутов
    
    int bus_label_font_size = 0;                     // Размер шрифта для названий маршрутов
    svg::Point bus_label_offset = {0, 0};            // Смещение для названий маршрутов
    
    int stop_label_font_size = 0;                    // Размер шрифта для названий остановок
    svg::Point stop_label_offset = {0, 0};           // Смещение для названий остановок
    
    svg::Color underlayer_color = {svg::NoneColor};  // Цвет подложки для текста
    double underlayer_width = 0;                     // Ширина подложки для текста
    
    std::vector<svg::Color> color_palette {};        // Палитра цветов для маршрутов
};

class MapRenderer {
public:
    // Конструктор, принимающий настройки рендеринга
    MapRenderer(const RenderSettings& render_settings)
        : render_settings_(render_settings) {}

    // Функции для получения элементов SVG
    std::vector<svg::Polyline> GetRouteLines(const std::map<std::string_view, const transport::Bus*>& buses, const SphereProjector& sp) const;
    std::vector<svg::Text> GetBusLabels(const std::map<std::string_view, const transport::Bus*>& buses, const SphereProjector& sp) const;
    std::vector<svg::Circle> GetStopSymbols(const std::map<std::string_view, const transport::Stop*>& stops, const SphereProjector& sp) const;
    std::vector<svg::Text> GetStopLabels(const std::map<std::string_view, const transport::Stop*>& stops, const SphereProjector& sp) const;

    // Функция для получения SVG-документа с картой
    svg::Document GetSVG(const std::map<std::string_view, const transport::Bus*>& buses) const;

private:
    // Вспомогательные функции для добавления элементов в SVG-документ
    void AddRouteLinesToSVG(svg::Document& doc, const std::map<std::string_view, const transport::Bus*>& buses, const SphereProjector& sp) const;
    void AddBusLabelsToSVG(svg::Document& doc, const std::map<std::string_view, const transport::Bus*>& buses, const SphereProjector& sp) const;
    void AddStopSymbolsToSVG(svg::Document& doc, const std::map<std::string_view, const transport::Stop*>& stops, const SphereProjector& sp) const;
    void AddStopLabelsToSVG(svg::Document& doc, const std::map<std::string_view, const transport::Stop*>& stops, const SphereProjector& sp) const;

    const RenderSettings render_settings_;  // Настройки рендеринга
};

} // namespace renderer