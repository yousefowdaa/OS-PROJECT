#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct Task {
  int properties[6];
};

void enqueue(struct Task *tasks, int current_time, int count) {
  for (int i = 0; i < count; i++) {
    int *properties = tasks[i].properties;
    if (properties[5] == 0 && properties[1] == current_time) {
      properties[5] = 1;
    }
  }
}

void executeTask(struct Task *runningTasks, int taskIndex, int count) {
  for (int i = 0; i < count; i++) {
    int *properties = runningTasks[i].properties;
    if (i == taskIndex && properties[4] == 0) {
      properties[0] = properties[0] - 1;
      if (properties[0] == 0) {
        properties[4] = 1;
      }
    } else if (properties[5] == 1 && properties[4] == 0) {
      properties[3] = properties[3] + 1;
    }
  }
}

int selectTaskToExecute(struct Task *tasks, int type, int count) {
  int selectedTaskIndex = -1;
  int lowestTaskValue = 0;
  int isTaskSelected = 0;

  for (int i = 0; i < count; i++) {
    int *properties = tasks[i].properties;

    if ((isTaskSelected == 0 && properties[5] == 1 && properties[4] == 0) ||
        (isTaskSelected == 1 && properties[5] == 1 && properties[4] == 0 &&
         properties[type] < lowestTaskValue)) {
      lowestTaskValue = properties[type];
      selectedTaskIndex = i;
      isTaskSelected = 1;
    }
  }

  return selectedTaskIndex;
}

void preemptiveScheduling(struct Task *tTasks, int type, int count) {
  int isRunning = 1;
  int current_time = 0;
  printf("Preemptive Scheduling starts\n");
  while (isRunning == 1) {
    enqueue(tTasks, current_time, count);
    int taskToExecute = selectTaskToExecute(tTasks, type, count);
    if (taskToExecute == -1) {
      isRunning = 0;
    } else {
      executeTask(tTasks, taskToExecute, count);
      current_time++;
    }
  }
  printf("Scheduling Finish\n");
  printf("Total Time: %d time\n", current_time);
}

void nonPreemptiveScheduling(struct Task *tTasks, int type, int count) {
  int isRunning = 1;
  int current_time = 0;
  printf("Non-Preemptive Scheduling starts\n");
  enqueue(tTasks, current_time, count);
  while (isRunning == 1) {
    int taskToExecute = selectTaskToExecute(tTasks, type, count);
    if (taskToExecute == -1) {
      isRunning = 0;
    } else {
      int *properties = tTasks[taskToExecute].properties;
      while (properties[4] == 0) {
        executeTask(tTasks, taskToExecute, count);
        current_time++;
        enqueue(tTasks, current_time, count);
      }
    }
  }
  printf("Scheduling Finish\n");
  printf("Total Time: %d time\n", current_time);
}

void roundRobinScheduling(struct Task *tTasks, int quantum, int count) {
  int isRunning = 1;
  int current_time = 0;
  printf("Round Robin Scheduling starts\n");
  enqueue(tTasks, current_time, count);
  int taskListSize = count;
  int currentTaskIndex = 0;
  while (isRunning == 1) {
    for (int i = 0; i < quantum; i++) {
      int taskToExecute = selectTaskToExecute(tTasks, 1, count);
      int *properties = tTasks[currentTaskIndex].properties;
      if (taskToExecute == -1) {
        isRunning = 0;
        break;
      } else if (properties[4] == 1) {
        break;
      } else {
        executeTask(tTasks, currentTaskIndex, count);
        enqueue(tTasks, current_time, count);
        current_time++;
      }
    }
    currentTaskIndex++;
    if (currentTaskIndex == taskListSize) {
      currentTaskIndex = 0;
    }
  }
}

float calculateAverageWaitingTime(struct Task *tTasks, int count) {
  int sum = 0;
  for (int i = 0; i < count; i++) {
    int *properties = tTasks[i].properties;
    sum = sum + properties[3];
  }
  return (float)sum / count;
}

void displayWaitingTime(struct Task *tTasks, int count) {
  for (int i = 0; i < count; i++) {
    int *properties = tTasks[i].properties;
    printf("T%d: %dms\n", i + 1, properties[3]);
  }
}

void writeToOutputFile(char *output) {
  FILE *file = fopen(output, "w");
  if (file == NULL) {
    printf("Error opening file\n");
    exit(1);
  }

  printf("Enter your message: ");
  fprintf(file, "Average: ms\n");
  fclose(file);
}

char *inputFileName = NULL;
char *outputFileName = NULL;

