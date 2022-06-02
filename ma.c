#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define MAZE_WIDTH  10
#define MAZE_HEIGHT 10

#define THEME DEFAULT

#define DEFAULT_WALL '|'
#define DEFAULT_FLOOR '_'
#define DEFAULT_BLANK ' '

#define FUTURE_WALL '/'
#define FUTURE_FLOOR '_'
#define FUTURE_BLANK ' '

#define SURREAL_WALL '@'
#define SURREAL_FLOOR '?'
#define SURREAL_BLANK '%'

#define MAKE_THEME(A, B) MAKE_THEME_T(A, B)
#define MAKE_THEME_T(A, B) A ## _ ## B
#define ERROR_EXIT(A) {\
    fprintf(stderr, A);\
    exit(EXIT_FAILURE);\
}

enum Direction {
    NORTH, EAST, SOUTH, WEST, ALL_DIR
};

enum SolveResult {
    DEADEND, SOLUTION
};

union Tile {
    struct {
        unsigned char n : 1;
        unsigned char e : 1;
        unsigned char s : 1;
        unsigned char w : 1;
    };
    unsigned char ov;
};

struct Maze {
    size_t ys;
    size_t ye;
    union Tile map[MAZE_HEIGHT][MAZE_WIDTH];
};

static struct {
    signed char x : 2;
    signed char y : 2;
} MOV_DIR[] = {
    {0, -1},
    {1, 0},
    {0, 1},
    {-1, 0}
};

// Maze Functions
void solveMaze(struct Maze* maze);
void dispMaze(struct Maze* maze);
void makeMaze(struct Maze* maze);

// Helper Functions
void shuffle(enum Direction* array, size_t n);
void recuMaze(union Tile map[MAZE_HEIGHT][MAZE_WIDTH], int x, int y);
int recuSolve(struct Maze* maze, enum Direction prev, size_t x, size_t y);

int
main(void) {
    srand(time(NULL));
    
    HANDLE haOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD moPrev;
    GetConsoleMode(haOutput, &moPrev);
    if (!SetConsoleMode(haOutput, moPrev | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        ERROR_EXIT("Virtual Terminal Sequences not supported...try on Windows 10!");
    }
    
    SetConsoleCursorInfo(haOutput, &(CONSOLE_CURSOR_INFO) {100, FALSE});
   
    struct Maze maze = {0};
    
    makeMaze(&maze);
    
    dispMaze(&maze);
    
    puts("Press Enter to Solve");
    getchar();
    
    solveMaze(&maze);

}

void
solveMaze(struct Maze* maze) {
    recuSolve(maze, EAST, 0, maze->ys);
    printf("\x1b[48;2;0;0;0m\x1b[%d;1H", MAZE_HEIGHT + 1);
}

enum SolveResult
recuSolve(struct Maze* maze, enum Direction prev, size_t x, size_t y) {
    static enum Direction oppDir[] = {
        SOUTH, WEST, NORTH, EAST
    };
    
    for (size_t i = 0; i < ALL_DIR; i++) {
        if (maze->map[y][x].ov>>i & 1 && i != oppDir[prev]) { // went in that direction
            size_t xn = x + MOV_DIR[i].x, yn = y + MOV_DIR[i].y;
            if (xn == MAZE_WIDTH - 1 && yn == maze->ye) { // yf is final y
                printf("\x1b[48;2;82;48;117m\x1b[%zd;%zdH%c", y + 2, x * 2 + 2, maze->map[y][x].s ? ' ' : '_');
                if (prev == EAST) {
                    puts("\x1b[2D_");
                }
                printf("\x1b[48;2;82;48;117m\x1b[%zd;%zdH%c ", yn + 2, xn * 2 + 2, maze->map[yn][xn].s ? ' ' : '_');
                if (i == EAST) {
                    puts("\x1b[3D_");
                }
                return SOLUTION;
            }
            if (recuSolve(maze, i, xn, yn)) {
                printf("\x1b[48;2;82;48;117m\x1b[%zd;%zdH%c", y + 2, x * 2 + 2, maze->map[y][x].s ? ' ' : '_');
                if (prev == EAST) {
                    puts(!x && y == maze->ys ? "\x1b[2D " : "\x1b[2D_");
                } else if (prev == WEST) {
                    putchar('_');
                }
                return SOLUTION;
            }
        }
    }
    return DEADEND;
}

void
makeMaze(struct Maze* maze) {
    maze->ys = rand() % MAZE_HEIGHT, maze->ye = rand() % MAZE_HEIGHT;
    recuMaze(maze->map, rand() % MAZE_WIDTH, rand() % MAZE_HEIGHT);
}

void
dispMaze(struct Maze* maze) {
    printf("%s", "\x1b[2J\x1b[1;1H\x1b[38;2;255;255;255m\x1b[A");
    for (size_t x = 0; x < MAZE_WIDTH * 2 + 1; x++) {
       putchar(MAKE_THEME(THEME, FLOOR));
    }
    putchar('\n');
    
    for (size_t y = 0; y < MAZE_HEIGHT; y++) {
        for (size_t x = 0; x < MAZE_WIDTH; x++) {
            if (!x) {
                putchar(y == maze->ys ? MAKE_THEME(THEME, BLANK) : MAKE_THEME(THEME, WALL));
            }   
            putchar(maze->map[y][x].s ? MAKE_THEME(THEME, BLANK) : MAKE_THEME(THEME, FLOOR));
            if (x == MAZE_WIDTH - 1 && y == maze->ye) {
                putchar(MAKE_THEME(THEME, BLANK));
            } else {
                putchar(maze->map[y][x].e ? MAKE_THEME(THEME, FLOOR) : MAKE_THEME(THEME, WALL));
            }
        }
        putchar('\n');
    }
    
}

void
recuMaze(union Tile map[MAZE_HEIGHT][MAZE_WIDTH], int x, int y) {
    static enum Direction dirAll[] = {
        NORTH, EAST, SOUTH, WEST
    };
    shuffle(dirAll, ALL_DIR);

    for (size_t i = 0; i < ALL_DIR; i++) {
        int xn = x + MOV_DIR[dirAll[i]].x, yn = y + MOV_DIR[dirAll[i]].y;
        if (xn >= 0 && yn >= 0 && xn < MAZE_WIDTH && yn < MAZE_HEIGHT && !map[yn][xn].ov) {
            switch(dirAll[i]) {
                case NORTH:
                    map[y][x].n = map[yn][xn].s = 1;
                    break;
                case EAST:
                    map[y][x].e = map[yn][xn].w = 1;
                    break;
                case SOUTH:
                    map[y][x].s = map[yn][xn].n = 1;
                    break;
                case WEST:
                    map[y][x].w = map[yn][xn].e = 1;
                    break;
            }
            recuMaze(map, xn, yn);
        }
    }
}

// taken from https://benpfaff.org/writings/clc/shuffle.html
void
shuffle(enum Direction* array, size_t n) {
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          enum Direction t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}