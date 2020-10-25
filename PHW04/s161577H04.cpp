#include <algorithm>
#include <climits>
#include <cmath>
#include <utility>

static void exchange_sort(
    const double* Y,
    unsigned* Yid,
    const int low,
    const int high
);
static void mergesort(
    const double* X,
    unsigned* Xid,
    unsigned* TMP,
    const int low,
    const int high
);
static void merge(
    const double* X,
    unsigned* Xid,
    unsigned* TMP,
    const int low,
    const int mid,
    const int high
);
static inline double sq_euclidean_distance(
    const double* X, const double* Y,
    const int p1, const int p2
);

void   sortXid(double* X, unsigned* Xid, unsigned* TMP, unsigned N) {
    mergesort(X, Xid, TMP, 0, N - 1);
}

double closestPairDC(
    unsigned L, unsigned R,	  // current leftmost and rightmost indices
    unsigned* pt1, unsigned* pt2, // closest pair points indices
    double* X, double* Y,         // (x,y) positions array(input)
    unsigned* Xid,  // point index array(sorted by x coordinates)
    unsigned* Yid,  // point index array(gradually sorted by y coordinates)
    unsigned* TMP,  // temporal index array
    unsigned THR	// threshold value
) {
    if (R - L + 1 <= THR) {
        for (unsigned i = L; i <= R; i++) {
            Yid[i] = Xid[i];
        }
        exchange_sort(Y, Yid, L, R);

        if (THR <= 1) {
            return DBL_MAX;
        }

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
        unsigned ptl1, ptl2, ptr1, ptr2;
        double dmin, dm;

        const int M = (L + R) / 2;
        const double dl = closestPairDC(L, M, &ptl1, &ptl2, X, Y, Xid, Yid, TMP, THR);
        const double dr = closestPairDC(M + 1, R, &ptr1, &ptr2, X, Y, Xid, Yid, TMP, THR);
        merge(Y, Yid, TMP, L, M, R);

        *pt1 = ptl1;
        *pt2 = ptl2;
        dmin = dl;
        if (dl > dr) {
            *pt1 = ptr1;
            *pt2 = ptr2;
            dmin = dr;
        }
        dmin *= dmin; // TODO : CHECK FOR CASE WHEN THR <= 1 -> dl/dr = DBL_MAX

        for (unsigned i = L; i < R; i++) {
            for (unsigned j = i + 1; j <= R; j++) {
                if (Y[Yid[j]] - Y[Yid[i]] >= dmin) {
                    break;
                }
                dm = sq_euclidean_distance(X, Y, Yid[i], Yid[j]);
                if (dm < dmin) {
                    *pt1 = Yid[i];
                    *pt2 = Yid[j];
                    dmin = dm;
                }
            }
        }

        return std::sqrt(dmin);
    }
}

static void exchange_sort(
    const double* Y,
    unsigned* Yid,
    const int low,
    const int high
) {
    int min_index;
    double min_value;

    for (int i = low; i < high; i++) {
        min_index = i;
        min_value = Y[Yid[i]];
        for (int j = i + 1; j <= high; j++) {
            if (Y[Yid[j]] < min_value) {
                min_index = j;
                min_value = Y[Yid[j]];
            }
        }
        std::swap(Yid[i], Yid[min_index]);
    }
}

static void mergesort(const double* X, unsigned* Xid, unsigned* TMP, const int low, const int high) {
    if (low < high) {
        const int mid = (low + high) / 2;
        mergesort(X, Xid, TMP, low, mid);
        mergesort(X, Xid, TMP, mid + 1, high);
        merge(X, Xid, TMP, low, mid, high);
    }
}

static void merge(const double* X, unsigned* Xid, unsigned* TMP, const int low, const int mid, const int high) {
    int i, j, k;
    i = k = low;
    j = mid + 1;

    while (i <= mid && j <= high) {
        if (X[Xid[i]] <= X[Xid[j]]) {
            TMP[k++] = Xid[i++];
        }
        else {
            TMP[k++] = Xid[j++];
        }
    }

    while (i <= mid) {
        TMP[k++] = Xid[i++];
    }
    while (j <= high) {
        TMP[k++] = Xid[j++];
    }

    for (i = low; i <= high; i++) {
        Xid[i] = TMP[i];
    }
}

static inline double sq_euclidean_distance(const double* X, const double* Y, const int p1, const int p2) {
    const double x = X[p1] - X[p2];
    const double y = Y[p1] - Y[p2];
    return x * x + y * y;
}