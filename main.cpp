#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <limits>
#include <map>
#include <numeric>
#include <tuple>
#include <cmath>

// константные значения для ползунка скорости
const float SLIDER_WIDTH = 300;
const float SLIDER_HEIGHT = 20;
const float KNOB_RADIUS = 15;
const std::vector<float> MARK_POSITIONS = { 1.0f, 1.5f, 1.75f, 2.0f, 2.5f };

const std::vector<std::string> MUSIC_FILES = {
    "music 1.mp3",
    "music 2.mp3",
    "music 3.mp3"
};

enum SimulationType { CLASSIC, COLORED, CHESS, EXIT };
//класс для создания ползунка для выбора скорости от 1.0 до 2.5 по определённым меткам
class Slider {
private:
    sf::RectangleShape track;
    sf::CircleShape knob;
    std::vector<sf::CircleShape> marks;
    float minValue;
    float maxValue;
    float currentValue;
    bool isDragging;

public:
    Slider(float x, float y, float min, float max)
        : minValue(min), maxValue(max), currentValue(min), isDragging(false) {

        //создание линии ползунка
        track.setSize(sf::Vector2f(SLIDER_WIDTH, SLIDER_HEIGHT));
        track.setPosition(x, y);
        track.setFillColor(sf::Color(100, 100, 100));

        //создание круга для перемещения
        knob.setRadius(KNOB_RADIUS);
        knob.setOrigin(KNOB_RADIUS, KNOB_RADIUS);
        knob.setFillColor(sf::Color::White);
        knob.setOutlineThickness(2);
        knob.setOutlineColor(sf::Color::Black);

        //выставление меток на линии ползунка
        for (float value : MARK_POSITIONS) {
            sf::CircleShape mark(5);
            mark.setOrigin(5, 5);
            mark.setFillColor(sf::Color::Black);
            marks.push_back(mark);
        }

        updateVisuals();
    }
    //обработка действий мыши с ползунком
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (event.type == sf::Event::MouseButtonPressed) {
            if (knob.getGlobalBounds().contains(mousePos)) {
                isDragging = true;
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased) {
            isDragging = false;
            snapToNearestMark();
        }
        else if (event.type == sf::Event::MouseMoved && isDragging) {
            float newX = std::max(track.getPosition().x,
                std::min(mousePos.x, track.getPosition().x + SLIDER_WIDTH));
            float normalized = (newX - track.getPosition().x) / SLIDER_WIDTH;
            currentValue = minValue + normalized * (maxValue - minValue);
            snapToNearestMark();
        }
    }
    //находим ближайшую отметку
    void snapToNearestMark() {
        float closest = MARK_POSITIONS[0];
        float minDist = std::abs(currentValue - closest);

        for (float mark : MARK_POSITIONS) {
            float dist = std::abs(currentValue - mark);
            if (dist < minDist) {
                minDist = dist;
                closest = mark;
            }
        }

        currentValue = closest;
        updateVisuals();
    }
    //обновляем информацию о местоположении 
    void updateVisuals() {
        float normalized = (currentValue - minValue) / (maxValue - minValue);
        float knobX = track.getPosition().x + normalized * SLIDER_WIDTH;
        knob.setPosition(knobX, track.getPosition().y + SLIDER_HEIGHT / 2);

        for (size_t i = 0; i < MARK_POSITIONS.size(); ++i) {
            float markNormalized = (MARK_POSITIONS[i] - minValue) / (maxValue - minValue);
            float markX = track.getPosition().x + markNormalized * SLIDER_WIDTH;
            marks[i].setPosition(markX, track.getPosition().y + SLIDER_HEIGHT + 15);
        }
    }
    //получение значений скорости
    float getValue() const {
        return currentValue;
    }
    // рисуем графические элементы
    void draw(sf::RenderWindow& window) {
        window.draw(track);

        for (const auto& mark : marks) {
            window.draw(mark);
        }

        window.draw(knob);

        // Draw value text (simplified version without font loading)
        sf::Text valueText;
        valueText.setString(std::to_string(currentValue));
        valueText.setCharacterSize(20);
        valueText.setFillColor(sf::Color::Black);
        valueText.setPosition(track.getPosition().x + SLIDER_WIDTH + 20,
            track.getPosition().y - 10);
        window.draw(valueText);
    }
};
//выбор типа симуляции
SimulationType chooseSimulationType(sf::RenderWindow& window) {
    sf::Font font;
    if (!font.loadFromFile("calibri.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return EXIT;
    }

    sf::Text title("Choose Simulation Type", font, 40);
    title.setFillColor(sf::Color(71, 74, 81));
    title.setPosition(window.getSize().x / 2 - title.getLocalBounds().width / 2,
        window.getSize().y / 3 - title.getLocalBounds().height / 2);

    sf::RectangleShape classicButton(sf::Vector2f(300, 80));
    classicButton.setPosition(window.getSize().x / 2 - 150, window.getSize().y / 2 - 120);
    classicButton.setFillColor(sf::Color(165, 165, 165));

    sf::Text classicText("Classic Game of Life", font, 30);
    classicText.setFillColor(sf::Color(71, 74, 81));
    classicText.setPosition(classicButton.getPosition().x + 30, classicButton.getPosition().y + 20);

    sf::RectangleShape coloredButton(sf::Vector2f(300, 80));
    coloredButton.setPosition(window.getSize().x / 2 - 150, classicButton.getPosition().y + 110);
    coloredButton.setFillColor(sf::Color(165, 165, 165));

    sf::Text coloredText("Colored Game of Life", font, 30);
    coloredText.setFillColor(sf::Color(71, 74, 81));
    coloredText.setPosition(coloredButton.getPosition().x + 30, coloredButton.getPosition().y + 20);

    sf::RectangleShape chessButton(sf::Vector2f(300, 80));
    chessButton.setPosition(window.getSize().x / 2 - 150, coloredButton.getPosition().y + 110);
    chessButton.setFillColor(sf::Color(165, 165, 165));

    sf::Text chessText("Chess Game of Life", font, 30);
    chessText.setFillColor(sf::Color(71, 74, 81));
    chessText.setPosition(chessButton.getPosition().x + 30, chessButton.getPosition().y + 20);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return EXIT;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (classicButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return CLASSIC;
                }
                else if (coloredButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return COLORED;
                }
                else if (chessButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return CHESS;
                }
            }
        }

        window.clear(sf::Color(220, 220, 220));
        window.draw(title);
        window.draw(classicButton);
        window.draw(classicText);
        window.draw(coloredButton);
        window.draw(coloredText);
        window.draw(chessButton);
        window.draw(chessText);
        window.display();
    }

    return EXIT;
}
//ввод размеров поля
void getInput(sf::RenderWindow& window, unsigned int screenWidth, unsigned int screenHeight, unsigned int size[], SimulationType& simType) {
    sf::Font font;
    if (!font.loadFromFile("calibri.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return;
    }

    sf::Text widthLabel("Enter the number of cells in width (1-150):", font, 24);
    float labelWidth = widthLabel.getLocalBounds().width;
    float inputBoxWidth = labelWidth + 40;
    float inputBoxHeight = 50;

    sf::RectangleShape widthInputBox(sf::Vector2f(inputBoxWidth, inputBoxHeight));
    widthInputBox.setFillColor(sf::Color(220, 220, 220));
    widthInputBox.setPosition(screenWidth / 2 - inputBoxWidth / 2, screenHeight / 2 - 100);
    widthInputBox.setOutlineThickness(2);
    widthInputBox.setOutlineColor(sf::Color(71, 74, 81));

    sf::RectangleShape heightInputBox(sf::Vector2f(inputBoxWidth, inputBoxHeight));
    heightInputBox.setFillColor(sf::Color(220, 220, 220));
    heightInputBox.setPosition(screenWidth / 2 - inputBoxWidth / 2, screenHeight / 2 + 40);
    heightInputBox.setOutlineThickness(2);
    heightInputBox.setOutlineColor(sf::Color(71, 74, 81));

    sf::Text widthText("", font, 24);
    widthText.setFillColor(sf::Color(71, 74, 81));
    widthText.setPosition(widthInputBox.getPosition().x + 10, widthInputBox.getPosition().y + 10);

    sf::Text heightText("", font, 24);
    heightText.setFillColor(sf::Color(71, 74, 81));
    heightText.setPosition(heightInputBox.getPosition().x + 10, heightInputBox.getPosition().y + 10);

    sf::Text heightLabel("Enter the number of cells in height (1-150):", font, 24);
    heightLabel.setFillColor(sf::Color(71, 74, 81));
    heightLabel.setPosition(screenWidth / 2 - heightLabel.getLocalBounds().width / 2,
        heightInputBox.getPosition().y - 45);

    widthLabel.setFillColor(sf::Color(71, 74, 81));
    widthLabel.setPosition(screenWidth / 2 - widthLabel.getLocalBounds().width / 2,
        widthInputBox.getPosition().y - 45);

    sf::Text errorText("PLEASE ENTER VALUES BETWEEN 1 AND 150", font, 30);
    errorText.setFillColor(sf::Color(128, 24, 24));
    errorText.setPosition(screenWidth / 2 - errorText.getLocalBounds().width / 2, screenHeight / 2 - 300);

    //поле видимости текста в поле ввода
    sf::View textView_1;
    textView_1.setViewport(sf::FloatRect(
        widthInputBox.getPosition().x / screenWidth,
        widthInputBox.getPosition().y / screenHeight,
        widthInputBox.getSize().x / screenWidth,
        widthInputBox.getSize().y / screenHeight
    ));
    textView_1.reset(sf::FloatRect(0, 0, widthInputBox.getSize().x, widthInputBox.getSize().y));

    sf::View textView_2;
    textView_2.setViewport(sf::FloatRect(
        heightInputBox.getPosition().x / screenWidth,
        heightInputBox.getPosition().y / screenHeight,
        heightInputBox.getSize().x / screenWidth,
        heightInputBox.getSize().y / screenHeight
    ));
    textView_2.reset(sf::FloatRect(0, 0, heightInputBox.getSize().x, heightInputBox.getSize().y));

    std::string widthInput, heightInput;
    bool widthActive = false;
    bool heightActive = false;
    bool widthComplete = false;
    bool heightComplete = false;
    bool showError = false;
    float widthScrollOffset = 0.0f;
    float heightScrollOffset = 0.0f;
    const float scrollSpeed = 10.0f; //смещение текста в полях ввода

    widthText.setPosition(10 - widthScrollOffset, 10);
    heightText.setPosition(10 - heightScrollOffset, 10);

    sf::RectangleShape backButton(sf::Vector2f(60, 60));
    backButton.setPosition(10, screenHeight - 180);
    backButton.setFillColor(sf::Color(165, 165, 165));

    sf::ConvexShape arrow;
    arrow.setPointCount(3);
    arrow.setPoint(0, sf::Vector2f(35, 15));
    arrow.setPoint(1, sf::Vector2f(15, 30));
    arrow.setPoint(2, sf::Vector2f(35, 45));
    arrow.setFillColor(sf::Color::Black);
    arrow.setPosition(backButton.getPosition().x, backButton.getPosition().y);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                simType = EXIT;
                window.close();
                return;
            }

            //выбор поля ввода
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

                    // проверка нажатия на кнопку "Назад"
                    if (backButton.getGlobalBounds().contains(mousePos)) {
                        window.close();
                        return;
                    }

                    // выбор поля ввода
                    widthActive = widthInputBox.getGlobalBounds().contains(mousePos) && !widthComplete;
                    heightActive = heightInputBox.getGlobalBounds().contains(mousePos) && !heightComplete;

                    widthInputBox.setOutlineColor(widthActive ? sf::Color::Blue :
                        (widthComplete ? sf::Color(106, 95, 49) : sf::Color(71, 74, 81)));
                    heightInputBox.setOutlineColor(heightActive ? sf::Color::Blue :
                        (heightComplete ? sf::Color(106, 95, 49) : sf::Color(71, 74, 81)));
                }
            }
            // ввод значений в поле ввоода
            if (event.type == sf::Event::TextEntered) {
                bool isDigit = (event.text.unicode >= '0' && event.text.unicode <= '9');
                bool isControlChar = (event.text.unicode == '\b' || event.text.unicode == '\r');
                //для первого поля
                if (widthActive && (isDigit || isControlChar)) {
                    //обработка события backspace
                    if (event.text.unicode == '\b') {
                        if (!widthInput.empty()) {
                            widthInput.pop_back();
                            widthScrollOffset = std::max(0.0f, widthScrollOffset - scrollSpeed);
                        }
                    }
                    //обработка события enter
                    else if (event.text.unicode == '\r') {
                        if (!widthInput.empty()) {
                            try {
                                int value = std::stoi(widthInput);
                                if (value >= 1 && value <= 150) {
                                    widthComplete = true;
                                    widthInputBox.setOutlineColor(sf::Color::Green);
                                    showError = false;
                                    size[0] = value;
                                }
                                else {
                                    showError = true;
                                }
                            }
                            catch (...) {
                                showError = true;
                            }
                        }
                    }
                    else {
                        widthInput += static_cast<char>(event.text.unicode);
                        if (widthText.getLocalBounds().width > widthInputBox.getSize().x - 20) {
                            widthScrollOffset = widthText.getLocalBounds().width - (widthInputBox.getSize().x - 20);
                        }
                    }
                    widthText.setString(widthInput);
                }
                //для второго поля
                else if (heightActive && (isDigit || isControlChar)) {
                    if (event.text.unicode == '\b') {
                        if (!heightInput.empty()) {
                            heightInput.pop_back();
                            heightScrollOffset = std::max(0.0f, heightScrollOffset - scrollSpeed);
                        }
                    }
                    else if (event.text.unicode == '\r') {
                        if (!heightInput.empty()) {
                            try {
                                int value = std::stoi(heightInput);
                                if (value >= 1 && value <= 150) {
                                    heightComplete = true;
                                    heightInputBox.setOutlineColor(sf::Color::Green);
                                    showError = false;
                                    size[1] = value;
                                }
                                else {
                                    showError = true;
                                }
                            }
                            catch (...) {
                                showError = true;
                            }
                        }
                    }
                    else {
                        heightInput += static_cast<char>(event.text.unicode);
                        if (heightText.getLocalBounds().width > heightInputBox.getSize().x - 20) {
                            heightScrollOffset = heightText.getLocalBounds().width - (heightInputBox.getSize().x - 20);
                        }
                    }
                    heightText.setString(heightInput);
                }
            }
            //движение текста в поле вввода
            if (event.type == sf::Event::KeyPressed) {
                if (widthActive) {
                    if (event.key.code == sf::Keyboard::Left) {
                        widthScrollOffset = std::max(0.0f, widthScrollOffset - scrollSpeed);
                    }
                    else if (event.key.code == sf::Keyboard::Right) {
                        float maxOffset = std::max(0.0f, widthText.getLocalBounds().width - (widthInputBox.getSize().x - 20));
                        widthScrollOffset = std::min(maxOffset, widthScrollOffset + scrollSpeed);
                    }
                    else if (event.key.code == sf::Keyboard::Delete) {
                        widthInput.clear();
                        widthScrollOffset = 0.0f;
                        widthText.setString("");
                    }
                }
                else if (heightActive) {
                    if (event.key.code == sf::Keyboard::Left) {
                        heightScrollOffset = std::max(0.0f, heightScrollOffset - scrollSpeed);
                    }
                    else if (event.key.code == sf::Keyboard::Right) {
                        float maxOffset = std::max(0.0f, heightText.getLocalBounds().width - (heightInputBox.getSize().x - 20));
                        heightScrollOffset = std::min(maxOffset, heightScrollOffset + scrollSpeed);
                    }
                    else if (event.key.code == sf::Keyboard::Delete) {
                        heightInput.clear();
                        heightScrollOffset = 0.0f;
                        heightText.setString("");
                    }
                }
            }
        }
        //вывод элементов графического интерфейса
        window.clear(sf::Color(220, 220, 220));

        window.draw(backButton);
        window.draw(arrow);
        window.draw(widthLabel);
        window.draw(widthInputBox);
        window.draw(heightLabel);
        window.draw(heightInputBox);

        if (widthActive || widthComplete) {
            window.setView(textView_1);
            widthText.setPosition(10 - widthScrollOffset, 10);
            window.draw(widthText);
            window.setView(window.getDefaultView());
        }

        if (heightActive || heightComplete) {
            window.setView(textView_2);
            heightText.setPosition(10 - heightScrollOffset, 10);
            window.draw(heightText);
            window.setView(window.getDefaultView());
        }

        if (showError) {
            window.draw(errorText);
        }

        if (widthComplete && heightComplete) {
            window.close();
        }

        window.display();
    }
}
//вычисление размера клетки в зависимости от размеров поля
int getCellSize(int cols, int rows, int screenWidth, int screenHeight) {
    int maxCellWidth = (screenWidth - 800) / cols;
    int maxCellHeight = (screenHeight - 200) / rows;
    return std::min(maxCellWidth, maxCellHeight);
}
//класс для правил чб симуляции
class ChessGameOfLife {
private:
    int rows, cols, CELL_SIZE;
    // 2D-вектор для хранения состояния клеток (0 - пусто, 1 - белая, 2 - черная)
    std::vector<std::vector<int>> grid;
    bool running = false;
    int generation = 0;
    int changes = 0;
    int currentColor = 1;

public:
    ChessGameOfLife(int rows, int cols, int CELL_SIZE) : rows(rows), cols(cols), CELL_SIZE(CELL_SIZE) {
        // инициализация сетки нулями
        grid.resize(rows, std::vector<int>(cols, 0));
    }
    // переключение состояния клетки
    void toggleCell(int x, int y) {
        if (grid[y][x] == 0) {
            grid[y][x] = currentColor;
        }
        else if (grid[y][x] == 1) {
            grid[y][x] = 2;
        }
        else {
            grid[y][x] = 0;
        }
    }
    // подсчет живых клеток каждого цвета
    std::pair<int, int> countLiveCells() const {
        int white = 0, black = 0;
        for (size_t i = 0; i < grid.size(); ++i) {
            for (size_t j = 0; j < grid[i].size(); ++j) {
                if (grid[i][j] == 1) white++;
                else if (grid[i][j] == 2) black++;
            }
        }
        return std::pair<int, int>(white, black);
    }
    // очистка игрового поля
    void clear() {
        for (auto& row : grid) {
            for (auto& cell : row) {
                cell = 0;
            }
        }
        generation = 0;
    }
    // вычисление следующего поколения клеток
    void change() {
        if (!running) return;

        std::vector<std::vector<int>> newGrid = grid;
        changes = 0;

        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                int whiteNeighbors = 0;
                int blackNeighbors = 0;

                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue;

                        int nx = (x + dx + cols) % cols;
                        int ny = (y + dy + rows) % rows;

                        if (grid[ny][nx] == 1) whiteNeighbors++;
                        else if (grid[ny][nx] == 2) blackNeighbors++;
                    }
                }

                int totalNeighbors = whiteNeighbors + blackNeighbors;
                int current = grid[y][x];
                int newState = 0;
                // правила игры:
                if (current == 1) {
                    if ((whiteNeighbors == 2 || whiteNeighbors == 3) && (blackNeighbors != 3)) {
                        newState = 1;
                    }
                }
                else if (current == 2) {
                    if ((blackNeighbors == 2 || blackNeighbors == 3) && (whiteNeighbors != 3)) {
                        newState = 2;
                    }
                }
                else {
                    if (whiteNeighbors == 3 && blackNeighbors != 3) {
                        newState = 1;
                    }
                    else if (blackNeighbors == 3 && whiteNeighbors != 3) {
                        newState = 2;
                    }
                }

                if (newState != current) {
                    changes++;
                    newGrid[y][x] = newState;
                }
            }
        }

        grid = newGrid;
        generation++;
    }
    // отрисовка игрового поля
    void draw(sf::RenderWindow& window) {
        sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));

        int offsetX = (window.getSize().x - cols * CELL_SIZE) / 2;
        int offsetY = (window.getSize().y - rows * CELL_SIZE) / 2;
        // отрисовка клеток
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                if (grid[y][x] == 1) {
                    cell.setFillColor(sf::Color::White);
                }
                else if (grid[y][x] == 2) {
                    cell.setFillColor(sf::Color::Black);
                }
                else {
                    cell.setFillColor(sf::Color(220, 220, 220));
                }
                cell.setPosition(x * CELL_SIZE + offsetX, y * CELL_SIZE + offsetY);
                window.draw(cell);
            }
        }
        // отрисовка сетки
        sf::RectangleShape line;
        line.setFillColor(sf::Color(71, 74, 81));

        for (int i = 0; i <= cols; i++) {
            line.setSize(sf::Vector2f(1, rows * CELL_SIZE));
            line.setPosition(i * CELL_SIZE + offsetX, offsetY);
            window.draw(line);
        }

        for (int i = 0; i <= rows; i++) {
            line.setSize(sf::Vector2f(cols * CELL_SIZE, 1));
            line.setPosition(offsetX, i * CELL_SIZE + offsetY);
            window.draw(line);
        }
    }

    void toggleSimulation() {
        running = !running;
    }

    bool isRunning() const {
        return running;
    }

    int getGeneration() const {
        return generation;
    }

    int getChanges() const {
        return changes;
    }

    int checkVictory() const {
        auto counts = countLiveCells();
        if (counts.first == 0 && counts.second == 0) return 0;
        if (counts.first == 0) return 2;
        if (counts.second == 0) return 1;
        return -1;
    }
};
//класс для правил цветной симуляции
class ColorGameOfLife {
private:
    int rows, cols, CELL_SIZE;
    std::vector<std::vector<sf::Color>> grid;
    bool running = false;
    int generation = 0;
    int changes = 0;
    sf::Color currentColor = sf::Color::Black;

