/*
 * This solution is based on the idea provided in the following site:
 * https://koosaga.com/220
 */

#include <cmath>

#include <algorithm>
#include <ios>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);

    int n;
    cin >> n;

    // Input the radii and sort in non-decreasing order
    vector<double> radius(n);
    for (int i = 0; i < n; i++) {
        cin >> radius[i];
    }
    sort(radius.begin(), radius.end());

    vector<int> big_centered(n);    // Index ordering with the largest circle in the center
    vector<int> small_centered(n);  // Index ordering with the smallest circle in the center
    int inc_big, dec_big;           // Used to iterate through indexes while filling in `big_centered`
    int inc_small, dec_small;       // Used to iterate through indexes while filling in `small_centered`

    // Fill in the centers
    big_centered[n / 2] = n - 1;    // The largest
    small_centered[n / 2] = 0;      // The smallest

    // Fill in the index order from the center to front
    inc_big = 0; dec_big = n - 2;
    inc_small = 1; dec_small = n - 1;
    for (int i = n / 2 - 1, j = 0; i >= 0; i--, j++) {
        if (j % 2 == 0) {
            big_centered[i] = inc_big;
            inc_big += 2;
            small_centered[i] = dec_small;
            dec_small -= 2;
        }
        else {
            big_centered[i] = dec_big;
            dec_big -= 2;
            small_centered[i] = inc_small;
            inc_small += 2;
        }
    }

    // Fill in the index order from the center to back
    inc_big = 1; dec_big = n - 3;
    inc_small = 2; dec_small = n - 2;
    for (int i = n / 2 + 1, j = 0; i < n; i++, j++) {
        if (j % 2 == 0) {
            big_centered[i] = inc_big;
            inc_big += 2;
            small_centered[i] = dec_small;
            dec_small -= 2;
        }
        else {
            big_centered[i] = dec_big;
            dec_big -= 2;
            small_centered[i] = inc_small;
            inc_small += 2;
        }
    }

    // Calculate horizontal distance using Pythagoras's theorem
    double ans_big = radius[big_centered[0]];       // Horizontal distance with `big_centered` ordering
    double ans_small = radius[small_centered[0]];   // Horizontal distance with `small_centered` ordering
    for (int i = 1; i < n; i++) {
        ans_big += 2 * sqrt(radius[big_centered[i - 1]] * radius[big_centered[i]]);
        ans_small += 2 * sqrt(radius[small_centered[i - 1]] * radius[small_centered[i]]);
    }
    ans_big += radius[big_centered[n - 1]];
    ans_small += radius[small_centered[n - 1]];

    cout << fixed << setprecision(6) << min(ans_big, ans_small) << '\n'; // Print the smaller distance

    return 0;
}
