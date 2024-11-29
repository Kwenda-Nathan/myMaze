#include <iostream>
#include <raylib.h>
using namespace std;

// Enum to manage game states
enum GameState {
    MENU,
    GAME
};

int main() {
    // Initialize the window
    const int screenWidth = 800;
    const int screenHeight = 600;
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

            // Draw circle
            DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 5, WHITE);

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

