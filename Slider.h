#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <limits>
#include <map>
#include <numeric>
#include <tuple>
#include <cmath>

const float SLIDER_WIDTH = 300;
const float SLIDER_HEIGHT = 20;
const float KNOB_RADIUS = 15;
const std::vector<float> MARK_POSITIONS = { 1.0f, 1.5f, 1.75f, 2.0f, 2.5f };


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