int main(int argc, char *argv[]) {
  int option;
  if (argc < 5) {
    printf("You should introduce 2 arguments");
    exit(0);
  }
  while ((option = getopt(argc, argv, "f:o:")) != -1) {
    switch (option) {
    case 'f':
      inputFileName = optarg;
      break;
    case 'o':
      outputFileName = optarg;
      break;
    default:
      break;
    }
  }
  if (inputFileName == NULL || outputFileName == NULL) {
    printf("Use the parameter -f for input and -o for output");
    exit(0);
  }
  int taskCount = 0;
  struct Task tempArray[100];  // Assuming a maximum of 100 tasks
  FILE *file = fopen(inputFileName, "r");
  if (file == NULL) {
    printf("Error opening file\n");
    exit(1);
  }
  char line[256];
  while (fgets(line, sizeof(line), file)) {
    char *token = strtok(line, ":");
    int properties[6];
    int count = 0;
    while (token != NULL) {
      properties[count] = atoi(token);
      token = strtok(NULL, ":");
      count++;
    }
    properties[2] = atoi(token);
    properties[3] = 0;
    properties[4] = 0;
    properties[5] = 0;
    struct Task task;
    memcpy(task.properties, properties, sizeof(properties));
    tempArray[taskCount] = task;
    taskCount++;
  }
  fclose(file);

  struct Task tasks[taskCount];
  for (int j = 0; j < taskCount; j++) {
    tasks[j] = tempArray[j];
  }

  int userChoice;
  int isFinished = 0;
  while (isFinished == 0) {
    printf("Choose Action: \n");
    printf("1. Preemptive Scheduling\n");
    printf("2. Non-Preemptive Scheduling\n");
    printf("3. Show Result\n");
    printf("4. End Program\n");
    scanf("%d", &userChoice);
    switch (userChoice) {
    case 1: {
      int preemptiveSchedulingType;
      printf("Choose Between the five Preemptive Scheduling Types \n");
      printf("1. None\n");
      printf("2. First Come, First Served Scheduling\n");
      printf("3. Shortest-Job-First Scheduling\n");
      printf("4. Priority Scheduling\n");
      printf("5. Round-Robin Scheduling\n");
      scanf("%d", &preemptiveSchedulingType);
      switch (preemptiveSchedulingType) {
      case 1:
        printf("None\n");
        break;
      case 2:
        printf("First Come, First Served Scheduling\n");
        preemptiveScheduling(tasks, 1, taskCount);
        break;
      case 3:
        printf("Shortest-Job-First Scheduling\n");
        preemptiveScheduling(tasks, 0, taskCount);
        break;
      case 4:
        printf("Priority Scheduling\n");
        preemptiveScheduling(tasks, 2, taskCount);
        break;
      case 5:
        printf("Round-Robin Scheduling\n");
        printf("Insert Quantum\n");
        int quantum;
        scanf("%d", &quantum);
        roundRobinScheduling(tasks, quantum, taskCount);
        break;
      default:
        printf("You have to choose between 1 - 5");
        break;
      }
    } break;
    case 2: {
      int nonPreemptiveSchedulingType;
      printf("Choose Between the five Non-Preemptive Scheduling Types \n");
      printf("1. None\n");
      printf("2. First Come, First Served Scheduling\n");
      printf("3. Shortest-Job-First Scheduling\n");
      printf("4. Priority Scheduling\n");
      scanf("%d", &nonPreemptiveSchedulingType);
      switch (nonPreemptiveSchedulingType) {
      case 1:
        printf("None Scheduling method chosen try again \n");
        break;
      case 2:
        printf("First Come, First Served Scheduling\n");
        nonPreemptiveScheduling(tasks, 1, taskCount);
        break;
      case 3:
        printf("Shortest-Job-First Scheduling\n");
        nonPreemptiveScheduling(tasks, 0, taskCount);
        break;
      case 4:
        printf("Priority Scheduling\n");
        nonPreemptiveScheduling(tasks, 2, taskCount);
        break;
      default:
        printf("You have to choose between 1 - 4");
        break;
      }
    } break;
    case 3: {
      printf("The Result is:\n");
      printf("Tasks Waiting Time \n");
      float average;
      average = calculateAverageWaitingTime(tasks, taskCount);
      displayWaitingTime(tasks, taskCount);
      printf("Average: %fms\n", average);
    } break;
    case 4: {
      printf("Exit Program, Thank you  \n");
      float averages;
      averages = calculateAverageWaitingTime(tasks, taskCount);
      FILE *file = fopen(outputFileName, "w");
      if (file == NULL) {
        printf("Error opening file\n");
        exit(1);
      }
      fprintf(file, "Tasks Waiting Time \n");
      for (int i = 0; i < taskCount; i++) {
        int *properties = tasks[i].properties;
        fprintf(file, "T%d: %dms\n", i + 1, properties[3]);
      }
      fprintf(file, "Average: %fms\n", averages);
      fclose(file);
      isFinished = 1;
    } break;
    default:
      printf("You have to choose between 1 - 4");
      break;
    }
  }
  return 0;
}
