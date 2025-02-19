#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define GRID_SIZE 15
#define CELL_SIZE 40
#define WINDOW_SIZE (GRID_SIZE * CELL_SIZE)

int grid[GRID_SIZE][GRID_SIZE] = {0};

void drawGrid(SDL_Renderer *renderer) {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderClear(renderer);

    // Draw the grid
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            SDL_Rect cellRect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            if (grid[y][x]) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black
            }
            SDL_RenderFillRect(renderer, &cellRect);

            // Draw cell border
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);  // Gray
            SDL_RenderDrawRect(renderer, &cellRect);
        }
    }

    SDL_RenderPresent(renderer);
}

void toggleCell(int x, int y) {
    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
        grid[y][x] = !grid[y][x];
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

int main(int argc, char* args[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("15x15 Grid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_SIZE, WINDOW_SIZE, SDL_WINDOW_SHOWN);
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

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                int gridX = mouseX / CELL_SIZE;
                int gridY = mouseY / CELL_SIZE;
                toggleCell(gridX, gridY);
                drawGrid(renderer);
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_SPACE) {
                    printGridToConsole();
                }
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}