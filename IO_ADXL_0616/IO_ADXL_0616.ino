#include <Timer.h>

#define NUMBER 5
#define LED 7
#define X A1
#define Y A3
#define BRAKES 2

Timer t1;

float ADXLArray[NUMBER];
int Tire;
/////讀取ADXL數值/////
void ReadADXL(){
  for (int i=NUMBER-1; i>0; i--){
    ADXLArray[i] = ADXLArray[i-1];
  }
  ADXLArray[0] = analogRead(X);
}
/////ADXL平均值/////
float ADXLAvg(){
  float sum=0;
  for (int i=0; i<NUMBER; i++){
    sum += ADXLArray[i];
  }  
  return sum/NUMBER;
}
/////ADXL標準差/////
float ADXLVariance() {
  float sum;
  for(int i=0;i<NUMBER;i++){
    sum +=pow(ADXLArray[i]-ADXLAvg(),2);
  }
  return sqrt(sum);
}
/////數值輸出到螢幕/////
void List(){
  for (int i=0; i<NUMBER; i++){
    Serial.print(ADXLArray[i], 1);    
    Serial.print("\t");    
  }
   Serial.println();
   Serial.print("Average:");
   Serial.print(ADXLAvg());
   Serial.print("  Variance:");
   Serial.print(ADXLVariance());
   Serial.println();    
}

/////煞車控制/////
void Brakescontrol(){
  int Barkes =digitalRead(BRAKES);
  if(Barkes == LOW){
    ReadADXL();
    List();
  }
}
/////程式初始化/////
void setup(){
  Serial.begin(9600);
  pinMode(LED,OUTPUT);
  pinMode(X,INPUT);
  pinMode(Y,INPUT);
  pinMode(BRAKES,INPUT);
  for (int i=0; i<NUMBER; i++){
    ADXLArray[i] = analogRead(X);
  }
  t1.every(1.2,Brakescontrol);
}

void loop(){
  t1.update();
}
