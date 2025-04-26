#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <limits>

const int CELL_SIZE = 30;
const float UPDATE_TIME = 1.0f;
const int PADDING = 50;

void getInput(sf::RenderWindow& window, unsigned int screenWidth, unsigned int screenHeight, unsigned int size[]) {
    sf::Font font;
    if (!font.loadFromFile("calibri.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return;
    }

    sf::RectangleShape widthInputBox(sf::Vector2f(300, 50));
    widthInputBox.setFillColor(sf::Color(220, 220, 220));
    widthInputBox.setPosition(screenWidth / 2 - 150, screenHeight / 2 - 160);
    widthInputBox.setOutlineThickness(2);
    widthInputBox.setOutlineColor(sf::Color(71, 74, 81));

    sf::RectangleShape heightInputBox(sf::Vector2f(300, 50));
    heightInputBox.setFillColor(sf::Color(220, 220, 220));
    heightInputBox.setPosition(screenWidth / 2 - 150, screenHeight / 2 + 120);
    heightInputBox.setOutlineThickness(2);
    heightInputBox.setOutlineColor(sf::Color(71, 74, 81));

    sf::Text widthText("", font, 24);
    widthText.setFillColor(sf::Color(71, 74, 81));
    widthText.setPosition(widthInputBox.getPosition().x + 10, widthInputBox.getPosition().y + 10);

    sf::Text heightText("", font, 24);
    heightText.setFillColor(sf::Color(71, 74, 81));
    heightText.setPosition(heightInputBox.getPosition().x + 10, heightInputBox.getPosition().y + 10);

    sf::Text widthLabel("Enter the number of cells in width (1-25):", font, 24);
    widthLabel.setFillColor(sf::Color(71, 74, 81));
    widthLabel.setPosition(widthInputBox.getPosition().x, widthInputBox.getPosition().y - 35);

    sf::Text heightLabel("Enter the number of cells in height (1-25):", font, 24);
    heightLabel.setFillColor(sf::Color(71, 74, 81));
    heightLabel.setPosition(heightInputBox.getPosition().x, heightInputBox.getPosition().y - 35);

    sf::Text errorText("Please enter values between 1 and 25", font, 24);
    errorText.setFillColor(sf::Color(128, 24, 24));
    errorText.setPosition(screenWidth / 2 - errorText.getLocalBounds().width / 2, screenHeight / 2 - 500);

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
    const float scrollSpeed = 10.0f;

    widthText.setPosition(10 - widthScrollOffset, 10);
    heightText.setPosition(10 - heightScrollOffset, 10);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

                    widthActive = widthInputBox.getGlobalBounds().contains(mousePos) && !widthComplete;
                    heightActive = heightInputBox.getGlobalBounds().contains(mousePos) && !heightComplete;

                    widthInputBox.setOutlineColor(widthActive ? sf::Color::Blue :
                        (widthComplete ? sf::Color(106, 95, 49) : sf::Color(71, 74, 81)));
                    heightInputBox.setOutlineColor(heightActive ? sf::Color::Blue :
                        (heightComplete ? sf::Color(106, 95, 49) : sf::Color(71, 74, 81)));
                }
            }

            if (event.type == sf::Event::TextEntered) {
                bool isDigit = (event.text.unicode >= '0' && event.text.unicode <= '9');
                bool isControlChar = (event.text.unicode == '\b' || event.text.unicode == '\r');

                if (widthActive && (isDigit || isControlChar)) {
                    if (event.text.unicode == '\b') {
                        if (!widthInput.empty()) {
                            widthInput.pop_back();
                            widthScrollOffset = std::max(0.0f, widthScrollOffset - scrollSpeed);
                        }
                    }
                    else if (event.text.unicode == '\r') {
                        if (!widthInput.empty()) {
                            try {
                                int value = std::stoi(widthInput);
                                if (value >= 1 && value <= 25) {
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
                                if (value >= 1 && value <= 25) {
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

        window.clear(sf::Color(220, 220, 220));

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

class GameOfLife {
private:
    int rows, cols;
    std::vector<std::vector<int>> grid;
    bool running = false;
    int generation = 0;
    int changes = 0;

public:
    GameOfLife(int rows, int cols) : rows(rows), cols(cols) {
        grid.resize(rows, std::vector<int>(cols, 0));
    }

    void toggleCell(int x, int y) {
        grid[y][x] = !grid[y][x];
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

int main() {
    sf::Font font;
    if (!font.loadFromFile("calibri.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    unsigned int size[2];
    unsigned int screenWidth = desktopMode.width;
    unsigned int screenHeight = desktopMode.height;
    sf::RenderWindow Inputwindow(sf::VideoMode(screenWidth, screenHeight), "Input Fields Example");
    getInput(Inputwindow, screenWidth, screenHeight, size);
    int cols = size[0], rows = size[1];
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "The Game of Life");
    GameOfLife game(rows, cols);

    sf::Text startText("Start", font, 20);
    startText.setFillColor(sf::Color(71, 74, 81));
    startText.setPosition(20, 25);

    sf::RectangleShape startButton(sf::Vector2f(200, 60));
    startButton.setPosition(10, 10);
    startButton.setFillColor(sf::Color(165, 165, 165));

    sf::RectangleShape generationButton(sf::Vector2f(200, 60));
    generationButton.setPosition(10, 500);
    generationButton.setFillColor(sf::Color(165, 165, 165));

    sf::Text generationText("", font, 20);
    generationText.setFillColor(sf::Color(71, 74, 81));
    generationText.setPosition(generationButton.getPosition().x + 50, generationButton.getPosition().y + 20);

    sf::RectangleShape setPattern(sf::Vector2f(200, 60));
    setPattern.setPosition(10, 80);
    setPattern.setFillColor(sf::Color(165, 165, 165));

    sf::Text patternsText("Patterns", font, 20);
    patternsText.setFillColor(sf::Color(71, 74, 81));
    patternsText.setPosition(setPattern.getPosition().x + 50, setPattern.getPosition().y + 20);

    sf::RectangleShape patternBlock(sf::Vector2f(200, 60));
    patternBlock.setPosition(10, 150);
    patternBlock.setFillColor(sf::Color(165, 165, 165));

    sf::Text blockText("Block", font, 20);
    blockText.setFillColor(sf::Color(71, 74, 81));
    blockText.setPosition(70, 165);

    sf::RectangleShape patternGlider(sf::Vector2f(200, 60));
    patternGlider.setPosition(10, 220);
    patternGlider.setFillColor(sf::Color(165, 165, 165));

    sf::Text gliderText("Glider", font, 20);
    gliderText.setFillColor(sf::Color(71, 74, 81));
    gliderText.setPosition(70, 235);

    sf::RectangleShape patternEight(sf::Vector2f(200, 60));
    patternEight.setPosition(10, 290);
    patternEight.setFillColor(sf::Color(165, 165, 165));

    sf::Text eightText("Eight", font, 20);
    eightText.setFillColor(sf::Color(71, 74, 81));
    eightText.setPosition(70, 305);

    sf::RectangleShape endScreen(sf::Vector2f(400, 200));
    endScreen.setFillColor(sf::Color(165, 165, 165));
    endScreen.setOutlineThickness(5);
    endScreen.setOutlineColor(sf::Color(71, 74, 81));
    endScreen.setPosition((screenWidth - 400) / 2, (screenHeight - 200) / 2);

    sf::Text Error("The field is too small for this pattern", font, 20);
    Error.setPosition(screenWidth/2, 100);
    Error.setFillColor(sf::Color(128, 24, 24));

    sf::Text endText("Simulation is ended", font, 40);
    endText.setFillColor(sf::Color(71, 74, 81));
    endText.setPosition(
        endScreen.getPosition().x + (endScreen.getSize().x - endText.getLocalBounds().width) / 2,
        endScreen.getPosition().y + (endScreen.getSize().y - endText.getLocalBounds().height) / 2 -20
    );

    bool simulationEnded = false;
    bool showPatternMenu = false;
    bool showError = false;

    sf::Clock clock;
    bool isMousePressed = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }


            if (!simulationEnded && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                int offsetX = (window.getSize().x - cols * CELL_SIZE) / 2;
                int offsetY = (window.getSize().y - rows * CELL_SIZE) / 2;

                if (setPattern.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    showPatternMenu = !showPatternMenu;
                }

                if (mousePos.y >= offsetY) {
                    showError = false;
                    int x = (mousePos.x - offsetX) / CELL_SIZE;
                    int y = (mousePos.y - offsetY) / CELL_SIZE;
                    if (x >= 0 && x < cols && y >= 0 && y < rows) {
                        game.toggleCell(x, y);
                    }
                }
                else if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    showError = false;
                    game.toggleSimulation();
                    if (game.isRunning()) {
                        startText.setString("Stop");
                    }
                    else {
                        startText.setString("Start");
                    }
                }
                else if (showPatternMenu) {
                    if (patternBlock.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (rows >= 2 && cols >= 2) {
                            showError = false;
                            game.toggleCell(cols / 2, rows / 2);
                            game.toggleCell(cols / 2, rows / 2 - 1);
                            game.toggleCell(cols / 2 - 1, rows / 2);
                            game.toggleCell(cols / 2 - 1, rows / 2 - 1);
                            showPatternMenu = false;
                        }
                        else {
                            showError = true;
                        }
                    }
                    else if (patternGlider.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (rows >= 3 && cols >= 3) {
                            showError = false;
                            game.toggleCell(1, 0);
                            game.toggleCell(2, 1);
                            game.toggleCell(0, 2);
                            game.toggleCell(1, 2);
                            game.toggleCell(2, 2);
                        }
                        else {
                            showError = true;
                        }
                    }
                    else if (patternEight.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (rows >= 8 && cols >= 8) {
                            int numEight = 2;
                            showError = false;
                            for (int i = 0; i <= numEight; i++) {
                                for (int j = 0; j <= numEight; j++) {
                                    game.toggleCell(cols / 2 - i - 1, rows / 2 - j - 1);
                                    game.toggleCell(cols / 2 + i, rows / 2 + j);
                                }
                            }
                        }
                        else {
                            showError = true;
                        }
                    }
                }
            }
        }

        if (!simulationEnded && clock.getElapsedTime().asSeconds() >= UPDATE_TIME && game.isRunning()) {
            game.change();
            clock.restart();

            if (game.countLiveCells() == 0) {
                simulationEnded = true;
                game.toggleSimulation();
            }
        }

        std::stringstream ss;
        ss << "Generation: " << game.getGeneration();
        generationText.setString(ss.str());

        window.clear(sf::Color(220, 220, 220));
        game.draw(window);
        window.draw(startButton);
        window.draw(generationButton);
        window.draw(generationText);
        window.draw(startText);
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

        if (simulationEnded) {
            window.draw(endScreen);
            window.draw(endText);
        }

        window.display();
    }

    return 0;
}
