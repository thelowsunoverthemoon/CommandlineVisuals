#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
    
#define ERROR_EXIT(A) {\
    puts(A);           \
    exit(EXIT_FAILURE);\
}

// Predefined Systems

#define FERN_IFS                          \
    {0.00, 0.00, 0.00, 0.16, 0.00, 0.00}, \
    {0.85, 0.04, -0.04, 0.85, 0.00, 1.60},\
    {0.20, -0.26, 0.23, 0.22, 0.00, 1.60},\
    {-0.15, 0.28, 0.26, 0.24, 0.00, 0.44}
    

#define MAPLE_IFS                           \
    {0.14, 0.01, 0.00, 0.51, -0.08, -1.31}, \
    {0.43, 0.52, -0.45, 0.50, 1.49, -0.75}, \
    {0.45, -0.49, 0.47, 0.47, -1.62, -0.74},\
    {0.49, 0.00, 0.00, 0.51, 0.02, 1.62}
        
#define SNOWFLAKE_IFS                       \
    {0.38, 0.00, 0.00, 0.38, 0.31, 0.57},   \
    {0.12, -0.36, 0.36, 0.11, 0.36, 0.33},  \
    {0.12, 0.36, -0.36, 0.11, 0.52, 0.69},  \
    {-0.31, -0.22, 0.22, -0.31, 0.61, 0.31},\
    {-0.31, 0.22, -0.22, -0.31, 0.70, 0.53},\
    {0.38, 0.00, 0.00, -0.38, 0.31, 0.68}
        
// Deterministic IFS
struct Transform {
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;
};

struct Position {
    double x;
    double y;
};

struct Node {
    struct Position pos;
    struct Node*    next;
};

// Helper Functions
void resizeConsole(HANDLE haOutput, SHORT fontX, SHORT fontY);
inline struct Node genNodeData(struct Node* next, struct Position* prevCoord, struct Transform* tran);
inline struct Node* genNode(struct Node* next, struct Position* prevCoord, struct Transform* tran);

// Gen Functions
struct Node* genRootList(SHORT x, SHORT y);
struct Node* genLinkList(struct Transform tranArray[], size_t tranSize, struct Node* next, struct Position* prevCoord);
struct Node* genIFS(struct Node* rootList, struct Transform tranArray[], size_t tranSize, size_t iter);

// Print Functions
void printIFS(struct Node* ifs, SHORT x, SHORT y, int multX, int multY);

int
main(void) {

    HANDLE haOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    
    if (!IsValidCodePage(65001)) {
        ERROR_EXIT("CodePage 65001 not installed");
    }
    SetConsoleOutputCP(65001);

    DWORD modePrev;
    GetConsoleMode(haOutput, &modePrev);
    if (!SetConsoleMode(haOutput, modePrev | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        ERROR_EXIT("Virtual Terminal Sequences not Supported");
    }
    
    resizeConsole(haOutput, 2, 2); 

    struct Transform tranArray[] = {
        FERN_IFS
    };

    struct Node* ifs = genIFS(
        genRootList(0, 0),
        tranArray,
        sizeof(tranArray)/sizeof(struct Transform),
        9
    );
    
    printIFS(ifs, 400, 5, 70, 20);

}

void
printIFS(struct Node* ifs, SHORT x, SHORT y, int multX, int multY) {
    puts("\x1b[?25l\x1b[2J");
    int v = rand() % 255 + 1;
    int l = rand() % 255 + 1;
    for (int i = 0, c = 1; ifs; ifs = ifs->next) {
        printf("\x1b[38;2;%d;%d;%dm\x1b[%d;%dH%s",
               l,
               v,
               255 - i,
               (int) roundf(ifs->pos.y * multY + y),
               (int) roundf(ifs->pos.x * multX + x),
               "█"
        );
        if (i == 255) {
            c = -1;
        } else if (i == 0) {
            c = 1, v = rand() % 255 + 1, l = rand() % 255 + 1;
        }
        i += c;
    }
    getchar();
}

struct Node*
genRootList(SHORT x, SHORT y) {
    return genNode(
        NULL,
        (struct Position[]) {
            {x, y}
        },
        (struct Transform[]) {
            0
        }
    );
}

inline struct Node*
genNode(struct Node* next, struct Position* prevCoord, struct Transform* tran) { // creates a new node
    struct Node* new = malloc(sizeof(struct Node));
    if (!new) {
        ERROR_EXIT("Failed to Allocate Node");
    }
    *new = genNodeData(next, prevCoord, tran);
    return new;
}

// Formula : x = ax + by + e, y = cx + dy + f
inline struct Node
genNodeData(struct Node* next, struct Position* prevCoord, struct Transform* tran) {
    return (struct Node) {
        (struct Position) {
            (tran->a * prevCoord->x) + (tran->b * prevCoord->y) + tran->e,
            (tran->c * prevCoord->x) + (tran->d * prevCoord->y) + tran->f
        },
        next
    };
}

struct Node*
genIFS(struct Node* rootList, struct Transform tranArray[], size_t tranSize, size_t iter) {
    for (int i = 0; i < iter; i++) {
        struct Node* node = rootList->next;
        *rootList = genNodeData( // reason for genNodeData and genNode, is that instead of malloc a new node, replace the root node
                        genLinkList(tranArray, tranSize, node, &rootList->pos), // returns "next" node (so goes in reverse)
                        &rootList->pos,
                        tranArray
                    );
        while (node) {
            struct Node* prev = node->next; // same thing but for rest of nodes
            *node = genNodeData(
                        genLinkList(tranArray, tranSize, node->next, &node->pos),
                        &node->pos,
                        tranArray
                    );
            node = prev;
        }
    }
    return rootList; // return the root
}

struct Node*
genLinkList(struct Transform tranArray[], size_t tranSize, struct Node* next, struct Position* prevCoord) {
    struct Node* new = NULL;
    for (int i = tranSize - 1; i > 0; i--) { // create a link list (add a node to each transform), returns last node
        new = genNode(
            next,
            prevCoord,
            tranArray + i
        );
        next = new;
    }
    return new;
}

void
resizeConsole(HANDLE haOutput, SHORT fontX, SHORT fontY) {
    CONSOLE_FONT_INFOEX font = {
        .cbSize = sizeof(CONSOLE_FONT_INFOEX)
    };
    GetCurrentConsoleFontEx(haOutput, FALSE, &font);
    font.dwFontSize = (COORD) {
        fontX, fontY
    };
    SetCurrentConsoleFontEx(haOutput, FALSE, &font);
    
    HWND hwnd = GetConsoleWindow();
    COORD newSize = GetLargestConsoleWindowSize(haOutput);
    SMALL_RECT dispRect = {
        0,
        0,
        newSize.X - 1,
        newSize.Y - 1
    };
    SetConsoleScreenBufferSize(haOutput, newSize);
    SetConsoleWindowInfo(haOutput, TRUE, &dispRect);
}