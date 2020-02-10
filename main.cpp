#include <math.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <utility> // std::pair
#include <stdexcept> // std::runtime_error
#include <sstream> // std::stringstream

using namespace std;

vector<vector<string> > parseCSV(string filename)
{
    ifstream data(filename);
    string line;
    vector<vector<string> > parsedCsv;
    while(getline(data,line))
    {
        stringstream lineStream(line);
        string cell;
        vector<string> parsedRow;
        while(getline(lineStream,cell,','))
        {
            parsedRow.push_back(cell);
        }

        parsedCsv.push_back(parsedRow);
    }
    return parsedCsv;
};

class tauState {
public:
    double tau_value;
    double s;
    vector<vector<string> > data;

    tauState(double tau, vector<vector<string> > dataSet){
        tau_value = tau;
        data = dataSet;
        s = calculateStatistic();
    }

    int getNumRows(bool includeTitles){
        if(includeTitles){
            return data[0].size();
        }else{
            return data[0].size() - 1;
        }
    }

    double getData(int row, int column, bool includeTitles){
        if(includeTitles){
            if(row==0){
                return -1;
            }else{
                return stod(data[row][column]);
            }
        }else{
            return stod(data[row+1][column]);
        }
    }

    double getThickness(int row){
        return getData(row, 0, false);
    }

    double getMeasured(int row){
        return getData(row, 1, false);
    }

    double getSigmaTrans(int row){
        return getData(row, 2, false);
    }

    double calculateStatistic(){
        double tau = tau_value;
        int point_num = getNumRows(false);
        double curr_error = 0;
        for(int i = 0; i < point_num; i++){
            double ev_trans = pow(0.5, getThickness(i)/tau);
            double error = ev_trans - getMeasured(i);
            double point_error = pow(error/getSigmaTrans(i), 2);
            curr_error += point_error;
        }
        return curr_error;
    }
};



int main(int argc, char** argv) {
    vector<vector<string> > dataSet = parseCSV("greenalum.csv");
    int accuracy_digit = 5;
    double curr_accuracy_digit = 1;
    double tau_init = 0.5;
    double curr_tau = tau_init;
    double run = true;
    while(run){
        double high_tau = curr_tau + pow(10, -1 * curr_accuracy_digit);
        double low_tau = curr_tau - pow(10, -1 * curr_accuracy_digit);
        tauState high_state = tauState(high_tau, dataSet);
        tauState low_state = tauState(low_tau, dataSet);
        tauState curr_state = tauState(curr_tau, dataSet);
        if(high_state.s < curr_state.s){
            curr_tau = high_tau;
        }else if(low_state.s < curr_state.s){
            curr_tau = low_tau;
        }else{
            if(curr_accuracy_digit == accuracy_digit){
                run = false;
            }else{
                curr_accuracy_digit += 1;
            }
        }
    }
    tauState final_state = tauState(curr_tau, dataSet);
    cout << "Tau: " << curr_tau;
    cout << "S-Statistic: " << final_state.s;
    return 0;
}