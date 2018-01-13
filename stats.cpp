#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <math.h>
#include <ctime>

using namespace std;

int trackingStats(const char inputName[], const char outputname[]);
float minimum(const float dataset[], const int size);
float average(const float dataset[], const int size);
float maximum(const float dataset[], const int size);
float popStdDev(const float dataset[], const int size);
bool selection(int dataset[], const int size);
float median(const float dataset[], const int size);

double degToRad(double deg) {
        double rad = deg * (3.141592653 / 180);
        return rad;
}

double getDistance(double lat1, double long1, double lat2, double long2) {
        double earthRadiusKm = 6371.0;
        double lat1rad = degToRad(lat1);
        double long1rad = degToRad(long1);
        double lat2rad = degToRad(lat2);
        double long2rad = degToRad(long2);
        double u = sin((lat2rad - lat1rad) / 2);
        double v = sin((long2rad - long1rad) / 2);
        return 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1rad) * cos(lat2rad) * v * v));
}

int trackingStats(const char inputName[], const char outputname[]){

        ifstream readfile;
        readfile.open(inputName);
        if(!readfile.is_open()) {
                return -1;
        }

        float longitudes[999] = {0};
        float latitudes[999] = {0};
        int entryNumber = 0;

        bool done = false;
        while(!done) { // LOOPS THROUGH THE FILE, FLIPPNG THROUGH EACH LINE
                char dataset[999] = {0}; //what the line is loaded into
                if(!readfile.getline(dataset, 999)) { //loads line and checks for end of file
                        if(readfile.eof()) {
                                done = true;
                        } else {
                                return -1;
                        }
                }
////////////////////////////////////////////////// IF LINE HAS INFO, RECORD IT //////////////////////////////////////////////////
                if(dataset[0] == 'L') {
                        ////////// RECORDS LONGITUDE //////////
                        bool negative = false;
                        bool decimal = false;
                        int decimalCount = 0;
                        for(int i = 11; i < 19; i++) {  //loops through the section with longitude and record it (E/W)
                                if(('0' <= dataset[i]) && (dataset[i] <= '9')) { //checks for valid number
                                        if(decimal) { //if number is behind a decimal, them its a decimal number
                                                decimalCount++;
                                                longitudes[entryNumber] += (dataset[i]-48)/pow(10, decimalCount);

                                        } else { //if the number is before the decimal, then its a normal number
                                                longitudes[entryNumber] *= 10;
                                                longitudes[entryNumber] += dataset[i] - 48;
                                        }
                                } else if (dataset[i] == '.') { //checks for the decimal
                                        decimal = true;
                                } else if (dataset[i] == '-') { //checks for negative
                                        negative = true;
                                }
                        }
                        if(negative) {
                                longitudes[entryNumber] *= -1;
                        }

                        ////////// RECORDS LATITUDE //////////
                        negative = false;
                        decimal = false;
                        decimalCount = 0;
                        for(int i = 30; i < 39; i++) {  //loops through the section with latitude and record it (N/S)
                                if(('0' <= dataset[i]) && (dataset[i] <= '9')) { //checks for valid number
                                        if(decimal) { //if number is behind a decimal, them its a decimal number
                                                decimalCount++;
                                                latitudes[entryNumber] += (dataset[i]-48)/pow(10, decimalCount);

                                        } else { //if the number is before the decimal, then its a normal number
                                                latitudes[entryNumber] *= 10;
                                                latitudes[entryNumber] += dataset[i] - 48;
                                        }
                                } else if (dataset[i] == '.') { //checks for the decimal
                                        decimal = true;
                                } else if (dataset[i] == '-') { //checks for negative
                                        negative = true;
                                }
                        }
                        if(negative) {
                                latitudes[entryNumber] *= -1;
                        }

                        entryNumber++;
                }
////////////////////////////////////////////////// IF LINE HAS INFO, RECORD IT //////////////////////////////////////////////////

        }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// PARSING IS DONE ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        float latMin = minimum(latitudes, entryNumber);
        float longMin = minimum(longitudes, entryNumber);

        float latMax = maximum(latitudes, entryNumber);
        float longMax = maximum(longitudes, entryNumber);

        float latAverage = average(latitudes, entryNumber);
        float longAverage = average(longitudes, entryNumber);

        float latStd = popStdDev(latitudes, entryNumber);
        float longStd = popStdDev(longitudes, entryNumber);

        float latMedian = median(latitudes, entryNumber);
        float longMedian = median(longitudes, entryNumber);

        float distances[entryNumber] = {0};
        for(int i = 0; i < entryNumber; i++) {
                distances[i] = getDistance(43.471084, -80.539157, latitudes[i], longitudes[i]);
        }
        float averageDistance = average(distances, entryNumber);


        //grabs time: obtained from reference @ http://www.cplusplus.com/reference/ctime/localtime/
        time_t rawtime;
        struct tm * timeinfo;
        time (&rawtime);
        timeinfo = localtime (&rawtime);
        //Open file for appending
        ofstream outfile;
        outfile.open("stats.txt");
        if(!outfile.is_open()) {
                return -1;
        }
        //Append coordinates on a new line
        outfile << "Stats at: " << asctime(timeinfo);
        // outfile << "Current Longitude: " << longitudes[entryNumber-1] << "  Current Latitude: " << latitudes[entryNumber-1] <<  endl;
        outfile << "Max Long: " << longMax << "  Max Lat: " << latMax << endl;
        outfile << "Min Long: " << longMin << "  Min Lat: " << latMin << endl;
        outfile << "Avg Long: " << longAverage << "  Avg lat: " << latAverage << endl;
        outfile << "Med Long: " << longMedian << "  Med lat: " << latMedian << endl;
        outfile << "Average Distance: " << averageDistance << endl;

        outfile.close();
        return 0;

}

float minimum(const float dataset[], const int size) {
        float min = dataset[0];
        for(int i = 0; i < size; i++) {
                if(dataset[i] < min) {
                        min = dataset[i];
                }
        }
        return min;
}

float average(const float dataset[], const int size) {
        float sum = 0;
        for(int i = 0; i < size; i++) {
                sum += dataset[i];
        }
        float avg = sum/size;
        return avg;
}

float maximum(const float dataset[], const int size) {
        float max = dataset[0];
        for(int i = 0; i < size; i++) {
                if(max < dataset[i]) {
                        max = dataset[i];
                }
        }
        return max;
}

float popStdDev(const float dataset[], const int size) {
        float diff_sum = 0;
        for(int i = 0; i < size; i++) {
                diff_sum += (dataset[i] - average(dataset, size))*(dataset[i] - average(dataset, size));
        }
        return sqrt(diff_sum/size);
}


bool selection(float dataset[], const int size){
        for(int i = 0; i < size; i++) {
                int index_min = i;
                for(int j = i; j < size; j++) {
                        if(dataset[j] < dataset[index_min]) {
                                index_min = j;
                        }
                }
                float temp = dataset[i];
                dataset[i] = dataset[index_min];
                dataset[index_min] = temp;
        }
        return true;
}

float median(const float dataset[], const int size) {
        float temp[size];
        for(int i = 0; i < size; i++) {
                temp[i] = dataset[i];
        }
        selection(temp, size);
        if(size % 2) {
                return temp[size/2];
        } else {
                return (temp[(size-1)/2] + temp[(size+1)/2])/2;
        }
}


int main(int argc, char * argv[]) {
        trackingStats("log.txt", "haha");
        return 0;
}
