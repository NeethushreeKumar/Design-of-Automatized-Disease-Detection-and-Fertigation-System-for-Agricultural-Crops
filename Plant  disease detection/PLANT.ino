#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <DHT.h>
#define    DHTPIN     2

DHT dht(DHTPIN, DHT11, 15);// Start DHT sensor

int   msec_cnt=0,CLR_CNT=0;
LiquidCrystal lcd(8,7,6,5,4,3);

const int NODE_D0_PIN=A0; 
const int NODE_D1_PIN=A1;
const int NODE_D2_PIN=A2;
const int NODE_D3_PIN=A3;
const int NODE_D4_PIN=A4;

const int N_SV_PIN=9;
const int P_SV_PIN=10;
const int K_SV_PIN=11;
const int PUMP_PIN=12;


boolean MODE_FLAG=0;
int MOTOR_ON_CNT=0,STAGE=0;
boolean BEEP_FLG=0,TOGGLE_FLG=0,MODE_FLG=0; 
int BEEP_CNT=0,LCD_CLR_CNT=0,TEN_MCNT=0;

char RX_DATA[10];
int K=0;

boolean N_SV_ON_FLG=0,P_SV_ON_FLG=0,K_SV_ON_FLG=0,PUMP_ON_FLG=0,PUMP1_ON_FLG=0;

boolean RX_FLG=0,C1_FLG=0;
int RX_COUNT=0;
int D=0,I=0;
int TEMP=0,HUMIDITY=0;
boolean Tenmec_Flg=0,DISP_TOGGGLE;

boolean LCD_CLR_FLG=0,NORMAL_FLG=0;
int NUM=0;int STAGE_CNT=0;


boolean  IP_PIN1_FLAG=0,IP_PIN2_FLAG=0,IP_PIN3_FLAG=0,IP_PIN4_FLAG=0,IP_PIN5_FLAG=0;

void setup(){
  Serial.begin(9600);
  
  dht.begin();
  pinMode(13, OUTPUT);   
  pinMode(N_SV_PIN,OUTPUT); 
  pinMode(P_SV_PIN,OUTPUT);
  pinMode(K_SV_PIN,OUTPUT); 
  pinMode(PUMP_PIN,OUTPUT);
   
  pinMode(NODE_D0_PIN,INPUT); 
  pinMode(NODE_D1_PIN,INPUT); 
  pinMode(NODE_D2_PIN,INPUT); 
  pinMode(NODE_D3_PIN,INPUT); 
  pinMode(NODE_D4_PIN,INPUT); 
          
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print(" PLANT DISEASE      ");
  lcd.setCursor(0,1);
  lcd.print("DETECTION SYSTEM      ");  delay(2000); lcd.clear();
}

 
void loop(){
  
  RecieveData();
  TEMP_SENSOR();timerIsr();
  LCD_INDICATION();
           
}




void LCD_INDICATION(void){
  
   if(LCD_CLR_FLG){ lcd.clear(); LCD_CLR_FLG=0; }

   if(DISP_TOGGGLE){
    lcd.setCursor(0,0);
    lcd.print(TEMP);
    lcd.print("\xDF\C ");
  }else{
    lcd.setCursor(0,0);
    lcd.print(HUMIDITY); 
    lcd.print("\x25\RH");  
  }
   if(MODE_FLAG==0){
       lcd.setCursor(6,0);
       lcd.print("M:AI "); 
      if(NORMAL_FLG){
         lcd.setCursor(0,1);
         lcd.print("LEAF IS HEALTHY    "); 
      }else if(STAGE==1){
          lcd.setCursor(0,1);
          lcd.print("NPK DISPENSING ");
      }else if(STAGE==2){
          lcd.setCursor(0,1);
          lcd.print("WATER DISPENSING ");
      }else{
           lcd.setCursor(0,1);
           lcd.print("WAIT.FR AI INPUT");
      }

      if(PUMP1_ON_FLG){
           lcd.setCursor(11,0);
           lcd.print("W:ON ");       
       }else{ 
           lcd.setCursor(11,0);
           lcd.print("W:OFF");
       }
       
  }else{
       
       lcd.setCursor(6,0);
       lcd.print("M:BL"); 
       
       if(N_SV_ON_FLG){
          lcd.setCursor(0,1);
          lcd.print("N:OFF");       
       }else{ lcd.setCursor(0,1);
              lcd.print("N:ON ");  
        }
        
       if(P_SV_ON_FLG){
          lcd.setCursor(6,1);
          lcd.print("P:OFF");   
       }else{ 
          lcd.setCursor(6,1);
          lcd.print("P:ON ");   
       }
        
       if(K_SV_ON_FLG){
          lcd.setCursor(12,1);
          lcd.print("K:OFF"); 
       }else{lcd.setCursor(12,1);
          lcd.print("K:ON "); 
      } 

      if(PUMP_ON_FLG){
           lcd.setCursor(11,0);
           lcd.print("W:OFF");       
       }else{ 
           lcd.setCursor(11,0);
           lcd.print("W:ON ");
       }
                               
   }   
}


void TEMP_SENSOR(void){ 
  if(Tenmec_Flg){

       TEMP = dht.readTemperature();
   HUMIDITY = dht.readHumidity();
       Tenmec_Flg=0;
   }
    
}

