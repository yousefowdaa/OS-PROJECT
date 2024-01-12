#include <algorithm>
#include <fstream>
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
using namespace std;

struct Process {
  int attributes[6];
};

void insert(Process *processes, int currentTime, int count) {
  for (int i = 0; i < count; i++) {
    int *attributes = processes[i].attributes;
    if (attributes[5] == 0 && attributes[1] == currentTime) {
      attributes[5] = 1;
    }
  }
}

void executeProcess(Process *executionProcesses, int processIndex, int count) {
  for (int i = 0; i < count; i++) {
    int *attributes = executionProcesses[i].attributes;
    if (i == processIndex && attributes[4] == 0) {
      attributes[0] = attributes[0] - 1;
      if (attributes[0] == 0) {
        attributes[4] = 1;
      }
    } else if (attributes[5] == 1 && attributes[4] == 0) {
      attributes[3] = attributes[3] + 1;
    }
  }
}

int selectProcessToExecute(Process *processes, int type, int count) {
  int selectedProcessIndex = -1;
  int lowestProcessValue = 0;
  bool isProcessSelected = false;

  for (int i = 0; i < count; i++) {
    int *attributes = processes[i].attributes;

    if ((isProcessSelected == false && attributes[5] == 1 && attributes[4] == 0) ||
        (isProcessSelected == true && attributes[5] == 1 && attributes[4] == 0 &&
         attributes[type] < lowestProcessValue)) {
      lowestProcessValue = attributes[type];
      selectedProcessIndex = i;
      isProcessSelected = true;
    }
  }

  return selectedProcessIndex;
}

void preemptiveScheduling(Process *pProcesses, int type, int count) {
  bool isRunning = true;
  int currentTime = 0;
  cout << "Preemptive Scheduling starts\n";
  while (isRunning == true) {
    insert(pProcesses, currentTime, count);
    int processToExecute = selectProcessToExecute(pProcesses, type, count);
    if (processToExecute == -1) {
      isRunning = false;
    } else {
      executeProcess(pProcesses, processToExecute, count);
      currentTime++;
    }
  }
  cout << "Scheduling Finish\n";
  cout << "Total Time: " << currentTime << " time\n";
}

void nonPreemptiveScheduling(Process *pProcesses, int type, int count) {
  bool isRunning = true;
  int currentTime = 0;
  cout << "Non-Preemptive Scheduling starts\n";
  insert(pProcesses, currentTime, count);
  while (isRunning == true) {
    int processToExecute = selectProcessToExecute(pProcesses, type, count);
    if (processToExecute == -1) {
      isRunning = false;
    } else {
      int *attributes = pProcesses[processToExecute].attributes;
      while (attributes[4] == 0) {
        executeProcess(pProcesses, processToExecute, count);
        currentTime++;
        insert(pProcesses, currentTime, count);
      }
    }
  }
  cout << "Scheduling Finish\n";
  cout << "Total Time: " << currentTime << " time\n";
}

void roundRobinScheduling(Process *pProcesses, int quantum, int count) {
  bool isRunning = true;
  int currentTime = 0;
  cout << "Round Robin Scheduling starts\n";
  insert(pProcesses, currentTime, count);
  int processListSize = count;
  int currentProcessIndex = 0;
  while (isRunning == true) {
    for (int i = 0; i < quantum; i++) {
      int processToExecute = selectProcessToExecute(pProcesses, 1, count);
      int *attributes = pProcesses[currentProcessIndex].attributes;
      if (processToExecute == -1) {
        isRunning = false;
        break;
      } else if (attributes[4] == 1) {
        break;
      } else {
        executeProcess(pProcesses, currentProcessIndex, count);
        insert(pProcesses, currentTime, count);
        currentTime++;
      }
    }
    currentProcessIndex++;
    if (currentProcessIndex == processListSize) {
      currentProcessIndex = 0;
    }
  }
}

float calculateAverageWaitingTime(Process *pProcesses, int count) {
  int sum = 0;
  for (int i = 0; i < count; i++) {
    int *attributes = pProcesses[i].attributes;
    sum = sum + attributes[3];
  }
  return (sum / static_cast<float>(count));
}

void displayWaitingTime(Process *pProcesses, int count) {
  for (int i = 0; i < count; i++) {
    int *attributes = pProcesses[i].attributes;
    cout << "P" << i + 1 << ": " << attributes[3] << "ms\n";
  }
}

void writeToFile(string output) {
  ofstream file;
  file.open(output);
  cout << "Enter your message: ";
  file << "Average: ms\n";
  file.close();
}

char *inputFileName = nullptr;
char *outputFileName = nullptr;

