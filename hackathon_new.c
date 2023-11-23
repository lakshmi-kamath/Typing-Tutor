#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TEXT_LENGTH 100
#define MAX_INPUT_LENGTH 100
#define LEADERBOARD_SIZE 10
#define STACK_SIZE 100

typedef struct {
    char items[STACK_SIZE];
    int top;
} Stack;

typedef struct UserData {
    char username[MAX_INPUT_LENGTH];
    double highestSpeed;
    struct UserData *next;
} UserData;

typedef struct {
    char text[MAX_TEXT_LENGTH];
    int difficulty;
} TypingExercise;

typedef struct {
    int correctChars;
    int totalChars;
    double timeTaken;
} TypingResult;

double calculateWPM(const TypingResult *result) {
    return (result->totalChars / 5.0) / (result->timeTaken / 60.0);
}

TypingExercise generateExercise(int difficulty) {
    TypingExercise exercise;

    switch (difficulty) {
        case 1:
            strcpy(exercise.text, "This is an easy typing exercise for beginners.");
            break;
        case 2:
            strcpy(exercise.text, "This is a moderate typing exercise to improve your skills.");
            break;
        case 3:
            strcpy(exercise.text, "This is a difficult typing exercise to challenge advanced typists.");
            break;
        default:
            strcpy(exercise.text, "This is a default typing exercise.");
    }

    exercise.difficulty = difficulty;

    return exercise;
}

void initializeStack(Stack *s) {
    s->top = -1;
}

int isStackEmpty(Stack *s) {
    return (s->top == -1);
}

int isStackFull(Stack *s) {
    return (s->top == STACK_SIZE - 1);
}

void push(Stack *s, char value) {
    if (!isStackFull(s)) {
        s->items[++s->top] = value;
    }
}

char pop(Stack *s) {
    if (!isStackEmpty(s)) {
        return s->items[s->top--];
    }
    return '\0'; // Return null character for an empty stack
}

void reverseString(const char *str, Stack *s) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        push(s, str[i]);
    }
}

int compareStackWithUserInput(Stack *s, const char *userInput) {
    int errorCount = 0;
    int userInputLength = strlen(userInput);
    for (int i = 0; i < userInputLength; i++) {
        char stackTop = pop(s);
        if (stackTop != userInput[i]) {
            errorCount++;
        }
    }
    return errorCount;
}

TypingResult takeTypingTest(const TypingExercise *exercise) {
    TypingResult result = {0};

    printf("Typing Exercise: %s\n", exercise->text);
    printf("Type the given text:\n");

    Stack charStack;
    initializeStack(&charStack);
    reverseString(exercise->text, &charStack); // Store exercise text in reverse order in the stack

    time_t startTime, endTime;
    time(&startTime);

    int errorCount = 0;
    int totalChars = strlen(exercise->text);
    printf("%s\n", exercise->text); // Display the entire text for user reference

    for (int i = 0; i < totalChars; i++) {
        char userInput = getchar();
        if (userInput == '\n') {
            break;
        }
        if (userInput != exercise->text[i]) {
            errorCount++;
        }
    }

    while (getchar() != '\n'); // Clear remaining characters in input buffer

    result.totalChars = totalChars;
    result.correctChars = totalChars - errorCount;

    time(&endTime);
    result.timeTaken = difftime(endTime, startTime);

    printf("\n\nTyping completed!\n");
    printf("Errors: %d\n", errorCount);
    printf("Typing Speed: %.2f WPM\n", calculateWPM(&result));
    printf("Accuracy: %.2f%%\n", ((double)result.correctChars / (double)result.totalChars) * 100.0);

    return result;
}

void updateLeaderboard(UserData** head, const char* username, double speed) {
    UserData* newUserData = (UserData*)malloc(sizeof(UserData));
    strcpy(newUserData->username, username);
    newUserData->highestSpeed = speed;
    newUserData->next = NULL;

    UserData* current = *head;
    UserData* prev = NULL;

    while (current != NULL && speed <= current->highestSpeed) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        newUserData->next = *head;
        *head = newUserData;
    } else {
        prev->next = newUserData;
        newUserData->next = current;
    }

    // Keep leaderboard size within limit
    UserData* temp = *head;
    int count = 0;
    while (temp != NULL && count < LEADERBOARD_SIZE) {
        temp = temp->next;
        count++;
    }

    if (temp != NULL) {
        prev = *head;
        while (prev->next != temp) {
            prev = prev->next;
        }
        prev->next = NULL;
        free(temp);
    }
}   

void displayLeaderboard(UserData* head) {
    printf("\n--- Leaderboard ---\n");
    printf("Username\tHighest Speed (WPM)\n");
    while (head != NULL) {
        printf("%s\t\t%.2f\n", head->username, head->highestSpeed);
        head = head->next;
    }
}

void freeList(UserData* head) {
    UserData* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    int numUsers;
    printf("Enter the number of users: ");
    scanf("%d", &numUsers);

    UserData* users = NULL;

    for (int i = 0; i < numUsers; i++) {
        printf("\nEnter username for user %d: ", i + 1);
        char username[MAX_INPUT_LENGTH];
        scanf("%s", username);

        while (getchar() != '\n'); // Clear input buffer

        int difficulty = 1;

        while (1) {
            TypingExercise exercise = generateExercise(difficulty);
            TypingResult result = takeTypingTest(&exercise);

            updateLeaderboard(&users, username, calculateWPM(&result));

            printf("\nDo you want to try again? (1 for yes, 0 for no): ");
            int retry;
            scanf("%d", &retry);

            if (retry == 0) {
                break;
            }

            while (getchar() != '\n');

            difficulty++;
        }
    }

    displayLeaderboard(users);
    freeList(users);
    printf("\nThanks for using the typing test program!\n");

    return 0;
}