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

static void minheap_insert(vertex* V, int* heap, int& last, const int x);
static int minheap_delete(vertex* V, int* heap, int& last);
static void minheap_adjust(vertex* V, int* heap, const int& last, int idx);

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
    int treeCost = 0;
    // *** 이 함수를 작성하자 ***
    // 반드시 min-heap을 사용하여 O((n+m)logn) 알고리즘을 구현해야 한다(아니면 trivial한 프로그램임)
    // heap 연산 등 필요한 함수는 자유롭게 작성하여 추가한다.
    // 그러나 global 변수, dynamic array 등은 추가로 사용하지 않는다(실제로 필요 없다)
    int e, heap_last = 0;

    V[src].distance = 0;
    V[src].inS = true;

    // Iterate through front edges
    e = V[src].f_hd;
    while (e != NONE) {
        V[E[e].vr].distance = E[e].cost;
        e = E[e].fp;
    }

    // Iterate through rear edges
    e = V[src].r_hd;
    while (e != NONE) {
        V[E[e].vf].distance = E[e].cost;
        e = E[e].rp;
    }

    for (int i = 0; i < Vnum; i++) {
        if (!V[i].inS) {
            minheap_insert(V, minHeap, heap_last, i);
        }
    }

    for (int i = 0; i < Vnum - 2; i++) {
        const int idx = minheap_delete(V, minHeap, heap_last);
        V[idx].inS = true;

        // Iterate through front edges
        e = V[idx].f_hd;
        while (e != NONE) {
            if (!V[E[e].vr].inS && V[idx].distance + E[e].cost < V[E[e].vr].distance) {
                V[E[e].vr].distance = V[idx].distance + E[e].cost;
                minheap_adjust(V, minHeap, heap_last, V[E[e].vr].heap_idx);
            }
            e = E[e].fp;
        }

        // Iterate through rear edges
        e = V[idx].r_hd;
        while (e != NONE) {
            if (!V[E[e].vf].inS && V[idx].distance + E[e].cost < V[E[e].vf].distance) {
                V[E[e].vf].distance = V[idx].distance + E[e].cost;
                minheap_adjust(V, minHeap, heap_last, V[E[e].vf].heap_idx);
            }
            e = E[e].rp;
        }
    }
    V[minheap_delete(V, minHeap, heap_last)].inS = true; // Mark last vertex

    for (int i = 0; i < Vnum; i++) {
        if (i == src) {
            continue;
        }
        int max_cost = INT_MIN, idx;
        // Iterate through front edges
        e = V[i].f_hd;
        while (e != NONE) {
            if (V[i].distance == V[E[e].vr].distance + E[e].cost) {
                if (V[E[e].vr].distance > max_cost) {
                    max_cost = V[E[e].vr].distance;
                    idx = e;
                }
            }
            e = E[e].fp;
        }

        // Iterate through rear edges
        e = V[i].r_hd;
        while (e != NONE) {
            if (V[i].distance == V[E[e].vf].distance + E[e].cost) {
                if (V[E[e].vf].distance > max_cost) {
                    max_cost = V[E[e].vf].distance;
                    idx = e;
                }
            }
            e = E[e].rp;
        }

        E[idx].flag = true;
        treeCost += E[idx].cost;
    }

    return treeCost;
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

    // Store the given graph as adjacent list in array
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

static void minheap_insert(vertex* V, int* heap, int& last, const int x) {
    int idx = ++last;
    heap[idx] = x;
    while (idx > 1 && V[heap[idx]].distance < V[heap[idx / 2]].distance) {
        V[heap[idx / 2]].heap_idx = idx / 2;
        std::swap(heap[idx], heap[idx / 2]);
        idx /= 2;
    }
    V[heap[idx]].heap_idx = idx;
}

static int minheap_delete(vertex* V, int* heap, int& last) {
    const int ret = heap[1];
    int idx = 1, next;

    V[ret].heap_idx = NONE;
    heap[idx] = heap[last];
    heap[last--] = NONE;

    if (last == 0) {
        return ret;
    }

    while (true) {
        next = idx;
        if (idx * 2 <= last && V[heap[idx * 2]].distance < V[heap[idx]].distance) {
            next = idx * 2;
        }
        if (idx * 2 + 1 <= last && V[heap[idx * 2 + 1]].distance < V[heap[next]].distance) {
            next = idx * 2 + 1;
        }
        if (next == idx) {
            break;
        }
        V[heap[next]].heap_idx = next;
        std::swap(heap[idx], heap[next]);
        idx = next;
    }
    V[heap[idx]].heap_idx = idx;

    return ret;
}

static void minheap_adjust(vertex* V, int* heap, const int& last, int idx) {
    while (idx > 1 && V[heap[idx]].distance < V[heap[idx / 2]].distance) {
        V[heap[idx / 2]].heap_idx = idx;
        std::swap(heap[idx / 2], heap[idx]);
        idx /= 2;
    }
    V[heap[idx]].heap_idx = idx;
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