    std::vector<sf::Color> getNeighbors(int x, int y) const {
        std::vector<sf::Color> neighbors;

        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;

                int nx = (x + dx + cols) % cols;
                int ny = (y + dy + rows) % rows;

                neighbors.push_back(grid[ny][nx]);
            }
        }
        return neighbors;
    }

    sf::Color averageColor(const std::vector<sf::Color>& colors) const {
        if (colors.empty()) return sf::Color::Transparent;

        int r = 0, g = 0, b = 0;
        int count = 0;

        for (const auto& color : colors) {
            if (color != sf::Color::Transparent) {
                r += color.r;
                g += color.g;
                b += color.b;
                count++;
            }
        }

        if (count == 0) return sf::Color::Transparent;

        return sf::Color(
            static_cast<sf::Uint8>(r / count),
            static_cast<sf::Uint8>(g / count),
            static_cast<sf::Uint8>(b / count)
        );
    }

public:
    ColorGameOfLife(int rows, int cols, int CELL_SIZE) : rows(rows), cols(cols), CELL_SIZE(CELL_SIZE) {
        grid.resize(rows, std::vector<sf::Color>(cols, sf::Color::Transparent));
    }

    void setCurrentColor(int r, int g, int b) {
        r = std::min(255, std::max(0, r));
        g = std::min(255, std::max(0, g));
        b = std::min(255, std::max(0, b));
        currentColor = sf::Color(r, g, b);
    }

    void toggleCell(int x, int y) {
        if (grid[y][x] == sf::Color::Transparent) {
            grid[y][x] = currentColor;
        }
        else {
            grid[y][x] = sf::Color::Transparent;
        }
    }

    int countLiveCells() const {
        int count = 0;
        for (const auto& row : grid) {
            for (const auto& cell : row) {
                if (cell != sf::Color::Transparent) count++;
            }
        }
        return count;
    }

    void clear() {
        for (auto& row : grid) {
            for (auto& cell : row) {
                if (cell != sf::Color::Transparent) {
                    cell = sf::Color::Transparent;
                }
            }
        }
        generation = 0;
    }

    void change() {
        if (!running) return;

        std::vector<std::vector<sf::Color>> newGrid = grid;
        changes = 0;

        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                auto neighbors = getNeighbors(x, y);
                int liveNeighbors = std::count_if(neighbors.begin(), neighbors.end(),
                    [](const sf::Color& c) { return c != sf::Color::Transparent; });

                sf::Color current = grid[y][x];
                sf::Color newColor = current;

                if (current != sf::Color::Transparent) {
                    if (liveNeighbors != 2 && liveNeighbors != 3) {
                        newColor = sf::Color::Transparent;
                    }
                }
                else {
                    if (liveNeighbors == 3) {
                        std::vector<sf::Color> liveColors;
                        for (const auto& color : neighbors) {
                            if (color != sf::Color::Transparent) {
                                liveColors.push_back(color);
                            }
                        }

                        if (liveColors.size() == 3) {
                            newColor = averageColor(liveColors);
                        }
                    }
                }

                if (newColor != current) {
                    changes++;
                    newGrid[y][x] = newColor;
                }
            }
        }

        grid = newGrid;
        generation++;
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));

        int offsetX = (window.getSize().x - cols * CELL_SIZE) / 2;
        int offsetY = (window.getSize().y - rows * CELL_SIZE) / 2;

        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                cell.setFillColor(grid[y][x]);
                cell.setPosition(x * CELL_SIZE + offsetX, y * CELL_SIZE + offsetY);
                window.draw(cell);
            }
        }

        sf::RectangleShape line;
        line.setFillColor(sf::Color(71, 74, 81));

        for (int i = 0; i <= cols; i++) {
            line.setSize(sf::Vector2f(1, rows * CELL_SIZE));
            line.setPosition(i * CELL_SIZE + offsetX, offsetY);
            window.draw(line);
        }

        for (int i = 0; i <= rows; i++) {
            line.setSize(sf::Vector2f(cols * CELL_SIZE, 1));
            line.setPosition(offsetX, i * CELL_SIZE + offsetY);
            window.draw(line);
        }

    }

    void toggleSimulation() {
        running = !running;
    }

    bool isRunning() const {
        return running;
    }

    int getGeneration() const {
        return generation;
    }

    int getChanges() const {
        return changes;
    }

    sf::Color getCurrentColor() const {
        return currentColor;
    }
};

