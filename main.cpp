#include <math.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <utility> // std::pair
#include <stdexcept> // std::runtime_error
#include <sstream> // std::stringstream
#include <cstdlib>

using namespace std;

vector<vector<string> > parseCSV(string filename)
{
    std::ifstream data(filename);
    std::string line;
    std::vector<std::vector<std::string> > parsedCsv;
    while(std::getline(data,line))
    {
        std::stringstream lineStream(line);
        std::string cell;
        std::vector<std::string> parsedRow;
        while(std::getline(lineStream,cell,','))
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
    bool defaultIncludeTitles;
    vector<vector<string> > data;

    // int getNumRows(bool includeTitles); double getData(int row, int column, bool includeTitles); double getThickness(int row); double getMeasured(int row); double getSigmaTrans(int row); double calculateStatistic();

    tauState(double tau, vector<vector<string> > dataSet, bool includeTitles){
        tau_value = tau;
        data = dataSet;
        defaultIncludeTitles = includeTitles;
        s = calculateStatistic(defaultIncludeTitles);
    }

    int getNumRows(bool includeTitles){
        if(includeTitles){
            return data.size();
        }else{
            return data.size() - 1;
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

    double getThickness(int row, bool includeTitles){
        return getData(row, 0, includeTitles);
    }

    double getMeasured(int row, bool includeTitles){
        return getData(row, 1, includeTitles);
    }

    double getSigmaTrans(int row, bool includeTitles){
        return getData(row, 2, includeTitles);
    }

    double calculateStatistic(bool includeTitles){
        double tau = tau_value;
        int point_num = getNumRows(includeTitles);
        double curr_error = 0;
        int ioff = 0;
        for(int i = ioff; i < point_num + ioff; i++){
            double ev_trans = pow(0.5, getThickness(i, includeTitles)/tau);
            double error = ev_trans - getMeasured(i, includeTitles);
            double sigmaTrans = getSigmaTrans(i, includeTitles);
            double point_error;
            if(sigmaTrans == 0){
                point_error = 0;
            }else{
                point_error = pow(error/sigmaTrans, 2);
            }
            curr_error += point_error;
        }
        return curr_error;
    }
};



int main(int argc, char** argv) {
    vector<vector<string> > dataSet = parseCSV("greenplastic.csv");
    int accuracy_digit = 3;
    double curr_accuracy_digit = 1;
    double tau_init = 0.5;
    double curr_tau = tau_init;
    bool doIncludeTitles = false;
    bool run = true;
    while(run){
        double high_tau = curr_tau + pow(10, -1 * curr_accuracy_digit);
        double low_tau = curr_tau - pow(10, -1 * curr_accuracy_digit);
        tauState high_state = tauState(high_tau, dataSet, doIncludeTitles);
        tauState low_state = tauState(low_tau, dataSet, doIncludeTitles);
        tauState curr_state = tauState(curr_tau, dataSet, doIncludeTitles);
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
    tauState final_state = tauState(curr_tau, dataSet, doIncludeTitles);
    cout << "Tau: " << curr_tau << "\n";
    cout << "S-Statistic: " << final_state.s << "\n";
    return 0;
}