#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

/*----------------------------------------------------------PREDEFINED-STRUCTURES-----------------------------------------------------------------------------*/

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 900
#define GRID_SIZE 4

#define GRID_RENDER_SIZE 400
#define FONT_PATH "Arial.ttf"
#define PLAYER_TURN 0
#define MACHINE_TURN 1
#define STOP_BUTTON_WIDTH 100
#define STOP_BUTTON_HEIGHT 50
#define STOP_BUTTON_X (WINDOW_WIDTH / 2 - STOP_BUTTON_WIDTH / 2) // Center horizontally
#define STOP_BUTTON_Y 50 // 50 pixels from the top (moved lower)
SDL_Rect stopButton = {STOP_BUTTON_X, STOP_BUTTON_Y, STOP_BUTTON_WIDTH, STOP_BUTTON_HEIGHT};

// Colors
SDL_Color black = {0, 0, 0, 255}; // Black
SDL_Color white = {255, 255, 255, 255}; // White
SDL_Color beige = {205, 193, 180, 255}; // Beige for empty cells
SDL_Color background = {100, 120, 139, 255}; // Background color blue
SDL_Color scoreBg = {187, 173, 160, 255}; // Score background
SDL_Color button = {0, 121, 107, 255}; // Button color
SDL_Color buttonHover = {0, 150, 136, 255}; // Button hover color

// Number colors
SDL_Color colors[] = {
    {176, 196, 222}, // 2
    {174, 198, 255},
    {120, 150, 200},
    {100, 130, 180},
    {70, 105, 150},
    {60, 90, 140},
    {237, 207, 114},
    {237, 204, 97},
    {237, 200, 80},
    {237, 197, 63}
};

// Game state structure
typedef struct {
    int grid[GRID_SIZE][GRID_SIZE];
    int currentScore;
    bool gameOver;
    bool paused;
    bool stopped;
} GameState;

// Game modes
typedef enum {
    MENU_MODE,
    PLAYER_MODE,
    MACHINE_MODE,
    PLAYER_VS_MACHINE_MODE,
    QUIT
} GameMode;

// Movement directions
typedef enum {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
} Direction;

/*------------------------------------------------BASIC-FUNCTIONS-------------------------------------------------------------------------*/

// Best Score
int bestScore = 0;

// Fonction pour charger le meilleur score depuis un fichier
void loadBestScore() {
    FILE *file = fopen("best_score.txt", "r");
    if (file) {
        fscanf(file, "%d", &bestScore);
        fclose(file);
    }
}

// Fonction pour sauvegarder le meilleur score dans un fichier
void saveBestScore() {
    FILE *file = fopen("best_score.txt", "w");
    if (file) {
        fprintf(file, "%d", bestScore);
        fclose(file);
    }
}

