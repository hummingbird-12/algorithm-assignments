#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

bool skip;  // Flag for quick return from backtracking's pruning

void backtracking(
    vector<int>& count,     // Count of sticks per length
    const int largest,      // The largest length
    const int n,            // Number of sticks
    const int target,       // The possible answer to be tested
    int sum = 0,            // Sum of lengths for current partition
    int cnt = 0,            // Count of sticks used
    int last = 50           // The length of last stick used
) {
    if (sum == target && cnt == n) {
        // Answer found, print and exit right away
        cout << target << '\n';
        exit(0);
    }
    if (sum == target) {
        // A partition is complete, start a new partition
        sum = 0;
        last = largest;
    }

    // [Pruning 4] To avoid duplicate chekcing, fill the rest of partition with only equal length or smaller sticks
    // [Pruning 5] The possible sticks to choose are of length <= `target - sum`
    for (int len = min(last, target - sum); len >= 1; len--) {
        if (count[len] > 0) {
            count[len]--;
            backtracking(count, largest, n, target, sum + len, cnt + 1, len);
            count[len]++;
            if (skip) {
                // Non-promising case
                return;
            }
            // [Pruning 6] Failed to make a new partition using `len`, no need to check further
            if (sum == 0) {
                // [Pruning 7] Failed to make the first partition, `target` can not be the answer
                if (cnt == 0) {
                    skip = true;
                }
                return;
            }
        }
    }
}

int main() {
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);

    int n;
    cin >> n;

    int len;
    int largest = 0;        // The largest stick's length
    int sum = 0;            // Sum of all sticks' lengths
    vector<int> count(51);  // Count of sticks per length
    for (int i = 0; i < n; i++) {
        cin >> len;
        count[len]++;
        largest = max(largest, len);
        sum += len;
    }

    // [Pruning 1] The smallest possible answer is the length of the largest stick
    // [Pruning 2] If no answer is present until `sum / 2`, the answer is `sum` (all sticks together)
    for (int test = largest; test <= sum / 2; test++) {
        // [Pruning 3] Only divisors of `sum` are the possible answers
        if (sum % test == 0) {
            skip = false;
            backtracking(count, largest, n, test);
        }
    }
    cout << sum << '\n';

    return 0;
}