int main(int argc, char *argv[]) {
  int option;
  if (argc < 5) {
    cout << "You should introduce 2 arguments";
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
  if (inputFileName == nullptr || outputFileName == nullptr) {
    cout << "Use the parameter -f for input and -o for output";
    exit(0);
  }
  int processCount = 0;
  vector<Process> tempArray{};
  ifstream file(inputFileName);
  string line;
  while (getline(file, line)) {
    vector<string> words{};
    string delimiter = ":";
    int pos;
    string text = line;
    while ((pos = text.find(delimiter)) != string::npos) {
      words.push_back(text.substr(0, pos));
      text.erase(0, pos + delimiter.length());
    }
    int attributes[6];
    int count = 0;
    for (const auto &word : words) {
      attributes[count] = stoi(word);
      count++;
    }
    attributes[2] = stoi(text);
    attributes[3] = 0;
    attributes[4] = 0;
    attributes[5] = 0;
    Process process = {{attributes[0], attributes[1], attributes[2],
                        attributes[3], attributes[4], attributes[5]}};
    tempArray.push_back(process);
    processCount++;
  }
  Process processes[processCount];
  int j = 0;
  for (const auto &element : tempArray) {
    processes[j] = element;
    j++;
  }
  int userChoice;
  bool isFinished = false;
  while (isFinished == false) {
    cout << "Choose Action: \n";
    cout << "1. Preemptive Scheduling\n";
    cout << "2. Non-Preemptive Scheduling\n";
    cout << "3. Show Result\n";
    cout << "4. End Program\n";
    cin >> userChoice;
    switch (userChoice) {
    case 1: {
      int preemptiveSchedulingType;
      cout << "Choose Between the five Preemptive Scheduling Types \n";
      cout << "1. None\n";
      cout << "2. First Come, First Served Scheduling\n";
      cout << "3. Shortest-Job-First Scheduling\n";
      cout << "4. Priority Scheduling\n";
      cout << "5. Round-Robin Scheduling\n";
      cin >> preemptiveSchedulingType;
      switch (preemptiveSchedulingType) {
      case 1:
        cout << "None\n";
        break;
      case 2:
        cout << "First Come, First Served Scheduling\n";
        preemptiveScheduling(processes, 1, processCount);
        break;
      case 3:
        cout << "Shortest-Job-First Scheduling\n";
        preemptiveScheduling(processes, 0, processCount);
        break;
      case 4:
        cout << "Priority Scheduling\n";
        preemptiveScheduling(processes, 2, processCount);
        break;
      case 5:
        cout << "Round-Robin Scheduling\n";
        cout << "Insert Quantum\n";
        int quantum;
        cin >> quantum;
        roundRobinScheduling(processes, quantum, processCount);
        break;
      default:
        cout << "You have to choose between 1 - 5";
        break;
      }
    } break;
    case 2: {
      int nonPreemptiveSchedulingType;
      cout << "Choose Between the five Non-Preemptive Scheduling Types \n";
      cout << "1. None\n";
      cout << "2. First Come, First Served Scheduling\n";
      cout << "3. Shortest-Job-First Scheduling\n";
      cout << "4. Priority Scheduling\n";
      cin >> nonPreemptiveSchedulingType;
      switch (nonPreemptiveSchedulingType) {
      case 1:
        cout << "None Scheduling method chosen try again \n";
        break;
      case 2:
        cout << "First Come, First Served Scheduling\n";
        nonPreemptiveScheduling(processes, 1, processCount);
        break;
      case 3:
        cout << "Shortest-Job-First Scheduling\n";
        nonPreemptiveScheduling(processes, 0, processCount);
        break;
      case 4:
        cout << "Priority Scheduling\n";
        nonPreemptiveScheduling(processes, 2, processCount);
        break;
      default:
        cout << "You have to choose between 1 - 4";
        break;
      }
    } break;
    case 3: {
      cout << "The Result is:\n";
      cout << "Processes Waiting Time \n";
      float average;
      average = calculateAverageWaitingTime(processes, processCount);
      displayWaitingTime(processes, processCount);
      cout << "Average: " << average << "ms\n";
    } break;
    case 4: {
      cout << "Exit Program, Thank you  \n";
      float averages;
      averages = calculateAverageWaitingTime(processes, processCount);
      ofstream file;
      file.open(outputFileName);
      file << "Processes Waiting Time \n";
      for (int i = 0; i < processCount; i++) {
        int *attributes = processes[i].attributes;
        file << "P" << i + 1 << ": " << attributes[3] << "ms\n";
      }
      file << "Average: " << averages << "ms\n";
      file.close();
      isFinished = true;
    } break;
    default:
      cout << "You have to choose between 1 - 4";
      break;
    }
  }
  return 0;
}