//fonction pour les movements de mode player
bool slideGrid(GameState *state, Direction direction) {
    bool moved = false;
    int row, col;

    switch (direction) {
        case UP:
            for (col = 0; col < GRID_SIZE; col++) {
                for (row = 1; row < GRID_SIZE; row++) {
                    if (state->grid[row][col] != 0) {
                        int currentRow = row;
                        while (currentRow > 0 && state->grid[currentRow - 1][col] == 0) {
                            state->grid[currentRow - 1][col] = state->grid[currentRow][col];
                            state->grid[currentRow][col] = 0;
                            currentRow--;
                            moved = true;
                        }
                        if (currentRow > 0 && state->grid[currentRow - 1][col] == state->grid[currentRow][col]) {
                            state->grid[currentRow - 1][col] *= 2;
                            state->currentScore += state->grid[currentRow - 1][col];
                            state->grid[currentRow][col] = 0;
                            moved = true;
                        }
                    }
                }
            }
            break;

        case DOWN:
            for (col = 0; col < GRID_SIZE; col++) {
                for (row = GRID_SIZE - 2; row >= 0; row--) {
                    if (state->grid[row][col] != 0) {
                        int currentRow = row;
                        while (currentRow < GRID_SIZE - 1 && state->grid[currentRow + 1][col] == 0) {
                            state->grid[currentRow + 1][col] = state->grid[currentRow][col];
                            state->grid[currentRow][col] = 0;
                            currentRow++;
                            moved = true;
                        }
                        if (currentRow < GRID_SIZE - 1 && state->grid[currentRow + 1][col] == state->grid[currentRow][col]) {
                            state->grid[currentRow + 1][col] *= 2;
                            state->currentScore += state->grid[currentRow + 1][col];
                            state->grid[currentRow][col] = 0;
                            moved = true;
                        }
                    }
                }
            }
            break;

        case LEFT:
            for (row = 0; row < GRID_SIZE; row++) {
                for (col = 1; col < GRID_SIZE; col++) {
                    if (state->grid[row][col] != 0) {
                        int currentCol = col;
                        while (currentCol > 0 && state->grid[row][currentCol - 1] == 0) {
                            state->grid[row][currentCol - 1] = state->grid[row][currentCol];
                            state->grid[row][currentCol] = 0;
                            currentCol--;
                            moved = true;
                        }
                        if (currentCol > 0 && state->grid[row][currentCol - 1] == state->grid[row][currentCol]) {
                            state->grid[row][currentCol - 1] *= 2;
                            state->currentScore += state->grid[row][currentCol - 1];
                            state->grid[row][currentCol] = 0;
                            moved = true;
                        }
                    }
                }
            }
            break;

        case RIGHT:
            for (row = 0; row < GRID_SIZE; row++) {
                for (col = GRID_SIZE - 2; col >= 0; col--) {
                    if (state->grid[row][col] != 0) {
                        int currentCol = col;
                        while (currentCol < GRID_SIZE - 1 && state->grid[row][currentCol + 1] == 0) {
                            state->grid[row][currentCol + 1] = state->grid[row][currentCol];
                            state->grid[row][currentCol] = 0;
                            currentCol++;
                            moved = true;
                        }
                        if (currentCol < GRID_SIZE - 1 && state->grid[row][currentCol + 1] == state->grid[row][currentCol]) {
                            state->grid[row][currentCol + 1] *= 2;
                            state->currentScore += state->grid[row][currentCol + 1];
                            state->grid[row][currentCol] = 0;
                            moved = true;
                        }
                    }
                }
            }
            break;
    }

    return moved;
}

// Spawn a random number (2 or 4) in an empty cell
void spawnRandomNumber(int grid[GRID_SIZE][GRID_SIZE]) {
    int emptyCells[GRID_SIZE * GRID_SIZE][2];
    int count = 0;

    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            if (grid[row][col] == 0) {
                emptyCells[count][0] = row;
                emptyCells[count][1] = col;
                count++;
            }
        }
    }

    if (count == 0) return;

    int index = rand() % count;
    grid[emptyCells[index][0]][emptyCells[index][1]] = (rand() % 10 < 9) ? 2 : 4;
}

// Initialize the game state
void initGame(GameState *state) {
    memset(state->grid, 0, sizeof(state->grid));
    state->currentScore = 0;
    state->gameOver = false;
    state->paused = false;
    state->stopped = false;
    spawnRandomNumber(state->grid);
    spawnRandomNumber(state->grid);

    // Charger le meilleur score au dÃ©marrage
    loadBestScore();
}

// Draw the header (title and score)
void drawHeader(SDL_Renderer *renderer, TTF_Font *font, GameState *state, const char *title, int startX, int startY) {
    char buffer[128];
    sprintf(buffer, "%s - Score: %d - Best: %d", title, state->currentScore, bestScore);

    SDL_Surface *surface = TTF_RenderText_Blended(font, buffer, black);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect = {startX, startY, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Draw the grid with numbers
void drawGrid(SDL_Renderer *renderer, TTF_Font *font, GameState *state, int startX, int startY) {
    int cellSize = GRID_RENDER_SIZE / GRID_SIZE;

    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            SDL_Rect cell = {startX + col * cellSize, startY + row * cellSize, cellSize, cellSize};
            int colorIndex = (row * GRID_SIZE + col) % 10;

            SDL_Color color = (state->grid[row][col] == 0) ? beige : colors[colorIndex];
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &cell);

            if (state->grid[row][col] != 0) {
                char number[8];
                sprintf(number, "%d", state->grid[row][col]);
                SDL_Surface *surface = TTF_RenderText_Blended(font, number, black);
                SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

                SDL_Rect textRect = {cell.x + 20, cell.y + 20, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, NULL, &textRect);

                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
            }
        }
    }
}

