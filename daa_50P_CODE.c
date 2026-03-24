#include <stdio.h>
#include <string.h>

#define MAX 100

struct Student {
    int id;
    char answers[200];  // increased size for spaces
    int time;
};

struct Student students[MAX];
int adj[MAX][MAX];
int visited[MAX];
int n;

// similarity function
double similarity(char a[], char b[]) {
    int len = strlen(a);
    int match = 0;

    for (int i = 0; i < len && i < strlen(b); i++) {
        if (a[i] == b[i])
            match++;
    }

    return (double)match / len;
}

// DFS
void dfs(int node) {
    visited[node] = 1;
    printf("%d ", students[node].id);

    for (int i = 0; i < n; i++) {
        if (adj[node][i] == 1 && !visited[i]) {
            dfs(i);
        }
    }
}

// sort by time
void sortStudents() {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (students[j].time > students[j + 1].time) {
                struct Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
}

int main() {
    printf("========================================\n");
    printf(" DIGITAL EXAM CHEATING DETECTION SYSTEM \n");
    printf("========================================\n\n");

    printf("Enter total number of students: ");
    scanf("%d", &n);
    getchar(); // clear buffer

    printf("\n--- Input Student Details ---\n");
    printf("NOTE: Now you can enter answers with spaces (e.g., A B C D)\n\n");

    for (int i = 0; i < n; i++) {
        printf("\nStudent %d\n", i + 1);
        printf("----------------------\n");

        printf("Enter Student ID: ");
        scanf("%d", &students[i].id);
        getchar(); // clear buffer

        printf("Enter Answer String: ");
        fgets(students[i].answers, sizeof(students[i].answers), stdin);

        // remove newline
        students[i].answers[strcspn(students[i].answers, "\n")] = 0;

        printf("Enter Submission Time: ");
        scanf("%d", &students[i].time);
        getchar(); // clear buffer
    }

    // initialize
    for (int i = 0; i < n; i++) {
        visited[i] = 0;
        for (int j = 0; j < n; j++) {
            adj[i][j] = 0;
        }
    }

    sortStudents();

    int TIME_THRESHOLD = 5;
    double SIM_THRESHOLD = 0.7;

    // build graph
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {

            if (students[j].time - students[i].time > TIME_THRESHOLD)
                break;

            double sim = similarity(students[i].answers, students[j].answers);

            if (sim >= SIM_THRESHOLD) {
                adj[i][j] = 1;
                adj[j][i] = 1;
            }
        }
    }

    printf("\n--- Suspicious Groups ---\n");

    int found = 0;

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            int hasEdge = 0;

            for (int j = 0; j < n; j++) {
                if (adj[i][j] == 1) {
                    hasEdge = 1;
                    break;
                }
            }

            if (hasEdge) {
                found = 1;
                printf("Group: ");
                dfs(i);
                printf("\n");
            }
        }
    }

    if (!found) {
        printf("No suspicious groups detected.\n");
    }

    printf("\nDetection Completed.\n");

    return 0;
}