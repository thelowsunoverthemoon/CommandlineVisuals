#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Possible Symbols
// A - Z : move in current direction, except for constants
// +     : turn right
// -     : turn left
// {     : change colour to blue
// }     : change colour to purple
// ~     : pseudo-random colour
// |     : reverse direction (180 degree turn)

// Curve Presets
#define DRAGON_CURVE()               \
	char const rootString[] = "UQ";  \
	char const rootConst[] = "";     \
	struct Rule const ruleArray[] = {\
		{                            \
			'Q',                     \
			"Q+~EU+"                 \
		},                           \
		{                            \
			'E',                     \
			"-UQ-}E"                 \
		},                           \
		{0}                          \
    };

#define HILBERT_CURVE()              \
	char const rootString[] = "A";   \
	char const rootConst[] = "AB";   \
	struct Rule const ruleArray[] = {\
		{                            \
			'A',                     \
			"-BC+ACA+CB-"            \
		},                           \
		{                            \
			'B',                     \
			"+AC-B{CB-}CA+"          \
		},                           \
		{0}                          \
	};
	
#define MOSAIC_CURVE()                  \
	char const rootString[] = "A-A-A-A";\
	char const rootConst[] = "";        \
	struct Rule const ruleArray[] = {   \
		{                               \
			'A',                        \
			"AA-{A+A-A-A}A"             \
		},                              \
		{0}                             \
	};
	
#define ERROR_EXIT(A) {\
    puts(A);           \
    exit(EXIT_FAILURE);\
}

//struct Curve

struct Node {
	char         symbol;
	struct Node* next;
};

struct Rule {
	char  find;
	char* replace;
};

struct LinkPair {
	struct Node* new;
	struct Node* prev;
};

// Print Function
int printLSystem(HANDLE haInput, struct Node* node, char const rootConst[], SHORT x, SHORT y);

// Generate Functions
struct Node* genRootList(char const* rootString);
struct LinkPair genLinkList(char const* rootString, struct Node* next);
struct Node* genLSystem(struct Node* rootList, struct Rule const ruleArray[], size_t iter);

// Helper Functions
inline void adjAngle(int* angle, int num);
void searchRule(struct Rule const ruleArray[], struct Node* node, struct Node* next, struct Node** new, struct Node** prev);
void resizeConsole(HANDLE haOutput, SHORT fontX, SHORT fontY);

int
main(void) {
	
	srand(time(NULL));
	
    HANDLE haOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE haInput = GetStdHandle(STD_INPUT_HANDLE);
	
    if (!IsValidCodePage(65001)) {
        ERROR_EXIT("CodePage 65001 not installed");
    }
    SetConsoleOutputCP(65001);

    DWORD modePrev;
    GetConsoleMode(haOutput, &modePrev);
    if (!SetConsoleMode(haOutput, modePrev | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        ERROR_EXIT("Virtual Terminal Sequences not Supported");
    }
	
    resizeConsole(haOutput, 1, 1); 

    DRAGON_CURVE()
	
	struct Node* lSystem = genLSystem(genRootList(rootString), ruleArray, 13);
	
	SHORT sysX = 800, sysY = 100;
    if (!printLSystem(haInput, lSystem, rootConst, sysX, sysY)) {
		return EXIT_SUCCESS;
	}
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

int
printLSystem(HANDLE haInput, struct Node* node, char const rootConst[], SHORT x, SHORT y) {
	char static const* angSeq[] = {
		"\x1b[A\x1b[D█\x1b[A\x1b[D█",
		"██",
		"\x1b[D\x1b[B█\x1b[D\x1b[B█",
		"\x1b[2D█\x1b[2D█"
	};
	int angle = 0; 

	printf("\x1b[?25l\x1b[2J\x1b[38;2;132;194;209m\x1b[%d;%dH\x1b[s", y, x);
	while (node) {
        if (isalpha(node->symbol)) {
            for (int i = 0; rootConst[i]; i++) {
                if (node->symbol == rootConst[i]) {
                    goto skip;
                }
            }
            printf("\x1b[u%s\x1b[s", angSeq[angle]);
        } else {
            switch (node->symbol) {
                case '+':
                    adjAngle(&angle, 1);
                    break;
                case '-':
                    adjAngle(&angle, -1);
                    break;
                case '|':
                    adjAngle(&angle, 2);
                    break;
                case '~':
                    printf("\x1b[38;2;%d;%d;%dm", rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1);
                    break;
                case '{':
                    puts("\x1b[38;2;132;194;209m");
                    break;
                case '}':
                    puts("\x1b[38;2;125;93;194m");
                    break;
            }
        }
        skip:
        node = node->next;
	}
	return 1;
}

inline void
adjAngle(int* angle, int num) {
	*angle += num;
	if (*angle > 3) {
		*angle = *angle - 4;
	} else if (*angle < 0) {
		*angle = 4 + *angle;
	}
}

struct Node*
genRootList(char const* rootString) {
	return genLinkList(rootString, NULL).new;
}

void
searchRule(struct Rule const ruleArray[], struct Node* node, struct Node* next, struct Node** new, struct Node** prev) {
	for (int z = 0; ruleArray[z].find; z++) {
		if (node->symbol == ruleArray[z].find) {
			struct LinkPair pair = genLinkList(ruleArray[z].replace, next);
			free(node);
			*new = pair.new;
			*prev = pair.prev;
			break;
		}
	}
}

struct Node*
genLSystem(struct Node* rootList, struct Rule const ruleArray[], size_t iter) {
	for (int i = 0; i < iter; i++) {
		struct Node* next = rootList->next, * prev = rootList;
		searchRule(ruleArray, rootList, next, &rootList, &prev); // special case for replacing root (replace rootList)
		while (next) {
			searchRule(ruleArray, next, next->next, &prev->next, &next); // normal "insert" node
			prev = next;
			next = next->next;
		}
	}
	return rootList;
}

struct LinkPair
genLinkList(char const* rootString, struct Node* next) {
	struct Node* new, * prev;
	size_t  rootLen = strlen(rootString) - 1;
	for (int i = rootLen; i >= 0; i--) {
		new = malloc(sizeof(struct Node));
		if (!new) {
			ERROR_EXIT("Failed to Allocate Node");
		}
		if (i == rootLen) {  // the "last node"
			prev = new;
		}
		*new = (struct Node) {
			rootString[i],
			next
		};
		next = new;
	}
	return (struct LinkPair) {
		new,
		prev
	};
}

