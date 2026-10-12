/*
 * DAA Lab-04, Q4: Application of Sorting - IV
 * ---------------------------------------------------------------
 * n persons p_i, each with a distinct entry time a_i and distinct
 * exit time b_i (b_i > a_i, and all 2n timestamps are distinct --
 * no ties). Find the time at which the most people were
 * simultaneously present, in O(n log n).
 *
 * ALGORITHM (sweep line)
 * -------------------------
 *   1. Build 2n events: an ARRIVE event (+1) at each a_i and a
 *      LEAVE event (-1) at each b_i.
 *   2. Sort all events by time                       -- O(n log n)
 *   3. Sweep left to right keeping a running "currently present"
 *      count: += 1 on ARRIVE, -= 1 on LEAVE. After every ARRIVE,
 *      check if the running count is a new maximum; if so, record
 *      it together with the current time.
 * The peak can only occur right after some arrival (a running count
 * never increases except on an arrival), so checking only at
 * arrivals is sufficient. Sorting dominates -> O(n log n).
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int time;
    int delta;      /* +1 = arrival, -1 = departure */
} Event;

int cmpEvent(const void* pa, const void* pb) {
    const Event* a = pa;
    const Event* b = pb;
    return a->time - b->time;   /* times are all distinct, per problem statement */
}

/* Fills *bestTime and *bestCount with the time of maximum overlap
 * and how many people were present then. */
void maxSimultaneous(int a[], int b[], int n, int* bestTime, int* bestCount) {
    Event* events = malloc(2 * n * sizeof(Event));
    for (int i = 0; i < n; i++) {
        events[2 * i]     = (Event){a[i], +1};
        events[2 * i + 1] = (Event){b[i], -1};
    }
    qsort(events, 2 * n, sizeof(Event), cmpEvent);

    int count = 0, best = 0, bestT = events[0].time;
    for (int i = 0; i < 2 * n; i++) {
        count += events[i].delta;
        if (events[i].delta == +1 && count > best) {
            best = count;
            bestT = events[i].time;
        }
    }
    *bestTime = bestT;
    *bestCount = best;
    free(events);
}

void manualRun(void) {
    int n;
    printf("Enter number of persons: ");
    scanf("%d", &n);
    int* a = malloc(n * sizeof(int));
    int* b = malloc(n * sizeof(int));
    printf("Enter %d (entry exit) pairs, e.g. '9 17' (entry < exit):\n", n);
    for (int i = 0; i < n; i++) scanf("%d %d", &a[i], &b[i]);

    int bestTime, bestCount;
    maxSimultaneous(a, b, n, &bestTime, &bestCount);
    printf("\nMax simultaneous attendance: %d people, at time %d\n", bestCount, bestTime);

    free(a); free(b);
}

/* naive O(n^2) check for cross-validation: for every arrival time,
 * count how many intervals [a_i,b_i) contain it. */
int naiveMaxOverlap(int a[], int b[], int n) {
    int best = 0;
    for (int i = 0; i < n; i++) {
        int cnt = 0;
        for (int j = 0; j < n; j++)
            if (a[j] <= a[i] && a[i] < b[j]) cnt++;
        if (cnt > best) best = cnt;
    }
    return best;
}

void autoValidate(void) {
    int sizes[] = {10, 100, 1000, 10000, 100000, 1000000};
    int ns = 6;
    srand((unsigned)time(NULL));

    printf("\n%-10s%-14s%-14s%-10s\n", "n", "MaxCount", "Time(s)", "CheckVsNaive");
    for (int t = 0; t < ns; t++) {
        int n = sizes[t];
        int* a = malloc(n * sizeof(int));
        int* b = malloc(n * sizeof(int));

        /* generate 2n distinct timestamps, then randomly pair them as (a,b) with a<b */
        int total = 2 * n;
        int* times = malloc(total * sizeof(int));
        for (int i = 0; i < total; i++) times[i] = i * 3 + (rand() % 3); /* strictly increasing-ish, unique */
        /* shuffle */
        for (int i = total - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int tmp = times[i]; times[i] = times[j]; times[j] = tmp;
        }
        for (int i = 0; i < n; i++) {
            int x = times[2 * i], y = times[2 * i + 1];
            a[i] = x < y ? x : y;
            b[i] = x < y ? y : x;
        }

        clock_t c0 = clock();
        int bestTime, bestCount;
        maxSimultaneous(a, b, n, &bestTime, &bestCount);
        clock_t c1 = clock();

        int checkStr = 1;
        if (n <= 2000) { /* naive O(n^2) only for smaller n */
            int naive = naiveMaxOverlap(a, b, n);
            checkStr = (naive == bestCount);
        }

        printf("%-10d%-14d%-14.5f%-10s\n", n, bestCount,
               (double)(c1 - c0) / CLOCKS_PER_SEC,
               (n <= 2000) ? (checkStr ? "OK" : "FAIL") : "-");

        free(a); free(b); free(times);
    }

    /* worked example */
    int ad[] = {9, 10, 11, 13};
    int bd[] = {17, 12, 16, 15};
    int bestTime, bestCount;
    maxSimultaneous(ad, bd, 4, &bestTime, &bestCount);
    printf("\nDemo: entries=(9,10,11,13) exits=(17,12,16,15) -> max %d people at time %d\n",
           bestCount, bestTime);
}

int main(void) {
    printf("=========== Max Simultaneous Attendance (Sweep Line) ===========\n\n");
    int choice;
    printf("1. Automatic validation (random schedules, timing, naive cross-check)\n");
    printf("2. Manual input\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 2) manualRun();
    else autoValidate();

    return 0;
}
