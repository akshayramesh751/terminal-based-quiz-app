#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char question[256];
    char options[4][100];
    int correct_option;
} Question;

void shuffle_questions(Question *questions, int total_questions) {
    srand(time(0));
    for (int i = 0; i < total_questions; i++) {
        int random_index = rand() % total_questions;
        Question temp = questions[i];
        questions[i] = questions[random_index];
        questions[random_index] = temp;
    }
}

int play_quiz(char *username, char *filename) {
    Question questions[50];
    int total_questions = 0;
    int score = 0;
    int skip_used = 0;

    // Read questions from file
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return 0;
    }

    while (fscanf(file, " %[^\n]", questions[total_questions].question) == 1) {
        for (int i = 0; i < 4; i++) {
            fscanf(file, " %[^\n]", questions[total_questions].options[i]);
        }
        fscanf(file, "%d", &questions[total_questions].correct_option);
        total_questions++;
    }
    fclose(file);

    // Shuffle questions and pick the first 10
    shuffle_questions(questions, total_questions);

    printf("\nStarting the quiz for %s...\n\n", username);

    for (int i = 0; i < 10; i++) {
        printf("Q%d: %s\n", i + 1, questions[i].question);
        for (int j = 0; j < 4; j++) {
            printf("%d. %s\n", j + 1, questions[i].options[j]);
        }
        printf("5. Skip this question\n");
        printf("Enter your choice (1-5): ");
        
        int user_choice;
        scanf("%d", &user_choice);

        if (user_choice == 5) {
            if (skip_used) {
                printf("You have already used your skip. Please choose an option.\n");
                i--;
                continue;
            } else {
                skip_used = 1;
                printf("You skipped this question. No points awarded or deducted.\n\n");
                continue;
            }
        }

        if (user_choice < 1 || user_choice > 5) {
            printf("Invalid choice. Please try again.\n");
            i--;
            continue;
        }

        printf("Correct answer: %d. %s\n", questions[i].correct_option,
               questions[i].options[questions[i].correct_option - 1]);

        if (user_choice == questions[i].correct_option) {
            printf("Correct! +20 points\n\n");
            score += 20;
        } else {
            printf("Wrong! -5 points\n\n");
            score -= 5;
        }
    }

    printf("Quiz finished! %s's score: %d\n\n", username, score);
    return score;
}

void update_leaderboard(char *username, int score) {
    FILE *file = fopen("leaderboard.txt", "a");
    if (file == NULL) {
        printf("Error opening leaderboard file.\n");
        return;
    }
    fprintf(file, "%s %d\n", username, score);
    fclose(file);
}

void display_leaderboard() {
    FILE *file = fopen("leaderboard.txt", "r");
    if (file == NULL) {
        printf("Error opening leaderboard file.\n");
        return;
    }

    typedef struct {
        char name[100];
        int score;
    } LeaderboardEntry;

    LeaderboardEntry entries[100];
    int total_entries = 0;

    while (fscanf(file, "%s %d", entries[total_entries].name, &entries[total_entries].score) == 2) {
        total_entries++;
    }
    fclose(file);

    // Sort leaderboard by score in descending order
    for (int i = 0; i < total_entries - 1; i++) {
        for (int j = i + 1; j < total_entries; j++) {
            if (entries[i].score < entries[j].score) {
                LeaderboardEntry temp = entries[i];
                entries[i] = entries[j];
                entries[j] = temp;
            }
        }
    }

    printf("Leaderboard:\n");
    printf("------------------------\n");
    printf("Rank | Name       | Score\n");
    printf("------------------------\n");

    for (int i = 0; i < total_entries; i++) {
        printf("%4d | %-10s | %d\n", i + 1, entries[i].name, entries[i].score);
    }
    printf("------------------------\n");
}

int main() {
    char username[100];
    char difficulty[10];
    int score;

    printf("Welcome to the Quiz Application!\n");
    printf("Please enter your name: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0'; // Remove newline character

    printf("Choose a difficulty level (Easy, Medium, Hard): ");
    scanf("%s", difficulty);

    // Determine the file to use based on difficulty
    char filename[20];
    if (strcmp(difficulty, "Easy") == 0) {
        strcpy(filename, "easy.txt");
    } else if (strcmp(difficulty, "Medium") == 0) {
        strcpy(filename, "medium.txt");
    } else if (strcmp(difficulty, "Hard") == 0) {
        strcpy(filename, "hard.txt");
    } else {
        printf("Invalid difficulty level! Exiting...\n");
        return 1;
    }

    score = play_quiz(username, filename);

    // Update and display the leaderboard
    update_leaderboard(username, score);
    display_leaderboard();

    return 0;
}
