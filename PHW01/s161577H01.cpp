#include "Graph_adj_list.h"
#include <queue>

extern void errorExit(const char* s);

SLList2 pool;
unsigned long UsedMemoryForArray = 0;

void Read_Graph_adj_list(int Vnum, vertex* V, int Enum, edge* E) {
    // ���Ͽ��� �׷��� ������ �о� V�� E �迭�� �����Ѵ�.
    // V[ ].name�� E[ ].name�� ������ �ʿ䰡 ������ ���ǻ� �� �迭������
    //   index�� �����Ѵ�.
    // V[ ].S�� adjacent list header�� ���� ����ε� ���ȴ�.
    // ���� edge (i,j)�� ������ V[i].S�� edge index k�� push�ϰ�
    //  ���������� V[j].S�� k�� push�Ѵ�.
    // V[].flag�� E[].flag�� �ݵ�� false�� �����ؾ� �Ѵ�.

    // ***�� �Լ��� �ۼ��Ѵ�

    int u, v, c;
    SLL* p;

    // Initialize vertex array V[]
    for (int i = 0; i < Vnum; i++) {
        V[i].name = i;
        V[i].flag = false;
    }

    // Store the given graph as adjacent list
    for (int i = 0; i < Enum; i++) {
        scanf_s("%d%d%d", &u, &v, &c);

        E[i].name = i;
        E[i].vf = u;
        E[i].vr = v;
        E[i].cost = c;
        E[i].flag = false;

        p = pool.allocSLL();
        p->i = i;
        V[u].S.push(p);

        p = pool.allocSLL();
        p->i = i;
        V[v].S.push(p);
    }
}

void Free_Graph_adj_list(int Vnum, vertex* V) {
    // V �迭�� adjacency list�� ���Ҹ� pop()�� ����Ͽ�
    // ���  pool�� �ǵ��� �ش�

    // ***�� �Լ��� �ۼ��Ѵ�

    for (int i = 0; i < Vnum; i++) {
        // Move all elements in V[i].S into the pool
        while (!V[i].S.empty()) {
            pool.freeSLL(V[i].S.pop());
        }
    }
}

int BFS_Tree_adj_list(
    int     src,   // source node index
    int     Vnum,  // number of vertices
    vertex* V,     // vertex structure array (starting index 0)
    int     Enum,  // number of edges
    edge* E     // edge structure array (starting index 0)
) {
    // BFS�� ����Ͽ� BFS tree�� ���ϴ� ������ flag�� true�� �����Ѵ�.
    // BFS ���� vertex�� �Է� ���Ͽ��� �����ȴ�(src).
    // V[]�� adjacent list Ž���� .top() ��� �Լ��� ����Ͽ�
    //  adj list ����� ��� �̸� ���󰡸� ���� vertex�� Ž���Ѵ�.
    // BFS tree�� ���� ������ cost ���� return �Ѵ�
    //   (not unique but ��� �� edge cost�� 1�̸� unique)
    // -- �Լ��� ��� parameter�� ���� �ʿ�� ����.
    // -- BFS�� ���� queue�� �ʿ��ϸ� STL�� queue�� ����ص� ����

    // ***�� �Լ��� �ۼ��Ѵ�

    std::queue<int> q;
    int totalCost = 0;

    // Initialize BFS state
    q.push(src);
    V[src].flag = true;

    while (!q.empty()) {
        const int& current = q.front(); // Current vertex in BFS iteration

        for (SLL* p = V[current].S.top(); p != NULL; p = p->p) {
            edge& e = E[p->i]; // An edge incident to `current`
            const int& next = (e.vf == current ? e.vr : e.vf); // Vertex adjacent to `current` by `e`

            // Check if `next` is not yet visited
            if (!V[next].flag) {
                V[next].flag = true;
                q.push(next);

                e.flag = true;
                totalCost += e.cost;
            }
        }

        q.pop();
    }

    return totalCost;
}

// SLL management functions
// �Ʒ� �Լ����� 02_04_CSE3081_CPrg&DataStructure_�߰��ڷ�.pdf��
//   �����Ͽ� �� �Լ����� ä�� ����.
// ���ø� �ʿ��ϹǷ� ť�� ��� �Լ��� �Ʒ� �Լ� ����Ʈ�� ����

// stack member fuctions
void sllStack2::push(SLL* p) {
    // insert p to the top of stack ST
    p->p = ST;
    ST = p;
}

SLL* sllStack2::pop(void) {
    // remove and return p at the top of ST

    // ***�� �Լ��� �ۼ��Ѵ�

    SLL* p = ST;
    ST = ST->p;
    return p;
}

SLL* sllStack2::top(void) {
    // return p at the top of ST
    return ST;
}

bool sllStack2::empty(void) {
    // return true if empty
    if (ST == NULL)
        return true;
    else
        return false;
}

// SLList2 member functions
SLL* SLList2::allocSLL(void) {		// allocate an SLL element
    SLL* p;

    // ***�� �Լ��� �ۼ��Ѵ�

    // Pool is empty
    if (SLL_pool == NULL) {
        p = (SLL*)malloc(sizeof(SLL));
        if (p == NULL) {
            errorExit("Memory allocation error (allocSLL)");
        }
        else {
            UsedMemoryForSLLs += sizeof(SLL);
            p->i = NONE;
        }
    }
    // Obtain element from pool
    else {
        p = SLL_pool;
        SLL_pool = SLL_pool->p;
    }

    p->p = NULL;
    SLL_cnt++;

    return(p);
}

void SLList2::freeSLL(SLL* p) {		// free *p (move it to SLL_pool
    if (p->i == NONE) {
        errorExit("This is already freed(freeSLL).");
    }

    // ***�� �Լ��� �ۼ��Ѵ�

    p->i = NONE;
    p->p = SLL_pool;
    SLL_pool = p;
    SLL_cnt--;
}

void SLList2::freeSLL_pool(void) {	// clear SLL_pool
    SLL* p = SLL_pool;

    // ***�� �Լ��� �ۼ��Ѵ�

    while (p != NULL) {
        SLL_pool = p->p;
        free(p);
        UsedMemoryForSLLs -= sizeof(SLL);
        p = SLL_pool;
    }

    if (SLL_cnt != 0) {
        errorExit("Non-zero SLL_cnt after cleanup.");
    }
}