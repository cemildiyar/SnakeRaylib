#include <deque>
#include <iostream>
#include <raylib.h>
#include <string>

#include "raymath.h"


auto green = DARKGRAY;
auto darkGreen = BLACK;
int cellSize = 29;
int cellCount = 30;
double lastUpdateTime = 0;
bool running = true;
int offset = 75;

bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

bool ElementInDeque(const Vector2 vector, const std::deque<Vector2> &deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if (Vector2Equals(vector, deque[i])) {
            return true;
        }
    }
    return false;
}

class Food {
public:
    Vector2 position;
    Texture2D texture;

    Food() {
        Image image = LoadImage("Graphics/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPosition(std::deque<Vector2>());
    }

    ~Food() {
        UnloadTexture(texture);
    }

    void draw() const {
        DrawTexture(
            texture,
            position.x * static_cast<float>(cellSize) + static_cast<float>(offset),
            position.y * static_cast<float>(cellSize) + static_cast<float>(offset),
            WHITE);
    }

    static Vector2 generateRandomCell()  {
        const auto x = static_cast<float>(GetRandomValue(0, cellCount - 1));
        const auto y = static_cast<float>(GetRandomValue(0, cellCount - 1));
        return Vector2{x, y};
    }

    static Vector2 GenerateRandomPosition(const std::deque<Vector2> &snakeBody) {
        Vector2 position = generateRandomCell();
        while (ElementInDeque(position, snakeBody)) {
            position = generateRandomCell();
        }
        return position;
    }
};

class Snake {
public:
    std::deque<Vector2> body = {
        Vector2{6, 9},
        Vector2{5, 9},
        Vector2{4, 9}
    };
    bool addSegment = true;
    Vector2 direction = {1, 0};

    void Draw() const {
        for (unsigned int i = 0; i < body.size(); i++) {
            const float x = body[i].x * static_cast<float>(cellSize);
            const float y = body[i].y * static_cast<float>(cellSize);
            const auto segment = Rectangle{static_cast<float>(offset)+x, static_cast<float>(offset)+y, static_cast<float>(cellSize), static_cast<float>(cellCount)};
            DrawRectangleRounded(segment, 0.5, 4, darkGreen);
        }
    }

    void Update() {

        body.push_front(Vector2Add(body[0], direction));
        if (addSegment) {
            addSegment = false;
        }
        else {
            body.pop_back();
        }
    }

    void Reset() {
        body = {
            Vector2{6, 9},Vector2{5,9}, Vector2{4,9}, Vector2{3,9}};
            direction = {1, 0};
    }
};

// this class controller for snake and food
class Game {
public:
    int score = 0;
    Snake snake = Snake();
    Food food = Food();

    Sound wall;
    Sound eat;

    Game() {
        InitAudioDevice();
        wall = LoadSound("Sounds/wall.mp3");
        eat = LoadSound("Sounds/eat.mp3");
    }


    void Draw() const {
        snake.Draw();
        food.draw();
    }

    void Update() {
        if (running) {
            snake.Update();
            checkCollision();
            checkCollisionEdges();
            checkCollisionSnake();
        }
    }

    void checkCollision() {
        if (Vector2Equals(snake.body[0], food.position)) {
            food.position = food.GenerateRandomPosition(snake.body);
            snake.addSegment = true;
            PlaySound(eat);
            score++;
        }
    }

    void checkCollisionEdges() {
        if (snake.body[0].x < 0 || snake.body[0].x == static_cast<float>(cellCount)) {
            GameOver();
            PlaySound(wall);
        }
        if (snake.body[0].y < 0 || snake.body[0].y == static_cast<float>(cellCount)) {
            GameOver();
            PlaySound(wall);
        }
    }

    void checkCollisionSnake() {
        std::deque<Vector2> snakeBody = snake.body;
        snakeBody.pop_front();
        if (ElementInDeque(snake.body[0], snakeBody)) {
            PlaySound(wall);
            GameOver();
        }
    }


    void GameOver() {
        snake.Reset();
        food.position = food.GenerateRandomPosition(snake.body);
        running = false;
        score = 0;
    }

    ~Game() {
        UnloadSound(wall);
        UnloadSound(eat);
        CloseAudioDevice();
    }

};

int main() {
    // variable and constants of game
    InitWindow(offset*2 + cellSize * cellCount, offset*2 + cellCount * cellSize, "Retro Snake Game");
    SetTargetFPS(60);

    Game game = Game();

    // loop of game  start area
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(green);

        if (eventTriggered(0.15)) {
            game.Update();
        }

        
        if (IsKeyDown(KEY_UP) && game.snake.direction.y != 1) {
            game.snake.direction = {0, -1};
            running = true;
        }
        if (IsKeyDown(KEY_DOWN) && game.snake.direction.y != -1) {
            game.snake.direction = {0, 1};
            running = true;
        }
        if (IsKeyDown(KEY_LEFT) && game.snake.direction.x != 1) {
            game.snake.direction = {-1, 0};
            running = true;
        }
        if (IsKeyDown(KEY_RIGHT) && game.snake.direction.x != -1) {
            game.snake.direction = {1, 0};
            running = true;
        }
        game.Draw();

        DrawRectangleLinesEx(Rectangle{static_cast<float>(offset)-5, static_cast<float>(offset)-5, static_cast<float>(cellSize * cellCount+10), static_cast<float>(cellCount * cellSize+10)}, 5, BLACK);
        DrawText("Retro Snake Game", offset, 20, 40, BLACK);
        DrawText(("Score: " + std::to_string(game.score)).c_str(), (cellSize * cellCount) - offset -15, 20, 40, BLACK);
        EndDrawing();

    }
    // loog of game end area

    return 0;
}
