#include <algorithm>
#include <climits>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <queue>

#define NONE -1

typedef struct elm_edge {
    int  vf, vr;  // e = (vf, vr) where vf, vr are vertex array indices
    int  cost;    // edge cost
    bool flag;    // true if the edge is in the SP spanning tree, false otherwise
    int  fp, rp;  // adj list ptr of forward and reverse (-1 if none)
} edge;

typedef struct elm_vertex {
    int  f_hd, r_hd;	// adj list header(-1 if none)
    int  distance;		// distance from src to this vertex
    int  heap_idx;		// minHeap index
    bool inS;			// true if SP length found(true when move from V-S to S)
} vertex;

class MinHeap {
    int* heap;      // Array for min-heap implementation
    int size;       // Size of min-heap
    vertex* varr;   // Array of vertices

public:
    /*
     * Initialize by providing an allocated memory
     * and the array of vertices.
     */
    MinHeap(int* arr, vertex* V) {
        heap = arr;
        size = 0;
        varr = V;
    }

    /*
     * Adds `x` into min-heap and adjusts accordingly.
     */
    void push(const int x) {
        heap[++size] = x;
        adjust_up(size);
    }

    /*
     * Returns the root of min-heap and adjusts accordingly.
     */
    int pop() {
        int ret = heap[1];
        heap[1] = heap[size];
        heap[size--] = NONE;
        adjust_down(1);
        return ret;
    }

    /*
     * Adjusts the min-heap by moving upwards
     * the element at index `i`.
     */
    void adjust_up(int i) {
        // Compare with parent
        while (i > 1 && varr[heap[i]].distance < varr[heap[i / 2]].distance) {
            std::swap(heap[i], heap[i / 2]);
            varr[heap[i]].heap_idx = i;
            i /= 2;
        }
        varr[heap[i]].heap_idx = i;
    }

    /*
     * Adjusts the min-heap by moving downwards
     * the element at index `i`.
     */
    void adjust_down(int i) {
        int next;
        while (true) {
            next = i;
            // Compare with left child
            if (i * 2 <= size && varr[heap[i]].distance > varr[heap[i * 2]].distance) {
                next = i * 2;
            }
            // Compare with right child
            if (i * 2 + 1 <= size && varr[heap[next]].distance > varr[heap[i * 2 + 1]].distance) {
                next = i * 2 + 1;
            }

            if (next != i) {
                // Move downwards
                std::swap(heap[i], heap[next]);
                varr[heap[i]].heap_idx = i;
                i = next;
            }
            else {
                // End movement
                break;
            }
        }
        varr[heap[i]].heap_idx = i;
    }
};

/*
 * Marks edges that are part of the SPT while
 * back-tracing from the result of Dijkstra's algorithm.
 * Returns the SPT's total cost.
 */
static int back_trace(const int src, const int Vnum, const vertex* V, edge* E);

int SPT_Dijkstra(
    int src,	// source vertex index
    // graph structure array
    // 1. the adjacency list structure is the same as PHW02
    // 2. additional fields are added for Dijkstra's algorithm(see .h file)
    int Vnum, vertex* V,	// Vertex array size and the array
    int Enum, edge* E,		// Edge array size and the array

    int* minHeap	// array for min heap (array size = Vnum+1)
        // heap index range is 1 ~ (Vnum - 1) note: src must not in the initial heap
        // just arry is passed. must construct min-heap in this function

    // OUTPUT
    // 1. V[].distance : shortest path length from src to this vertex
    // 2. shortest path spanning tree : set E[].flag = true if the edge is in the tree
    // 3. return the sum of edge costs in the shortest path spanning tree.
    //    should be small as possible (think a way to make it small)
) {
    // *** 이 함수를 작성하자 ***
    // 반드시 min-heap을 사용하여 O((n+m)logn) 알고리즘을 구현해야 한다(아니면 trivial한 프로그램임)
    // heap 연산 등 필요한 함수는 자유롭게 작성하여 추가한다.
    // 그러나 global 변수, dynamic array 등은 추가로 사용하지 않는다(실제로 필요 없다)
    int e;
    int treeCost = 0;

    // Initialize `src`
    V[src].distance = 0;
    V[src].inS = true;

    // Initialize distances to vertices adjacent to `src`
    e = V[src].f_hd;
    while (e != NONE) {
        // Iterate through front edges
        V[E[e].vr].distance = E[e].cost;
        e = E[e].fp;
    }
    e = V[src].r_hd;
    while (e != NONE) {
        // Iterate through rear edges
        V[E[e].vf].distance = E[e].cost;
        e = E[e].rp;
    }

    // Construct min-heap
    MinHeap MH = MinHeap(minHeap, V);
    for (int i = 0; i < Vnum; i++) {
        if (!V[i].inS) {
            MH.push(i);
        }
    }

    for (int i = 0; i < Vnum - 2; i++) {
        const int u = MH.pop(); // Get the closest vertex and remove it from min-heap
        V[u].inS = true; // Mark vertex as part of S

        // Iterate through all vertices adjacent to `u`
        e = V[u].f_hd;
        while (e != NONE) {
            // Iterate through front edges (u, w)
            const int& w = E[e].vr;
            if (!V[w].inS && V[u].distance + E[e].cost < V[w].distance) {
                // Update distance and adjust min-heap
                V[w].distance = V[u].distance + E[e].cost;
                MH.adjust_up(V[w].heap_idx);
            }
            e = E[e].fp;
        }
        e = V[u].r_hd;
        while (e != NONE) {
            // Iterate through rear edges (u, w)
            const int& w = E[e].vf;
            if (!V[w].inS && V[u].distance + E[e].cost < V[w].distance) {
                // Update distance and adjust min-heap
                V[w].distance = V[u].distance + E[e].cost;
                MH.adjust_up(V[w].heap_idx);
            }
            e = E[e].rp;
        }
    }
    V[MH.pop()].inS = true; // Mark last vertex as part of S

    return (treeCost = back_trace(src, Vnum, V, E));
}

