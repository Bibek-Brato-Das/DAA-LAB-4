/*
 * DAA Lab-04, Q5: Application of Sorting - V
 * ---------------------------------------------------------------
 * Given a list I of n intervals (x_i, y_i), merge all overlapping
 * ones. E.g. {(1,3),(2,6),(8,10),(7,18)} -> {(1,6),(7,18)}.
 * Required: worst-case O(n log n).
 *
 * ALGORITHM
 * -----------
 *   1. Sort the intervals by start x_i               -- O(n log n)
 *   2. Walk through once, keeping a "current merged interval":
 *        - if the next interval's start <= current interval's end,
 *          they overlap (or touch) -> extend current's end to
 *          max(current.end, next.end)
 *        - otherwise, the current merged interval is finalised;
 *          the next interval starts a new merged interval
 *                                                     -- O(n)
 * Total: O(n log n), dominated by the sort.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct { int x, y; } Interval;

int cmpInterval(const void* pa, const void* pb) {
    const Interval* a = pa;
    const Interval* b = pb;
    return a->x - b->x;
}

/* Merges intervals in-place style: writes the merged result into
 * 'out' (caller-allocated, capacity >= n) and returns how many
 * merged intervals were produced. Does not modify 'in'. */
int mergeIntervals(Interval in[], int n, Interval out[]) {
    if (n == 0) return 0;
    Interval* sorted = malloc(n * sizeof(Interval));
    for (int i = 0; i < n; i++) sorted[i] = in[i];
    qsort(sorted, n, sizeof(Interval), cmpInterval);

    int m = 0;
    out[0] = sorted[0];
    for (int i = 1; i < n; i++) {
        if (sorted[i].x <= out[m].y) {
            if (sorted[i].y > out[m].y) out[m].y = sorted[i].y;
        } else {
            m++;
            out[m] = sorted[i];
        }
    }
    free(sorted);
    return m + 1;
}

void printIntervals(Interval arr[], int n) {
    printf("{");
    for (int i = 0; i < n; i++) printf("(%d,%d)%s", arr[i].x, arr[i].y, (i < n - 1) ? ", " : "");
    printf("}\n");
}

void manualRun(void) {
    int n;
    printf("Enter number of intervals: ");
    scanf("%d", &n);
    Interval* in = malloc(n * sizeof(Interval));
    printf("Enter %d intervals as 'x y' pairs:\n", n);
    for (int i = 0; i < n; i++) scanf("%d %d", &in[i].x, &in[i].y);

    Interval* out = malloc(n * sizeof(Interval));
    int m = mergeIntervals(in, n, out);

    printf("\nInput : "); printIntervals(in, n);
    printf("Merged: "); printIntervals(out, m);

    free(in); free(out);
}

/* naive check: verify no two OUTPUT intervals overlap/touch, and
 * that the union of the input intervals exactly matches the union
 * of the output intervals, by checking a sample of points. */
int verifyMerge(Interval in[], int n, Interval out[], int m) {
    /* output must be internally sorted and disjoint (gap between consecutive) */
    for (int i = 1; i < m; i++)
        if (out[i].x <= out[i - 1].y) return 0;

    /* sample coverage check at every input endpoint and midpoint */
    for (int i = 0; i < n; i++) {
        int pts[3] = {in[i].x, in[i].y, (in[i].x + in[i].y) / 2};
        for (int p = 0; p < 3; p++) {
            int coveredIn = 0, coveredOut = 0;
            for (int j = 0; j < n; j++)
                if (in[j].x <= pts[p] && pts[p] <= in[j].y) coveredIn = 1;
            for (int j = 0; j < m; j++)
                if (out[j].x <= pts[p] && pts[p] <= out[j].y) coveredOut = 1;
            if (coveredIn != coveredOut) return 0;
        }
    }
    return 1;
}

void autoValidate(void) {
    int sizes[] = {10, 100, 1000, 10000, 100000, 1000000};
    int ns = 6;
    srand((unsigned)time(NULL));

    printf("\n%-10s%-12s%-14s%-10s\n", "n", "MergedTo", "Time(s)", "Valid?");
    for (int t = 0; t < ns; t++) {
        int n = sizes[t];
        Interval* in = malloc(n * sizeof(Interval));
        for (int i = 0; i < n; i++) {
            int start = rand() % (n * 4);
            int len = 1 + rand() % 20;
            in[i].x = start;
            in[i].y = start + len;
        }
        Interval* out = malloc(n * sizeof(Interval));

        clock_t c0 = clock();
        int m = mergeIntervals(in, n, out);
        clock_t c1 = clock();

        int ok = 1;
        if (n <= 5000) ok = verifyMerge(in, n, out, m);

        printf("%-10d%-12d%-14.5f%-10s\n", n, m,
               (double)(c1 - c0) / CLOCKS_PER_SEC,
               (n <= 5000) ? (ok ? "OK" : "FAIL") : "-");

        free(in); free(out);
    }

    /* worked example from the lab sheet */
    Interval demo[] = {{1, 3}, {2, 6}, {8, 10}, {7, 18}};
    Interval out[4];
    int m = mergeIntervals(demo, 4, out);
    printf("\nDemo: I = "); printIntervals(demo, 4);
    printf("Merged   = "); printIntervals(out, m);
    printf("(expected {(1,6), (7,18)})\n");
}

int main(void) {
    printf("=========== Merge Overlapping Intervals in O(n log n) ===========\n\n");
    int choice;
    printf("1. Automatic validation (random intervals, timing, correctness)\n");
    printf("2. Manual input\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 2) manualRun();
    else autoValidate();

    return 0;
}
