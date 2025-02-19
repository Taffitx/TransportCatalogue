# Transport Catalogue

## Описание
Проект реализует систему транспортного каталога, которая позволяет:
- Хранить информацию об остановках и маршрутах
- Рассчитывать расстояния между остановками
- Визуализировать карту маршрутов в формате SVG
- Находить оптимальные маршруты между остановками с учетом времени ожидания и скорости транспорта
- Работать с данными в формате JSON

Система состоит из нескольких ключевых компонентов, взаимодействующих между собой.

## Основные компоненты

### Transport Catalogue
Ядро системы, отвечает за:
- Хранение данных об остановках и маршрутах
- Добавление и поиск остановок/маршрутов
- Управление расстояниями между остановками
- Получение статистики по маршрутам

### JSON Reader
Обрабатывает входящие JSON-запросы:
- Чтение и парсинг JSON данных
- Заполнение каталога данными из JSON
- Обработка запросов на маршруты, остановки, карты
- Поддержка настроек рендеринга и маршрутизации

### Map Renderer
Визуализация карты маршрутов:
- Преобразование географических координат в SVG
- Генерация линий маршрутов, меток остановок
- Поддержка настраиваемых параметров отображения
- Экспорт карты в SVG формат

### Transport Router
Маршрутизация между остановками:
- Построение графа маршрутов
- Поиск оптимального пути с учетом времени и скорости
- Интеграция с основным каталогом

## Сборка проекта
Проект собирается с помощью CMake. Для сборки выполните следующие шаги:

1. Создайте директорию для сборки:
```bash
mkdir build
cd build
```

2. Запустите CMake для генерации Makefile:
```bash
cmake ..
```

3. Соберите проект:
```bash
cmake --build .
```

После успешной сборки в директории build будет создан исполняемый файл TransportCatalogue.

## Запуск проекта
Для корректной работы проекта выполните следующие шаги:

1. Создайте файл input.json в папке build:
   - Используйте текстовый редактор (например, Notepad++ или VSCode)
   - Или выполните команду в командной строке:
     ```cmd
     copy nul input.json
     ```
   - Вставьте следующий пример данных:
```json
  {
      "base_requests": [
          {
              "is_roundtrip": true,
              "name": "297",
              "stops": [
                  "Biryulyovo Zapadnoye",
                  "Biryulyovo Tovarnaya",
                  "Universam",
                  "Biryulyovo Zapadnoye"
              ],
              "type": "Bus"
          },
          {
              "is_roundtrip": false,
              "name": "635",
              "stops": [
                  "Biryulyovo Tovarnaya",
                  "Universam",
                  "Prazhskaya"
              ],
              "type": "Bus"
          },
          {
              "latitude": 55.574371,
              "longitude": 37.6517,
              "name": "Biryulyovo Zapadnoye",
              "road_distances": {
                  "Biryulyovo Tovarnaya": 2600
              },
              "type": "Stop"
          },
          {
              "latitude": 55.587655,
              "longitude": 37.645687,
              "name": "Universam",
              "road_distances": {
                  "Biryulyovo Tovarnaya": 1380,
                  "Biryulyovo Zapadnoye": 2500,
                  "Prazhskaya": 4650
              },
              "type": "Stop"
          },
          {
              "latitude": 55.592028,
              "longitude": 37.653656,
              "name": "Biryulyovo Tovarnaya",
              "road_distances": {
                  "Universam": 890
              },
              "type": "Stop"
          },
          {
              "latitude": 55.611717,
              "longitude": 37.603938,
              "name": "Prazhskaya",
              "road_distances": {},
              "type": "Stop"
          }
      ],
      "render_settings": {
          "bus_label_font_size": 20,
          "bus_label_offset": [
              7,
              15
          ],
          "color_palette": [
              "green",
              [
                  255,
                  160,
                  0
              ],
              "red"
          ],
          "height": 200,
          "line_width": 14,
          "padding": 30,
          "stop_label_font_size": 20,
          "stop_label_offset": [
              7,
              -3
          ],
          "stop_radius": 5,
          "underlayer_color": [
              255,
              255,
              255,
              0.85
          ],
          "underlayer_width": 3,
          "width": 200
      },
      "routing_settings": {
          "bus_velocity": 40,
          "bus_wait_time": 6
      },
      "stat_requests": [
          {
              "id": 1,
              "name": "297",
              "type": "Bus"
          },
          {
              "id": 2,
              "name": "635",
              "type": "Bus"
          },
          {
              "id": 3,
              "name": "Universam",
              "type": "Stop"
          },
          {
              "from": "Biryulyovo Zapadnoye",
              "id": 4,
              "to": "Universam",
              "type": "Route"
          },
          {
              "from": "Biryulyovo Zapadnoye",
              "id": 5,
              "to": "Prazhskaya",
              "type": "Route"
          }
      ]
  }
```

2. Создайте пустой файл output.xml в папке build:
```cmd
copy nul > output.xml
```

3. Запустите программу:
```bash
./TransportCatalogue
```

Результат работы программы:
- Статистика по маршрутам будет выведена в консоль в формате JSON
- Карта маршрутов будет сохранена в файл output.xml в формате SVG

## Инструменты разработки
- Язык программирования: C++17
- Сборка: CMake 3.11+
- Компилятор: GCC (MinGW-W64) 13.2.0+ или Clang 18.1.8+
- IDE: Visual Studio Code, CLion или любая другая с поддержкой CMake