class GameOfLife {
private:
    int rows, cols, CELL_SIZE;
    std::vector<std::vector<int>> grid;
    bool running = false;
    int generation = 0;
    int changes = 0;

public:
    GameOfLife(int rows, int cols, int CELL_SIZE) : rows(rows), cols(cols), CELL_SIZE(CELL_SIZE) {
        grid.resize(rows, std::vector<int>(cols, 0));
    }

    void toggleCell(int x, int y) {
        grid[y][x] = !grid[y][x];
    }

    void setCell(int x, int y, int state) {
        if (x >= 0 && x < cols && y >= 0 && y < rows) {
            grid[y][x] = state;
        }
    }

    int countLiveCells() const {
        int count = 0;
        for (const auto& row : grid) {
            for (int cell : row) {
                if (cell) count++;
            }
        }
        return count;
    }

    void clear() {
        for (auto& row : grid) {
            for (auto& cell : row) {
                if (cell) {
                    cell = false;
                }
            }
        }
        generation = 0;
    }

    void change() {
        if (!running) return;

        std::vector<std::vector<int>> duplicatedMatrix(rows + 2, std::vector<int>(cols + 2, 0));
        changes = 0;

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                duplicatedMatrix[i + 1][j + 1] = grid[i][j];
            }
        }

        for (int j = 0; j < cols; ++j) {
            duplicatedMatrix[duplicatedMatrix.size() - 1][j + 1] = grid[0][j];
            duplicatedMatrix[0][j + 1] = grid[rows - 1][j];
        }

        for (int i = 0; i < rows; ++i) {
            duplicatedMatrix[i + 1][duplicatedMatrix[0].size() - 1] = grid[i][0];
            duplicatedMatrix[i + 1][0] = grid[i][cols - 1];
        }

        duplicatedMatrix[0][0] = grid[rows - 1][cols - 1];
        duplicatedMatrix[0][duplicatedMatrix[0].size() - 1] = grid[rows - 1][0];
        duplicatedMatrix[duplicatedMatrix.size() - 1][0] = grid[0][cols - 1];
        duplicatedMatrix[duplicatedMatrix.size() - 1][duplicatedMatrix[0].size() - 1] = grid[0][0];

        for (int i = 1; i < duplicatedMatrix.size() - 1; ++i) {
            for (int j = 1; j < duplicatedMatrix[0].size() - 1; ++j) {
                int sum = duplicatedMatrix[i - 1][j] + duplicatedMatrix[i + 1][j] +
                    duplicatedMatrix[i][j + 1] + duplicatedMatrix[i][j - 1] +
                    duplicatedMatrix[i - 1][j - 1] + duplicatedMatrix[i - 1][j + 1] +
                    duplicatedMatrix[i + 1][j - 1] + duplicatedMatrix[i + 1][j + 1];

                if (sum == 3) {
                    if (grid[i - 1][j - 1] == 0) changes++;
                    grid[i - 1][j - 1] = 1;
                }
                else if (sum != 2) {
                    if (grid[i - 1][j - 1] == 1) changes++;
                    grid[i - 1][j - 1] = 0;
                }
            }
        }
        generation++;
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));

        int offsetX = (window.getSize().x - cols * CELL_SIZE) / 2;
        int offsetY = (window.getSize().y - rows * CELL_SIZE) / 2;

        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                cell.setFillColor(grid[y][x] ? sf::Color(71, 74, 81) : sf::Color(220, 220, 220));
                cell.setPosition(x * CELL_SIZE + offsetX, y * CELL_SIZE + offsetY);
                window.draw(cell);
            }
        }

        sf::RectangleShape line;
        line.setFillColor(sf::Color(71, 74, 81));

        for (int i = 0; i <= cols; i++) {
            line.setSize(sf::Vector2f(1, rows * CELL_SIZE));
            line.setPosition(i * CELL_SIZE + offsetX, offsetY);
            window.draw(line);
        }

        for (int i = 0; i <= rows; i++) {
            line.setSize(sf::Vector2f(cols * CELL_SIZE, 1));
            line.setPosition(offsetX, i * CELL_SIZE + offsetY);
            window.draw(line);
        }
    }

    void toggleSimulation() {
        running = !running;
    }

    bool isRunning() const {
        return running;
    }

    int getGeneration() const {
        return generation;
    }

    int getChanges() const {
        return changes;
    }

};
// класс для изменения количества строк и столбцов в сетке
class CellCountControl {
private:
    sf::RectangleShape rowsValueButton;
    sf::RectangleShape increaseRowsButton;
    sf::RectangleShape decreaseRowsButton;
    sf::Text rowsText;

