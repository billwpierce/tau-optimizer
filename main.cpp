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

/**
 * Parses a CSV file to an array.
 */
vector<vector<string> > parseCSV(string filename){
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

/**
 * Taustate is a class that, for convenience, stores the data that we're referencing (pulled elsewhere from the CSV file), the tau value that we are testing, and the corresponding s statistic. In addition, it checks whether or not the data contains headers (which is ugly, but the best solution I could come up with just to get the output desired).
 */
class tauState {
public:
    double tau_value;
    double s;
    bool defaultIncludeTitles;
    vector<vector<string> > data;

    /**
     * Taustate constructor
     */
    tauState(double tau, vector<vector<string> > dataSet, bool includeTitles){
        tau_value = tau;
        data = dataSet;
        defaultIncludeTitles = includeTitles;
        s = calculateStatistic(defaultIncludeTitles);
    }

    /**
     * Get the number of rows in the data
     */
    int getNumRows(bool includeTitles){
        if(includeTitles){
            return data.size();
        }else{
            return data.size() - 1;
        }
    }

    /**
     * Get specific data entries
     */
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

    /**
     * Get the thickness of a specific entry.
     */
    double getThickness(int row, bool includeTitles){
        return getData(row, 0, includeTitles);
    }

    /**
     * Get the measured transmission of a specific entry.
     */
    double getMeasured(int row, bool includeTitles){
        return getData(row, 1, includeTitles);
    }

    /**
     * Get the uncertainty of the transmission of a specific entry.
     */
    double getSigmaTrans(int row, bool includeTitles){
        return getData(row, 2, includeTitles);
    }

    /**
     * Calculate the s statistic for the passed-in tau parameter.
     */
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
    vector<vector<string> > dataSet = parseCSV("yellowplastic.csv"); // Records the data.
    int accuracy_digit = 3; // The accuracy to which we want to calculate (in number after decimal place).
    double curr_accuracy_digit = 0; // The starting accuracy digit.
    double tau_init = 0.5; // The initial tau value, though it doesn't really matter.
    double curr_tau = tau_init; // Set our current tau value to the initial one.
    bool doIncludeTitles = false; // Do we include the first row in our data analysis?
    bool run = true; // State variable, whether or not to keep running the loop.
    while(run){
        double high_tau = curr_tau + pow(10, -1 * curr_accuracy_digit); // Sets the upper tau value to test to one accuracy digit above the current.
        double low_tau = curr_tau - pow(10, -1 * curr_accuracy_digit);  // Sets the lower tau value to test to one accuracy digit below the current.
        tauState high_state = tauState(high_tau, dataSet, doIncludeTitles);
        tauState low_state = tauState(low_tau, dataSet, doIncludeTitles);
        tauState curr_state = tauState(curr_tau, dataSet, doIncludeTitles);
        // The following if/elseif/else statement adjusts our current tau value (what we assume to be closest) based on which has the lowest s-statistic.
        if(high_state.s < curr_state.s){
            curr_tau = high_tau;
        }else if(low_state.s < curr_state.s){
            curr_tau = low_tau;
        }else{
            if(curr_accuracy_digit == accuracy_digit){
                run = false; // Once we have reached a value that is more accurate that the ones around it, and we are satisfied with our accuracy, stop the loop.
            }else{
                curr_accuracy_digit += 1;
            }
        }
    }
    tauState final_state = tauState(curr_tau, dataSet, doIncludeTitles);
    // Print out our results.
    cout << "Tau: " << curr_tau << "\n";
    cout << "S-Statistic: " << final_state.s << "\n";
    return 0;
}