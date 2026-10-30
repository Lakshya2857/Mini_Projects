#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define V 9          /* number of locations */
#define BUF 256

/* ---------- Location names ---------- */
const char *locations[V] = {
    "Main Gate", "Library", "Block 1", "Block 2", "Cafeteria",
    "Auditorium", "Parking", "Lab", "Sports Ground"
};

/* ---------- Adjacency list node ---------- */
typedef struct AdjNode {
    int vertex;
    struct AdjNode *next;
} AdjNode;

AdjNode *adjList[V];      /* adjacency list: array of linked lists */
int adjMatrix[V][V];      /* adjacency matrix */
int edgeCount = 0;

/* ======================================================
 *  GRAPH CREATION
 * ====================================================== */

/* append v at the end of u's list (keeps neighbour order = insertion order) */
void appendNeighbour(int u, int v) {
    AdjNode *n = (AdjNode *)malloc(sizeof(AdjNode));
    if (n == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    n->vertex = v;
    n->next = NULL;

    if (adjList[u] == NULL) {
        adjList[u] = n;
    } else {
        AdjNode *t = adjList[u];
        while (t->next != NULL) t = t->next;
        t->next = n;
    }
}

/* add undirected edge; returns 1 if added, 0 if invalid/duplicate */
int addEdge(int u, int v) {
    if (u < 0 || u >= V || v < 0 || v >= V || u == v) return 0;
    if (adjMatrix[u][v] == 1) return 0;          /* already exists */

    appendNeighbour(u, v);
    appendNeighbour(v, u);
    adjMatrix[u][v] = adjMatrix[v][u] = 1;
    edgeCount++;
    return 1;
}

void buildCampusGraph(void) {
    addEdge(0, 1);   /* Main Gate  - Library    */
    addEdge(0, 6);   /* Main Gate  - Parking    */
    addEdge(1, 2);   /* Library    - Block 1    */
    addEdge(1, 4);   /* Library    - Cafeteria  */
    addEdge(2, 3);   /* Block 1    - Block 2    */
    addEdge(2, 7);   /* Block 1    - Lab        */
    addEdge(3, 5);   /* Block 2    - Auditorium */
    addEdge(4, 5);   /* Cafeteria  - Auditorium */
    addEdge(6, 7);   /* Parking    - Lab        */
    /* Sports Ground (8) has no path yet -> disconnected location */
}

void freeGraph(void) {
    int i;
    for (i = 0; i < V; i++) {
        AdjNode *t = adjList[i];
        while (t != NULL) {
            AdjNode *next = t->next;
            free(t);
            t = next;
        }
        adjList[i] = NULL;
    }
}

/* ======================================================
 *  DISPLAY FUNCTIONS
 * ====================================================== */

void displayLocations(void) {
    int i;
    printf("\n===== CAMPUS LOCATIONS =====\n");
    for (i = 0; i < V; i++) printf("%d. %s\n", i, locations[i]);
}

void displayAdjList(void) {
    int i;
    printf("\n===== ADJACENCY LIST =====\n");
    for (i = 0; i < V; i++) {
        AdjNode *t = adjList[i];
        printf("%-14s : ", locations[i]);
        if (t == NULL) printf("(no connections)");
        while (t != NULL) {
            printf("%s", locations[t->vertex]);
            if (t->next != NULL) printf(", ");
            t = t->next;
        }
        printf("\n");
    }
}

void displayAdjMatrix(void) {
    int i, j;
    printf("\n===== ADJACENCY MATRIX =====\n      ");
    for (i = 0; i < V; i++) printf("%2d ", i);
    printf("\n");
    for (i = 0; i < V; i++) {
        printf("%2d -> ", i);
        for (j = 0; j < V; j++) printf("%2d ", adjMatrix[i][j]);
        printf("\n");
    }
}

/* ======================================================
 *  BFS  (shortest path, minimum edges)
 * ====================================================== */

/* returns 1 if route found (path[] filled source->dest, length in *len),
   0 otherwise. Uses queue + visited + parent array. */
int bfsRoute(int src, int dest, int path[], int *len) {
    int queue[V], visited[V] = {0}, parent[V];
    int front = 0, rear = 0, i, u;

    for (i = 0; i < V; i++) parent[i] = -1;

    queue[rear++] = src;
    visited[src] = 1;

    while (front < rear) {
        u = queue[front++];
        if (u == dest) break;

        AdjNode *t = adjList[u];
        while (t != NULL) {
            if (!visited[t->vertex]) {
                visited[t->vertex] = 1;
                parent[t->vertex] = u;       /* remember how we reached it */
                queue[rear++] = t->vertex;
            }
            t = t->next;
        }
    }

    if (!visited[dest]) return 0;

    /* reconstruct path backwards using parent[] */
    int temp[V], count = 0;
    u = dest;
    while (u != -1) {
        temp[count++] = u;
        u = parent[u];
    }
    *len = count;
    for (i = 0; i < count; i++) path[i] = temp[count - 1 - i];
    return 1;
}

/* path -> "A -> B -> C" */
void pathToString(int path[], int len, char *buf) {
    int i;
    buf[0] = '\0';
    for (i = 0; i < len; i++) {
        strcat(buf, locations[path[i]]);
        if (i < len - 1) strcat(buf, " -> ");
    }
}

void findRouteBFS(int src, int dest) {
    int path[V], len = 0;
    char buf[BUF];

    if (bfsRoute(src, dest, path, &len)) {
        pathToString(path, len, buf);
        printf("\nShortest Route (BFS):\n%s\n", buf);
        printf("Number of edges (stops - 1): %d\n", len - 1);
    } else {
        printf("\nNo route available between %s and %s.\n",
               locations[src], locations[dest]);
    }
}

/* ======================================================
 *  DFS  (traversal, reachability, components)
 * ====================================================== */

void dfsUtil(int u, int visited[], int printOrder) {
    visited[u] = 1;
    if (printOrder) printf("%s  ", locations[u]);

    AdjNode *t = adjList[u];
    while (t != NULL) {
        if (!visited[t->vertex]) dfsUtil(t->vertex, visited, printOrder);
        t = t->next;
    }
}

/* DFS from src: show traversal order, reachable and disconnected locations */
void dfsReachable(int src) {
    int visited[V] = {0}, i, found = 0;

    printf("\nDFS traversal order from %s:\n", locations[src]);
    dfsUtil(src, visited, 1);
    printf("\n");

    printf("\nReachable locations from %s:\n", locations[src]);
    for (i = 0; i < V; i++)
        if (visited[i] && i != src) printf("- %s\n", locations[i]);

    printf("\nDisconnected (unreachable) locations:\n");
    for (i = 0; i < V; i++) {
        if (!visited[i]) {
            printf("- %s\n", locations[i]);
            found = 1;
        }
    }
    if (!found) printf("None. All locations are reachable.\n");
}

/* DFS based reachability check */
int isReachable(int src, int dest) {
    int visited[V] = {0};
    dfsUtil(src, visited, 0);
    return visited[dest];
}

/* connected components */
void showComponents(void) {
    int visited[V] = {0}, i, j, comp = 0;

    printf("\n===== CONNECTED COMPONENTS =====\n");
    for (i = 0; i < V; i++) {
        if (!visited[i]) {
            int before[V];
            memcpy(before, visited, sizeof(visited));
            dfsUtil(i, visited, 0);

            comp++;
            printf("Component %d: ", comp);
            for (j = 0; j < V; j++)
                if (visited[j] && !before[j]) printf("%s  ", locations[j]);
            printf("\n");
        }
    }
    printf("Total components: %d (%s)\n", comp,
           comp == 1 ? "campus fully connected" : "campus has disconnected locations");
}

/* ======================================================
 *  TEST CASES  (expected vs actual)
 * ====================================================== */

int runOneTest(int no, const char *title, int src, int dest, const char *expected) {
    int path[V], len = 0;
    char actual[BUF];

    if (bfsRoute(src, dest, path, &len)) pathToString(path, len, actual);
    else strcpy(actual, "No route");

    int pass = (strcmp(actual, expected) == 0);

    printf("\nTest %d: %s\n", no, title);
    printf("  Input    : %s -> %s\n", locations[src], locations[dest]);
    printf("  Expected : %s\n", expected);
    printf("  Actual   : %s\n", actual);
    printf("  Result   : %s\n", pass ? "PASS" : "FAIL");
    return pass;
}

void runTests(void) {
    int passed = 0, total = 5;
    printf("\n===== TEST CASES =====");
    passed += runOneTest(1, "Direct route", 0, 1, "Main Gate -> Library");
    passed += runOneTest(2, "Multi-stop route", 0, 5,
                         "Main Gate -> Library -> Cafeteria -> Auditorium");
    passed += runOneTest(3, "Multi-stop route (via Parking side)", 6, 4,
                         "Parking -> Main Gate -> Library -> Cafeteria");
    passed += runOneTest(4, "Unreachable destination", 0, 8, "No route");
    passed += runOneTest(5, "Source and destination same", 2, 2, "Block 1");
    printf("\nSummary: %d / %d test cases passed.\n", passed, total);
}

/* ======================================================
 *  COMPLEXITY COMPARISON
 * ====================================================== */

void showComplexity(void) {
    printf("\n===== COMPLEXITY ANALYSIS =====\n");
    printf("Graph here: V = %d vertices, E = %d edges\n\n", V, edgeCount);
    printf("%-22s %-16s %-16s\n", "", "Adjacency List", "Adjacency Matrix");
    printf("%-22s %-16s %-16s\n", "BFS time", "O(V + E)", "O(V^2)");
    printf("%-22s %-16s %-16s\n", "DFS time", "O(V + E)", "O(V^2)");
    printf("%-22s %-16s %-16s\n", "Graph storage space", "O(V + E)", "O(V^2)");
    printf("%-22s %-16s %-16s\n", "BFS extra space", "O(V) queue", "O(V) queue");
    printf("%-22s %-16s %-16s\n", "DFS extra space", "O(V) stack", "O(V) stack");
    printf("%-22s %-16s %-16s\n", "Check edge (u,v)", "O(degree)", "O(1)");
    printf("%-22s %-16s %-16s\n", "Best for", "Sparse graphs", "Dense graphs");
    printf("\nHere: matrix uses %d cells, list uses %d nodes (2 x E).\n",
           V * V, 2 * edgeCount);
}

/* ======================================================
 *  MAIN
 * ====================================================== */

int readInt(const char *msg, int *x) {
    printf("%s", msg);
    return scanf("%d", x) == 1;
}

int main(void) {
    int choice, src, dest, u, v;

    buildCampusGraph();

    do {
        printf("\n=====================================\n");
        printf("        CAMPUS ROUTE FINDER\n");
        printf("=====================================\n");
        printf("1.  Display campus locations\n");
        printf("2.  Display adjacency list\n");
        printf("3.  Display adjacency matrix\n");
        printf("4.  Find shortest route (BFS)\n");
        printf("5.  Reachable locations + DFS traversal\n");
        printf("6.  Check if destination is reachable (DFS)\n");
        printf("7.  Show connected components\n");
        printf("8.  Add a new path\n");
        printf("9.  Run test cases\n");
        printf("10. Show complexity comparison\n");
        printf("0.  Exit\n");

        if (!readInt("Enter choice: ", &choice)) {
            printf("Invalid input. Please enter a number.\n");
            break;
        }

        switch (choice) {
            case 1:
                displayLocations();
                break;
            case 2:
                displayAdjList();
                break;
            case 3:
                displayAdjMatrix();
                break;
            case 4:
                displayLocations();
                if (!readInt("\nEnter source number: ", &src) ||
                    !readInt("Enter destination number: ", &dest)) {
                    printf("Invalid input.\n");
                    choice = 0;
                    break;
                }
                if (src < 0 || src >= V || dest < 0 || dest >= V)
                    printf("Invalid location number! Use 0 to %d.\n", V - 1);
                else
                    findRouteBFS(src, dest);
                break;
            case 5:
                displayLocations();
                if (!readInt("\nEnter starting location number: ", &src)) {
                    printf("Invalid input.\n");
                    choice = 0;
                    break;
                }
                if (src < 0 || src >= V)
                    printf("Invalid location number! Use 0 to %d.\n", V - 1);
                else
                    dfsReachable(src);
                break;
            case 6:
                displayLocations();
                if (!readInt("\nEnter source number: ", &src) ||
                    !readInt("Enter destination number: ", &dest)) {
                    printf("Invalid input.\n");
                    choice = 0;
                    break;
                }
                if (src < 0 || src >= V || dest < 0 || dest >= V)
                    printf("Invalid location number! Use 0 to %d.\n", V - 1);
                else
                    printf("\n%s is %s from %s.\n", locations[dest],
                           isReachable(src, dest) ? "REACHABLE" : "NOT reachable",
                           locations[src]);
                break;
            case 7:
                showComponents();
                break;
            case 8:
                displayLocations();
                if (!readInt("\nEnter first location number: ", &u) ||
                    !readInt("Enter second location number: ", &v)) {
                    printf("Invalid input.\n");
                    choice = 0;
                    break;
                }
                if (addEdge(u, v))
                    printf("Path added: %s <-> %s\n", locations[u], locations[v]);
                else
                    printf("Could not add path (invalid numbers or path already exists).\n");
                break;
            case 9:
                runTests();
                break;
            case 10:
                showComplexity();
                break;
            case 0:
                printf("Thank you for using Campus Route Finder!\n");
                break;
            default:
                printf("Invalid choice! Please select 0 to 10.\n");
        }
    } while (choice != 0);

    freeGraph();
    return 0;
}