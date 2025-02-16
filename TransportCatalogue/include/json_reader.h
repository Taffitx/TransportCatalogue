#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"

#include <iostream>

// Класс JsonReader предоставляет функциональность для чтения и обработки JSON-запросов
class JsonReader {
public:
    // Конструктор принимает входной поток и загружает JSON-документ
    JsonReader(std::istream& input)
        : input_(json::Load(input)) {}

    // Получение различных частей JSON-запросов
    const json::Node& GetBaseRequests() const;
    const json::Node& GetStatRequests() const;
    const json::Node& GetRenderSettings() const;
    const json::Node& GetRoutingSettings() const;

    // Обработка статических запросов и передачи их обработчику запросов
    void ProcessRequests(const json::Node& stat_requests, RequestHandler& rh) const;

    // Заполнение каталога транспортной системы
    void FillCatalogue(transport::Catalogue& catalogue);
    
    // Вспомогательная функция для заполнения остановок
    void FillStops(transport::Catalogue& catalogue, const json::Array& arr) const;
    // Вспомогательная функция для заполнения расстояний между остановками в каталоге
    void FillStopDistances(transport::Catalogue& catalogue, const json::Array& arr) const;
    // Вспомогательная функция для заполнения маршрутов
    void FillRoutes(transport::Catalogue& catalogue, const json::Array& arr) const;
    
    // Заполнение настроек рендеринга карты
    renderer::MapRenderer FillRenderSettings(const json::Node& settings) const;
    // Заполнение настроек маршрутизации из JSON-данных
    transport::Router FillRoutingSettings(const json::Node& settings) const;
    
    // Вспомогательная функция для парсинга цвета
    svg::Color ParseColor(const json::Node& color_node) const;
  
    // Обработка и вывод различных типов запросов
    const json::Node PrintRoute(const json::Dict& request_map, RequestHandler& rh) const;
    const json::Node PrintStop(const json::Dict& request_map, RequestHandler& rh) const;
    const json::Node PrintMap(const json::Dict& request_map, RequestHandler& rh) const;
    const json::Node PrintRouting(const json::Dict& request_map, RequestHandler& rh) const;

private:
    json::Document input_;        // Входной JSON-документ
    json::Node dummy_ = nullptr;  // Заглушка для возвращения значений по умолчанию

    // Вспомогательные методы для заполнения данных о остановках и маршрутах
    std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> FillStop(const json::Dict& request_map) const;
    void FillStopDistances(transport::Catalogue& catalogue) const;
    std::tuple<std::string_view, std::vector<const transport::Stop*>, bool> FillRoute(const json::Dict& request_map, transport::Catalogue& catalogue) const;
    
    // Вспомогательные функции для формирования JSON-ответов
    json::Node CreateErrorResponse(int id, const std::string& error_message) const;
    json::Node CreateRouteResponse(int id, const transport::BusStat& route_info) const;
};