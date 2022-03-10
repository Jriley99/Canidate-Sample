//Candiate Sample
// Jacob Riley

#include <iostream>
#include <filesystem>
#include<fstream>
#include <direct.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;


ifstream infile; // holds the input file

string checkversion() {
    //Checks whether the file is going to be ascii or binary, and returns that information
    string vers; //Holds the command line for either ascii or binary
    string output; // Used to return if file is ascii or binary

    while (vers != "$$ASCII" && vers != "$$BINARY") {// reads until it reaches whether file is ascii or binary
        infile >> vers;
    }
    if (vers == "$$ASCII") { //Checks if command is saying it's an ascii file
        output = "ascii";
    }
    else {
        output = "binary";
    }
    return output;
}


string parsecommands(string commandline, string* b, string delimiter) {
    //Has 3 parameters being the entire command line, a pointer to command a string that will hold
    // the parsed command, and the string of where it will parse. 
    //Function is used to parse a command line where the command is seperated from the rest of the data
    size_t pos = 0;
    string command;
    
    //cout << "in parse func" << "\n";
    while ((pos = commandline.find(delimiter)) != string::npos) { // Parses of the command
        command = commandline.substr(0, pos);
        //cout << command << "\n";
        commandline.erase(0, pos + delimiter.length());
    }
    //cout << commandline << " " << command << "\n";
    *b = command;   //puts the parsed command into the pointer
    return commandline; //returns the rest of the command lines information
}

vector<int> parseints(string data) {
    //Takes the data string of the command line.
    //Function seperates the string, and change the string values into intergers which are stored
    //in a vector. This vector of ints is returned ot function call
    size_t pos = 0;
    string svalue;
    string delimiter = ",";

    //cout << "in parsecoords func" << data << "\n";
    vector<int> coordinates;
    int i = 0;
    while ((pos = data.find(delimiter)) != string::npos) { //Parses string at commas to seperate ints
        svalue = data.substr(0, pos);                       //and stores them in vector
        data.erase(0, pos + delimiter.length());
        //cout << svalue << endl;
        int temp = stoi(svalue);
        coordinates.push_back(temp);
        
    }
    return coordinates;
}

double polyarea(vector<int> data, vector<int> dimension) {
    //Takes two parameters one a vector of the data from a polyline command, and the second being
    // a vector of the dimensions of the project.
    // Functions is used to calculate the layer part area of a layer
    //calculate area of layer
    if (data[1] == 0) { //clockwise polyline
        double a = 0, b = 0;
        for (int i = 3; i < data[2]*2; i = i + 4) {
            a += data[i] * data[i + 3]; //x1 * y2
            b += data[i+1] * data[i + 2]; //x2 * y1
        }
        int innerarea = abs((a - b) / 2) * (1/100);
        //cout << innerarea << endl;
        return innerarea;
    }
    else if (data[1] == 1) { //counter clockwise polyline
        double a = 0, b = 0;
        for (int i = 3; i < data[2] * 2; i = i + 4) {
            a += data[i] * data[i + 3];
            b += data[i + 1] * data[i + 2];
        }
        int innerarea = abs((a - b) / 2) * (1/100); // area inside polyline
        int fullarea = (dimension[3] - dimension[0]) * (dimension[4] - dimension[1]); // area of dimesions
        int exteriorarea = fullarea - innerarea; // area of exterior of polyline
        //cout << fullarea << "  " << innerarea << "   " << exteriorarea << endl;
        return exteriorarea;
    }
}

void asciioutput(int index, float layerheight, double area) {
    //Takes three parameters being the Layer index value, the layer height, and the layer part area
    //Outputs the data for a layer
    layerheight = layerheight * 0.001;
    cout << "Layer Index:" << setw(5) << index << setw(15) << "Layer Height:" 
        << setw(7) << setprecision(3) << fixed << layerheight << setw(20) << 
        "Layer Part Area:" << setw(9) << area << endl;
}

void polyline(string values, vector<int> dimension, int layerh, int index) {
    //Takes four parameters. Takes the unparsed string of values from the polyline command. A vector
    // of the dimesions of the project. The height of the layer it is on, and the index of the current layer
    
    vector<int> coords = parseints(values); //Sends the values to get parsed
    double area = polyarea(coords,dimension); //Calculates the layer part area
    asciioutput(index, layerh, area);   //Sends data to output function
}


void asciifile() {
    //Function to go through an asciifile and get required information from different commands,
    // and call functions that are needed to execute commands

    string commandline; // holds the entire command line, or the data of the command line after being parsed
    string command; //Will hold the command
    int numberoflayers; //Total number of layers
    vector<int> dimensions; // Int vector that holds the dimensions of the project
    float layerheight = 0;  //Holds the current layer height
    int layerindex = 0;     //Holds the current layer index
    //cout << "in asciifile func" << "\n";
    while (!infile.eof()) { //Goes through entire input file
        infile >> commandline;
        commandline = parsecommands(commandline, &command, "/");
        //cout << commandline << " " << command << "\n";
        if (command == "$$DIMENSION") {     //Takes the dimensions of the project
            dimensions = parseints(commandline);
        }
        else if (command == "$$LAYERS") {   //Takes the total number of layers of the project
            numberoflayers = stoi(commandline);
        }
        else if(command == "$$POLYLINE") {  //Calls the polyline function to process polyline command
            polyline(commandline,dimensions,layerheight, layerindex);
            layerindex++;
        }
        else if (command == "$$LAYER") {    //Takes the current layer height
            layerheight = stof(commandline);
        }
    }
}

void binaryfile() {

}

int main()
{
    string filename;   //Holds inputed file name
    cout << "Enter file name: ";
    cin >> filename;
    infile.open(filename);
    if (infile.is_open()) { //checks if file is open
        cout << "open \n";
    }
    //box_cli_bin.cli
    //box_cli_ascii.cli
    //VulcanFormsSamplePart
    string filetype;
    filetype = checkversion();

    if (filetype == "ascii") { //Calls function that goes through asciifiles.
        asciifile();
    }
    else {
        //Binary file reading not complete
        string test;
        infile.close();
        infile.open(filename, ios::binary);
        if (infile.is_open()) {
            cout << "open \n";
        }
        while (!infile.eof()) {
            infile >> test;
            cout << test << endl;
        }
        binaryfile();
    }
    
    

    infile.close();

    return 0;
    
}
