// PhysicsSystem.cpp
// Frame (skeleton) for a falling-sand sandbox simulator.
// Every function exists and the file compiles, but the interesting parts are
// marked TODO for you to fill in, one stage at a time.

// ---------------------------------------------------------------------------
// 1. INCLUDES: pull in code that other people wrote so we can use it
// ---------------------------------------------------------------------------
#include <iostream>   // std::cout, for printing messages to the console
#include <vector>     // std::vector, a resizable list (we use it for the grid)
#include <cstdlib>    // rand(), for random numbers
#include <SDL2/SDL.h> // SDL2: opens the window, draws, reads mouse/keyboard

// This block only gets compiled when building with Emscripten (the web build).
// It lets you also compile the same file on your desktop for easier testing.
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// ---------------------------------------------------------------------------
// 2. CONSTANTS: values that never change while the program runs
// ---------------------------------------------------------------------------
const int GRID_W    = 200;  // number of cells across
const int GRID_H    = 150;  // number of cells down
const int CELL_SIZE = 4;    // each cell is drawn as a 4x4 pixel square
// Window size = 200*4 by 150*4 = 800x600, matching the canvas in shell.html

// ---------------------------------------------------------------------------
// 3. MATERIAL TYPES: names for what a cell can hold
// ---------------------------------------------------------------------------
enum Material {
    EMPTY,   // = 0
    SAND,    // = 1
    WALL,    // = 2
    WATER    // = 3
    // TODO: add more materials here later
};

// ---------------------------------------------------------------------------
// 4. GLOBAL VARIABLES: live outside functions so they survive between frames
// ---------------------------------------------------------------------------
std::vector<Material> grid(GRID_W * GRID_H, EMPTY); // every cell starts EMPTY
SDL_Window*   window   = nullptr;  // the window (set up in main)
SDL_Renderer* renderer = nullptr;  // the thing we draw with (set up in main)
Material currentMaterial = SAND;   // what the mouse places
bool running = true;               // becomes false when the window is closed
bool paused  = false;              // used by the desktop build's Stop button

// ---------------------------------------------------------------------------
// 5. GRID HELPERS: safe ways to read and write cells
// ---------------------------------------------------------------------------

// Is (x, y) actually inside the grid? Always check before touching the array.
bool inBounds(int x, int y) {
    // TODO: return true only if x is 0..GRID_W-1 AND y is 0..GRID_H-1
    return false;
}

// Read the material at column x, row y.
Material getCell(int x, int y) {
    // TODO: if out of bounds, return WALL (so things can't fall off the edge)
    // TODO: otherwise return grid[y * GRID_W + x]
    return EMPTY;
}

// Write a material into column x, row y.
void setCell(int x, int y, Material m) {
    // TODO: do nothing if out of bounds
    // TODO: otherwise grid[y * GRID_W + x] = m;
}

// ---------------------------------------------------------------------------
// 6. THE THREE JOBS OF EVERY FRAME: input, update, draw
// ---------------------------------------------------------------------------

void handleInput() {
    SDL_Event event;
    // SDL keeps a queue of things that happened; this loop empties the queue.
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        // TODO (later): number keys 1-4 change currentMaterial
    }

    // TODO: check if the left mouse button is held (SDL_GetMouseState)
    // TODO: convert mouse pixels to grid cells (divide by CELL_SIZE)
    // TODO: setCell(...) at that position with currentMaterial
}

void updateSimulation() {
    // TODO: loop through the grid from the BOTTOM row up to the top row
    //       (bottom-up so a grain isn't moved twice in one frame)
    // TODO: for each SAND cell: try below, then down-left/down-right (random order)
    // TODO (later): WATER rules, which also try moving sideways
}

void render() {
    // Fill the whole screen with the background color (R, G, B, alpha)
    SDL_SetRenderDrawColor(renderer, 12, 16, 24, 255);
    SDL_RenderClear(renderer);

    // TODO: loop over every cell
    // TODO: pick a color based on its Material (skip EMPTY)
    // TODO: SDL_SetRenderDrawColor(...) then SDL_RenderFillRect(...)
    //       using an SDL_Rect of size CELL_SIZE x CELL_SIZE

    SDL_RenderPresent(renderer); // show everything we just drew
}

// ---------------------------------------------------------------------------
// 7. ONE FRAME: the main loop calls this over and over
// ---------------------------------------------------------------------------
void frame() {
    handleInput();
    if (!paused) {
        updateSimulation();
    }
    render();
}

// ---------------------------------------------------------------------------
// 8. BUTTON FUNCTIONS: called by the toolbar in shell.html
// ---------------------------------------------------------------------------
void resetSimulation() {
    // TODO: set every cell in the grid back to EMPTY
}

// extern "C" keeps the function names plain so JavaScript can find them.
extern "C" {
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_KEEPALIVE void sim_pause()  { emscripten_pause_main_loop(); }
    EMSCRIPTEN_KEEPALIVE void sim_resume() { emscripten_resume_main_loop(); }
#else
    void sim_pause()  { paused = true; }   // desktop build version
    void sim_resume() { paused = false; }
#endif
    EMSCRIPTEN_KEEPALIVE void sim_reset()  { resetSimulation(); }
    EMSCRIPTEN_KEEPALIVE void sim_mute()   { /* TODO: only if you add sound */ }
    EMSCRIPTEN_KEEPALIVE void sim_unmute() { /* TODO: only if you add sound */ }
}

// ---------------------------------------------------------------------------
// 9. main(): where the program starts
// ---------------------------------------------------------------------------
int main() {
    // Start SDL's video system. It returns a negative number if it fails.
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL failed to start: " << SDL_GetError() << '\n';
        return 1;
    }

    // Create the window and the renderer.
    window = SDL_CreateWindow("Sandbox Simulator",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              GRID_W * CELL_SIZE, GRID_H * CELL_SIZE, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!window || !renderer) {
        std::cout << "Could not create window: " << SDL_GetError() << '\n';
        return 1;
    }

    std::cout << "Sandbox started!" << '\n'; // shows up in the page's console box

#ifdef __EMSCRIPTEN__
    // Web: hand frame() to the browser. 0 = match the screen's refresh rate,
    // true = keep running forever (the browser takes over from here).
    emscripten_set_main_loop(frame, 0, true);
#else
    // Desktop: a normal loop that runs until the window is closed.
    while (running) {
        frame();
        SDL_Delay(16); // wait ~16 ms, which is about 60 frames per second
    }
#endif

    // Clean up (only reached on desktop; the web version never gets here).
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
