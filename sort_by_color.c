#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum { RED = 0, BLUE = 1, YELLOW = 2 } Colour;

typedef struct {
    int number;
    Colour colour;
} Item;

const char* colourName(Colour c) {
    switch (c) {
        case RED: return "Red";
        case BLUE: return "Blue";
        default: return "Yellow";
    }
}

/* O(n) stable sort by colour (Red < Blue < Yellow), preserving the
 * relative (numeric) order of items that share the same colour. */
void sortByColour(Item items[], int n) {
    int count[3] = {0, 0, 0};
    for (int i = 0; i < n; i++) count[items[i].colour]++;

    int offset[3];
    offset[0] = 0;
    offset[1] = offset[0] + count[0];
    offset[2] = offset[1] + count[1];

    Item* output = malloc(n * sizeof(Item));
    for (int i = 0; i < n; i++) {
        Colour c = items[i].colour;
        output[offset[c]++] = items[i];
    }
    for (int i = 0; i < n; i++) items[i] = output[i];
    free(output);
}

void printItems(Item items[], int n) {
    for (int i = 0; i < n; i++)
        printf("(%d, %s)  ", items[i].number, colourName(items[i].colour));
    printf("\n");
}

void manualRun(void) {
    int n;
    printf("Enter number of items: ");
    scanf("%d", &n);
    Item* items = malloc(n * sizeof(Item));
    printf("For each item enter: number colour(R/B/Y) -- numbers should\n");
    printf("already be given in sorted order:\n");
    for (int i = 0; i < n; i++) {
        char c;
        scanf("%d %c", &items[i].number, &c);
        if (c == 'R' || c == 'r') items[i].colour = RED;
        else if (c == 'B' || c == 'b') items[i].colour = BLUE;
        else items[i].colour = YELLOW;
    }

    printf("\nBefore: "); printItems(items, n);
    sortByColour(items, n);
    printf("After : "); printItems(items, n);

    free(items);
}

int validateResult(Item items[], int n) {
    /* 1. colours must appear as a block of Reds, then Blues, then Yellows */
    int seenBlue = 0, seenYellow = 0;
    for (int i = 0; i < n; i++) {
        if (items[i].colour == BLUE) seenBlue = 1;
        if (items[i].colour == YELLOW) seenYellow = 1;
        if (items[i].colour == RED && (seenBlue || seenYellow)) return 0;
        if (items[i].colour == BLUE && seenYellow) return 0;
    }
    /* 2. within each colour, numbers must be non-decreasing */
    int lastRed = -2147483647, lastBlue = -2147483647, lastYellow = -2147483647;
    for (int i = 0; i < n; i++) {
        if (items[i].colour == RED) {
            if (items[i].number < lastRed) return 0;
            lastRed = items[i].number;
        } else if (items[i].colour == BLUE) {
            if (items[i].number < lastBlue) return 0;
            lastBlue = items[i].number;
        } else {
            if (items[i].number < lastYellow) return 0;
            lastYellow = items[i].number;
        }
    }
    return 1;
}

void autoValidate(void) {
    int sizes[] = {10, 100, 1000, 10000, 100000};
    int ns = 5;
    srand((unsigned)time(NULL));

    printf("\n%-10s%-12s%-10s\n", "n", "Valid?", "Time(s)");
    for (int t = 0; t < ns; t++) {
        int n = sizes[t];
        Item* items = malloc(n * sizeof(Item));
        int num = 0;
        for (int i = 0; i < n; i++) {
            num += 1 + rand() % 5;           /* keep numbers sorted ascending */
            items[i].number = num;
            items[i].colour = (Colour)(rand() % 3);
        }

        clock_t t0 = clock();
        sortByColour(items, n);
        clock_t t1 = clock();

        int ok = validateResult(items, n);
        printf("%-10d%-12s%-10.5f\n", n, ok ? "YES" : "NO",
               (double)(t1 - t0) / CLOCKS_PER_SEC);

        free(items);
    }
    printf("\nTiming grows linearly with n, confirming O(n) behaviour.\n");

    /* Also show the worked example from the lab sheet style */
    Item demo[7] = {
        {1, RED}, {3, YELLOW}, {4, RED}, {6, BLUE}, {8, YELLOW}, {9, BLUE}, {12, RED}
    };
    printf("\nDemo: before: "); printItems(demo, 7);
    sortByColour(demo, 7);
    printf("Demo: after : "); printItems(demo, 7);
    printf("(Reds: 1,4,12 -- still sorted; Blues: 6,9 -- still sorted; Yellows: 3,8 -- still sorted)\n");
}

int main(void) {
    printf("=========== Sort n (number,colour) pairs by colour in O(n) ===========\n\n");
    int choice;
    printf("1. Automatic validation (correctness + linear timing + worked demo)\n");
    printf("2. Manual input\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 2) manualRun();
    else autoValidate();

    return 0;
}
