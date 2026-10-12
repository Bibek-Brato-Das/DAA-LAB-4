#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int cmpInt(const void* a, const void* b) { return (*(int*)a - *(int*)b); }

/* Standard binary search for 'target' in sorted arr[0..n-1].
 * Returns index if found, -1 otherwise. */
int binarySearch(int arr[], int n, int target) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] == target) return mid;
        if (arr[mid] < target) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

/* Returns 1 and fills *outA,*outB if a pair summing to x is found;
 * returns 0 otherwise. S2 is sorted in place. */
int findPairSum(int S1[], int n1, int S2[], int n2, int x, int* outA, int* outB) {
    qsort(S2, n2, sizeof(int), cmpInt);
    for (int i = 0; i < n1; i++) {
        int need = x - S1[i];
        int idx = binarySearch(S2, n2, need);
        if (idx != -1) {
            *outA = S1[i];
            *outB = S2[idx];
            return 1;
        }
    }
    return 0;
}

void manualRun(void) {
    int n, x;
    printf("Enter size n of each set: ");
    scanf("%d", &n);
    int* S1 = malloc(n * sizeof(int));
    int* S2 = malloc(n * sizeof(int));
    printf("Enter %d elements of S1: ", n);
    for (int i = 0; i < n; i++) scanf("%d", &S1[i]);
    printf("Enter %d elements of S2: ", n);
    for (int i = 0; i < n; i++) scanf("%d", &S2[i]);
    printf("Enter target x: ");
    scanf("%d", &x);

    int a, b;
    if (findPairSum(S1, n, S2, n, x, &a, &b))
        printf("\nFound: %d (from S1) + %d (from S2) = %d\n", a, b, x);
    else
        printf("\nNo pair (a in S1, b in S2) sums to %d.\n", x);

    free(S1); free(S2);
}

void autoValidate(void) {
    int sizes[] = {10, 100, 1000, 10000, 100000, 1000000};
    int ns = 6;
    srand((unsigned)time(NULL));

    printf("\n%-10s%-10s%-14s%-10s\n", "n", "Found?", "Time(s)", "Check");
    for (int t = 0; t < ns; t++) {
        int n = sizes[t];
        int* S1 = malloc(n * sizeof(int));
        int* S2 = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) { S1[i] = rand() % (n * 10); S2[i] = rand() % (n * 10); }

        /* Plant a guaranteed pair so "Found" trials are meaningful */
        int x = S1[n / 2] + S2[n / 3];

        clock_t t0 = clock();
        int a, b;
        int found = findPairSum(S1, n, S2, n, x, &a, &b);
        clock_t t1 = clock();

        int check = found && (a + b == x);
        printf("%-10d%-10s%-14.5f%-10s\n", n, found ? "YES" : "NO",
               (double)(t1 - t0) / CLOCKS_PER_SEC, check ? "OK" : "FAIL");

        free(S1); free(S2);
    }

    /* worked example */
    int S1d[] = {1, 4, 7, 10};
    int S2d[] = {2, 3, 9, 20};
    int a, b;
    int found = findPairSum(S1d, 4, S2d, 4, 13, &a, &b);
    printf("\nDemo: S1={1,4,7,10}, S2={2,3,9,20}, x=13 -> %s",
           found ? "found " : "not found");
    if (found) printf("(%d + %d = 13)", a, b);
    printf("\n");
}

int main(void) {
    printf("=========== Pair Sum Across Two Sets in O(n log n) ===========\n\n");
    int choice;
    printf("1. Automatic validation (planted pairs, timing growth)\n");
    printf("2. Manual input\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 2) manualRun();
    else autoValidate();

    return 0;
}