int N_COUNT=0;
void timerIsr()
{
    msec_cnt++;if(msec_cnt>=500){
          LCD_CLR_CNT++;if(LCD_CLR_CNT>=4){ LCD_CLR_FLG=1;  LCD_CLR_CNT=0;  DISP_TOGGGLE=!DISP_TOGGGLE; }
          TOGGLE_FLG=!TOGGLE_FLG;               
          CLR_CNT++;if(CLR_CNT>=6){ CLR_CNT=0; LCD_CLR_FLG=1;}
          digitalWrite(13, digitalRead( 13 ) ^ 1 ); 

          if(STAGE==1){
               STAGE_CNT++; if(STAGE_CNT>=120){ STAGE=2; STAGE_CNT=0; }
          }else if(STAGE==2){
               STAGE_CNT++; if(STAGE_CNT>=60){  STAGE=0; STAGE_CNT=0; }
          }
         if(NORMAL_FLG){ N_COUNT++; if(N_COUNT>10){ NORMAL_FLG=0; N_COUNT=0; }  } else{ N_COUNT=0; }
          msec_cnt=0;         
   }
   
      MOTOR_CONTROL();
      TEN_MCNT++;if(TEN_MCNT>=10){ 
               Tenmec_Flg=1;    TEN_MCNT=0; 
               IP_PIN_D0_SENSE(); IP_PIN_D1_SENSE(); IP_PIN_D2_SENSE(); IP_PIN_D3_SENSE(); IP_PIN_D4_SENSE();
       }
  

}


void MOTOR_CONTROL(void){
  
 if(MODE_FLAG==0){
      if(STAGE==1){
          digitalWrite(N_SV_PIN,LOW);  digitalWrite(P_SV_PIN,LOW);  digitalWrite(K_SV_PIN,LOW); 
      }else if(STAGE==2){
          digitalWrite(N_SV_PIN,HIGH);  digitalWrite(P_SV_PIN,HIGH);  digitalWrite(K_SV_PIN,HIGH); 
          digitalWrite(PUMP_PIN,LOW); PUMP1_ON_FLG=1;
      }else{
          PUMP1_ON_FLG=0;digitalWrite(PUMP_PIN,HIGH);digitalWrite(N_SV_PIN,HIGH);  digitalWrite(P_SV_PIN,HIGH);  
          digitalWrite(K_SV_PIN,HIGH); 
      }
  }else{
       if(N_SV_ON_FLG){
            digitalWrite(N_SV_PIN,HIGH);   
       }else{ digitalWrite(N_SV_PIN,LOW);  }
        
       if(P_SV_ON_FLG){
            digitalWrite(P_SV_PIN,HIGH);  digitalWrite(K_SV_PIN,HIGH);
       }else{  digitalWrite(P_SV_PIN,LOW);  }
        
       if(K_SV_ON_FLG){
          digitalWrite(P_SV_PIN,HIGH);  digitalWrite(K_SV_PIN,HIGH);
       }else{  digitalWrite(K_SV_PIN,LOW);  }

       if(PUMP_ON_FLG){
               digitalWrite(PUMP_PIN,HIGH);  
       }else{  digitalWrite(PUMP_PIN,LOW);  }
     
  }

}


void RecieveData(void)
{     
if(Serial.available()>0)
{ 
   RX_DATA[0] = Serial.read();
  
         if((RX_DATA[0]=='D')){ STAGE=1; NORMAL_FLG=0; } 
         if((RX_DATA[0]=='N')){ NORMAL_FLG=1;   }
           
}
}





void IP_PIN_D0_SENSE(void)//CALLING IN TIMER INT
{  
 if(digitalRead(NODE_D0_PIN))
  {      
    if(IP_PIN1_FLAG==0)       
   {
          MODE_FLAG=1;       
          IP_PIN1_FLAG=1; 
      
    } 
  }else{  IP_PIN1_FLAG=0;  MODE_FLAG=0; 
         
       }
}

  
void IP_PIN_D1_SENSE(void)//CALLING IN TIMER INT
{ 
 if(!digitalRead(NODE_D1_PIN))
  {      
   if(IP_PIN2_FLAG==0)      
   {
            N_SV_ON_FLG=1;   
          IP_PIN2_FLAG=1;  
      
    } 
   }else{ IP_PIN2_FLAG=0;  N_SV_ON_FLG=0; 
        
      }
}


   
void IP_PIN_D2_SENSE(void)//CALLING IN TIMER INT
{ 
 if(!digitalRead(NODE_D2_PIN))
  {      
   if(IP_PIN3_FLAG==0)      
   {
             P_SV_ON_FLG=1;
          IP_PIN3_FLAG=1;       
    } 
  }else{  IP_PIN3_FLAG=0; P_SV_ON_FLG=0; 
         
      }
}



void IP_PIN_D3_SENSE(void)//CALLING IN TIMER INT
{  
  if(!digitalRead(NODE_D3_PIN))
  {        
   if(IP_PIN4_FLAG==0)      
   {
      IP_PIN4_FLAG=1;       K_SV_ON_FLG=1;
      
    } 
  }else{   IP_PIN4_FLAG=0;    K_SV_ON_FLG=0;    
      
       }
}



void IP_PIN_D4_SENSE(void)//CALLING IN TIMER INT
{  
  if(!digitalRead(NODE_D4_PIN))
  {        
   if(IP_PIN5_FLAG==0)      
   {
       IP_PIN5_FLAG=1;    PUMP_ON_FLG=1;
      
    } 
  }else{    IP_PIN5_FLAG=0; PUMP_ON_FLG=0;
          
       }
}
 
