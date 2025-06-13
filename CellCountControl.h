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
