#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <numeric>
#include <limits>

/*Author: Alissa Owens
Date: 3/25/2024 - 3/31/2024
Project Details: This program is used to allow a user to attach a file of their processes, the CPU cycles, and Arrival time of each. 
It takes this input and allows the user to select what algrothim they would like to run : FCFS, SJN, SRT, and RR with the user to set the quatum.*/

using namespace std;

struct Process 
{
    int id;
    int arrivalTime;
    int cpu;
    int remainingTime;
    int startTime;
    int finishTime;
    int waitingTime;
    int turnaroundTime;
};

//These are the comparison function used to compare the process's arrival time and cpu times
//Returns true if the times of process p1 is less than that of p2
bool compareArrivalTime(const Process& p1, const Process& p2) 
{
    return p1.arrivalTime < p2.arrivalTime;
}

bool compareCPUTime(const Process& p1, const Process& p2) 
{
    return p1.cpu < p2.cpu;
}

int main() 
{
    //To fix an error I recalled the using std
    using std::cout;
    using std::cin;

    //Set a variable "choice" and ask the user to enter their wanted algorithm the set the variable chosen. 
    int choice;
    cout << "Choose scheduling algorithm:\n";
    cout << "1. First Come First Serve (FCFS)\n2. Shortest Job Next (SJN)\n3. Shortest Remaining Time (SRT)\n4. Round Robin (RR)\n";
    cin >> choice;

    //string filename = "Arrival_CPU.txt"; ~my test file

    //User enters in their file to read.
    string filename;
    cout << "Enter input file name: ";
    cin >> filename;
    //Opens the file
    ifstream inputFile(filename);

    //Read the num of processes
    int numProcesses;
    inputFile >> numProcesses;
    //Vector to store info of each process
    vector<Process> processes(numProcesses);

    //Reads and set the information from each processor
    for (int i = 0; i < numProcesses; ++i) {
        inputFile >> processes[i].id >> processes[i].arrivalTime >> processes[i].cpu;
        processes[i].remainingTime = processes[i].cpu;
    }
    inputFile.close();

    //Intailize variables for the scheduling 
    int currentTime = 0;
    int completedProcesses = 0;
    //Track if it has finished executing
    vector<bool> completed(numProcesses, false);
    //These vectors store
    vector<int> waitingTimes(numProcesses, 0);
    vector<int> turnaroundTimes(numProcesses, 0);
    
    //I used if and else if statements to go through with the choices
    if (choice == 1) //First Come First Serve
    {
        //I am sorting the processes based on their arrival times
        sort(processes.begin(), processes.end(), compareArrivalTime);

        while (completedProcesses < numProcesses)
        {
            for (int i = 0; i < numProcesses; i++)
            {
                if (!completed[i] && processes[i].arrivalTime <= currentTime)
                {
                    //Assiging a reference to current process
                    Process& currentProcess = processes[i];
                    //Calculations
                    currentProcess.startTime = max(currentTime, currentProcess.arrivalTime); //reason for max to ensure that a value does not fall below
                    currentProcess.finishTime = currentProcess.startTime + currentProcess.cpu;
                    currentProcess.turnaroundTime = currentProcess.finishTime - currentProcess.arrivalTime;
                    currentProcess.waitingTime = max(currentProcess.startTime - currentProcess.arrivalTime, 0);
                    //Update current time to finish time
                    currentTime = currentProcess.finishTime;
                    completed[i] = true;
                    ++completedProcesses;
                    turnaroundTimes[currentProcess.id - 1] = currentProcess.turnaroundTime;
                    waitingTimes[currentProcess.id - 1] = currentProcess.waitingTime;
                }
            }
            ++currentTime;
        }
    }
    else if (choice == 2) //Shorest Job Next 
    {
        //Sorting the processes based on arrival time
        sort(processes.begin(), processes.end(), compareCPUTime);
        while (completedProcesses < numProcesses) 
        {
            int shortestIndex = -1; //initialized the index to -1
            int shortestRemainingTime = numeric_limits<int>::max();//initialized to the max possible value

            for (int i = 0; i < numProcesses; ++i) 
            {
                if (!completed[i] && processes[i].arrivalTime <= currentTime && processes[i].remainingTime < shortestRemainingTime) 
                {
                    shortestIndex = i;
                    shortestRemainingTime = processes[i].remainingTime;
                }
            }

            if (shortestIndex != -1) 
            {
                //Assign a reference 
                Process& currentProcess = processes[shortestIndex];
                //Calculate
                currentProcess.startTime = max(currentTime, currentProcess.arrivalTime);
                currentProcess.finishTime = currentProcess.startTime + currentProcess.remainingTime;
                currentProcess.turnaroundTime = currentProcess.finishTime - currentProcess.arrivalTime;
                currentProcess.waitingTime = max(currentProcess.startTime - currentProcess.arrivalTime, 0);//max is used to make sure the outcome isn't negative
                //Update current time to finish time
                currentTime = currentProcess.finishTime;
                //Mark as complete
                completed[shortestIndex] = true;
                ++completedProcesses;
                //Update turn around and wait times
                turnaroundTimes[currentProcess.id - 1] = currentProcess.turnaroundTime;
                waitingTimes[currentProcess.id - 1] = currentProcess.waitingTime;
                currentProcess.remainingTime = 0; // Mark as completed for SJN
            }
            else 
            {
                ++currentTime;
            }
        }
    }
    else if (choice == 3)//Shorstest Remaining Time
    {
        while (completedProcesses < numProcesses) 
        {
            int shortestIndex = -1;
            int shortestRemainingTime = numeric_limits<int>::max();
            for (int i = 0; i < numProcesses; ++i) 
            {
                if (!completed[i] && processes[i].arrivalTime <= currentTime && processes[i].remainingTime < shortestRemainingTime) 
                {
                    shortestIndex = i;
                    shortestRemainingTime = processes[i].remainingTime;
                }
            }

            if (shortestIndex != -1) 
            {
                Process& currentProcess = processes[shortestIndex];
                //Calcualtions 
                currentProcess.startTime = max(currentTime, currentProcess.arrivalTime);
                currentProcess.finishTime = currentProcess.startTime + 1; // Execute one cycle
                currentProcess.turnaroundTime = currentProcess.finishTime - currentProcess.arrivalTime;
                currentProcess.waitingTime = max(currentProcess.startTime - currentProcess.arrivalTime, 0);
                //Update current to finish
                currentTime = currentProcess.finishTime;
                --currentProcess.remainingTime; // Decrement remaining time for SRT

                if (currentProcess.remainingTime == 0) 
                {
                    //Mark the current process as complete 
                    completed[shortestIndex] = true;
                    ++completedProcesses;
                    //Update
                    turnaroundTimes[currentProcess.id - 1] = currentProcess.turnaroundTime;
                    waitingTimes[currentProcess.id - 1] = currentProcess.waitingTime;
                }
            }
            else {
                ++currentTime;
            }
        }
    }
    else //Round Robin
    {
        //Ask the user to enter the quantum they would like applied
        int quantum;
        cout << "Enter quantum for RR: ";
        cin >> quantum;
        //Store prcess that is ready to run
        queue<Process> readyQueue;

        while (completedProcesses < numProcesses) 
        {
            for (int i = 0; i < numProcesses; ++i) 
            {
                if (!completed[i] && processes[i].arrivalTime <= currentTime) 
                {
                    readyQueue.push(processes[i]);
                    completed[i] = true;
                }
            }
            //Process each job in the ready queue 
            while (!readyQueue.empty()) 
            {
                //take the first process
                Process currentProcess = readyQueue.front();
                //remove from the ready queue
                readyQueue.pop();
                //Calculate 
                currentProcess.startTime = max(currentTime, currentProcess.arrivalTime);
                int executionTime = min(quantum, currentProcess.remainingTime);
                //Update the current by adding the execution time
                currentTime += executionTime;
                //Calculate
                currentProcess.finishTime = currentTime;
                currentProcess.turnaroundTime = currentProcess.finishTime - currentProcess.arrivalTime;
                currentProcess.waitingTime = max(currentProcess.startTime - currentProcess.arrivalTime, 0);//max is used to make sure its not negative
                //Update remaining time of the process by subtracting execution time
                currentProcess.remainingTime -= executionTime;
                //See if the process still has remaining time
                if (currentProcess.remainingTime > 0) 
                {
                    //If yes, push current process
                    readyQueue.push(currentProcess);
                }
                else 
                {
                    ++completedProcesses;
                    turnaroundTimes[currentProcess.id - 1] = currentProcess.turnaroundTime;
                    waitingTimes[currentProcess.id - 1] = currentProcess.waitingTime;
                }
            }
            if (readyQueue.empty()) 
            {
                ++currentTime;
            }
        }
    }

    // Display results of turn around and waiting times
    cout << "\nTurnaround time for each job:\n";
    for (int i = 0; i < numProcesses; ++i) 
    {
        cout << "Process " << processes[i].id << ": " << turnaroundTimes[i] << endl;
    }
    cout << "\nWaiting time for each job:\n";
    for (int i = 0; i < numProcesses; ++i) 
    {
        cout << "Process " << processes[i].id << ": " << waitingTimes[i] << endl;
    }

    //Calculate the averages, using float to get a more accurate average
    float avgTurnaroundTime = accumulate(turnaroundTimes.begin(), turnaroundTimes.end(), 0.0) / numProcesses;
    float avgWaitingTime = accumulate(waitingTimes.begin(), waitingTimes.end(), 0.0) / numProcesses;

    //Display the averages
    cout << "\nAverage Turnaround Time: " << avgTurnaroundTime << endl;
    cout << "Average Waiting Time: " << avgWaitingTime << endl;

    return 0;
}
