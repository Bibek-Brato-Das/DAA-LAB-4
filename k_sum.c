#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int cmpInt(const void* a, const void* b) { return (*(int*)a - *(int*)b); }

int binarySearchRange(int arr[], int lo, int hi, int target) {
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] == target) return mid;
        if (arr[mid] < target) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

/* Two-sum-via-binary-search over S[lo..hi] for 'need'.
 * On success returns 1 and fills idx[0],idx[1] (increasing indices). */
int twoSumInRange(int S[], int lo, int hi, int need, int idx[2]) {
    for (int i = lo; i <= hi; i++) {
        int complement = need - S[i];
        int j = binarySearchRange(S, i + 1, hi, complement);
        if (j != -1) { idx[0] = i; idx[1] = j; return 1; }
    }
    return 0;
}

int n_global;
int* S_global;
int result[64];        /* holds indices of the found combination */

/* chosenCount = how many of the k-2 "outer" indices picked so far
 * startIdx    = smallest index allowed next (keeps indices increasing)
 * sumSoFar    = sum of the chosen elements so far
 * remaining   = k - chosenCount (how many more numbers we still need) */
int findKSum(int startIdx, int chosenCount, long sumSoFar, int remaining, int T) {
    if (remaining == 2) {
        int idx[2];
        if (twoSumInRange(S_global, startIdx, n_global - 1, T - sumSoFar, idx)) {
            result[chosenCount] = idx[0];
            result[chosenCount + 1] = idx[1];
            return 1;
        }
        return 0;
    }
    if (remaining == 1) {
        int j = binarySearchRange(S_global, startIdx, n_global - 1, T - sumSoFar);
        if (j != -1) { result[chosenCount] = j; return 1; }
        return 0;
    }
    if (remaining == 0) return (sumSoFar == T);

    for (int i = startIdx; i <= n_global - remaining; i++) {
        result[chosenCount] = i;
        if (findKSum(i + 1, chosenCount + 1, sumSoFar + S_global[i], remaining - 1, T))
            return 1;
    }
    return 0;
}

/* Sorts S (n elements) in place and tests whether some k of them sum
 * to T. If found, fills outIdx[0..k-1] with the (sorted-array)
 * indices of the k elements used. Returns 1/0. */
int kSumExists(int S[], int n, int k, int T, int outIdx[]) {
    qsort(S, n, sizeof(int), cmpInt);
    n_global = n;
    S_global = S;
    if (k < 1 || k > n) return 0;
    int found = findKSum(0, 0, 0, k, T);
    if (found) for (int i = 0; i < k; i++) outIdx[i] = result[i];
    return found;
}

void manualRun(void) {
    int n, k, T;
    printf("Enter n (size of S): ");
    scanf("%d", &n);
    int* S = malloc(n * sizeof(int));
    printf("Enter %d integers: ", n);
    for (int i = 0; i < n; i++) scanf("%d", &S[i]);
    printf("Enter k: ");
    scanf("%d", &k);
    printf("Enter target T: ");
    scanf("%d", &T);

    int idx[64];
    if (kSumExists(S, n, k, T, idx)) {
        printf("\nFound %d numbers that sum to %d: ", k, T);
        long sum = 0;
        for (int i = 0; i < k; i++) { printf("%d ", S[idx[i]]); sum += S[idx[i]]; }
        printf(" (sum = %ld)\n", sum);
    } else {
        printf("\nNo %d numbers in S sum to %d.\n", k, T);
    }

    free(S);
}

void autoValidate(void) {
    srand((unsigned)time(NULL));
    int trials[][2] = { {50, 3}, {50, 4}, {200, 3}, {500, 3} }; /* {n, k} */
    int nt = 4;

    printf("\n%-8s%-6s%-10s%-14s%-10s\n", "n", "k", "Found?", "Time(s)", "SumOK?");
    for (int t = 0; t < nt; t++) {
        int n = trials[t][0], k = trials[t][1];
        int* S = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) S[i] = 1 + rand() % (5 * n);

        /* plant a guaranteed k-combination that sums to T */
        int T = 0;
        int used[8];
        for (int i = 0; i < k; i++) { used[i] = rand() % n; T += S[used[i]]; }

        clock_t c0 = clock();
        int idx[64];
        int found = kSumExists(S, n, k, T, idx);
        clock_t c1 = clock();

        long sum = 0;
        if (found) for (int i = 0; i < k; i++) sum += S[idx[i]];

        printf("%-8d%-6d%-10s%-14.5f%-10s\n", n, k, found ? "YES" : "NO",
               (double)(c1 - c0) / CLOCKS_PER_SEC,
               (found && sum == T) ? "OK" : (found ? "FAIL" : "-"));

        free(S);
    }

    /* worked example: classic 3-sum */
    int demo[] = {2, 7, 4, 0, 9, 5, 1, 3};
    int n = 8, k = 3, T = 15;
    int idx[64];
    printf("\nDemo: S = {2,7,4,0,9,5,1,3}, k=3, T=15 -> ");
    if (kSumExists(demo, n, k, T, idx)) {
        printf("found: ");
        for (int i = 0; i < k; i++) printf("%d ", demo[idx[i]]);
        printf("\n");
    } else {
        printf("not found\n");
    }
}

int main(void) {
    printf("=========== k-Sum Test in O(n^(k-1) log n) ===========\n\n");
    int choice;
    printf("1. Automatic validation (planted k-combinations, timing)\n");
    printf("2. Manual input\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 2) manualRun();
    else autoValidate();

    return 0;
}
