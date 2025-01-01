#include <iostream>
#include <raylib.h> //https://www.raylib.com
#include <vector>
#include <stack>
#include <fstream>
#include <string>
#include <algorithm> // Required for std::max
#include <utility> // For std::pair

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

// Directions for maze generation
const vector<Vector2i> directions = {
    {0, -1}, // Up
    {1, 0},  // Right
    {0, 1},  // Down
    {-1, 0}  // Left
};


// Function to shuffle directions randomly
void ShuffleDirections(vector<Vector2i>& dirs) {
    for (int i = dirs.size() - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(dirs[i], dirs[j]);
    }
}

// Function to generate a random maze
vector<vector<int>> GenerateMaze() {
    vector<vector<int>> maze(mazeHeight, vector<int>(mazeWidth, 1)); // Initialize with walls
    stack<Vector2i> cellStack;
    Vector2i current = { 1, 1 };
    maze[current.y][current.x] = 0;

    while (!cellStack.empty() || current.x > 0 || current.y > 0) {
        vector<Vector2i> shuffledDirs = directions;
        ShuffleDirections(shuffledDirs);
        bool moved = false;

        for (const auto& dir : shuffledDirs) {
            Vector2i next = { current.x + dir.x * 2, current.y + dir.y * 2 };
            if (next.x > 0 && next.x < mazeWidth - 1 &&
                next.y > 0 && next.y < mazeHeight - 1 &&
                maze[next.y][next.x] == 1) {
                maze[current.y + dir.y][current.x + dir.x] = 0; // Remove wall
                maze[next.y][next.x] = 0; // Mark cell as visited
                cellStack.push(current);
                current = next;
                moved = true;
                break;
            }
        }

        if (!moved) {
            if (cellStack.empty()) break;
            current = cellStack.top();
            cellStack.pop();
        }
    }

    return maze;
}


// Game Data Definition
struct GameData {
    int currentLevel;
    int score;
    Vector2 playerPosition;
};

// Save Game Function
void SaveGame(const GameData& data, const std::string& filename = "savegame.txt") {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << data.currentLevel << '\n';
        file << data.score << '\n';
        file << data.playerPosition.x << ' ' << data.playerPosition.y << '\n';
        file.close();
        std::cout << "Game saved successfully!\n";
    }
    else {
        std::cerr << "Failed to open file for saving.\n";
    }
}

// Load Game Function
GameData LoadGame(const std::string& filename = "savegame.txt") {
    std::ifstream file(filename);
    GameData data = { 0, 0, {0.0f, 0.0f} }; // Default data
    if (file.is_open()) {
        file >> data.currentLevel;
        file >> data.score;
        file >> data.playerPosition.x >> data.playerPosition.y;
        file.close();
        std::cout << "Game loaded successfully!\n";
    }
    else {
        std::cerr << "No save file found. Starting a new game.\n";
    }
    return data;
}

