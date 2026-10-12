/*
 * DAA Lab-04, Q6: Application of Sorting - VI
 * ---------------------------------------------------------------
 * Given n intervals (l_i, r_i) on a line (endpoints count as being
 * IN the interval), find a point p covered by the largest number
 * of intervals, in O(n log n).
 *
 * ALGORITHM (sweep line, endpoint-inclusive)
 * ---------------------------------------------
 *   1. Build 2n events: a START event (+1) at each l_i, an END
 *      event (-1) at each r_i.
 *   2. Sort events by coordinate; TIES ARE BROKEN BY PUTTING START
 *      EVENTS BEFORE END EVENTS AT THE SAME COORDINATE. This is
 *      what makes endpoints inclusive: if interval A ends exactly
 *      where interval B starts, at that shared point BOTH must
 *      count, so A's "-1" must not fire before B's "+1" does.
 *   3. Sweep with a running count; every time we process a START
 *      event, compare the (now updated) count to the best-so-far
 *      and, if larger, record it together with the current
 *      coordinate as the candidate point.
 * Sorting dominates -> O(n log n).
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct { int l, r; } Interval;

typedef struct {
    int coord;
    int delta;      /* +1 = start, -1 = end */
} Event;

int cmpEvent(const void* pa, const void* pb) {
    const Event* a = pa;
    const Event* b = pb;
    if (a->coord != b->coord) return a->coord - b->coord;
    return b->delta - a->delta;   /* +1 (start) sorts before -1 (end) at same coord */
}

/* Fills *bestPoint and *bestCount. */
void maxPointCoverage(Interval S[], int n, int* bestPoint, int* bestCount) {
    Event* events = malloc(2 * n * sizeof(Event));
    for (int i = 0; i < n; i++) {
        events[2 * i]     = (Event){S[i].l, +1};
        events[2 * i + 1] = (Event){S[i].r, -1};
    }
    qsort(events, 2 * n, sizeof(Event), cmpEvent);

    int count = 0, best = 0, bestP = events[0].coord;
    for (int i = 0; i < 2 * n; i++) {
        count += events[i].delta;
        if (events[i].delta == +1 && count > best) {
            best = count;
            bestP = events[i].coord;
        }
    }
    *bestPoint = bestP;
    *bestCount = best;
    free(events);
}

void manualRun(void) {
    int n;
    printf("Enter number of intervals: ");
    scanf("%d", &n);
    Interval* S = malloc(n * sizeof(Interval));
    printf("Enter %d intervals as 'l r' pairs:\n", n);
    for (int i = 0; i < n; i++) scanf("%d %d", &S[i].l, &S[i].r);

    int bestPoint, bestCount;
    maxPointCoverage(S, n, &bestPoint, &bestCount);
    printf("\nPoint p = %d lies in %d intervals (the maximum).\n", bestPoint, bestCount);

    free(S);
}

/* naive O(n^2): for every distinct endpoint, count how many
 * intervals contain it (endpoints inclusive), take the max. */
int naiveMaxCoverage(Interval S[], int n) {
    int best = 0;
    for (int i = 0; i < n; i++) {
        int candidates[2] = {S[i].l, S[i].r};
        for (int c = 0; c < 2; c++) {
            int p = candidates[c];
            int cnt = 0;
            for (int j = 0; j < n; j++)
                if (S[j].l <= p && p <= S[j].r) cnt++;
            if (cnt > best) best = cnt;
        }
    }
    return best;
}

void autoValidate(void) {
    int sizes[] = {10, 100, 1000, 10000, 100000, 1000000};
    int ns = 6;
    srand((unsigned)time(NULL));

    printf("\n%-10s%-14s%-14s%-10s\n", "n", "MaxCoverage", "Time(s)", "CheckVsNaive");
    for (int t = 0; t < ns; t++) {
        int n = sizes[t];
        Interval* S = malloc(n * sizeof(Interval));
        for (int i = 0; i < n; i++) {
            int a = rand() % (n * 4);
            int len = 1 + rand() % 30;
            S[i].l = a;
            S[i].r = a + len;
        }

        clock_t c0 = clock();
        int bestPoint, bestCount;
        maxPointCoverage(S, n, &bestPoint, &bestCount);
        clock_t c1 = clock();

        int checkOk = 1;
        if (n <= 2000) checkOk = (naiveMaxCoverage(S, n) == bestCount);

        printf("%-10d%-14d%-14.5f%-10s\n", n, bestCount,
               (double)(c1 - c0) / CLOCKS_PER_SEC,
               (n <= 2000) ? (checkOk ? "OK" : "FAIL") : "-");

        free(S);
    }

    /* worked example from the lab sheet */
    Interval demo[] = {{10, 40}, {20, 60}, {50, 90}, {15, 70}};
    int bestPoint, bestCount;
    maxPointCoverage(demo, 4, &bestPoint, &bestCount);
    printf("\nDemo: S = {(10,40),(20,60),(50,90),(15,70)} -> p = %d is in %d intervals\n",
           bestPoint, bestCount);
    printf("(lab sheet's example answer was p=50 with count 3 -- any point\n");
    printf(" achieving the max count of 3 is an equally valid answer)\n");
}

int main(void) {
    printf("=========== Point of Maximum Interval Coverage in O(n log n) ===========\n\n");
    int choice;
    printf("1. Automatic validation (random intervals, timing, naive cross-check)\n");
    printf("2. Manual input\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 2) manualRun();
    else autoValidate();

    return 0;
}
