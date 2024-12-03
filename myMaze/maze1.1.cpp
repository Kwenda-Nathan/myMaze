#include <iostream>
#include <raylib.h>
using namespace std;

// Enum to manage game states
enum GameState {
    MENU,
    GAME
};

const int screenWidth = 800;
const int screenHeight = 600;
int foodsize = 25;

// player Character
class Player 
{
public:
    Texture2D image;
    Vector2 position;

    Player() {
        image = LoadTexture("pics/player1.png");
        position.x = 2;  // Starting position (aligned with the grid)
        position.y = 2;
    }
    ~Player() {
        UnloadTexture(image);
    }
    void Draw() {
        DrawTextureV(image, Vector2{ position.x * foodsize, position.y * foodsize }, WHITE);
    }
    void MoveLeft() {
        if (position.x > 2)position.x--;  // Prevent moving out of bounds
    }
    void MoveRight() {
        if (position.x < (screenWidth - 100) / foodsize - 1) position.x++;
    }
    void MoveUp() {
        if (position.y > 2)position.y--; // Prevent moving out of bounds
    }
    void MoveDown() {
        if (position.y < (screenHeight - 100) / foodsize - 1) position.y++;
    }


};

// food class
class Food {
public:
    Vector2 position;
    Texture2D texture;

    // Food image
    Food() 
    {
        Image image = LoadImage("pics/tile.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos();
    }

    // Destructor 
    ~Food() 
    {
        UnloadTexture(texture);
    }

    void Draw() {
        DrawTexture(texture,position.x*foodsize,position.y*foodsize, WHITE);
    }

    // Generate random food position
    Vector2 GenerateRandomPos() 
    {
        // Calculate the grid dimensions within the playable area
        int gridx = (screenWidth - 100) / foodsize; // Exclude 50px border on left and right
        int gridy = (screenHeight - 100) / foodsize; // Exclude 50px border on top and bottom

        // Generate random positions within the grid
        float x = GetRandomValue(2, gridx - 1);
        float y = GetRandomValue(2, gridy - 1);

        // offset the position to account for the border
        return Vector2{x ,y }; 
    }
};

int main() {
    // Initialize the window
    
    InitWindow(screenWidth, screenHeight, "Menu and Game Screen");

    // Colors
    Color bgColor = DARKGRAY;
    Color borderColor = WHITE;

    // Button properties
    Rectangle startButton = { screenWidth / 2 - 100, screenHeight / 2 - 60, 200, 50 };
    Rectangle quitButton = { screenWidth / 2 - 100, screenHeight / 2 + 20, 200, 50 };

    // Game state
    GameState currentState = MENU;  // Start with the menu state
    bool gameRunning = true;        // Control the main loop

    // Food object
    Food food = Food();

    // Player object
    Player player = Player();

    while (!WindowShouldClose() && gameRunning) {
        BeginDrawing();
        ClearBackground(bgColor);

        // Handle the current state
        if (currentState == MENU) {
            // Menu Screen
            DrawText("GAME MENU", screenWidth / 2 - MeasureText("GAME MENU", 40) / 2, 100, 40, WHITE);

            // Draw Start Button
            Vector2 mousePosition = GetMousePosition();
            bool mouseOverStart = CheckCollisionPointRec(mousePosition, startButton);
            DrawRectangleRec(startButton, mouseOverStart ? LIGHTGRAY : GRAY);
            DrawText("START", startButton.x + 50, startButton.y + 10, 30, BLACK);

            // Draw Quit Button
            bool mouseOverQuit = CheckCollisionPointRec(mousePosition, quitButton);
            DrawRectangleRec(quitButton, mouseOverQuit ? LIGHTGRAY : GRAY);
            DrawText("QUIT", quitButton.x + 60, quitButton.y + 10, 30, BLACK);

            // Handle button clicks
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (mouseOverStart) {
                    currentState = GAME;  // Transition to the game screen
                }
                if (mouseOverQuit) {
                    gameRunning = false;  // Exit the game
                }
            }
        }
        else if (currentState == GAME) {
            // Game Screen
            ClearBackground(BLACK);

            // Draw the borders
            DrawRectangleLines(50, 50, screenWidth - 100, screenHeight - 100, borderColor);

           // Handle player movement
            if (IsKeyPressed(KEY_A)) player.MoveLeft();
            if (IsKeyPressed(KEY_D)) player.MoveRight();
            if (IsKeyPressed(KEY_W)) player.MoveUp();
            if (IsKeyPressed(KEY_S)) player.MoveDown();

            
            // Collision detection between player and food
            if (player.position.x == food.position.x && player.position.y == food.position.y) {
                food.position = food.GenerateRandomPos(); // Respawn food
            }

            // Draw Food
            food.Draw();

            // Draw Player
            player.Draw();

            //DrawFPS
            DrawFPS(10, 10);

            // Add a return option or game content
            DrawText("Press ESC to return to the menu", screenWidth / 2 - 200, screenHeight - 80, 20, WHITE);

            // Return to menu if ESC is pressed
            if (IsKeyPressed(KEY_ESCAPE)) {
                currentState = MENU;
            }
        }

        EndDrawing();
    }

    // Close the window and clean up
    CloseWindow();

    return 0;
}

