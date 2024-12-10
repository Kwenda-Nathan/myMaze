#include <iostream>
#include <raylib.h>
//#include <vector>
using namespace std;

// Enum to manage game states
enum GameState {
    MENU,
    GAME
};

const int screenWidth = 960;
const int screenHeight = 600;
const int gridSize = 25;

// player Character
class Player 
{
public:
    Texture2D image;
    Vector2 position;

    Player() {
        image = LoadTexture("pics/player1.png");
        position = {1,1};  // Starting position (aligned with the grid)
        
    }
    ~Player() {
        UnloadTexture(image);
    }
    void Draw() {
        DrawTextureV(image, Vector2{ position.x * gridSize, position.y * gridSize }, WHITE);
    }
    void  MoveLeft() {
        if (position.x > 2) position.x--;  // Prevent moving out of bounds
    }
    void MoveRight() {
        if (position.x < (screenWidth - 100) / gridSize - 1) position.x++;
    }
    void MoveUp() {
        if (position.y > 2) position.y--; // Prevent moving out of bounds
    }
    void MoveDown() {
        if (position.y < (screenHeight - 100) / gridSize - 1) position.y++;
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
        DrawTexture(texture, position.x * gridSize, position.y * gridSize, WHITE);
    }

    // Generate random food position
    Vector2 GenerateRandomPos() 
    {
        // Calculate the grid dimensions within the playable area
        int gridx = (screenWidth - 100) / gridSize; // Exclude 50px border on left and right
        int gridy = (screenHeight - 100) / gridSize; // Exclude 50px border on top and bottom

        Vector2 newPos;
        do {
            newPos.x = GetRandomValue(2, gridx - 1);
            newPos.y = GetRandomValue(2, gridy - 1);
        } while (newPos.x == 2 && newPos.y == 2);  // Avoid spawning on player's initial position
        return newPos;
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

    // Score and timer
    int score = 0;
    float timer = 60.0f;  // 60 seconds countdown

    // Food object
    Food food = Food();

    // Player object
    Player player = Player();

    while (!WindowShouldClose() && gameRunning) {

        // Update game timer
        if (currentState == GAME) {
            timer -= GetFrameTime();
            if (timer <= 0) {
                timer = 0;
                currentState = MENU;  // Game over, return to menu
            }
        }

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
                    timer = 60.0f;  // Reset timer
                    score = 0;      // Reset score
                    player.position = { 2, 2 };  // Reset player position
                    food.position = food.GenerateRandomPos();
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
                score++;
            }

            // Draw Food
            food.Draw();

            // Draw Player
            player.Draw();

            // Display Score and Timer
            DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);
            DrawText(TextFormat("Time Left: %.1f", timer), screenWidth - 150, 10, 20, WHITE);

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

