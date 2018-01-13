#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <ctime>
#include <ugpio/ugpio.h>

using namespace std;

int ledR = 18;
int ledG = 19;


using namespace std;


float minimum(const float dataset[], const int size);
float average(const float dataset[], const int size);
float maximum(const float dataset[], const int size);
float popStdDev(const float dataset[], const int size);
bool selection(int dataset[], const int size);
float median(const float dataset[], const int size);
double getDistance(double lat1, double long1, double lat2, double long2);


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
                        for(int i = 10; i < 20; i++) {  //loops through the section with longitude and record it (E/W)
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
                        for(int i = 29; i < 40; i++) {  //loops through the section with latitude and record it (N/S)
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
        outfile.open(outputname);
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



//LED Alert for the wire, blink for 2s
void LEDAlert(int value) {
        if (value == 1) {
                for (int x = 0; x < 10; x++) {
                        gpio_set_value(ledR, 1);
                        gpio_set_value(ledG, 1);
                        usleep(50000);
                        gpio_set_value(ledR, 0);
                        gpio_set_value(ledG, 0);
                        usleep(50000);
                }
        } else if (value == 0) {
                gpio_set_value(ledR, 0);
                gpio_set_value(ledG, 0);
        } else {
                gpio_set_value(ledR, 0);
                gpio_set_value(ledG, 0);
        }
}

//Main LED Function
int LED(char color, int value, int blink) {
        if (blink == 0) {
                //green LED, switch on once
                if (color == 'g' || color == 'G') {
                        if (value == 0) {
                                gpio_set_value(ledG, 0);
                        } else if (value == 1) {
                                gpio_set_value(ledG, 1);
                        }
                }

                //red LED, switch on once
                if (color == 'r' || color == 'R') {
                        if (value == 0) {
                                gpio_set_value(ledR, 0);
                        } else if (value == 1) {
                                gpio_set_value(ledR, 1);
                        }
                }
        } else if (blink == 1) {
                //green LED, blink
                if (color == 'g' || color == 'G') {
                        if (value == 0) {
                                for (int x = 0; x < 4; x++) {
                                        gpio_set_value(ledG, 1);
                                        usleep(250000);
                                        gpio_set_value(ledG, 0);
                                        usleep(250000);
                                }
                        } else if (value == 1) {
                                for (int x = 0; x < 4; x++) {
                                        gpio_set_value(ledG, 1);
                                        usleep(250000);
                                        gpio_set_value(ledG, 0);
                                        usleep(250000);
                                }
                                gpio_set_value(ledG, 1);
                        } else {
                                //any other input for led
                                gpio_set_value(ledG, 0);
                        }
                }

                //red LED
                if (color == 'r' || color == 'R') {
                        if (value == 0) {
                                for (int x = 0; x < 4; x++) {
                                        gpio_set_value(ledR, 1);
                                        usleep(250000);
                                        gpio_set_value(ledR, 0);
                                        usleep(250000);
                                }

                        } else if (value == 1) {
                                for (int x = 0; x < 4; x++) {
                                        gpio_set_value(ledR, 1);
                                        usleep(250000);
                                        gpio_set_value(ledR, 0);
                                        usleep(250000);
                                }
                                gpio_set_value(ledR, 1);
                        }
                } else {
                        //any other input for led
                        gpio_set_value(ledR, 0);
                }
        }
        return 0;
}

//checks if wire is connected, returns true if it is, false if it isn't
bool isWireConnected(const int gpio) {
        int input = gpio_get_value(gpio);
        if (input == 0) {
                return false;
        }
        return true;
}

//GPIO SETUP; io = 1 for input; io = 0 for output
bool gpioSetup(const int gpio, const int io) {
        int isRequested;
        int returnValue;

        // CHECK IF GPIO IS ALREADY EXPORTED
        isRequested = gpio_is_requested(gpio);
        if (isRequested < 0) {
                return false; //error occured when checking if it was requested; return error
        }

        // EXPORT GPIO
        if (!isRequested) { //if its not already exported, export it
                returnValue = gpio_request(gpio, NULL);
                if (returnValue < 0) { //if request doesn't go through return error
                        return false;
                }
        }

        if (io == 1) { // SET GPIO TO INPUT
                gpio_direction_input(gpio);
        } else if (io == 0) { // SET GPIO TO OUTPUT
                gpio_direction_output(gpio, 0);
        }
        return true;
}

double degToRad(double deg) {
        double rad = deg * (3.141592653 / 180);
        return rad;
}

//Haversine formula to calculate the distance between two latitude and longitudes
//https://stackoverflow.com/a/10205532
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

//Generates a random latitude within a given radius
//Procedure from https://gis.stackexchange.com/a/25883
double randLat(double centerLat, double r) {
        double centerLatRad = degToRad(centerLat);
        //Generate two random values between 0 and 1
        double u = ((double) rand() / (RAND_MAX));
        double v = ((double) rand() / (RAND_MAX));
        //Convert radius from degrees to km
        r = r / 111.300;
        double w = r * sqrt(u);
        double t = 2.0 * 3.141592653 * v;
        double x = w * cos(t);
        double y = w * sin(t);

        //Adjust the x-coordinate for the shrinking of the east-west distances:
        x = x / cos(centerLatRad);
        double newLat = y + centerLat;
        return newLat;
}

//Generates a random longitude within a given radius
//Procedure from https://gis.stackexchange.com/a/25883
double randLong(double centerLong, double r) {
        //Generate two random values between 0 and 1
        double u = ((double) rand() / (RAND_MAX));
        double v = ((double) rand() / (RAND_MAX));
        //Convert radius from degrees to km
        r = r / 111.300;
        double w = r * sqrt(u);
        double t = 2.0 * 3.141592653 * v;
        double x = w * cos(t);
        double y = w * sin(t);

        double newLong = x + centerLong;
        return newLong;
}

//misc
double moveLat(double curLat, double endLat) {
        if (endLat > curLat) {
                curLat += 0.00001;
                return curLat;
        }
        if (endLat < curLat) {
                curLat -= 0.00001;
                return curLat;
        } else {
                return endLat;
        }
}

//misc
double moveLong(double curLong, double endLong) {
        if (endLong > curLong) {
                curLong += 0.00001;
                return curLong;
        }
        if (endLong < curLong) {
                curLong -= 0.00001;
                return curLong;
        } else {
                return endLong;
        }
}

int logCoordinates(const char filename[], const double longitude, const double latitude, const char comment[]) {
        //grabs time: obtained from reference @ http://www.cplusplus.com/reference/ctime/localtime/
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        double distance = getDistance(43.471084, -80.539157, latitude, longitude);

        //Open file for appending
        ofstream outfile;
        outfile.open(filename, fstream::app);
        if (!outfile.is_open()) {
                return -1;
        }

        //Append coordinates on a new line
        outfile << asctime(timeinfo) << "Longitude: " << longitude << "   Latitude: " << latitude
             << "   Distance from center: " << distance
             << "     Status: "<< comment << endl << endl;
        cout << asctime(timeinfo) << "Longitude: " << longitude << "   Latitude: " << latitude
             << "   Distance from center: " << distance
             << "     Status: "<< comment << endl << endl;

        outfile.close();
        return 0;
}

int main() {
///////////////////////// GPIO SETUP /////////////////////////
        //led gpios
        gpioSetup(ledR, 0);
        gpioSetup(ledG, 0);

        //wire gpios
        int tamperDetectionGPIO = 11;
        gpioSetup(tamperDetectionGPIO, 1);


///////////////////////// VARIABLE SETUP /////////////////////////
        double curLat = 0;
        double curLong = 0;
        bool tamper = false;

///////////////////////// LOOP /////////////////////////
        bool done = false;
        while (!done) {
                sleep(2);

                curLat = randLat(43.471084, 0.100);
                curLong = randLong(-80.539157, 0.100);
                double distance = getDistance(43.471084, -80.539157, curLat, curLong);


                //If wire is tampered with, LEDAlert
                if (!isWireConnected(tamperDetectionGPIO)) {
                        tamper = true;
                        done = true;

                } else if (distance > 0.070) { //If person is past 70m from the starting point, LEDAlert
                        logCoordinates("log.txt", curLong, curLat, "WARNING, OUT OF BOUNDRY");
                        trackingStats("log.txt", "stats.txt");
                        LEDAlert(1);

                } else if ((distance < 0.070) && (distance >= 0.065)) { //If person is within the caution zone (5m of the 70m fence), blink red LED
                        logCoordinates("log.txt", curLong, curLat, "WARNING, APPROACHING BOUNDRY");
                        trackingStats("log.txt", "stats.txt");
                        LED('R', 0, 1);

                } else if (distance < 0.065) {  //If person is within the safe zone (less than 65m from the starting point), blink green LED
                        logCoordinates("log.txt", curLong, curLat, "Normal");
                        trackingStats("log.txt", "stats.txt");
                        LED('G', 0, 1);
                }

        }



///////////////////////// Wire Broken Loop /////////////////////////
        while (tamper) {
                logCoordinates("log.txt", curLong, curLat, "WARNING, DISCONNECTED");
                trackingStats("log.txt", "stats.txt");
                LEDAlert(1);
        }



        return 0;
}