/*
 * Marks edges that are part of the SPT while
 * back-tracing from the result of Dijkstra's algorithm.
 * Returns the SPT's total cost.
 */
static int back_trace(const int src, const int Vnum, const vertex* V, edge* E) {
    int e, cost = 0;
    int max_dist, max_idx;

    // Iterate through all vertices
    for (int u = 0; u < Vnum; u++) {
        if (u == src) {
            continue;
        }

        /*
         * For a better chance of lowering the SPT's total cost,
         * find the vertex with the highest distance from `src`
         * among the possible back-trace vertices from `u`.
         */
        max_dist = INT_MIN;
        e = V[u].f_hd;
        while (e != NONE) {
            // Iterate through front edges (u, w)
            const int& w = E[e].vr;
            if (V[u].distance == V[w].distance + E[e].cost) {
                if (V[w].distance > max_dist) {
                    max_dist = V[w].distance;
                    max_idx = e;
                }
            }
            e = E[e].fp;
        }
        e = V[u].r_hd;
        while (e != NONE) {
            // Iterate through rear edges (u, w)
            const int& w = E[e].vf;
            if (V[u].distance == V[w].distance + E[e].cost) {
                if (V[w].distance > max_dist) {
                    max_dist = V[w].distance;
                    max_idx = e;
                }
            }
            e = E[e].rp;
        }

        // Include the edge in SPT and accumulate the cost
        E[max_idx].flag = true;
        cost += E[max_idx].cost;
    }

    return cost;
}

void Read_Graph(int Vnum, vertex* V, int Enum, edge* E) {
    // Graph 자료구조를 만드는 함수
    // *** 이 함수를 추가하자 ***
    // PHW02의 Read_Graph_adj_array()를 이 과제의 자료구조에 맞춰 살짝 수정하여 사용한다
    // 즉, Read_Graph_adj_array()에서 불필요한 내용을 제거한 후 사용.

    int u, v, c;

    // Initialize vertex array V[]
    for (int i = 0; i < Vnum; i++) {
        V[i].f_hd = V[i].r_hd = NONE;
        V[i].distance = INT_MAX;
        V[i].heap_idx = NONE;
        V[i].inS = false;
    }

    // Store the given graph as adjacent list and initializes
    for (int i = 0; i < Enum; i++) {
        scanf_s("%d%d%d", &u, &v, &c);

        E[i].vf = u;
        E[i].vr = v;
        E[i].cost = c;
        E[i].flag = false;

        E[i].fp = V[u].f_hd;
        E[i].rp = V[v].r_hd;

        V[u].f_hd = i;
        V[v].r_hd = i;
    }
}

// the following functions are for testing if the submitted program is correct.
int  Tree_Check(int Vnum, vertex* V, int Enum, edge* E, int* visited);
bool SPT_test(int src, int Vnum, vertex* V, int Enum, edge* E, int* minHeap);
void Error_Exit(const char* s);

int main(void) {
    int		src;
    vertex* V;		int Vnum;
    edge* E;		int Enum;
    int* minHeap;
    int    Tree_cost;
    int    Tnum; 		// # of test cases
    clock_t start, finish;
    double cmpt;

    scanf_s("%d", &Tnum);		// read # of tests

    for (int t = 0; t < Tnum; t++) {
        scanf_s("%d %d %d", &Vnum, &Enum, &src);
        V = new vertex[Vnum];
        E = new edge[Enum];
        minHeap = new int[Vnum + 1];	// heap array allocation
        if (V == NULL || E == NULL || minHeap == NULL) {
            Error_Exit("Memory Allocation Error");
        }
        Read_Graph(Vnum, V, Enum, E);

        /**/start = clock();	// start timer

        Tree_cost = SPT_Dijkstra(src, Vnum, V, Enum, E, minHeap);	// code by students

        /**/finish = clock();	// stop timer
        cmpt = ((double)(finish - start)) / (double)CLK_TCK;

        // 아래 Tree_Check와 SPT_test 함수는 첨부한 SPT_test.obj 파일에 있다.
        // 이 테스트에서 오류로 인하여 프로그램이 정지하면 뭔가 잘못된 것이다(제출해도 0점)
        if (Tree_Check(Vnum, V, Enum, E, minHeap) == 0) {
            Error_Exit("   ERROR The result is not a spanning tree");
        }
        if (SPT_test(src, Vnum, V, Enum, E, minHeap) == false) {
            Error_Exit("** Something wrong in applying Dijkstra's");
        }
        if (t != 0)
            printf("\n");

        printf("**T%d (Dijkstra) (V = %d, E = %d, time = %.3f sec) Tree Cost = %d\n",
            t + 1, Vnum, Enum, cmpt, Tree_cost);

        delete[] minHeap; delete[] V; delete[] E;
    }
    return 0;
}

void Error_Exit(const char* s) {
    printf("%s\n", s);
    exit(-1);
}