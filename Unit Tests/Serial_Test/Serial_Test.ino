/*
 * HASP Serial Communicationm Unit test.
 * 
 * This code should be run on a seperate microcontroller (Arduino) and then be connected
 * via serial connection to the hasp payload.
 * 
 * This will simulate reading data and sending commands to the HASP payload.
 */


// define commands
#define C1 0xAA
#define C2 0xBB
#define C3 0xCC
#define CS 0x1C

int i = 1;
int Idata [54]; // 54 bytes of data (int)
char Cdata [54]; // 54 bytes of data (char)

void setup() 
{
  Serial.begin(1200); // initialize serial connection to HASP baud rate
  delay(100);
}

void loop() 
{
  // read downlink data
  int k = 0;
  while(Serial.available() > 0 && k<54) // may need to make this stop after the 54 bytes have been read but idk
  {
    Idata[k] = Serial.read();
    k++;
  }
  // print downlink data to serial monitor
  for(int j=0;j<54;j++) // 54 bytes of data
  {
    Cdata[j] = Idata[j]; // convert int data to char 
                         // might be redundant, I can probably just say "Cdata = Serial.read();"
    Serial.print(Cdata[j]); 
  }
  Serial.println();
  
  // send 1 of 3 commands 
  if(i==1)
  {
    Serial.write(CS); // send checksum and command 1
    Serial.write(C1);
    Serial.println("Command 1 sent");
    i=2;
  }
  else if(i==2)
  {
    Serial.write(CS); // send checksum and command 2
    Serial.write(C2);
    Serial.println("Command 2 sent");
    i=3;  
  }
  else if(i==3)
  {
    Serial.write(CS); // send checksum and command 3
    Serial.write(C3);
    Serial.println("Command 3 sent");
    i=1;  
  }
  delay(1500);
  
}