    sf::RectangleShape colsValueButton;
    sf::RectangleShape increaseColsButton;
    sf::RectangleShape decreaseColsButton;
    sf::Text colsText;

    int currentRows;
    int currentCols;
    int minCount;
    int maxCount;

public:
    CellCountControl(float x, float y, int initialRows, int initialCols, int min, int max, const sf::Font& font)
        : currentRows(initialRows), currentCols(initialCols), minCount(min), maxCount(max) {

        decreaseRowsButton.setSize(sf::Vector2f(30, 30));
        decreaseRowsButton.setPosition(x, y);
        decreaseRowsButton.setFillColor(sf::Color(165, 165, 165));

        rowsValueButton.setSize(sf::Vector2f(120, 30));
        rowsValueButton.setPosition(x + decreaseRowsButton.getSize().x, y);
        rowsValueButton.setFillColor(sf::Color(71, 74, 81));

        increaseRowsButton.setSize(sf::Vector2f(30, 30));
        increaseRowsButton.setPosition(x + decreaseRowsButton.getSize().x + rowsValueButton.getSize().x, y);
        increaseRowsButton.setFillColor(sf::Color(165, 165, 165));

        rowsText.setString("Rows: " + std::to_string(currentRows));
        rowsText.setFont(font);
        rowsText.setCharacterSize(20);
        rowsText.setFillColor(sf::Color::White);
        rowsText.setPosition(
            static_cast<int>(rowsValueButton.getPosition().x + (rowsValueButton.getSize().x - rowsText.getLocalBounds().width) / 2),
            static_cast<int>(rowsValueButton.getPosition().y + (rowsValueButton.getSize().y - rowsText.getLocalBounds().height) / 2 - 5)
        );

        decreaseColsButton.setSize(sf::Vector2f(30, 30));
        decreaseColsButton.setPosition(x, y + 40);
        decreaseColsButton.setFillColor(sf::Color(165, 165, 165));

        colsValueButton.setSize(sf::Vector2f(120, 30));
        colsValueButton.setPosition(x + decreaseColsButton.getSize().x, y + 40);
        colsValueButton.setFillColor(sf::Color(71, 74, 81));

        increaseColsButton.setSize(sf::Vector2f(30, 30));
        increaseColsButton.setPosition(x + decreaseColsButton.getSize().x + colsValueButton.getSize().x, y + 40);
        increaseColsButton.setFillColor(sf::Color(165, 165, 165));

        colsText.setString("Cols: " + std::to_string(currentCols));
        colsText.setFont(font);
        colsText.setCharacterSize(20);
        colsText.setFillColor(sf::Color::White);
        colsText.setPosition(
            static_cast<int>(colsValueButton.getPosition().x + (colsValueButton.getSize().x - colsText.getLocalBounds().width) / 2),
            static_cast<int>(colsValueButton.getPosition().y + (colsValueButton.getSize().y - colsText.getLocalBounds().height) / 2 - 5)
        );
    }
    // обработка событий
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (increaseRowsButton.getGlobalBounds().contains(mousePos)) {
                if (currentRows < maxCount) {
                    currentRows++;
                    rowsText.setString("Rows: " + std::to_string(currentRows));
                }
            }
            else if (decreaseRowsButton.getGlobalBounds().contains(mousePos)) {
                if (currentRows > minCount) {
                    currentRows--;
                    rowsText.setString("Rows: " + std::to_string(currentRows));
                }
            }
            else if (increaseColsButton.getGlobalBounds().contains(mousePos)) {
                if (currentCols < maxCount) {
                    currentCols++;
                    colsText.setString("Cols: " + std::to_string(currentCols));
                }
            }
            else if (decreaseColsButton.getGlobalBounds().contains(mousePos)) {
                if (currentCols > minCount) {
                    currentCols--;
                    colsText.setString("Cols: " + std::to_string(currentCols));
                }
            }
        }
    }
    // отрисовка всех элементов управления
    void draw(sf::RenderWindow& window) {
        const sf::Font& font = *rowsText.getFont();

        window.draw(decreaseRowsButton);
        window.draw(rowsValueButton);
        window.draw(increaseRowsButton);
        window.draw(rowsText);

        window.draw(decreaseColsButton);
        window.draw(colsValueButton);
        window.draw(increaseColsButton);
        window.draw(colsText);

        sf::Text minusRowsText("-", font, 20);
        minusRowsText.setFillColor(sf::Color(71, 74, 81));
        minusRowsText.setPosition(
            decreaseRowsButton.getPosition().x + (decreaseRowsButton.getSize().x - minusRowsText.getLocalBounds().width) / 2,
            decreaseRowsButton.getPosition().y + (decreaseRowsButton.getSize().y - minusRowsText.getLocalBounds().height) / 2 - 5
        );
        window.draw(minusRowsText);

        sf::Text plusRowsText("+", font, 20);
        plusRowsText.setFillColor(sf::Color(71, 74, 81));
        plusRowsText.setPosition(
            increaseRowsButton.getPosition().x + (increaseRowsButton.getSize().x - plusRowsText.getLocalBounds().width) / 2,
            increaseRowsButton.getPosition().y + (increaseRowsButton.getSize().y - plusRowsText.getLocalBounds().height) / 2 - 5
        );
        window.draw(plusRowsText);

        sf::Text minusColsText("-", font, 20);
        minusColsText.setFillColor(sf::Color(71, 74, 81));
        minusColsText.setPosition(
            decreaseColsButton.getPosition().x + (decreaseColsButton.getSize().x - minusColsText.getLocalBounds().width) / 2,
            decreaseColsButton.getPosition().y + (decreaseColsButton.getSize().y - minusColsText.getLocalBounds().height) / 2 - 5
        );
        window.draw(minusColsText);

        sf::Text plusColsText("+", font, 20);
        plusColsText.setFillColor(sf::Color(71, 74, 81));
        plusColsText.setPosition(
            increaseColsButton.getPosition().x + (increaseColsButton.getSize().x - plusColsText.getLocalBounds().width) / 2,
            increaseColsButton.getPosition().y + (increaseColsButton.getSize().y - plusColsText.getLocalBounds().height) / 2 - 5
        );
        window.draw(plusColsText);
    }


    int getRows() const {
        return currentRows;
    }

    int getCols() const {
        return currentCols;
    }
};

