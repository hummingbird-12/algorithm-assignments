#include <algorithm>
#include <cstring>

void Edit_Distance(
    // inputs
    char* SS,		// Source string array
    char* TS,		// Edited string array
    int ins_cost, int del_cost, int sub_cost,
    // insertion, deletion, substitution cost
    // outputs
    int** Table,    // Cost Table Calculated (2-d array (|SS|+1) x (|TS|+1) will be set)
    char** SR,		// Source string with insertion to T marks '*'
    char** OP,		// Operation sequence('i', 'd', 's', or '.' (nop))
    char** TR,      // Edited string with deletion from S marks '*'
                    // NOTE: ST, OP, TR must be allocated inside the function as needed(minimum as possible)
    int* Mem_Allocated
) {
    const int SN = strlen(SS); // Length of `SS`
    const int TN = strlen(TS); // Length of `TN`
    int i, j;

    // Initialize 0-th row and column
    for (i = 0; i <= SN; i++) {
        Table[i][0] = i * del_cost;
    }
    for (j = 0; j <= TN; j++) {
        Table[0][j] = j * ins_cost;
    }

    // Fill in the edit distance in bottom-up fashion
    for (i = 1; i <= SN; i++) {
        for (j = 1; j <= TN; j++) {
            const int ins = Table[i][j - 1] + ins_cost;
            const int del = Table[i - 1][j] + del_cost;
            const int nop_sub = Table[i - 1][j - 1] + (SS[i - 1] == TS[j - 1] ? 0 : sub_cost);
            Table[i][j] = std::min(ins, std::min(del, nop_sub));
        }
    }

    // Do an initial back-tracing and count the number of required operations
    int op_cnt = 0;
    i = SN;
    j = TN;
    while (i != 0 || j != 0) {
        const int& ed = Table[i][j]; // Current edit distance
        op_cnt++;

        // No operation
        if (SS[i - 1] == TS[j - 1]) {
            i--;
            j--;
        }
        // Insertion
        else if (ed == Table[i][j - 1] + ins_cost) {
            j--;
        }
        // Deletion
        else if (ed == Table[i - 1][j] + del_cost) {
            i--;
        }
        // Substitution
        else {
            i--;
            j--;
        }
    }
    op_cnt += i + j; // Add up the number of remainig ins or del operations

    // Allocate memory to store the back-tracing results
    const int RLEN = op_cnt + 1;
    *OP = new char[RLEN];
    *SR = new char[RLEN];
    *TR = new char[RLEN];
    *Mem_Allocated = RLEN * 3 * sizeof(char);

    // Null-terminate the result strings
    (*SR)[RLEN - 1] = (*OP)[RLEN - 1] = (*TR)[RLEN - 1] = '\0';

    // Do a second back-tracing and record the editing process
    i = SN;
    j = TN;
    int index = RLEN - 2; // Index for the result strings
    while (i != 0 || j != 0) {
        const int& ed = Table[i][j];    // Current edit distance
        // Reference to current elements of the result strings
        char& op = (*OP)[index];
        char& sr = (*SR)[index];
        char& tr = (*TR)[index];
        index--;

        // No operation
        if (SS[i - 1] == TS[j - 1]) {
            op = '.';
            sr = SS[--i];
            tr = TS[--j];
        }
        // Insertion
        else if (ed == Table[i][j - 1] + ins_cost) {
            op = 'i';
            sr = '*';
            tr = TS[--j];
        }
        // Deletion
        else if (ed == Table[i - 1][j] + del_cost) {
            op = 'd';
            sr = SS[--i];
            tr = '*';
        }
        // Substitution
        else {
            op = 's';
            sr = SS[--i];
            tr = TS[--j];
        }
    }

    // Back-trace through the remaining del operations
    while (i > 0) {
        (*OP)[index] = 'd';
        (*SR)[index] = SS[--i];
        (*TR)[index] = '*';
        index--;
    }

    // Back-trace through the remaining ins operations
    while (j > 0) {
        (*OP)[index] = 'i';
        (*SR)[index] = '*';
        (*TR)[index] = TS[--j];
        index--;
    }
}