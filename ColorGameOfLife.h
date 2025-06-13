#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <limits>
#include <map>
#include <numeric>
#include <tuple>
#include <cmath>


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
        line.setFillColor(sf::Color(100, 100, 100, 50));

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