int main() {
    sf::Font font;
    if (!font.loadFromFile("calibri.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    unsigned int screenWidth = desktopMode.width;
    unsigned int screenHeight = desktopMode.height;

    sf::Music music;
    int currentMusicIndex = -1;
    bool musicPlaying = false;
    bool showMusicMenu = false;

    while (true) {
        sf::RenderWindow typeWindow(sf::VideoMode(screenWidth, screenHeight), "Choose Simulation Type");
        SimulationType simType = chooseSimulationType(typeWindow);
        typeWindow.close();

        if (simType == EXIT) {
            break;
        }

        unsigned int size[2] = { 0, 0 };
        sf::RenderWindow inputWindow(sf::VideoMode(screenWidth, screenHeight), "Input Fields Example");
        getInput(inputWindow, screenWidth, screenHeight, size, simType);
        inputWindow.close();

        if (simType == EXIT) {
            break;
        }

        if (size[0] > 0 && size[1] > 0) {
            int cols = size[0], rows = size[1];
            int CELL_SIZE = getCellSize(cols, rows, screenWidth, screenHeight);

            sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight),
                simType == CLASSIC ? "The Game of Life" :
                simType == COLORED ? "Colored Game of Life" : "Chess Game of Life");

            GameOfLife* classicGame = nullptr;
            ColorGameOfLife* coloredGame = nullptr;
            ChessGameOfLife* chessGame = nullptr;

            if (simType == CLASSIC) {
                classicGame = new GameOfLife(rows, cols, CELL_SIZE);
            }
            else if (simType == COLORED) {
                coloredGame = new ColorGameOfLife(rows, cols, CELL_SIZE);
                coloredGame->setCurrentColor(255, 0, 0);
            }
            else {
                chessGame = new ChessGameOfLife(rows, cols, CELL_SIZE);
            }

            //общее
            int topMargin = (window.getSize().y - rows * CELL_SIZE) / 2;
            int buttonSpacing = 20;

            Slider slider(screenWidth - 400, 80, 1.0f, 2.5f);
            CellCountControl cellCountControl(screenWidth - 400, 180, rows, cols, 1, 150, font);

            sf::RectangleShape startButton(sf::Vector2f(200, 60));
            startButton.setPosition(50, 100);
            startButton.setFillColor(sf::Color(165, 165, 165));

            sf::Text startText("Start", font, 20);
            startText.setFillColor(sf::Color(71, 74, 81));
            startText.setPosition(
                static_cast<int>(startButton.getPosition().x + (startButton.getSize().x - startText.getLocalBounds().width) / 2),
                static_cast<int>(startButton.getPosition().y + (startButton.getSize().y - startText.getLocalBounds().height) / 2 - 5)
            );

            sf::RectangleShape generationButton(sf::Vector2f(200, 60));
            generationButton.setPosition(50, 180);
            generationButton.setFillColor(sf::Color(165, 165, 165));

            sf::Text generationText("Generation: 0", font, 20);
            generationText.setFillColor(sf::Color(71, 74, 81));
            generationText.setPosition(
                static_cast<int>(generationButton.getPosition().x + (generationButton.getSize().x - generationText.getLocalBounds().width) / 2),
                static_cast<int>(generationButton.getPosition().y + (generationButton.getSize().y - generationText.getLocalBounds().height) / 2 - 5)
            );

            sf::RectangleShape windowEnd(sf::Vector2f(800, 600));
            windowEnd.setPosition(screenWidth / 2 - 400, screenHeight / 2 - 300);
            windowEnd.setOutlineColor(sf::Color(71, 74, 81));
            windowEnd.setFillColor(sf::Color(165, 165, 165));
            windowEnd.setOutlineThickness(10);

            sf::Text textEnd("End of simulation", font, 40);
            textEnd.setFillColor(sf::Color(71, 74, 81));
            textEnd.setPosition(windowEnd.getPosition().x + 300, windowEnd.getPosition().y + 280);

            sf::RectangleShape Clear(sf::Vector2f(200, 60));
            Clear.setPosition(50, 260);
            Clear.setFillColor(sf::Color(165, 165, 165));

            sf::Text clearText("Clear", font, 20);
            clearText.setPosition(
                static_cast<int>(Clear.getPosition().x + (Clear.getSize().x - clearText.getLocalBounds().width) / 2),
                static_cast<int>(Clear.getPosition().y + (Clear.getSize().y - clearText.getLocalBounds().height) / 2 - 5)
            );
            clearText.setFillColor(sf::Color(71, 74, 81));

            sf::RectangleShape backButton(sf::Vector2f(60, 60));
            backButton.setPosition(10, screenHeight - 180);
            backButton.setFillColor(sf::Color(165, 165, 165));

            sf::ConvexShape arrow;
            arrow.setPointCount(3);
            arrow.setPoint(0, sf::Vector2f(35, 15));
            arrow.setPoint(1, sf::Vector2f(15, 30));
            arrow.setPoint(2, sf::Vector2f(35, 45));
            arrow.setFillColor(sf::Color::Black);
            arrow.setPosition(backButton.getPosition().x, backButton.getPosition().y);

            sf::Text victoryText("", font, 40);
            victoryText.setFillColor(sf::Color(71, 74, 81));
            victoryText.setPosition(windowEnd.getPosition().x + 200, windowEnd.getPosition().y + 250);

            sf::RectangleShape rulesButton(sf::Vector2f(200, 60));
            rulesButton.setPosition(screenWidth - 250, screenHeight - 180);
            rulesButton.setFillColor(sf::Color(165, 165, 165));

            sf::Text rulesText("Rules", font, 20);
            rulesText.setFillColor(sf::Color(71, 74, 81));
            rulesText.setPosition(
                static_cast<int>(rulesButton.getPosition().x + (rulesButton.getSize().x - rulesText.getLocalBounds().width) / 2),
                static_cast<int>(rulesButton.getPosition().y + (rulesButton.getSize().y - rulesText.getLocalBounds().height) / 2 - 5)
            );

            sf::RectangleShape musicButton(sf::Vector2f(200, 60));
            musicButton.setPosition(screenWidth - 250, screenHeight - 180 - 70);
            musicButton.setFillColor(sf::Color(165, 165, 165));

            sf::Text musicText("Music", font, 20);
            musicText.setFillColor(sf::Color(71, 74, 81));
            musicText.setPosition(
                static_cast<int>(musicButton.getPosition().x + (musicButton.getSize().x - musicText.getLocalBounds().width) / 2),
                static_cast<int>(musicButton.getPosition().y + (musicButton.getSize().y - musicText.getLocalBounds().height) / 2 - 5)
            );

            sf::RectangleShape music1Button(sf::Vector2f(200, 60));
            music1Button.setPosition(musicButton.getPosition().x, musicButton.getPosition().y - 70);
            music1Button.setFillColor(sf::Color(165, 165, 165));

            sf::Text music1Text("Music 1", font, 20);
            music1Text.setFillColor(sf::Color(71, 74, 81));
            music1Text.setPosition(
                static_cast<int>(music1Button.getPosition().x + (music1Button.getSize().x - music1Text.getLocalBounds().width) / 2),
                static_cast<int>(music1Button.getPosition().y + (music1Button.getSize().y - music1Text.getLocalBounds().height) / 2 - 5)
            );

            sf::RectangleShape music2Button(sf::Vector2f(200, 60));
            music2Button.setPosition(music1Button.getPosition().x, music1Button.getPosition().y - 70);
            music2Button.setFillColor(sf::Color(165, 165, 165));

            sf::Text music2Text("Music 2", font, 20);
            music2Text.setFillColor(sf::Color(71, 74, 81));
            music2Text.setPosition(
                static_cast<int>(music2Button.getPosition().x + (music2Button.getSize().x - music2Text.getLocalBounds().width) / 2),
                static_cast<int>(music2Button.getPosition().y + (music2Button.getSize().y - music2Text.getLocalBounds().height) / 2 - 5)
            );

            sf::RectangleShape music3Button(sf::Vector2f(200, 60));
            music3Button.setPosition(music2Button.getPosition().x, music2Button.getPosition().y - 70);
            music3Button.setFillColor(sf::Color(165, 165, 165));

            sf::Text music3Text("Music 3", font, 20);
            music3Text.setFillColor(sf::Color(71, 74, 81));
            music3Text.setPosition(
                static_cast<int>(music3Button.getPosition().x + (music3Button.getSize().x - music3Text.getLocalBounds().width) / 2),
                static_cast<int>(music3Button.getPosition().y + (music3Button.getSize().y - music3Text.getLocalBounds().height) / 2 - 5)
            );

            sf::RectangleShape toggleMusicButton(sf::Vector2f(200, 60));
            toggleMusicButton.setPosition(music3Button.getPosition().x, music3Button.getPosition().y - 70);
            toggleMusicButton.setFillColor(sf::Color(165, 165, 165));

            sf::Text toggleMusicText("Turn on", font, 20);
            toggleMusicText.setFillColor(sf::Color(71, 74, 81));
            toggleMusicText.setPosition(
                static_cast<int>(toggleMusicButton.getPosition().x + (toggleMusicButton.getSize().x - toggleMusicText.getLocalBounds().width) / 2),
                static_cast<int>(toggleMusicButton.getPosition().y + (toggleMusicButton.getSize().y - toggleMusicText.getLocalBounds().height) / 2 - 5)
            );

            // для цветной
            sf::RectangleShape colorInputBox(sf::Vector2f(200, 60));
            colorInputBox.setPosition(50, 420);
            colorInputBox.setFillColor(sf::Color(165, 165, 165));

            sf::Text colorText("Color (RGB)", font, 20);
            colorText.setFillColor(sf::Color(71, 74, 81));
            colorText.setPosition(
                static_cast<int>(colorInputBox.getPosition().x + (colorInputBox.getSize().x - colorText.getLocalBounds().width) / 2),
                static_cast<int>(colorInputBox.getPosition().y + (colorInputBox.getSize().y - colorText.getLocalBounds().height) / 2 - 5)
            );

            sf::RectangleShape colorPreview(sf::Vector2f(50, 50));
            colorPreview.setPosition(260, 425);
            if (simType == COLORED) {
                colorPreview.setFillColor(coloredGame->getCurrentColor());
            }

            // для одноцветной
            sf::RectangleShape setPattern(sf::Vector2f(200, 60));
            setPattern.setPosition(50, 340);
            setPattern.setFillColor(sf::Color(165, 165, 165));

            sf::Text patternsText("Patterns", font, 20);
            patternsText.setFillColor(sf::Color(71, 74, 81));
            patternsText.setPosition(
                static_cast<int>(setPattern.getPosition().x + (setPattern.getSize().x - patternsText.getLocalBounds().width) / 2),
                static_cast<int>(setPattern.getPosition().y + (setPattern.getSize().y - patternsText.getLocalBounds().height) / 2 - 5)
            );

            sf::RectangleShape patternBlock(sf::Vector2f(200, 60));
            patternBlock.setPosition(setPattern.getPosition().x, setPattern.getPosition().y + setPattern.getSize().y + 10);
            patternBlock.setFillColor(sf::Color(165, 165, 165));

            sf::Text blockText("Block", font, 20);
            blockText.setFillColor(sf::Color(71, 74, 81));
            blockText.setPosition(
                static_cast<int>(patternBlock.getPosition().x + (patternBlock.getSize().x - blockText.getLocalBounds().width) / 2),
                static_cast<int>(patternBlock.getPosition().y + (patternBlock.getSize().y - blockText.getLocalBounds().height) / 2 - 5)
            );

            sf::RectangleShape patternGlider(sf::Vector2f(200, 60));
            patternGlider.setPosition(setPattern.getPosition().x, patternBlock.getPosition().y + patternBlock.getSize().y + 10);
            patternGlider.setFillColor(sf::Color(165, 165, 165));

            sf::Text gliderText("Glider", font, 20);
            gliderText.setFillColor(sf::Color(71, 74, 81));
            gliderText.setPosition(
                static_cast<int>(patternGlider.getPosition().x + (patternGlider.getSize().x - gliderText.getLocalBounds().width) / 2),
                static_cast<int>(patternGlider.getPosition().y + (patternGlider.getSize().y - gliderText.getLocalBounds().height) / 2 - 5)
            );

            sf::RectangleShape patternEight(sf::Vector2f(200, 60));
            patternEight.setPosition(setPattern.getPosition().x, patternGlider.getPosition().y + patternGlider.getSize().y + 10);
            patternEight.setFillColor(sf::Color(165, 165, 165));

            sf::Text eightText("Eight", font, 20);
            eightText.setFillColor(sf::Color(71, 74, 81));
            eightText.setPosition(
                static_cast<int>(patternEight.getPosition().x + (patternEight.getSize().x - eightText.getLocalBounds().width) / 2),
                static_cast<int>(patternEight.getPosition().y + (patternEight.getSize().y - eightText.getLocalBounds().height) / 2 - 5)
            );

            sf::Text Error("The field is too small for this pattern", font, 20);
            Error.setPosition(screenWidth / 2, 100);
            Error.setFillColor(sf::Color(128, 24, 24));

            //правила
            sf::RectangleShape ruleWindow(sf::Vector2f(800, 600));
            ruleWindow.setPosition(screenWidth / 2 - 400, screenHeight / 2 - 300);
            ruleWindow.setFillColor(sf::Color(165, 165, 165));
            ruleWindow.setOutlineThickness(10);
            ruleWindow.setOutlineColor(sf::Color(71, 74, 81));

            sf::Text ruleClassicText("Survival:\n    A live cell remains alive if it has 2 or 3 live neighbors.\nDeath:\n    From loneliness(fewer than 2 neighbors)\n    From overpopulation(more than 3 neighbors)\nBirth:\n    A dead cell comes to life if it has exactly 3 live neighbors.", font, 28);
            ruleClassicText.setPosition(ruleWindow.getPosition().x + 100, ruleWindow.getPosition().y + 120);
            ruleClassicText.setFillColor(sf::Color(71, 74, 81));

            sf::Text ruleColoredText("Survival:\n    A live cell remains alive if it has 2 or 3 live neighbors.\n   Its color is determined by the average color of its neighbors\nDeath:\n    From loneliness (fewer than 2 neighbors)\n    From overpopulation (more than 3 neighbors)\nBirth:\n    A dead cell comes to life if it has exactly 3 live neighbors.\n Its color is determined by the average color of its neighbors", font, 28);
            ruleColoredText.setPosition(ruleWindow.getPosition().x + 100, ruleWindow.getPosition().y + 120);
            ruleColoredText.setFillColor(sf::Color(71, 74, 81));

            sf::Text ruleChessText("White cells:\n    Survive with 2 or 3 white neighbors\n    Born with exactly 3 white neighbors\nBlack cells:\n    Survive with 2 or 3 black neighbors\n    Born with exactly 3 black neighbors\nDeath:\n    Any cell dies with fewer than 2 or more than 3 neighbors", font, 28);
            ruleChessText.setPosition(ruleWindow.getPosition().x + 100, ruleWindow.getPosition().y + 120);
            ruleChessText.setFillColor(sf::Color(71, 74, 81));

            //для шахматной
            sf::RectangleShape whiteCountBox(sf::Vector2f(200, 60));
            whiteCountBox.setPosition(50, 420);
            whiteCountBox.setFillColor(sf::Color(165, 165, 165));

            sf::Text whiteCountText("White: 0", font, 20);
            whiteCountText.setFillColor(sf::Color(71, 74, 81));
            whiteCountText.setPosition(
                static_cast<int>(whiteCountBox.getPosition().x + (whiteCountBox.getSize().x - whiteCountText.getLocalBounds().width) / 2),
                static_cast<int>(whiteCountBox.getPosition().y + (whiteCountBox.getSize().y - whiteCountText.getLocalBounds().height) / 2 - 5)
            );

            sf::RectangleShape blackCountBox(sf::Vector2f(200, 60));
            blackCountBox.setPosition(50, 500);
            blackCountBox.setFillColor(sf::Color(165, 165, 165));

            sf::Text blackCountText("Black: 0", font, 20);
            blackCountText.setFillColor(sf::Color(71, 74, 81));
            blackCountText.setPosition(
                static_cast<int>(blackCountBox.getPosition().x + (blackCountBox.getSize().x - blackCountText.getLocalBounds().width) / 2),
                static_cast<int>(blackCountBox.getPosition().y + (blackCountBox.getSize().y - blackCountText.getLocalBounds().height) / 2 - 5)
            );

            sf::Clock clock;
            bool simulationRunning = false;
            bool showPatternMenu = false;
            bool showError = false;
            bool showColorDialog = false;
            bool endSim = false;
            bool showRules = false;
            std::string colorInput = "255,0,0";

            while (window.isOpen()) {
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                        return 0;
                    }
                    if (event.type == sf::Event::MouseButtonPressed) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            window.close();
                            break;
                        }
                        else if (rulesButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            showRules = !showRules;
                        }
                        if (musicButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            showMusicMenu = !showMusicMenu;
                        }
                        else if (showMusicMenu) {
                            if (music1Button.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                if (music.openFromFile(MUSIC_FILES[0])) {
                                    currentMusicIndex = 0;
                                    music.setLoop(true);
                                    music.play();
                                    musicPlaying = true;
                                    toggleMusicText.setString("Turn off");
                                }
                            }
                            else if (music2Button.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                if (music.openFromFile(MUSIC_FILES[1])) {
                                    currentMusicIndex = 1;
                                    music.setLoop(true);
                                    music.play();
                                    musicPlaying = true;
                                    toggleMusicText.setString("Turn off");
                                }
                            }
                            else if (music3Button.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                if (music.openFromFile(MUSIC_FILES[2])) {
                                    currentMusicIndex = 2;
                                    music.setLoop(true);
                                    music.play();
                                    musicPlaying = true;
                                    toggleMusicText.setString("Turn off");
                                }
                            }
                            else if (toggleMusicButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                if (musicPlaying) {
                                    music.pause();
                                    toggleMusicText.setString("Turn on");
                                }
                                else if (currentMusicIndex != -1) {
                                    music.play();
                                    toggleMusicText.setString("Turn off");
                                }
                                musicPlaying = !musicPlaying;
                            }
                        }
                    }
                    slider.handleEvent(event, window);
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q) {
                        showRules = !showRules;
                    }
                    cellCountControl.handleEvent(event, window);
                    int newRows = cellCountControl.getRows();
                    int newCols = cellCountControl.getCols();
                    if (newRows != rows || newCols != cols) {
                        rows = newRows;
                        cols = newCols;
                        CELL_SIZE = getCellSize(cols, rows, screenWidth, screenHeight);

                        if (simType == CLASSIC) {
                            delete classicGame;
                            classicGame = new GameOfLife(rows, cols, CELL_SIZE);
                        }
                        else if (simType == COLORED) {
                            delete coloredGame;
                            coloredGame = new ColorGameOfLife(rows, cols, CELL_SIZE);
                            coloredGame->setCurrentColor(255, 0, 0);
                        }
                        else {
                            delete chessGame;
                            chessGame = new ChessGameOfLife(rows, cols, CELL_SIZE);
                        }
                    }

                    if (!endSim && event.type == sf::Event::MouseButtonPressed) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            simulationRunning = !simulationRunning;
                            startText.setString(simulationRunning ? "Stop" : "Start");
                            if (simType == CLASSIC) {
                                classicGame->toggleSimulation();
                            }
                            else if (simType == COLORED) {
                                coloredGame->toggleSimulation();
                            }
                            else {
                                chessGame->toggleSimulation();
                            }
                        }
                        else if (simType == COLORED && colorInputBox.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            showColorDialog = true;
                            colorInput = "";
                        }
                        else if (setPattern.getGlobalBounds().contains(mousePos.x, mousePos.y) && simType == CLASSIC) {
                            showPatternMenu = !showPatternMenu;
                        }
                        else if (Clear.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            if (simType == CLASSIC) {
                                classicGame->clear();
                            }
                            else if (simType == COLORED) {
                                coloredGame->clear();
                            }
                            else {
                                chessGame->clear();
                            }
                        }
                        else if (showPatternMenu && simType == CLASSIC) {
                            if (patternBlock.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                if (rows >= 2 && cols >= 2) {
                                    showError = false;
                                    classicGame->clear();
                                    classicGame->setCell(cols / 2, rows / 2, 1);
                                    classicGame->setCell(cols / 2, rows / 2 - 1, 1);
                                    classicGame->setCell(cols / 2 - 1, rows / 2, 1);
                                    classicGame->setCell(cols / 2 - 1, rows / 2 - 1, 1);
                                    showPatternMenu = false;
                                }
                                else {
                                    showError = true;
                                }
                            }
                            else if (patternGlider.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                if (rows >= 3 && cols >= 3) {
                                    showError = false;
                                    classicGame->clear();
                                    classicGame->setCell(1, 0, 1);
                                    classicGame->setCell(2, 1, 1);
                                    classicGame->setCell(0, 2, 1);
                                    classicGame->setCell(1, 2, 1);
                                    classicGame->setCell(2, 2, 1);
                                    showPatternMenu = false;
                                }
                                else {
                                    showError = true;
                                }
                            }
                            else if (patternEight.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                if (rows >= 8 && cols >= 8) {
                                    classicGame->clear();
                                    int numEight = 2;
                                    showError = false;
                                    for (int i = 0; i <= numEight; i++) {
                                        for (int j = 0; j <= numEight; j++) {
                                            classicGame->setCell(cols / 2 - i - 1, rows / 2 - j - 1, 1);
                                            classicGame->setCell(cols / 2 + i, rows / 2 + j, 1);
                                        }
                                    }
                                    showPatternMenu = false;
                                }
                                else {
                                    showError = true;
                                }
                            }
                        }
                        else {
                            int offsetX = (window.getSize().x - cols * CELL_SIZE) / 2;
                            int offsetY = (window.getSize().y - rows * CELL_SIZE) / 2;

                            if (mousePos.x >= offsetX && mousePos.y >= offsetY) {
                                int x = (mousePos.x - offsetX) / CELL_SIZE;
                                int y = (mousePos.y - offsetY) / CELL_SIZE;

                                if (x >= 0 && x < cols && y >= 0 && y < rows) {
                                    if (simType == CLASSIC) {
                                        classicGame->toggleCell(x, y);
                                    }
                                    else if (simType == COLORED) {
                                        coloredGame->toggleCell(x, y);
                                    }
                                    else {
                                        chessGame->toggleCell(x, y);
                                    }
                                }
                            }
                        }
                    }
                    else {
                        if (event.type == sf::Event::MouseButtonPressed) {
                            endSim = false;
                            if (simType == CLASSIC) {
                                classicGame->clear();
                            }
                            else if (simType == COLORED) {
                                coloredGame->clear();
                            }
                            else {
                                chessGame->clear();
                            }
                        }
                    }
                    if (showColorDialog && event.type == sf::Event::TextEntered) {
                        if (event.text.unicode == '\r') {
                            showColorDialog = false;
                            if (simType == COLORED) {
                                std::istringstream iss(colorInput);
                                int r, g, b;
                                char comma;
                                if (iss >> r >> comma >> g >> comma >> b) {
                                    coloredGame->setCurrentColor(r, g, b);
                                    colorPreview.setFillColor(coloredGame->getCurrentColor());
                                }
                            }
                        }
                        else if (event.text.unicode == '\b') {
                            if (!colorInput.empty()) colorInput.pop_back();
                        }
                        else if (event.text.unicode >= '0' && event.text.unicode <= '9' || event.text.unicode == ',') {
                            colorInput += static_cast<char>(event.text.unicode);
                        }
                    }
                }

                if (simulationRunning && clock.getElapsedTime().asSeconds() >= 1.0f / slider.getValue()) {
                    if (simType == CLASSIC) {
                        classicGame->change();
                        if (classicGame->countLiveCells() == 0) {
                            simulationRunning = false;
                            classicGame->toggleSimulation();
                            startText.setString("Start");
                            endSim = true;
                        }
                    }
                    else if (simType == COLORED) {
                        coloredGame->change();
                        if (coloredGame->countLiveCells() == 0) {
                            simulationRunning = false;
                            coloredGame->toggleSimulation();
                            startText.setString("Start");
                            endSim = true;
                        }
                    }
                    else {
                        chessGame->change();
                        auto counts = chessGame->countLiveCells();
                        whiteCountText.setString("White: " + std::to_string(counts.first));
                        blackCountText.setString("Black: " + std::to_string(counts.second));

                        int victory = chessGame->checkVictory();
                        if (victory != -1) {
                            simulationRunning = false;
                            chessGame->toggleSimulation();
                            startText.setString("Start");
                            endSim = true;

                            if (victory == 0) {
                                victoryText.setString("All cells died!");
                            }
                            else if (victory == 1) {
                                victoryText.setString("White cells won!");
                            }
                            else {
                                victoryText.setString("Black cells won!");
                            }
                            victoryText.setPosition(windowEnd.getPosition().x + 150, windowEnd.getPosition().y + 250);
                        }
                    }
                    clock.restart();
                    generationText.setString("Generation: " + std::to_string(
                        simType == CLASSIC ? classicGame->getGeneration() :
                        simType == COLORED ? coloredGame->getGeneration() : chessGame->getGeneration()));
                }

                window.clear(sf::Color(220, 220, 220));

                if (simType == CLASSIC) {
                    classicGame->draw(window);
                }
                else if (simType == COLORED) {
                    coloredGame->draw(window);
                }
                else {
                    chessGame->draw(window);
                }

                window.draw(backButton);
                window.draw(arrow);
                window.draw(rulesButton);
                window.draw(rulesText);
                window.draw(startButton);
                window.draw(startText);
                window.draw(generationButton);
                window.draw(generationText);
                window.draw(Clear);
                window.draw(clearText);
                slider.draw(window);
                cellCountControl.draw(window);
                window.draw(musicButton);
                window.draw(musicText);

                if (showMusicMenu) {
                    window.draw(music1Button);
                    window.draw(music1Text);
                    window.draw(music2Button);
                    window.draw(music2Text);
                    window.draw(music3Button);
                    window.draw(music3Text);
                    window.draw(toggleMusicButton);
                    window.draw(toggleMusicText);
                }

                if (simType == COLORED) {
                    window.draw(colorInputBox);
                    window.draw(colorText);
                    window.draw(colorPreview);

                    if (showColorDialog) {
                        sf::RectangleShape dialog(sf::Vector2f(300, 100));
                        dialog.setFillColor(sf::Color(200, 200, 200));
                        dialog.setPosition(window.getSize().x / 2 - 150, window.getSize().y / 2 - 50);

                        sf::Text prompt("Enter RGB (e.g. 255,0,0):", font, 20);
                        prompt.setFillColor(sf::Color::Black);
                        prompt.setPosition(dialog.getPosition().x + 10, dialog.getPosition().y + 10);

                        sf::Text inputText(colorInput, font, 20);
                        inputText.setFillColor(sf::Color::Black);
                        inputText.setPosition(dialog.getPosition().x + 10, dialog.getPosition().y + 40);

                        window.draw(dialog);
                        window.draw(prompt);
                        window.draw(inputText);
                    }

                    if (showRules) {
                        window.draw(ruleWindow);
                        window.draw(ruleColoredText);
                    }
                }
                else if (simType == CHESS) {
                    window.draw(whiteCountBox);
                    window.draw(whiteCountText);
                    window.draw(blackCountBox);
                    window.draw(blackCountText);

                    if (showRules) {
                        window.draw(ruleWindow);
                        window.draw(ruleChessText);
                    }
                }
                else {
                    window.draw(setPattern);
                    window.draw(patternsText);

                    if (showPatternMenu) {
                        window.draw(patternBlock);
                        window.draw(patternGlider);
                        window.draw(blockText);
                        window.draw(gliderText);
                        window.draw(patternEight);
                        window.draw(eightText);
                    }

                    if (showError) {
                        window.draw(Error);
                    }

                    if (showRules) {
                        window.draw(ruleWindow);
                        window.draw(ruleClassicText);
                    }


                }

                if (endSim) {
                    window.draw(windowEnd);
                    window.draw(textEnd);
                    if (simType == CHESS) {
                        window.draw(victoryText);
                    }
                }

                window.display();
            }
            if (classicGame) delete classicGame;
            if (coloredGame) delete coloredGame;
            if (chessGame) delete chessGame;
        }
    }

    return 0;
}
