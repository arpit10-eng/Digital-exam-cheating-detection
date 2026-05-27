#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX 100
#define BASE 31
#define MOD 1000000007

// STOPWORDS
char *stopwords[] = {"the","is","and","of","in","to","a","an","on","for","with"};
int stopCount = 11;

struct Student 
{
    int id;
    char answers[500];
    float time;
    long long hash;
    double suspicionScore;
};

struct Student students[MAX];
int adj[MAX][MAX];
int visited[MAX];
double simMatrix[MAX][MAX];
int group[MAX];

int n;

// HEADER
void printHeader()
{
    printf("\n============================================\n");
    printf("     DIGITAL CHEATING DETECTION SYSTEM\n");
    printf("============================================\n");
}

// STOPWORD CHECK
int isStopword(char *word)
{
    for (int i = 0; i < stopCount; i++)
        if (strcmp(word, stopwords[i]) == 0)
            return 1;
    return 0;
}

// NORMALIZE (remove stopwords + lowercase)
void normalize(char str[]) 
{
    char temp[500] = "";
    char word[50];
    int i = 0;

    while (sscanf(str + i, "%s", word) == 1)
    {
        if (!isStopword(word))
        {
            for (int k = 0; word[k]; k++)
                word[k] = tolower(word[k]);

            strcat(temp, word);
        }
        i += strlen(word) + 1;
    }

    strcpy(str, temp);
}

// HASH
long long computeHash(char str[])
{
    long long hash = 0, power = 1;

    for (int i = 0; str[i]; i++)
    {
        hash = (hash + (str[i] * power) % MOD) % MOD;
        power = (power * BASE) % MOD;
    }
    return hash;
}

// LCS SIMILARITY
double similarity(char a[], char b[]) 
{
    int lenA = strlen(a), lenB = strlen(b);
    static int dp[505][505];

    for (int i = 0; i <= lenA; i++)
        for (int j = 0; j <= lenB; j++)
        {
            if (i == 0 || j == 0)
                dp[i][j] = 0;
            else if (a[i-1] == b[j-1])
                dp[i][j] = dp[i-1][j-1] + 1;
            else
                dp[i][j] = dp[i-1][j] > dp[i][j-1] ? dp[i-1][j] : dp[i][j-1];
        }

    int lcs = dp[lenA][lenB];
    int maxLen = lenA > lenB ? lenA : lenB;

    return (double)lcs / maxLen;
}

// SORT BY TIME
void sortStudentsByTime()
{
    for (int i = 0; i < n-1; i++)
        for (int j = 0; j < n-i-1; j++)
            if (students[j].time > students[j+1].time)
            {
                struct Student t = students[j];
                students[j] = students[j+1];
                students[j+1] = t;
            }
}

// SORT BY SUSPICION
void sortBySuspicion()
{
    for (int i = 0; i < n-1; i++)
        for (int j = 0; j < n-i-1; j++)
            if (students[j].suspicionScore < students[j+1].suspicionScore)
            {
                struct Student t = students[j];
                students[j] = students[j+1];
                students[j+1] = t;
            }
}

// DFS GROUP
void dfs_collect(int node, int *size) 
{
    visited[node] = 1;
    group[(*size)++] = node;

    for (int i = 0; i < n; i++) 
        if (adj[node][i] && !visited[i])
            dfs_collect(i, size);
}

