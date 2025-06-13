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