// Game highscore
void SaveHighScore(int highScore) {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

int LoadHighScore() {
    std::ifstream file("highscore.txt");
    int highScore = 0;
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
    return highScore;
}

// player Character
class Player {
public:
    Texture2D image;
    Vector2 position; // Use Vector2 for position

    Player(Texture2D& texture) {
        image = texture;
        position = { 1.0f, 1.0f };  // Starting position (aligned with the grid)
    }

    void Draw(Vector2 offset) {
        DrawTextureV(image, { offset.x + position.x * gridSize, offset.y + position.y * gridSize }, WHITE);
    }

    void Move(Vector2 direction, const vector<vector<int>>& maze) {
        Vector2 nextPosition = { position.x + direction.x, position.y + direction.y };

        // Check if the next position is a valid path (not a wall)
        if (nextPosition.x >= 0 && nextPosition.x < maze[0].size() &&
            nextPosition.y >= 0 && nextPosition.y < maze.size() &&
            maze[static_cast<int>(nextPosition.y)][static_cast<int>(nextPosition.x)] == 0) {
            position = nextPosition;
        }
    }
};


// Enemy class
class Enemy {
public:
    Texture2D image;
    Vector2i position;
    vector<Vector2i> patrolPath;
    int patrolIndex = 0;
    float moveTimer = 0.0f; // Timer for movement
    float moveDelay = 0.5f; // Delay between movements in seconds

    Enemy(Texture2D& texture, const vector<Vector2i>& path) {
        image = texture;
        patrolPath = path;
        position = patrolPath[0];
    }

    void Draw(Vector2 offset) const {
        DrawTextureV(image, { offset.x + position.x * gridSize, offset.y + position.y * gridSize }, WHITE);
    }

    void Update(float deltaTime) {
        moveTimer += deltaTime;
        if (moveTimer >= moveDelay) {
            patrolIndex = (patrolIndex + 1) % patrolPath.size();
            position = patrolPath[patrolIndex];
            moveTimer = 0.0f; // Reset timer
        }
    }
};


// food class
class Food {
public:
    Vector2i position;
    Texture2D texture;

    // Food image
    Food(Texture2D& texture, const vector<vector<int>>& maze) : texture(texture)
    {
        position = GenerateRandomPos(maze);
    }

    void Draw(Vector2 offset) {
        DrawTexture(texture, offset.x + position.x * gridSize, offset.y + position.y * gridSize, WHITE);
    }

    // Generate random food position
    Vector2i GenerateRandomPos(const vector<vector<int>>& maze)
    {
        Vector2i newPos;
        do {
            // Generate a random position within the maze's boundaries
            newPos = { GetRandomValue(0, maze[0].size() - 1), GetRandomValue(0, maze.size() - 1) };
        } while (maze[newPos.y][newPos.x] != 0);  // Ensure food spawns on a valid path
        return newPos;
    }
};

// Sounds
Sound Foodsound;
Sound GOsound;
Sound Nextlevel;
Sound powerupsd;
Music backgsound;

void InitSounds() {
    InitAudioDevice();

    Foodsound = LoadSound("sounds/foodsound.wav");
    GOsound = LoadSound("sounds/gosound.wav");
    Nextlevel = LoadSound("sounds/nextlevel.wav");
    powerupsd = LoadSound("sounds/powerUpsound.wav");

    backgsound = LoadMusicStream("sounds/backgroundsound.mp3");
    PlayMusicStream(backgsound);
}

// Function to solve the puzzle in-game
bool SolvePuzzle(int& remainingAttempts) {
    srand(static_cast<unsigned>(time(0)));
    int a = rand() % 10 + 1;
    int b = rand() % 10 + 1;
    int correctAnswer = a + b;

    std::string input = "";
    bool solved = false;
    bool running = true;

    while (running) {
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw puzzle text
        DrawText(("Solve: " + std::to_string(a) + " + " + std::to_string(b)).c_str(), 50, 50, 20, WHITE);
        DrawText(("Your Answer: " + input).c_str(), 50, 100, 20, WHITE);
        DrawText(("Attempts Left: " + std::to_string(remainingAttempts)).c_str(), 50, 150, 20, RED);

        // Debugging output
        TraceLog(LOG_INFO, "Attempts: %d, Input: %s, Solved: %d, Running: %d",
            remainingAttempts, input.c_str(), solved, running);

        // Handle keyboard input
        if (IsKeyPressed(KEY_ENTER)) {
            if (!input.empty() && std::all_of(input.begin(), input.end(), ::isdigit)) {
                int playerAnswer = std::stoi(input);
                if (playerAnswer == correctAnswer) {
                    solved = true;
                    running = false;
                }
                else {
                    remainingAttempts--;
                    if (remainingAttempts > 0) {
                        input = "Try Again!";
                    }
                    else {
                        running = false; // Out of attempts
                    }
                }
            }
            else {
                input = "Invalid Input!";
            }
        }
        else if (IsKeyPressed(KEY_BACKSPACE) && !input.empty()) {
            input.pop_back();
        }
        else {
            // Check for numeric input
            for (int key = KEY_ZERO; key <= KEY_NINE; ++key) {
                if (IsKeyPressed(key)) {
                    input += (key - KEY_ZERO) + '0';
                }
            }
        }

        // Draw feedback if incorrect
        if (!solved && input == "Try Again!") {
            DrawText("Incorrect. Try again!", 50, 200, 20, RED);
        }
        else if (!solved && input == "Invalid Input!") {
            DrawText("Please enter a valid number!", 50, 200, 20, RED);
        }

        EndDrawing();

        // Exit if no attempts left
        if (remainingAttempts <= 0) {
            break;
        }
    }

    TraceLog(LOG_INFO, "Final Solved: %d, Remaining Attempts: %d", solved, remainingAttempts);
    return solved;
}

// Exit Struct
class Exit {
public:
    Vector2 position; // Grid position (floating-point for flexibility)
    bool active;

    Exit() : position({ -1.0f, -1.0f }), active(false) {}

    void Activate(const vector<vector<int>>& maze) {
        position = GenerateRandomPos(maze);
        active = true;
    }

    void Deactivate() {
        position = { -1.0f, -1.0f }; // Move to an invalid position
        active = false;
    }

    void Draw(Vector2 offset) {
        if (active) {
            DrawRectangleV({ offset.x + position.x * gridSize, offset.y + position.y * gridSize },
                { gridSize, gridSize }, GREEN);
        }
    }

private:
    Vector2 GenerateRandomPos(const vector<vector<int>>& maze) {
        Vector2 newPos;
        do {
            newPos = { static_cast<float>(GetRandomValue(0, maze[0].size() - 1)),
                       static_cast<float>(GetRandomValue(0, maze.size() - 1)) };
        } while (maze[static_cast<int>(newPos.y)][static_cast<int>(newPos.x)] != 0); // Ensure it's on a valid path
        return newPos;
    }
};



int main() {
    // Initialize the window
    srand(time(nullptr));
    InitWindow(screenWidth, screenHeight, "Menu and Game Screen");
    InitSounds();

    // Load resources
    Texture2D playerTexture = LoadTexture("pics/player1.png");
    Texture2D foodTexture = LoadTexture("pics/tile.png");
    Texture2D enemyTexture = LoadTexture("pics/enemy.png");
    
    

    // Colors
    Color bgColor = DARKGRAY;
    Color borderColor = WHITE;

    // Game state
    GameState currentState = MENU;  // Start with the menu state
    bool gameRunning = true;        // Control the main loop
    bool isPaused = false;

    // Game variables
    GameData data = LoadGame(); // Load game data if save file exists

    int currentLevel = data.currentLevel;
    int score = data.score;
    Vector2 playerPosition = data.playerPosition;
    const float playerSpeed = 200.0f;
   
    // Score, timer & level
    //int score = 0;
    float timer = 60.0f;  // 60 seconds countdown
    int level = 1;
    float timeSinceLastHit = 0.0f; // Timer for score reduction
    float scoreReductionDelay = 1.0f; // Delay in seconds between score reductions
    int nextLevelScore = 100; // Score required for Level 2
    float levelTimeReduction = 5.0f; // Time reduction per level
    int highScore = LoadHighScore();
    float volume = 0.5f; // Initial volume (50%)
    SetMasterVolume(volume); // Set initial volume
    int attempts = 3; // Maximum attempts for solving the puzzle
  
    // Initialize maze, player, food, and enemies
    vector<vector<int>> maze = GenerateMaze();
    Food food(foodTexture, maze);
    Player player(playerTexture);
    vector<Enemy> enemies;
    
    Exit exit; // Declare exit object

    // Create enemies with specific patrol paths
    enemies.push_back(Enemy(enemyTexture, { {3, 3}, {5, 3}, {5, 5}, {3, 5} }));
    enemies.push_back(Enemy(enemyTexture, { {7, 7}, {7, 9}, {9, 9}, {9, 7} }));

    Vector2 offset = {
        (screenWidth - mazeWidth * gridSize) / 2.0f,
        (screenHeight - mazeHeight * gridSize) / 2.0f
    };

    while (!WindowShouldClose() && gameRunning) {

        float deltaTime = GetFrameTime();
        timeSinceLastHit += deltaTime;
        UpdateMusicStream(backgsound);

        // Update game timer
        if (currentState == GAME && !isPaused && timer > 0) {
            timer -= deltaTime;
            if (timer <= 0) timer = 0;
        }

        BeginDrawing();
        ClearBackground(bgColor);

        // Handle the current state
        if (currentState == MENU) {
            // Menu Screen
            DrawText("GAME MENU", screenWidth / 2 - MeasureText("GAME MENU", 40) / 2, 100, 40, WHITE);

            // Draw Start Button
            Rectangle startButton = { screenWidth / 2 - 100, screenHeight / 2 - 120, 200, 50 };
            Vector2 mousePosition = GetMousePosition();
            bool mouseOverStart = CheckCollisionPointRec(mousePosition, startButton);
            DrawRectangleRec(startButton, mouseOverStart ? LIGHTGRAY : GRAY);
            DrawText("START", startButton.x + 50, startButton.y + 10, 30, BLACK);

            // Load Button
            Rectangle loadButton = { screenWidth / 2 - 100, screenHeight / 2 - 40, 200, 50 };
            bool mouseOverLoad = CheckCollisionPointRec(mousePosition, loadButton);
            DrawRectangleRec(loadButton, mouseOverLoad ? LIGHTGRAY : GRAY);
            DrawText("LOAD", loadButton.x + 50, loadButton.y + 10, 30, BLACK);

            // Draw Quit Button
            Rectangle quitButton = { screenWidth / 2 - 100, screenHeight / 2 + 40, 200, 50 };
            bool mouseOverQuit = CheckCollisionPointRec(mousePosition, quitButton);
            DrawRectangleRec(quitButton, mouseOverQuit ? LIGHTGRAY : GRAY);
            DrawText("QUIT", quitButton.x + 60, quitButton.y + 10, 30, BLACK);



            // Handle button clicks
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (mouseOverStart) {
                    currentState = GAME;  // Transition to the game screen
                    timer = 60.0f - (level - 1) * 5; // Reduce time as levels progress
                    score = 0;      // Reset score
                    player.position = { 1, 1 };  // Reset player position
                    maze = GenerateMaze();
                    food.position = food.GenerateRandomPos(maze);
                    
                    // load the super enemy
                    for (auto& enemy : enemies) {
                        enemy.moveDelay = max(0.2f, 0.5f - level * 0.05f); // Decrease delay per level
                        enemy.patrolIndex = 0;
                        enemy.position = enemy.patrolPath[0];
                    }

                }
                if (mouseOverQuit) {
                    gameRunning = false;  // Exit the game
                }
                // Load Progress
                if (mouseOverLoad) {
                    currentState = GAME;
                    data = LoadGame();
                    currentLevel = data.currentLevel;
                    score = data.score;
                    playerPosition = data.playerPosition;
                }
            }

            // Draw Volume Slider
            Rectangle volumeSlider = { screenWidth / 2 - 100, quitButton.y + quitButton.height + 30, 200, 20 };
            DrawRectangleRec(volumeSlider, LIGHTGRAY);

            // Slider knob position based on volume
            float knobX = volumeSlider.x + (volume * volumeSlider.width);
            DrawCircle(knobX, volumeSlider.y + volumeSlider.height / 2, 10, DARKGRAY);

            // Handle volume adjustment
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mousePosition, volumeSlider)) {
                    volume = (mousePosition.x - volumeSlider.x) / volumeSlider.width;
                    if (volume < 0.0f) volume = 0.0f;
                    if (volume > 1.0f) volume = 1.0f;

                    SetMasterVolume(volume); // Adjust the game's volume
                }
            }

            // Display volume percentage
            DrawText(TextFormat("Volume: %d%%", (int)(volume * 100)), volumeSlider.x + 50, volumeSlider.y + 30, 20, BLACK);

        }
        else if (currentState == GAME) {

            // Game Screen
            ClearBackground(BLACK);

            if (currentState == GAME && !isPaused && timer > 0) {

                // Handle player movement
                if (IsKeyPressed(KEY_W)) player.Move({ 0, -1 }, maze);
                if (IsKeyPressed(KEY_S)) player.Move({ 0, 1 }, maze);
                if (IsKeyPressed(KEY_A)) player.Move({ -1, 0 }, maze);
                if (IsKeyPressed(KEY_D)) player.Move({ 1, 0 }, maze);

                // Enemy movement
                for (auto& enemy : enemies) {
                    enemy.Update(deltaTime);
                }
               
            }


            // Collision detection between player and food
            if (player.position.x == food.position.x && player.position.y == food.position.y) {
                food.position = food.GenerateRandomPos(maze); // Respawn food
                score += 50; // add points to the Score
                PlaySound(Foodsound);
            }

            if (score >= nextLevelScore && !exit.active) {
                exit.Activate(maze); // Activate exit when score threshold is reached
            }

            // Progress to next level
            if (exit.active && player.position.x == static_cast<int>(exit.position.x) && player.position.y == static_cast<int>(exit.position.y)) {
                if (SolvePuzzle(attempts)) {
                    level++;
                    timer = max(10.0f, 60.0f - level * levelTimeReduction); // Adjust timer
                    nextLevelScore += 150; // Increment score threshold for the next level
                    timer = 60.0f - (level - 1) * 5;
                    maze = GenerateMaze();
                    player.position = { 1, 1 };
                    attempts = 3; // Reset attempts for the next puzzle
                    food.position = food.GenerateRandomPos(maze);
                    
                    PlaySound(Nextlevel);
                    for (auto& enemy : enemies) {
                        enemy.patrolIndex = 0;
                        enemy.position = enemy.patrolPath[0];
                    }
                    exit.Deactivate();
                }
                else if (attempts == 0) {
                    timer = 60.0f;        // Reset timer
                    score = 0;            // Reset score
                    level = 1;
                    nextLevelScore = 100;           // Reset score for level progression
                    player.position = { 1, 1 };     // Reset player position
                    attempts = 3;
                    maze = GenerateMaze();          // Generate a new maze
                    food.position = food.GenerateRandomPos(maze); // Reset food position
                    
                    for (auto& enemy : enemies) {   // Reset enemy logic
                        enemy.patrolIndex = 0;
                        enemy.position = enemy.patrolPath[0];
                        enemy.moveDelay = max(0.2f, 0.5f); // Reset to initial delay
                    }
                    if (score >= nextLevelScore) {
                        if (SolvePuzzle(attempts)) {
                            level++;
                            timer = max(10.0f, 60.0f - level * levelTimeReduction); // Adjust timer
                            nextLevelScore += 150; // Increment score threshold for the next level
                            timer = 60.0f - (level - 1) * 5;
                            maze = GenerateMaze();
                            player.position = { 1, 1 };
                            food.position = food.GenerateRandomPos(maze);

                            PlaySound(Nextlevel);
                            for (auto& enemy : enemies) {
                                enemy.patrolIndex = 0;
                                enemy.position = enemy.patrolPath[0];
                            }
                        }
                    }
                }
            }

            // Collision with enemies
            for (const auto& enemy : enemies) {
                if (player.position.x == enemy.position.x && player.position.y == enemy.position.y) {
                    if (timeSinceLastHit >= scoreReductionDelay) {
                        score = max(0, score - 1); // Gradual score reduction
                        timeSinceLastHit = 0.0f; // Reset the cooldown timer
                    }
                }
            }



            // Draw maze
            for (int y = 0; y < maze.size(); y++) {
                for (int x = 0; x < maze[0].size(); x++) {
                    if (maze[y][x] == 1) {
                        DrawRectangle(offset.x + x * gridSize, offset.y + y * gridSize, gridSize, gridSize, BLACK);
                        //DrawRectangleLines(offset.x + x * gridSize, offset.y + y * gridSize, gridSize, gridSize, WHITE);
                        DrawRectangleLinesEx({ offset.x + x * gridSize, offset.y + y * gridSize, gridSize, gridSize }, 2, RED);
                        
                    }
                }
            }

            //DrawRectangleLines(50, 50, screenWidth - 100, screenHeight - 100, borderColor);

            // Draw Food, Player, Enemy, Super enemy

            food.Draw(offset);
            player.Draw(offset);
          
            for (const auto& enemy : enemies) {
                enemy.Draw(offset);
            }
            exit.Draw(offset);
           

            // Display Score and Timer
            DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);
            DrawText(TextFormat("Time Left: %.1f", timer), screenWidth - 150, 10, 20, WHITE);
            DrawText(TextFormat("Level: %d", level), 10, 70, 20, WHITE);


            // Draw a Pause-Btn
            Rectangle pauseButton = { screenWidth - 150, 50, 120, 40 };
            Vector2 mousePosition = GetMousePosition();
            bool mouseOverPause = CheckCollisionPointRec(mousePosition, pauseButton);

            DrawRectangleRec(pauseButton, mouseOverPause ? LIGHTGRAY : GRAY);
            DrawText(isPaused ? "PLAY" : "PAUSE", pauseButton.x + 25, pauseButton.y + 10, 20, BLACK);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouseOverPause) {
                isPaused = !isPaused;
            }

            // Pause effects
            if (isPaused) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
                DrawText("Paused", screenWidth / 2 - MeasureText("Paused", 40) / 2, screenHeight / 2 - 20, 40, WHITE);
            }

            // Save Progress
            if (IsKeyPressed(KEY_U)) {
                GameData saveData = { currentLevel, score, playerPosition };
                SaveGame(saveData);
            }

            // Update high score
            if (score > highScore) {
                highScore = score;
                SaveHighScore(highScore); // Save the new high score to the file
            }

            DrawText("Press 'U' to save progress.", 10, 500, 20, DARKGRAY);
            DrawText("Press 'L' to load progress.", 650, 500, 20, DARKGRAY);
            DrawText(TextFormat("High Score: %d", highScore), 350, 10, 20, DARKGRAY);

            // Show "GAME OVER" text if the timer reaches 0
            if (timer <= 0) {
                DrawText("GAME OVER!", screenWidth / 2 - MeasureText("GAME OVER!", 40) / 2, screenHeight / 2 - 100, 40, RED);
                PlaySound(GOsound);

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
                    level = 1;
                    nextLevelScore = 100;           // Reset score for level progression
                    player.position = { 1, 1 };     // Reset player position
                    maze = GenerateMaze();          // Generate a new maze
                    food.position = food.GenerateRandomPos(maze); // Reset food position

                    for (auto& enemy : enemies) {   // Reset enemy logic
                        enemy.patrolIndex = 0;
                        enemy.position = enemy.patrolPath[0];
                        enemy.moveDelay = max(0.2f, 0.5f); // Reset to initial delay
                    }
                    if (score >= nextLevelScore) {
                        if (SolvePuzzle(attempts)) {
                            level++;
                            timer = max(10.0f, 60.0f - level * levelTimeReduction); // Adjust timer
                            nextLevelScore += 150; // Increment score threshold for the next level
                            timer = 60.0f - (level - 1) * 5;
                            maze = GenerateMaze();
                            player.position = { 1, 1 };
                            attempts = 3;
                            food.position = food.GenerateRandomPos(maze);
                            PlaySound(Nextlevel);
                            for (auto& enemy : enemies) {
                                enemy.patrolIndex = 0;
                                enemy.position = enemy.patrolPath[0];
                            }

                        }
                        else if (attempts == 0) {
                            timer = 60.0f;        // Reset timer
                            score = 0;            // Reset score
                            level = 1;
                            nextLevelScore = 100;           // Reset score for level progression
                            player.position = { 1, 1 };     // Reset player position
                            attempts = 3;
                            maze = GenerateMaze();          // Generate a new maze
                            food.position = food.GenerateRandomPos(maze); // Reset food position
                            for (auto& enemy : enemies) {   // Reset enemy logic
                                enemy.patrolIndex = 0;
                                enemy.position = enemy.patrolPath[0];
                                enemy.moveDelay = max(0.2f, 0.5f); // Reset to initial delay
                            }
                            if (score >= nextLevelScore) {
                                if (SolvePuzzle(attempts)) {
                                    level++;
                                    timer = max(10.0f, 60.0f - level * levelTimeReduction); // Adjust timer
                                    nextLevelScore += 150; // Increment score threshold for the next level
                                    timer = 60.0f - (level - 1) * 5;
                                    maze = GenerateMaze();
                                    player.position = { 1, 1 };
                                    food.position = food.GenerateRandomPos(maze);
                                    PlaySound(Nextlevel);
                                    for (auto& enemy : enemies) {
                                        enemy.patrolIndex = 0;
                                        enemy.position = enemy.patrolPath[0];
                                    }
                                }
                            }
                        }
                    }
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
    UnloadSound(powerupsd);
    UnloadSound(Nextlevel);
    UnloadSound(Foodsound);
    UnloadSound(GOsound);  
    UnloadMusicStream(backgsound);
    CloseAudioDevice();
    // Close the window and clean up
    CloseWindow();

    return 0;
}

