#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define GRID_SIZE 19
#define CELL_SIZE 30
#define COLOR_COUNT 8
#define WINDOW_WIDTH (GRID_SIZE * CELL_SIZE)
#define WINDOW_HEIGHT (GRID_SIZE * CELL_SIZE + 50)

int grid[GRID_SIZE][GRID_SIZE] = {0};
int clipboard[GRID_SIZE][GRID_SIZE] = {0};
SDL_Rect selection = {-1, -1, 0, 0};
int currentColor = 1;
SDL_Color colors[COLOR_COUNT] = {
    {0x01, 0x09, 0x09, 0xFF},  // #010909
    {0x08, 0x1A, 0x45, 0xFF},  // #081A45
    {0x37, 0x0B, 0x83, 0xFF},  // #370B83
    {0xC5, 0x0D, 0xBE, 0xFF},  // #C50DBE
    {0xF6, 0x1E, 0x5F, 0xFF},  // #F61E5F
    {0xFC, 0x93, 0x5A, 0xFF},  // #FC935A
    {0xFD, 0xFF, 0x99, 0xFF},  // #FDFF99
    {0xE7, 0xFF, 0xDB, 0xFF}   // #E7FFDB
};

void drawGrid(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            SDL_Rect cellRect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            SDL_SetRenderDrawColor(renderer, colors[grid[y][x]].r, colors[grid[y][x]].g, colors[grid[y][x]].b, colors[grid[y][x]].a);
            SDL_RenderFillRect(renderer, &cellRect);
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderDrawRect(renderer, &cellRect);
        }
    }

    if (selection.w > 0 && selection.h > 0) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);
        SDL_Rect largeSelection = {
            selection.x * CELL_SIZE, 
            selection.y * CELL_SIZE, 
            selection.w * CELL_SIZE, 
            selection.h * CELL_SIZE
        };
        SDL_RenderDrawRect(renderer, &largeSelection);
    }

    for (int i = 0; i < COLOR_COUNT; i++) {
        SDL_Rect colorRect = {i * (WINDOW_WIDTH / COLOR_COUNT), WINDOW_HEIGHT - 40, WINDOW_WIDTH / COLOR_COUNT, 30};
        SDL_SetRenderDrawColor(renderer, colors[i].r, colors[i].g, colors[i].b, colors[i].a);
        SDL_RenderFillRect(renderer, &colorRect);
        if (i == currentColor) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &colorRect);
        }
    }

    SDL_RenderPresent(renderer);
}

void setCell(int x, int y, int color) {
    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
        grid[y][x] = color;
    }
}

void nudgeGrid(int dx, int dy) {
    int temp[GRID_SIZE][GRID_SIZE];
    memcpy(temp, grid, sizeof(grid));
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            int newX = (x + dx + GRID_SIZE) % GRID_SIZE;
            int newY = (y + dy + GRID_SIZE) % GRID_SIZE;
            grid[newY][newX] = temp[y][x];
        }
    }
}

void copySelection() {
    for (int y = 0; y < selection.h; y++) {
        for (int x = 0; x < selection.w; x++) {
            clipboard[y][x] = grid[selection.y + y][selection.x + x];
        }
    }
}

void cutSelection() {
    copySelection();
    for (int y = 0; y < selection.h; y++) {
        for (int x = 0; x < selection.w; x++) {
            grid[selection.y + y][selection.x + x] = 0;
        }
    }
}

void pasteSelection(int startX, int startY) {
    for (int y = 0; y < selection.h && y + startY < GRID_SIZE; y++) {
        for (int x = 0; x < selection.w && x + startX < GRID_SIZE; x++) {
            grid[startY + y][startX + x] = clipboard[y][x];
        }
    }
}

void printGridToConsole() {
    printf("{\n");
    for (int y = 0; y < GRID_SIZE; y++) {
        printf("    {");
        for (int x = 0; x < GRID_SIZE; x++) {
            printf("%d", grid[y][x]);
            if (x < GRID_SIZE - 1) printf(",");
        }
        printf("}");
        if (y < GRID_SIZE - 1) printf(",");
        printf("\n");
    }
    printf("}\n");
}

void importData() {
    char line[1000];
    int row = 0;
    while (fgets(line, sizeof(line), stdin) && row < GRID_SIZE) {
        char *token = strtok(line, "{,}");
        int col = 0;
        while (token != NULL && col < GRID_SIZE) {
            grid[row][col] = atoi(token);
            token = strtok(NULL, "{,}");
            col++;
        }
        row++;
    }
}

int main(int argc, char* args[]) {
    if (argc > 1 && strcmp(args[1], "import") == 0) {
        importData();
        printGridToConsole();
        return 0;
    }

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("19x19 Grid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    bool quit = false;
    SDL_Event e;
    bool isSelecting = false;
    int startX, startY;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                int gridX = mouseX / CELL_SIZE;
                int gridY = mouseY / CELL_SIZE;

                if (mouseY >= WINDOW_HEIGHT - 40) {
                    currentColor = mouseX / (WINDOW_WIDTH / COLOR_COUNT);
                } else if (SDL_GetModState() & KMOD_LSHIFT) {
                    isSelecting = true;
                    startX = gridX;
                    startY = gridY;
                } else if (SDL_GetModState() & KMOD_LGUI) {
                    pasteSelection(gridX, gridY);
                } else {
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        setCell(gridX, gridY, currentColor);
                    } else if (e.button.button == SDL_BUTTON_RIGHT) {
                        setCell(gridX, gridY, 0);
                    }
                }
            } else if (e.type == SDL_MOUSEBUTTONUP) {
                if (isSelecting) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    int endX = mouseX / CELL_SIZE;
                    int endY = mouseY / CELL_SIZE;
                    selection.x = (startX < endX) ? startX : endX;
                    selection.y = (startY < endY) ? startY : endY;
                    selection.w = abs(endX - startX) + 1;
                    selection.h = abs(endY - startY) + 1;
                    isSelecting = false;
                }
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_SPACE:
                        printGridToConsole();
                        break;
                    case SDLK_UP:
                        nudgeGrid(0, -1);
                        break;
                    case SDLK_DOWN:
                        nudgeGrid(0, 1);
                        break;
                    case SDLK_LEFT:
                        nudgeGrid(-1, 0);
                        break;
                    case SDLK_RIGHT:
                        nudgeGrid(1, 0);
                        break;
                    case SDLK_c:
                        if (SDL_GetModState() & KMOD_LGUI) {
                            copySelection();
                        }
                        break;
                    case SDLK_x:
                        if (SDL_GetModState() & KMOD_LGUI) {
                            cutSelection();
                        }
                        break;
                }
            }
        }
        drawGrid(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}