#include <iostream>
#include <raylib.h>
#include <vector>
using namespace std;

// Custom struct for integer-based 2D vector 
struct Vector2i {
    int x;
    int y;
};

// Enum to manage game states
enum GameState {
    MENU,
    GAME
};

const int screenWidth = 960;
const int screenHeight = 600;
const int gridSize = 30;

// Maze dimensions
const int mazeWidth = 20;  // number of columns
const int mazeHeight = 11; // number of rows

// Maze grid: 1 = wall, 0 = path
vector<vector<int>> maze = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

// player Character
class Player 
{
public:
    Texture2D image;
    Vector2i position;

    Player() {
        image = LoadTexture("pics/player1.png");
        position = {2,2};  // Starting position (aligned with the grid)
        
    }
    ~Player() {
        UnloadTexture(image);
    }
    void Draw(Vector2 offset) {
        DrawTextureV(image, { offset.x + position.x * gridSize, offset.y + position.y * gridSize }, WHITE);
    }
    void Move(Vector2i direction, const vector<vector<int>>& maze) {
        Vector2i nextPosition = { position.x + direction.x, position.y + direction.y };

        // Check if the next position is a valid path (not a wall)
        if (nextPosition.x >= 0 && nextPosition.x < maze[0].size() &&
            nextPosition.y >= 0 && nextPosition.y < maze.size() &&
            maze[nextPosition.y][nextPosition.x] == 0) {
            position = nextPosition;
        }
    }


};

// food class
class Food {
public:
    Vector2i position;
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

    void Draw(Vector2 offset) {
        DrawTexture(texture, offset.x + position.x * gridSize, offset.y + position.y * gridSize, WHITE);
    }

    // Generate random food position
    Vector2i GenerateRandomPos() 
    {
        /* Calculate the grid dimensions within the playable area
        int gridx = (screenWidth - 100) / gridSize; // Exclude 50px border on left and right
        int gridy = (screenHeight - 100) / gridSize;  Exclude 50px border on top and bottom*/

        Vector2i newPos;
        do {
            newPos = { GetRandomValue(0, maze[0].size() - 1), GetRandomValue(0, maze.size() - 1) };
        } while (maze[newPos.y][newPos.x] != 0);  // Ensure food spawns on a path
        return newPos;
    }
};

int main() {
    // Initialize the window
    
    InitWindow(screenWidth, screenHeight, "Menu and Game Screen");

    if (maze.empty() || maze[0].empty()) {
        cerr << "Error: Maze is empty or improperly initialized!" << endl;
        CloseWindow();
        return -1;
    }

    cout << "Maze dimensions: " << maze.size() << " x " << maze[0].size() << endl;

    // Colors
    Color bgColor = DARKGRAY;
    Color borderColor = WHITE;

    Vector2 offset = {
        (screenWidth - maze[0].size() * gridSize) / 2.0f,
        (screenHeight - maze.size() * gridSize) / 2.0f
    };

    

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

        cout << "Player Position: (" << player.position.x << ", " << player.position.y << ")\n";
        cout << "Food Position: (" << food.position.x << ", " << food.position.y << ")\n";

        // Update game timer
        if (currentState == GAME) {
            timer -= GetFrameTime();
            if (timer <= 0) {
                timer = 0;               
            }
        }

        BeginDrawing();
        ClearBackground(bgColor);

        // Handle the current state
        if (currentState == MENU) {
            // Menu Screen
            DrawText("GAME MENU", screenWidth / 2 - MeasureText("GAME MENU", 40) / 2, 100, 40, WHITE);

            // Draw Start Button
            Rectangle startButton = { screenWidth / 2 - 100, screenHeight / 2 - 60, 200, 50 };
            Vector2 mousePosition = GetMousePosition();
            bool mouseOverStart = CheckCollisionPointRec(mousePosition, startButton);
            DrawRectangleRec(startButton, mouseOverStart ? LIGHTGRAY : GRAY);
            DrawText("START", startButton.x + 50, startButton.y + 10, 30, BLACK);

            // Draw Quit Button
            Rectangle quitButton = { screenWidth / 2 - 100, screenHeight / 2 + 20, 200, 50 };
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

            // Draw maze
            for (int y = 0; y < maze.size(); y++) {
                for (int x = 0; x < maze[0].size(); x++) {
                    if (maze[y][x] == 1) {
                        DrawRectangle(offset.x + x * gridSize, offset.y + y * gridSize, gridSize, gridSize, BLACK);
                        DrawRectangleLines(offset.x + x * gridSize, offset.y + y * gridSize, gridSize, gridSize, WHITE);
                    }
                }
            }


            
            //DrawRectangleLines(50, 50, screenWidth - 100, screenHeight - 100, borderColor);

           // Handle player movement
            if (IsKeyPressed(KEY_UP)) player.Move({ 0, -1 }, maze);
            if (IsKeyPressed(KEY_DOWN)) player.Move({ 0, 1 }, maze);
            if (IsKeyPressed(KEY_LEFT)) player.Move({ -1, 0 }, maze);
            if (IsKeyPressed(KEY_RIGHT)) player.Move({ 1, 0 }, maze);


            
            // Collision detection between player and food
            if (player.position.x == food.position.x && player.position.y == food.position.y) {
                food.position = food.GenerateRandomPos(); // Respawn food
                score++;
            }

            // Draw Food & player
            food.Draw(offset);
            player.Draw(offset);

            // Display Score and Timer
            DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);
            DrawText(TextFormat("Time Left: %.1f", timer), screenWidth - 150, 10, 20, WHITE);

            // Show "GAME OVER" text if the timer reaches 0
            if (timer <= 0) {
                DrawText("GAME OVER!", screenWidth / 2 - MeasureText("GAME OVER!", 40) / 2, screenHeight / 2 - 100, 40, RED);

                // Display M to return to the menu text
                DrawText("Press M to return to the menu",
                    screenWidth / 2 - MeasureText("Press M to return to the menu", 20) / 2,
                    screenHeight / 2,
                    20, WHITE);

                // Replay Button
                Rectangle replayButton = { screenWidth / 2 - 100, screenHeight / 2 + 50, 200, 50 };
                Vector2 mousePosition = GetMousePosition();
                bool mouseOverReplay = CheckCollisionPointRec(mousePosition, replayButton);
                DrawRectangleRec(replayButton, mouseOverReplay ? LIGHTGRAY : GRAY);
                DrawText("REPLAY", replayButton.x + 50, replayButton.y + 10, 30, BLACK);
                
                // Handle Replay button click
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouseOverReplay) {
                    // Reset game state
                    timer = 60.0f;        // Reset timer
                    score = 0;            // Reset score
                    player.position = { 2, 2 };  // Reset player position
                    food.position = food.GenerateRandomPos(); // Reset food position
                }
            }

            // Add a return option or game content
            DrawText("Press M to return to the menu", screenWidth / 2 - 200, screenHeight - 80, 20, WHITE);

            // Return to menu if M is pressed
            if (IsKeyPressed(KEY_M)) {
                currentState = MENU;
            }
        }

        EndDrawing();
    }

    // Close the window and clean up
    CloseWindow();

    return 0;
}

