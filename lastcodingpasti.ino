#include <SoftwareSerial.h>
SoftwareSerial gsm(3,2);
String destinationNumber="+60109117540";
int phread = A0;
#define Offset 0.00            //deviation compensate
#define samplingInterval 20
#define printInterval 800
#define smsInterval 120000
#define printGSM 8000000
#define ArrayLenth  40      //times of collection
int pHArray[ArrayLenth];    //Store the average value of the sensor feedback
int pHArrayIndex=0;
int smsCtr=0;
//int smsInterval = 30;
void setup(){
  Serial.begin(9600);   // initialize serial communication at 9600 bits per second:
  gsm.begin (4800);
  pinMode(phread, INPUT);   // pin an input:

}


//=============
void loop(void)
{
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue,voltage;
  char last_status; //0=normal, 1=asid, 2-alkaline
  String msg;
  long lastSMStime;
  
  if(millis()-samplingTime > samplingInterval)
  {
      pHArray[pHArrayIndex++]=analogRead(phread);
      if(pHArrayIndex==ArrayLenth)
  pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*(5.0/1024);
      pHValue = 3.5*voltage+Offset;
      samplingTime=millis();
      
  }
 
//Every 800 milliseconds, print a numerical, 
  if(millis() - printTime > printInterval)   
  {
    Serial.println("");
    Serial.print("PH water. ph value :");
    Serial.println(pHValue,2);
  
    printTime=millis();

    if(smsCtr<1)
    {
      if(!((pHValue>6)&&(pHValue<8)))
      {
        msg = (pHValue<6)?"AQUARIUM 1 WARNING! PH less than 7. Latest PH :" : "AQUARIUM 1 WARNING! PH over 8. Latest PH :";
        msg += pHValue;
        digitalWrite(13, HIGH);
      

        
        //msg = ssmsg.s;
        sendSMS(msg);
        Serial.println("");
          Serial.print("message sent!");
          Serial.println("");
          Serial.println("");
        lastSMStime = millis();
       delay(2000);
      } 
      }
    }
    
       if((smsCtr < 2) && (millis() - lastSMStime >120 000)) //1minit=60 000 milliseconds
          {
            msg = (pHValue<6)?"AQUARIUM 1 WARNING! PH less than 7. Latest PH :" : "AQUARIUM 1 WARNING! PH over 8. Latest PH :";
        msg += pHValue;
            sendSMS(msg);
            Serial.println("");
          Serial.print("message second sent!");
          digitalWrite(13, HIGH);
          Serial.println("");
          }
    }// end of loop smsCtr
   
  }// end of function



double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
                  for(i=0;i<number;i++)
                      {
                         amount+=arr[i];
                      }
               avg = amount/number;
               return avg;
              }

              
  else{
      if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
       }//if
  return avg;
}


void sendSMS(String msg)
{

  //gsm.begin (4800);
  gsm.print("AT+CMGF=l\r");
  delay(1000);
  gsm.println("AT+CMGS=\"" + destinationNumber+"\"");
  delay(1000);
  gsm.print(msg);
  delay(1000);
  gsm.write((char)26);

  smsCtr++;

}