int main() 
{
    float TIME_THRESHOLD;
    double SCORE_THRESHOLD;

    printHeader();

    printf("\nEnter number of students: ");
    scanf("%d", &n);
    while(getchar()!='\n');

    printf("Enter time threshold: ");
    scanf("%f", &TIME_THRESHOLD);

    printf("Enter score threshold: ");
    scanf("%lf", &SCORE_THRESHOLD);
    while(getchar()!='\n');

    printf("\n--- Input Student Details ---\n");

    for (int i = 0; i < n; i++) 
    {
        printf("\nStudent %d ID: ", i+1);
        scanf("%d", &students[i].id);
        while(getchar()!='\n');

        printf("Answer: ");
        fgets(students[i].answers, 500, stdin);
        students[i].answers[strcspn(students[i].answers,"\n")] = 0;

        normalize(students[i].answers);
        students[i].hash = computeHash(students[i].answers);
        students[i].suspicionScore = 0;

        printf("Time: ");
        scanf("%f", &students[i].time);
        while(getchar()!='\n');
    }

    // INIT
    for (int i = 0; i < n; i++)
    {
        visited[i] = 0;
        for (int j = 0; j < n; j++)
        {
            adj[i][j] = 0;
            simMatrix[i][j] = 0;
        }
    }

    sortStudentsByTime();

    FILE *fp = fopen("report.txt", "w");

    printf("\n--------------------------------------------\n");
    printf("           DETECTION PROCESS\n");
    printf("--------------------------------------------\n");

    printf("\n%-10s %-10s %-10s %-10s\n", "ID1", "ID2", "SIM", "SCORE");
    printf("--------------------------------------------\n");

    // DETECTION
    for (int i = 0; i < n; i++) 
    {
        for (int j = i+1; j < n; j++) 
        {
            if (students[j].time - students[i].time > TIME_THRESHOLD)
                break;

            int lenDiff = abs(strlen(students[i].answers) - strlen(students[j].answers));
            if (lenDiff > 20) 
                continue;

            double sim = similarity(students[i].answers, students[j].answers);

            simMatrix[i][j] = sim;
            simMatrix[j][i] = sim;

            double score = 0;

            if (students[i].hash == students[j].hash)
                score += 0.5;

            score += sim * 0.3;

            if (abs(students[i].time - students[j].time) < 0.5)
                score += 0.2;

            if (lenDiff < 3)
                score += 0.1;

            printf("%-10d %-10d %-10.2f %-10.2f\n",
                   students[i].id, students[j].id, sim, score);

            if (score >= SCORE_THRESHOLD)
            {
                adj[i][j] = adj[j][i] = 1;
                students[i].suspicionScore += score;
                students[j].suspicionScore += score;
            }
        }
    }

    // REPORT
    printf("\n============================================\n");
    printf("           SUSPICIOUS GROUPS\n");
    printf("============================================\n");

    fprintf(fp, "============================================\n");
    fprintf(fp, "        CHEATING DETECTION REPORT\n");
    fprintf(fp, "============================================\n");

    int groupNo = 1;

    for (int i = 0; i < n; i++) 
    {
        if (!visited[i]) 
        {
            int size = 0;
            dfs_collect(i, &size);

            if (size > 1)
            {
                printf("\nGroup %d\n", groupNo);
                printf("----------------------------------\n");
                fprintf(fp, "\nGroup %d\n", groupNo);
                fprintf(fp, "----------------------------------\n");

                printf("Students: ");
                fprintf(fp, "Students: ");

                for (int k = 0; k < size; k++)
                {
                    printf("%d ", students[group[k]].id);
                    fprintf(fp, "%d ", students[group[k]].id);
                }

                int edges = 0;
                double totalSim = 0;
                int pairs = 0;

                for (int x = 0; x < size; x++)
                    for (int y = x+1; y < size; y++)
                    {
                        if (adj[group[x]][group[y]]) edges++;
                        totalSim += simMatrix[group[x]][group[y]];
                        pairs++;
                    }

                double density = (double)edges / (size*(size-1)/2);
                double avgSim = pairs ? totalSim/pairs : 0;

                char *risk = (avgSim > 0.75 || density > 0.7) ? "HIGH" :
                             (avgSim > 0.5) ? "MEDIUM" : "LOW";

                printf("\nSize: %d", size);
                printf("\nConnections: %d", edges);
                printf("\nDensity: %.2f", density);
                printf("\nAvg Similarity: %.2f", avgSim);
                printf("\nRisk Level: %s\n", risk);

                fprintf(fp, "\nSize: %d", size);
                fprintf(fp, "\nConnections: %d", edges);
                fprintf(fp, "\nDensity: %.2f", density);
                fprintf(fp, "\nAvg Similarity: %.2f", avgSim);
                fprintf(fp, "\nRisk Level: %s\n", risk);

                groupNo++;
            }
        }
    }
    sortBySuspicion();

    printf("\n============================================\n");
    printf("        TOP SUSPICIOUS STUDENTS\n");
    printf("============================================\n");

    fprintf(fp, "\n============================================\n");
    fprintf(fp, "      TOP SUSPICIOUS STUDENTS\n");
    fprintf(fp, "============================================\n");

    printf("\n%-10s %-15s\n", "ID", "SCORE");
    printf("-----------------------------\n");

    for (int i = 0; i < n; i++)
    {
        printf("%-10d %-15.2f\n", students[i].id, students[i].suspicionScore);
        fprintf(fp, "%-10d %-15.2f\n", students[i].id, students[i].suspicionScore);
    }

    fclose(fp);

    printf("\n============================================\n");
    printf("Report successfully saved to report.txt\n");
    printf("============================================\n");

    return 0;
}