// Draw the elapsed time
void drawTime(SDL_Renderer *renderer, TTF_Font *font, Uint32 startTime, int startX, int startY) {
    Uint32 elapsedTime = SDL_GetTicks() - startTime;
    int seconds = elapsedTime / 1000;

    char timeStr[32];
    sprintf(timeStr, "Time: %d", seconds);

    SDL_Surface *surface = TTF_RenderText_Blended(font, timeStr, black);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect = {startX, startY, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Draw the game over screen
void drawEndScreen(SDL_Renderer *renderer, TTF_Font *font) {
    const char *msg = "Game Over!";
    SDL_Surface *surface = TTF_RenderText_Blended(font, msg, black);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect = {WINDOW_WIDTH / 2 - surface->w / 2, WINDOW_HEIGHT / 2 - surface->h / 2, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Check if a move is possible
bool canMove(int grid[GRID_SIZE][GRID_SIZE]) {
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            if (grid[row][col] == 0) return true;
            if (col < GRID_SIZE - 1 && grid[row][col] == grid[row][col + 1]) return true;
            if (row < GRID_SIZE - 1 && grid[row][col] == grid[row + 1][col]) return true;
        }
    }
    return false;
}

/*----------------------------------------MODE MACHINE-------------------------------------------------------*/

// Evaluate a move and return the score
int evaluateMove(GameState *state, int direction) {
    GameState tempState;
    memcpy(&tempState, state, sizeof(GameState));

    if (!slideGrid(&tempState, direction)) {
        return -1;
    }

    return tempState.currentScore;
}

// Make the machine play
void machineMove(GameState *state) {
    int bestMove = -1;
    int bestScore = -1;

    for (int direction = 0; direction < 4; direction++) {
        int score = evaluateMove(state, direction);
        if (score > bestScore) {
            bestScore = score;
            bestMove = direction;
        }
    }

    if (bestMove != -1) {
        if (slideGrid(state, bestMove)) {
            spawnRandomNumber(state->grid);
        }
    }
}

/*----------------------------------------MENU----------------------------------------------------*/

// Show the welcome screen
void showWelcomeScreen(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_SetRenderDrawColor(renderer, 0, 139, 139, 255);
    SDL_RenderClear(renderer);

    const char *welcomeText = "Number Slide";
    SDL_Color colors[] = {
        {0, 0, 255, 255}, {255, 255, 102, 255}, {255, 0, 0, 255},
        {255, 192, 203, 255}, {245, 245, 220, 255}, {128, 0, 128, 255},
        {255, 215, 0, 255}, {0, 255, 0, 255}, {255, 182, 193, 255},
        {255, 0, 255, 255}, {128, 0, 128, 255}
    };

    int totalLetters = strlen(welcomeText);
    int totalWidth = 0;

    for (int i = 0; i < totalLetters; i++) {
        char text[2] = {welcomeText[i], '\0'};
        SDL_Surface *surface = TTF_RenderText_Blended(font, text, colors[i % 11]);
        if (surface) {
            totalWidth += surface->w;
            SDL_FreeSurface(surface);
        }
    }

    int startX = (WINDOW_WIDTH - totalWidth) / 2;
    int posY = ((WINDOW_HEIGHT - 48) / 2) - 14;

    int currentX = startX;

    for (int i = 0; i < totalLetters; i++) {
        char text[2] = {welcomeText[i], '\0'};
        SDL_Surface *surface = TTF_RenderText_Blended(font, text, colors[i % 11]);
        if (surface) {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

            SDL_Rect rect = {currentX, posY, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            currentX += surface->w;

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(3000);
}

// Handle menu input
GameMode handleMenuInput(SDL_Event *event, int *currentSelection, SDL_Rect *buttons) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    SDL_Point mousePoint = {mouseX, mouseY};

    if (event->type == SDL_MOUSEMOTION) {
        for (int i = 0; i < 4; i++) {
            if (SDL_PointInRect(&mousePoint, &buttons[i])) {
                *currentSelection = i;
            }
        }
    }

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        if (SDL_PointInRect(&mousePoint, &buttons[*currentSelection])) {
            switch (*currentSelection) {
                case 0: return PLAYER_MODE;
                case 1: return MACHINE_MODE;
                case 2: return PLAYER_VS_MACHINE_MODE;
                case 3: return QUIT;
                default: return PLAYER_MODE;
            }
        }
    }

    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_UP) {
            *currentSelection = (*currentSelection == 0) ? 3 : *currentSelection - 1;
        } else if (event->key.keysym.sym == SDLK_DOWN) {
            *currentSelection = (*currentSelection == 3) ? 0 : *currentSelection + 1;
        } else if (event->key.keysym.sym == SDLK_RETURN) {
            switch (*currentSelection) {
                case 0: return PLAYER_MODE;
                case 1: return MACHINE_MODE;
                case 2: return PLAYER_VS_MACHINE_MODE;
                case 3: return QUIT;
                default: return PLAYER_MODE;
            }
        }
    }

    return MENU_MODE;
}

// Draw the mode selection screen
void drawModeSelectionScreen(SDL_Renderer *renderer, TTF_Font *font, int currentSelection, SDL_Rect *buttons) {
    SDL_SetRenderDrawColor(renderer, 100, 120, 139, 255);
    SDL_RenderClear(renderer);

    const char *options[] = {"Player Mode", "Machine Mode", "Player vs Machine", "Quit"};
    SDL_Color textColor = {255, 255, 255, 255}; // White text (unchanged)
    SDL_Color buttonColor = {255, 165, 0, 255}; // Orange buttons
    SDL_Color selectedColor = {255, 200, 0, 255}; // Light orange for hover/selected effect

    for (int i = 0; i < 4; i++) {
        // Set button color (orange for normal, light orange for selected/hover)
        SDL_SetRenderDrawColor(renderer, i == currentSelection ? selectedColor.r : buttonColor.r,
                                           i == currentSelection ? selectedColor.g : buttonColor.g,
                                           i == currentSelection ? selectedColor.b : buttonColor.b, 255);
        SDL_RenderFillRect(renderer, &buttons[i]);

        // Render the button text
        SDL_Surface *surface = TTF_RenderText_Blended(font, options[i], textColor);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        // Center the text on the button
        SDL_Rect textRect = {buttons[i].x + (buttons[i].w - surface->w) / 2,
                             buttons[i].y + (buttons[i].h - surface->h) / 2,
                             surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &textRect);

        // Clean up
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

void drawStopButton(SDL_Renderer *renderer, TTF_Font *font) {
    // Draw the button background
    SDL_SetRenderDrawColor(renderer, button.r, button.g, button.b, button.a);
    SDL_RenderFillRect(renderer, &stopButton);

    // Render the text "Stop"
    const char *text = "Stop";
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, white);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Center the text on the button
    SDL_Rect textRect = {
        stopButton.x + (stopButton.w - surface->w) / 2,
        stopButton.y + (stopButton.h - surface->h) / 2,
        surface->w,
        surface->h
    };
    SDL_RenderCopy(renderer, texture, NULL, &textRect);

    // Clean up
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

bool isPointInRect(int x, int y, SDL_Rect rect) {
    return (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);
}

/*---------------------------------------MAIN------------------------------------------------*/

int main(int argc, char *argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Create window and renderer
    SDL_Window *window = SDL_CreateWindow("Number Slide", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load fonts
    TTF_Font *font = TTF_OpenFont(FONT_PATH, 24);
    if (!font) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font *font48 = TTF_OpenFont(FONT_PATH, 48);
    if (!font48) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    // Show welcome screen
    showWelcomeScreen(renderer, font48);

    // Main game loop
    bool running = true;

    SDL_Event event;
    int currentSelection = 0;
    GameMode gameState = MENU_MODE;

    // Button zones for menu
    SDL_Rect buttons[4];
    const int buttonWidth = 300, buttonHeight = 50, startY = 200;
    for (int i = 0; i < 4; i++) {
        buttons[i] = (SDL_Rect) {
            (WINDOW_WIDTH - buttonWidth) / 2,
            startY + i * (buttonHeight + 20),
            buttonWidth,
            buttonHeight
        };
    }

    // Declaration of game state
    GameState PLAYER;
    GameState MACHINE;
    Uint32 startTime = SDL_GetTicks();
    GameState playerState;
    GameState machineState;

    // Load the best score at the start
    loadBestScore();

    while (running) {
        // Handle events based on the current game mode
        switch (gameState) {
            case MENU_MODE:
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        running = false;
                    }
                    gameState = handleMenuInput(&event, &currentSelection, buttons);
                }

                // Render the menu screen
                SDL_RenderClear(renderer);
                drawModeSelectionScreen(renderer, font, currentSelection, buttons);
                SDL_RenderPresent(renderer);
                break;

            /*--------------------------------------MODE-PLAYER-----------------------------------------*/
            case PLAYER_MODE:
                initGame(&PLAYER);

                while (running) {
                    while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT) {
                            running = false;
                        }
                        if (event.type == SDL_MOUSEBUTTONDOWN) {
                            int mouseX = event.button.x;
                            int mouseY = event.button.y;
                            if (isPointInRect(mouseX, mouseY, stopButton)) {
                                gameState = MENU_MODE; // Go back to the menu
                            }
                        }
                        if (event.type == SDL_KEYDOWN && !PLAYER.gameOver) {
                            Direction dir;
                            switch (event.key.keysym.sym) {
                                case SDLK_UP: dir = UP; break;
                                case SDLK_DOWN: dir = DOWN; break;
                                case SDLK_LEFT: dir = LEFT; break;
                                case SDLK_RIGHT: dir = RIGHT; break;
                                default: continue; // Ignore other keys
                            }
                            if (slideGrid(&PLAYER, dir)) {
                                spawnRandomNumber(PLAYER.grid);
                                if (!canMove(PLAYER.grid)) {
                                    PLAYER.gameOver = true;
                                    gameState = MENU_MODE;
                                }
                                // VÃ©rifier et mettre Ã  jour le meilleur score
                                if (PLAYER.currentScore > bestScore) {
                                    bestScore = PLAYER.currentScore;
                                    saveBestScore();
                                }
                            }
                        }
                    }

                    // Render the game screen
                    SDL_RenderClear(renderer);
                    drawHeader(renderer, font, &PLAYER, "Player", 50, 50);
                    drawGrid(renderer, font, &PLAYER, 400, 300);
                    drawTime(renderer, font, startTime, WINDOW_WIDTH - 150, 50);
                    drawStopButton(renderer, font);
                    SDL_RenderPresent(renderer);

                    // Break out of the loop if returning to the menu
                    if (gameState == MENU_MODE) {
                        break;
                    }
                }
                break;

            /*-----------------------MODE-MACHINE--------------------------*/
            case MACHINE_MODE:
                initGame(&MACHINE);
                while (running) {
                    // Stop button
                    while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT) {
                            running = false;
                        }
                        if (event.type == SDL_MOUSEBUTTONDOWN) {
                            int mouseX = event.button.x;
                            int mouseY = event.button.y;
                            if (isPointInRect(mouseX, mouseY, stopButton)) {
                                gameState = MENU_MODE; // Go back to the menu
                            }
                        }
                    }
                    if (!MACHINE.gameOver) {
                        machineMove(&MACHINE);
                        if (!canMove(MACHINE.grid)) {
                            MACHINE.gameOver = true;
                        }
                        // VÃ©rifier et mettre Ã  jour le meilleur score
                        if (MACHINE.currentScore > bestScore) {
                            bestScore = MACHINE.currentScore;
                            saveBestScore();
                        }
                    }

                    // Render the game screen
                    SDL_RenderClear(renderer);
                    drawHeader(renderer, font, &MACHINE, "Machine", 50, 50);
                    drawGrid(renderer, font, &MACHINE, 400, 300);
                    drawTime(renderer, font, startTime, WINDOW_WIDTH - 150, 50);
                    drawStopButton(renderer, font);
                    SDL_Delay(950);  // AI makes a move every 900 ms
                    SDL_RenderPresent(renderer);

                    // Break out of the loop if returning to the menu
                    if (gameState == MENU_MODE) {
                        break;
                    }
                }
                break;

            /*--------------------------MODE-PLAYER-VS-MACHINE----------------------------------*/
            case PLAYER_VS_MACHINE_MODE:
                initGame(&playerState);
                initGame(&machineState);
                while (running) {
                    while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT) {
                            running = false;
                        }
                        // Stop button
                        if (event.type == SDL_MOUSEBUTTONDOWN) {
                            int mouseX = event.button.x;
                            int mouseY = event.button.y;
                            if (isPointInRect(mouseX, mouseY, stopButton)) {
                                gameState = MENU_MODE; // Go back to the menu
                            }
                        }
                        // Mode player
                        if (!playerState.gameOver) {
                            if (event.type == SDL_KEYDOWN) {
                                Direction dir;
                                switch (event.key.keysym.sym) {
                                    case SDLK_UP: dir = UP; break;
                                    case SDLK_DOWN: dir = DOWN; break;
                                    case SDLK_LEFT: dir = LEFT; break;
                                    case SDLK_RIGHT: dir = RIGHT; break;
                                    default: continue; // Ignore other keys
                                }
                                if (slideGrid(&playerState, dir)) {
                                    spawnRandomNumber(playerState.grid);
                                    if (!canMove(playerState.grid)) {
                                        playerState.gameOver = true;
                                    }
                                    // VÃ©rifier et mettre Ã  jour le meilleur score
                                    if (playerState.currentScore > bestScore) {
                                        bestScore = playerState.currentScore;
                                        saveBestScore();
                                    }
                                }
                            }
                        }
                    }
                    // Mode machine
                    if (!machineState.gameOver) {
                        SDL_Delay(500);
                        machineMove(&machineState);
                        if (!canMove(machineState.grid)) {
                            machineState.gameOver = true;
                        }
                        // VÃ©rifier et mettre Ã  jour le meilleur score
                        if (machineState.currentScore > bestScore) {
                            bestScore = machineState.currentScore;
                            saveBestScore();
                        }
                    }

                    // Clear screen and render elements
                    SDL_RenderClear(renderer);
                    drawHeader(renderer, font, &playerState, "Player", 50, 50);
                    drawGrid(renderer, font, &playerState, 50, 100);
                    drawTime(renderer, font, startTime, WINDOW_WIDTH - 150, 50);
                    drawHeader(renderer, font, &machineState, "Machine", 700, 50);
                    drawGrid(renderer, font, &machineState, 700, 100);
                    drawStopButton(renderer, font);
                    SDL_RenderPresent(renderer);

                    // Break out of the loop if returning to the menu
                    if (gameState == MENU_MODE) {
                        break;
                    }
                }
                break;

            case QUIT:
                running = false;
                break;

            default:
                break;
        }
    }

    // Clean up
    TTF_CloseFont(font);
    TTF_CloseFont(font48);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    // Save the best score at the end
    saveBestScore();

    return 0;
}

