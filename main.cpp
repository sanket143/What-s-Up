#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>

using namespace std;

class Process{
    string name;
    int arrival_time;
    int service_time;
    int turn_around_time;
    int waiting_time;
    int finish_time;
    int remaining_time;
    int priority;
  
    public:
    Process(){
        name = "";
        arrival_time = 0;
        service_time = 0;
        priority = 0;
    }

    Process(string _name, int at, int st, int _priority){
        name = _name;
        arrival_time = at;
        service_time = st;
        priority = _priority;
        remaining_time = st;
    }

    void display(){
        cout << "ID: " << name << endl;
        cout << "AT: " << arrival_time << endl;
        cout << "ST: " << service_time << endl;
        cout << "PR: " << priority << endl;
    }

    // SETTERS
    void finish(int _fin){
        finish_time = _fin;
    }

    void waited(int _wait){
        waiting_time = _wait;
    }

    void turn_around(int _tat){
        turn_around_time = _tat;
    }

    void worked(){
        remaining_time -= 1;
    }

    // GETTERS
    string getID(){
        return name;
    }

    int getRemainingTime(){
        return remaining_time;
    }

    int getArrivalTime(){
        return arrival_time;
    }

    int getBurstTime(){
        return service_time;
    }

    int getFinishTime(){
        return finish_time;
    }
    
    int getTAT(){
        return turn_around_time;
    }

    int getWaitingTime(){
        return waiting_time;
    }

    int getPriority(){
        return priority;
    }
};

// Takes input from input.txt
vector<Process> input(){
    string line;
    vector<Process> arr;

    string id;
    int count, at, st, _priority;

    ifstream myfile ("input.txt");
    if(myfile.is_open()){
        freopen("input.txt", "r", stdin);
        count = 0;

        while (getline (myfile,line)){
            count++;
            cin >> id >> at >> st >> _priority;

            Process pr(id, at, st, _priority);
            arr.push_back(pr);
        }

        myfile.close();
        fclose(stdin);
    } else {
        cout << "Unable to open file";
    }

    return arr;
}

/*
* FCFS Scheduling
*/

void fcfs(){
    ofstream fcfsFile ("fcfs.txt");
    vector<Process> proc = input();
    vector<Process> fcfs_proc;

    int tick = 0;
    int time_passed = 0;
    float mean_wt, mean_tat;

    while(fcfs_proc.size() != proc.size()){
        for(auto i = proc.begin(); i != proc.end(); ++i){
            if(tick == (*i).getArrivalTime()){

                time_passed += (*i).getBurstTime();
                (*i).finish(time_passed);
                (*i).turn_around((*i).getFinishTime() - (*i).getArrivalTime());
                (*i).waited((*i).getTAT() - (*i).getBurstTime());

                fcfs_proc.push_back(*i);
            }
        }

        tick++;
    }

    mean_wt = fcfs_proc[0].getWaitingTime();
    mean_tat = fcfs_proc[0].getTAT();

    // Write Output
    for (auto i = fcfs_proc.begin(); i != fcfs_proc.end(); ++i){
        if (fcfsFile.is_open()){

            fcfsFile << (*i).getID() << " "
            << (*i).getWaitingTime() << " "
            << (*i).getFinishTime() << " "
            << (*i).getTAT() << "\n";

            mean_wt = (mean_wt + (*i).getWaitingTime()) / 2;
            mean_tat = (mean_tat + (*i).getTAT()) / 2;
        } else {
            cout << "Unable to open file";
        }
    }
    fcfsFile << "\nMean  WT: " << mean_wt << "\n"
    << "Mean TAT: " << mean_tat << "\n";

    fcfsFile.close();
}

/*
* SRTN Scheduling
*/

void srtn(){
    ofstream srtnFile ("srtn.txt");
    vector<Process> proc = input();
    vector<Process> srtn_proc;
    vector<Process> proc_buffer;

    int tick = 0;
    int remaining_time_1, remaining_time_2;
    float mean_wt, mean_tat;

    Process temp;

    while(srtn_proc.size() != proc.size()){
        for(auto i = proc.begin(); i != proc.end(); i++){
            if(tick == (*i).getArrivalTime()){
                proc_buffer.push_back(*i);
            }
        }

        for(int j = 0; j < proc_buffer.size(); j++){
            for(auto k = proc_buffer.begin(); k != proc_buffer.begin() + (proc_buffer.size() - j - 1); k++){
                if((*k).getRemainingTime() > (*(k + 1)).getRemainingTime()){
                    temp = *k;
                    *k = *(k + 1);
                    *(k + 1) = temp;
                }
            }
        }

        tick++;
        proc_buffer[0].worked();
        if(proc_buffer[0].getRemainingTime() == 0){
            proc_buffer[0].finish(tick);
            proc_buffer[0].turn_around(proc_buffer[0].getFinishTime() - proc_buffer[0].getArrivalTime());
            proc_buffer[0].waited(proc_buffer[0].getTAT() - proc_buffer[0].getBurstTime());
            srtn_proc.push_back(proc_buffer[0]);
            proc_buffer.erase(proc_buffer.begin());
        }
    }

    mean_wt = srtn_proc[0].getWaitingTime();
    mean_tat = srtn_proc[0].getTAT();

    for (auto i = srtn_proc.begin(); i != srtn_proc.end(); ++i){
        if (srtnFile.is_open()){

            srtnFile << (*i).getID() << " "
            << (*i).getWaitingTime() << " "
            << (*i).getFinishTime() << " "
            << (*i).getTAT() << "\n";

            mean_wt = (mean_wt + (*i).getWaitingTime()) / 2;
            mean_tat = (mean_tat + (*i).getTAT()) / 2;
        } else {
            cout << "Unable to open file";
        }
    }

    srtnFile << "\nMean  WT: " << mean_wt << "\n"
    << "Mean TAT: " << mean_tat << "\n";

    srtnFile.close();
}

