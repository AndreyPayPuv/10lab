#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <queue>

#ifdef _WIN32
#include <windows.h>
#endif

int** create_matrix(int n)
{
    int** G = (int**)malloc(n * sizeof(int*));
    if (G == NULL) {
        printf("Ошибка выделения памяти.\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) {
        G[i] = (int*)malloc(n * sizeof(int));
        if (G[i] == NULL) {
            printf("Ошибка выделения памяти.\n");
            exit(1);
        }
    }
    return G;
}

void free_matrix(int** G, int n)
{
    for (int i = 0; i < n; i++)
        free(G[i]);
    free(G);
}

void generate_matrix(int** G, int n, int weighted, int directed)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            G[i][j] = 0;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            if (i == j || (!directed && j < i))
                continue;
            if (rand() % 100 < 35) {
                G[i][j] = weighted ? 1 + rand() % 9 : 1;
                if (!directed)
                    G[j][i] = G[i][j];
            }
        }

    // Добавляем недостающие ребра цикла для связности графа.
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        if (G[i][j] == 0)
            G[i][j] = weighted ? 1 + rand() % 9 : 1;
        if (!directed)
            G[j][i] = G[i][j];
    }
}

void print_matrix(int** G, int n)
{
    printf("    ");
    for (int j = 0; j < n; j++)
        printf("%4d", j + 1);
    printf("\n");
    for (int i = 0; i < n; i++) {
        printf("%3d:", i + 1);
        for (int j = 0; j < n; j++)
            printf("%4d", G[i][j]);
        printf("\n");
    }
}

void BFSD(int** G, int n, int v, int DIST[])
{
    for (int i = 0; i < n; i++)
        DIST[i] = -1;

    std::queue<int> Q;
    Q.push(v);
    DIST[v] = 0;
    while (!Q.empty()) {
        v = Q.front();
        Q.pop();
        printf("%d ", v + 1);
        for (int i = 0; i < n; i++)
            if (G[v][i] > 0 && DIST[i] == -1) {
                Q.push(i);
                DIST[i] = DIST[v] + G[v][i];
            }
    }
    printf("\n");
}

void characteristics(int** D, int n)
{
    int* e = (int*)malloc(n * sizeof(int));
    if (e == NULL) {
        printf("Ошибка выделения памяти.\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) {
        e[i] = 0;
        for (int j = 0; j < n; j++)
            if (D[i][j] > e[i])
                e[i] = D[i][j];
        printf("e(%d) = %d\n", i + 1, e[i]);
    }

    int r = e[0], d = e[0];
    for (int i = 1; i < n; i++) {
        if (e[i] < r) r = e[i];
        if (e[i] > d) d = e[i];
    }
    printf("Радиус: %d\nДиаметр: %d\n", r, d);
    printf("Центральные вершины: ");
    for (int i = 0; i < n; i++)
        if (e[i] == r) printf("%d ", i + 1);
    printf("\nПериферийные вершины: ");
    for (int i = 0; i < n; i++)
        if (e[i] == d) printf("%d ", i + 1);
    printf("\n");
    free(e);
}

void run(int n, int weighted, int directed, int start)
{
    int** G = create_matrix(n);
    int** D = create_matrix(n);
    printf("\nГраф: %s, %s\n", weighted ? "взвешенный" : "невзвешенный",
        directed ? "ориентированный" : "неориентированный");
    generate_matrix(G, n, weighted, directed);
    printf("Матрица смежности:\n");
    print_matrix(G, n);

    if (start == 0) {
        printf("Введите начальную вершину (1..%d): ", n);
        if (scanf("%d", &start) != 1 || start < 1 || start > n) {
            printf("Некорректная вершина.\n");
            free_matrix(G, n);
            free_matrix(D, n);
            exit(1);
        }
    }
    start--;

    printf("Обход из выбранной вершины %d: ", start + 1);
    BFSD(G, n, start, D[start]);
    printf("Расстояния из вершины %d:\n", start + 1);
    for (int i = 0; i < n; i++)
        printf("До вершины %d: %d\n", i + 1, D[start][i]);

    for (int i = 0; i < n; i++) {
        if (i != start) {
            printf("Обход из вершины %d: ", i + 1);
            BFSD(G, n, i, D[i]);
        }
    }
    printf("Матрица расстояний:\n");
    print_matrix(D, n);
    characteristics(D, n);
    free_matrix(G, n);
    free_matrix(D, n);
}

int main(int argc, char* argv[])
{
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    srand((unsigned)time(NULL));
    int n = 0, start = 0, weighted = 1, directed = 0;
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 >= argc) {
            printf("Не указано значение параметра.\n");
            return 1;
        }
        int value = atoi(argv[i + 1]);
        if (strcmp(argv[i], "-weighted") == 0 && (value == 0 || value == 1))
            weighted = value;
        else if (strcmp(argv[i], "-directed") == 0 && (value == 0 || value == 1))
            directed = value;
        else if (strcmp(argv[i], "-n") == 0 && value >= 2)
            n = value;
        else if (strcmp(argv[i], "-start") == 0 && value >= 1)
            start = value;
        else {
            printf("Неизвестный параметр или неверное значение.\n");
            return 1;
        }
    }

    if (n == 0) {
        printf("Введите число вершин: ");
        if (scanf("%d", &n) != 1 || n < 2) {
            printf("Некорректный ввод.\n");
            return 1;
        }
    }
    if (start > n) {
        printf("Некорректная вершина.\n");
        return 1;
    }
    if (argc == 1) {
        run(n, 1, 0, start);
        run(n, 1, 1, start);
    }
    else
        run(n, weighted, directed, start);
    return 0;
}
