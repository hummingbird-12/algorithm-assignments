#include <algorithm>
#include <cfloat>
#include <cmath>
#include <utility>

/*
 * Sorts point indices in `Xid[]` based on the coordinate
 * in `X[]` in non-decreasing order using exchange sort,
 * a O(N^2) algorithm.
 */
static void exchange_sort(
    const double* X,
    unsigned* Xid,
    const int left,
    const int right
);

/*
 * Sorts point indices in `Xid[]` based on the coordinate
 * in `X[]` in non-decreasing order using merge sort for
 * size bigger than threshold and exchange sort for size
 * smaller than threshold.
 */
static void sort(
    const double* X,
    unsigned* Xid,
    unsigned* TMP,
    const int left,
    const int right
);

/*
 * Takes care of the merge step for the merge sort.
 * Uses `TMP[]` as the temporal index array.
 */
static void merge(
    const double* X,
    unsigned* Xid,
    unsigned* TMP,
    const int left,
    const int mid,
    const int right
);

/*
 * Calculates the square of the Euclidean distance
 * between the two points `p1` and `p2`.
 */
static inline double sq_euclidean_distance(
    const double* X, const double* Y,
    const int p1, const int p2
);

void   sortXid(double* X, unsigned* Xid, unsigned* TMP, unsigned N) {
    sort(X, Xid, TMP, 0, N - 1);
}

double closestPairDC(
    unsigned L, unsigned R,	        // current leftmost and rightmost indices
    unsigned* pt1, unsigned* pt2,   // closest pair points indices
    double* X, double* Y,           // (x,y) positions array
    unsigned* Xid, unsigned* Yid,   // point index array
    unsigned* TMP,                  // temporal index array
    unsigned THR	                // threshold value
) {
    if (R - L + 1 <= THR) {
        // Size less or equal to threshold `THR`

        // Copy indices to `Yid[]` and sort
        for (unsigned i = L; i <= R; i++) {
            Yid[i] = Xid[i];
        }
        exchange_sort(Y, Yid, L, R);

        if (THR <= 1) {
            return -1;
        }

        // O(N^2) algorithm for finding the closest pair points
        double d;
        double dmin = sq_euclidean_distance(X, Y, Xid[R - 1], Xid[R]);
        *pt1 = Xid[R - 1];
        *pt2 = Xid[R];
        for (unsigned i = L; i < R - 1; i++) {
            for (unsigned j = i + 1; j <= R; j++) {
                d = sq_euclidean_distance(X, Y, Xid[i], Xid[j]);
                if (d < dmin) {
                    dmin = d;
                    *pt1 = Xid[i];
                    *pt2 = Xid[j];
                }
            }
        }

        return std::sqrt(dmin);
    }
    else {
        // Size bigger than threshold `THR`
        unsigned ptl1, ptl2, ptr1, ptr2;
        double dmin, dm;

        // Divide
        const int M = (L + R) / 2;
        const double dl = closestPairDC(L, M, &ptl1, &ptl2, X, Y, Xid, Yid, TMP, THR);
        const double dr = closestPairDC(M + 1, R, &ptr1, &ptr2, X, Y, Xid, Yid, TMP, THR);

        // Merge the two sorted halves of `Yid[]`
        merge(Y, Yid, TMP, L, M, R);

        // Get the closest pair between the two halves
        if (dl <= dr) {
            *pt1 = ptl1;
            *pt2 = ptl2;
            dmin = dl;
        }
        else {
            *pt1 = ptr1;
            *pt2 = ptr2;
            dmin = dr;
        }

        // Gather points within the center region
        const double xmid = (X[Xid[M]] + X[Xid[M + 1]]) / 2;
        const double xmid_l = xmid - (dmin != -1 ? dmin : DBL_MAX);
        const double xmid_r = xmid + (dmin != -1 ? dmin : DBL_MAX);
        int k = 0;
        for (unsigned i = L; i <= R; i++) {
            if (xmid_l <= X[Yid[i]] && X[Yid[i]] <= xmid_r) {
                TMP[k++] = Yid[i];
            }
        }

        // Square the minimum distance for comparison
        dmin = (dmin != -1 ? dmin * dmin : DBL_MAX);

        // Find the closest pair within the center region
        for (int i = 0; i < k - 1; i++) {
            for (int j = i + 1; j < k; j++) {
                const unsigned& pi1 = TMP[i];
                const unsigned& pi2 = TMP[j];

                if (Y[pi2] - Y[pi1] >= dmin) {
                    break;
                }
                dm = sq_euclidean_distance(X, Y, pi1, pi2);
                if (dm < dmin) {
                    *pt1 = pi1;
                    *pt2 = pi2;
                    dmin = dm;
                }
            }
        }

        return std::sqrt(dmin);
    }
}

static void exchange_sort(
    const double* X,
    unsigned* Xid,
    const int left,
    const int right
) {
    int min_index;
    double min_value;

    for (int i = left; i < right; i++) {
        min_index = i;
        min_value = X[Xid[i]];

        // Find and swap with the minimum within (i+1)~(right)
        for (int j = i + 1; j <= right; j++) {
            if (X[Xid[j]] < min_value) {
                min_index = j;
                min_value = X[Xid[j]];
            }
        }
        std::swap(Xid[i], Xid[min_index]);
    }
}

static void sort(
    const double* X,
    unsigned* Xid,
    unsigned* TMP,
    const int left,
    const int right
) {
    const int THRESHOLD = (1 << 3);
    if (left < right) {
        if (right - left + 1 <= THRESHOLD) {
            // Use O(N^2) sorting algorithm for size less than threshold
            exchange_sort(X, Xid, left, right);
        }
        else {
            // Use mergesort
            const int mid = (left + right) / 2;
            sort(X, Xid, TMP, left, mid);
            sort(X, Xid, TMP, mid + 1, right);
            merge(X, Xid, TMP, left, mid, right);
        }
    }
}

static void merge(
    const double* X,
    unsigned* Xid,
    unsigned* TMP,
    const int left,
    const int mid,
    const int right)
{
    int i, j, k;
    i = k = left;
    j = mid + 1;

    // Merge the two halves
    while (i <= mid && j <= right) {
        if (X[Xid[i]] <= X[Xid[j]]) {
            TMP[k++] = Xid[i++];
        }
        else {
            TMP[k++] = Xid[j++];
        }
    }

    // Append remaining elements
    while (i <= mid) {
        TMP[k++] = Xid[i++];
    }
    while (j <= right) {
        TMP[k++] = Xid[j++];
    }

    // Copy-back to array
    for (i = left; i <= right; i++) {
        Xid[i] = TMP[i];
    }
}

static inline double sq_euclidean_distance(
    const double* X, const double* Y,
    const int p1, const int p2
) {
    // Returns (x1 - x2)^2 + (y1 - y2)^2
    const double x = X[p1] - X[p2];
    const double y = Y[p1] - Y[p2];
    return x * x + y * y;
}