/*
* Priority 
*/

void priority(){
    ofstream priorityFile ("priority.txt");
    vector<Process> proc = input();
    vector<Process> prior_proc;
    vector<Process> proc_buffer;

    int fin;
    int tick = 0;
    int finish_time_c = 0;
    float mean_wt, mean_tat;

    Process temp;
    
    while(prior_proc.size() != proc.size()){
        for(auto i = proc.begin(); i != proc.end(); i++){
            if(tick == (*i).getArrivalTime()){
                proc_buffer.push_back(*i);

                for(int j = 0; j < proc_buffer.size(); j++){
                    for(auto k = proc_buffer.begin(); k != proc_buffer.begin() + (proc_buffer.size() - j - 1); k++){
                        if((*k).getPriority() > (*(k + 1)).getPriority()){
                            temp = *k;
                            *k = *(k + 1);
                            *(k + 1) = temp;
                        }
                    }
                }

                fin = finish_time_c;
                for(auto j = proc_buffer.begin(); j != proc_buffer.end(); j++){
                    (*j).finish((*j).getBurstTime() + fin);
                    (*j).turn_around((*j).getFinishTime() - (*j).getArrivalTime());
                    (*j).waited((*j).getTAT() - (*j).getBurstTime());
                    fin = (*j).getFinishTime();
                }
            }
        }

        if(tick >= finish_time_c && proc_buffer.size()){
            finish_time_c = proc_buffer[0].getFinishTime();
            prior_proc.push_back(proc_buffer[0]);
            proc_buffer.erase(proc_buffer.begin());
        }

        tick++;
    }

    mean_wt = prior_proc[0].getWaitingTime();
    mean_tat = prior_proc[0].getTAT();

    // Write Output
    for (auto i = prior_proc.begin(); i != prior_proc.end(); ++i){
        if (priorityFile.is_open()){

            priorityFile << (*i).getID() << " "
            << (*i).getWaitingTime() << " "
            << (*i).getFinishTime() << " "
            << (*i).getTAT() << "\n";

            mean_wt = (mean_wt + (*i).getWaitingTime()) / 2;
            mean_tat = (mean_tat + (*i).getTAT()) / 2;
        } else {
            cout << "Unable to open file";
        }
    }

    priorityFile << "\nMean  WT: " << mean_wt << "\n"
    << "Mean TAT: " << mean_tat << "\n";

    priorityFile.close();
}

/*
* HRRN 
*/

void hrrn(){
    ofstream hrrnFile ("hrrn.txt");
    vector<Process> proc = input();
    vector<Process> hrrn_proc, proc_buffer;

    int fin;
    int tick = 0;
    int finish_time_c = 0;
    float response_ratio_1, waiting_time_1;
    float response_ratio_2, waiting_time_2;
    float mean_wt, mean_tat;

    Process temp;

    while(hrrn_proc.size() != proc.size()){
        for(auto i = proc.begin(); i != proc.end(); i++){
            if(tick == (*i).getArrivalTime()){
                proc_buffer.push_back(*i);
            }

            for(int j = 0; j < proc_buffer.size(); j++){
                for(auto k = proc_buffer.begin(); k != proc_buffer.begin() + (proc_buffer.size() - j - 1); k++){
                    waiting_time_1 = tick - (*k).getArrivalTime();
                    waiting_time_2 = tick - (*(k + 1)).getArrivalTime();

                    response_ratio_1 = 1.0 + (waiting_time_1 / (*k).getBurstTime());
                    response_ratio_2 = 1.0 + (waiting_time_2 / (*(k + 1)).getBurstTime());

                    if(response_ratio_1 < response_ratio_2){
                        temp = *k;
                        *k = *(k + 1);
                        *(k + 1) = temp;
                    }
                }
            }

            fin = finish_time_c;
            for(auto j = proc_buffer.begin(); j != proc_buffer.end(); j++){
                (*j).finish((*j).getBurstTime() + fin);
                (*j).turn_around((*j).getFinishTime() - (*j).getArrivalTime());
                (*j).waited((*j).getTAT() - (*j).getBurstTime());
                fin = (*j).getFinishTime();
            }

        }

        if(tick >= finish_time_c && proc_buffer.size()){
            finish_time_c = proc_buffer[0].getFinishTime();
            hrrn_proc.push_back(proc_buffer[0]);
            proc_buffer.erase(proc_buffer.begin());
        }
        tick++;
    }

    mean_wt = hrrn_proc[0].getWaitingTime();
    mean_tat = hrrn_proc[0].getTAT();

    // Write Output
    for (auto i = hrrn_proc.begin(); i != hrrn_proc.end(); ++i){
        if (hrrnFile.is_open()){

            hrrnFile << (*i).getID() << " "
            << (*i).getWaitingTime() << " "
            << (*i).getFinishTime() << " "
            << (*i).getTAT() << "\n";

            mean_wt = (mean_wt + (*i).getWaitingTime()) / 2;
            mean_tat = (mean_tat + (*i).getTAT()) / 2;
        } else {
            cout << "Unable to open file";
        }
    }

    hrrnFile << "\nMean  WT: " << mean_wt << "\n"
    << "Mean TAT: " << mean_tat << "\n";

    hrrnFile.close();
}

int main(){
    if(!fork()){
        fcfs();
        exit(1);
    }

    if(!fork()){
        srtn();
        exit(1);
    }

    if(!fork()){
        priority();
        exit(1);
    }

    if(!fork()){
        hrrn();
        exit(1);
    }

    return 0;
}
