//HASP Serial Data Parser
//HASP 2019 Flight

//Written Summer 2019

/*This code is utilize to parse the data string file recieved from HASP into
a CSV format that can be opened on Excel. In order to parse the data, it is
recommended that the .raw file supplied by HASP is opened using Microsoft Word
or its equivalent and saved as a .dat file. However, this code should work
for any file type that can be read by C++.*/

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

#define DATA_STRING 56

using namespace std;

int main(){
	string datafile,logfile;												//Variables for HASP Serial
	ifstream data;
	ofstream file;
	char testByte1, testByte2, dataChar[DATA_STRING], endChar;				//Raw Data
	bool goodString = false, end = false;
	int fn = 1;

	file.open("data.csv");													//Logging File Setup
	if (file.fail()){
		cout<< "Error opening file!";
		exit(1);
	}
    cout << "logging file opened!" << endl;
    
    file<<"Time,Latitude,Longitude,Altitude,Temp 1,Temp N3,Temp PCB,PMS Bin,Flight State,OPC State"<<endl;

	cout << "header written!" << endl;									

while (!end){	
	cout<< "Enter data file name: ";							        	//Data File Setup
	cin>> datafile;
	cout<< "Name recieved:" << datafile << endl;

	data.open(datafile.c_str());
	if (data.fail()){
		cout<< "Error opening data!";
		exit(1);
	}
	cout << "data file opened!" << endl;
	file << "file number: " << fn << endl;
	fn++;

	while (!data.eof()){	
		while ((!goodString)&&(!data.eof())){								//This detects the beginning of the string
			data>>testByte1;
			if (testByte1 == 'U')
			{
				data>>testByte2;
				if (testByte2 == 'M') goodString = true;
			}
		}
	
		for (int i = 0; i<DATA_STRING; i++){								//Once a string is detected, each byte is loaded into an array
			data>>dataChar[i];
		}
	
		string time = "";													//Set variables to empty
		string latitude = "";
		string longitude = "";
		string altitude = "";
		string t1 = "";
		string t2 = "";
		string t3 = "";
		string pms = "";
		char OPC = '0';
		char flight = '0';
	
		if (dataChar[52] == '1'){											//Set the latitude and longitude signs
			latitude = "-";
		} else {
			latitude = "";
		}
		
		if (dataChar[53] == '1'){
			longitude = "-";
		} else {
			longitude = "";
		}

		for (int i = 0; i<8; i++) time += dataChar[i];						//Save the data to each variable
		for (int i = 8; i<13; i++) latitude += dataChar[i];					
		for (int i = 13; i<19; i++) longitude += dataChar[i];
		for (int i = 19; i<28; i++) altitude += dataChar[i];
		for (int i = 28; i<34; i++) t1 += dataChar[i];
		for (int i = 34; i<40; i++) t2 += dataChar[i];
		for (int i = 40; i<46; i++) t3 += dataChar[i];
		for (int i = 46; i<52; i++){
			if (dataChar[i] == '#') dataChar[i] = '0';
			pms += dataChar[i];
		}
		flight = dataChar[54];
		OPC = dataChar[55];
	
		goodString = false;
	
		file<<time<<','<<latitude<<','<<longitude<<','<<altitude<<','<<t1;  //save the data in CSV format
		file<<','<<t2<<','<<t3<<','<<pms<<','<<flight<<','<<OPC<<endl;	
	
	}
	
	cout << endl << "exited loop!" << endl;
	data.close();
	
	cout << "Do you want to convert another data file?" << endl;
	cout << "Press \"y\" to insert another file or \"n\" to end the program: ";
	cin >> endChar;
	if (endChar != 'y') end = true;
}
	
	file.close();
	cout << "Data parse complete!" << endl;
	
	return 0;
}
	
