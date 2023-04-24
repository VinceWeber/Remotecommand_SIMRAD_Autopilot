/*
  Simple example for receiving
  
  https://github.com/sui77/rc-switch/
*/

/* 
 *  Bouton 1 = Received 11377576 / 24bit Protocol: 1
 *  Bouton 2 = Received 11377572 / 24bit Protocol: 1
 *  Bouton 3 = Received 11377580 / 24bit Protocol: 1
 *  Bouton 4 = Received 11377570 / 24bit Protocol: 1
 *  Bouton 5 = Received 11377578 / 24bit Protocol: 1
 *  Bouton 6 = Received 11377574 / 24bit Protocol: 1
 *  
 *  Action 1 = Presse bouton "Mode"
 *  Action 2 = Presse bouton "Standby" = Babord
 *  Action 3 = Presse bouton "Port" = Tribord
 *  Action 4 = +10°
 *  Action 5 = -10°
 *  Action 6 = +80 ° (Tack en mode vent apparent)
 *  Action 7 = -80 ° (Tack en mode vent apparent)
 *    
*/

#include <RCSwitch.h>
#include <string.h>

RCSwitch mySwitch = RCSwitch();

// constantes
const int Pin_Btn_left=4;
const int Pin_Btn_Mode=5;
const int Pin_Btn_Right=6;
const int Pin_Buzzer=8;

const int Code_btn_1=11377576;
const int Code_btn_2=11377572;
const int Code_btn_3=11377580;
const int Code_btn_4=11377570;
const int Code_btn_5=11377578;
const int Code_btn_6=11377574;

const int Duree_pression=150; //150
const int Duree_inter_action=600; //400
const int Duree_inter_cling=150; //150
const int Angle_Virement_bord=80;

bool Btn_left_etat=LOW;
bool Btn_Mode_etat=LOW;
bool Btn_Right_etat=LOW;
int valeur_recue=0;
unsigned long action_encours[]={0,0,0,0}; // Etat: n° de l'action en cours, Initialisation 0/1, Heure d'obsolescence, Ittérations restantes
int num_action=0;
int pin_action[]={0,Pin_Btn_Mode,Pin_Btn_left,Pin_Btn_Right,Pin_Btn_left,Pin_Btn_Right,Pin_Btn_left,Pin_Btn_Right};
int freq_action[]={3000,3000,2000,2100,2000,2100,1000,1100};
unsigned long time_deb_action=0;
int valeur_btn_recue=0;
unsigned long currentTime=0;
unsigned long previousTime=0;
unsigned long clignTime=0;
unsigned long Btn_codif_Time=0;
int boucle_cpt=0;
char seq_btn[]={0,0,0};
int intseqbt=0;
int timerseqbtn=4000;


void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
  pinMode(Pin_Btn_left,OUTPUT);
  pinMode(Pin_Btn_Mode,OUTPUT);
  pinMode(Pin_Btn_Right,OUTPUT);
  pinMode(Pin_Buzzer,OUTPUT);

}

void loop() {

// ECOUTE 
  delay(50);
  currentTime=millis();
  //Serial.println(currentTime);
  
  if (mySwitch.available()) {
    int valeur_recue=mySwitch.getReceivedValue();
    /*
    Serial.print("Received ");
    Serial.print( mySwitch.getReceivedValue() );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );
    */
    
    if (valeur_recue==Code_btn_1) {valeur_btn_recue=1;}
    else if (valeur_recue==Code_btn_2) {valeur_btn_recue=2;}
    else if (valeur_recue==Code_btn_3) {valeur_btn_recue=3;}
    else if (valeur_recue==Code_btn_4) {valeur_btn_recue=4;}
    else if (valeur_recue==Code_btn_5) {valeur_btn_recue=5;}
    else if (valeur_recue==Code_btn_6) {valeur_btn_recue=6;}

    //Serial.println(valeur_btn_recue);

    mySwitch.resetAvailable();
  }
// FIN ECCOUTE

    
// ANALYSE ET MODIFICATION LOGIQUE DES ETATS 
   
  //Serial.print(currentTime);
  if (action_encours[0]!=0 && action_encours[0]!=1 && valeur_btn_recue==1) {annul_actions(); } //Annuler toutes les actions en cours
  
  if((currentTime-previousTime)>Duree_inter_action && (action_encours[0]==0 || action_encours[0]==10  )){
    previousTime=currentTime;

    if (Btn_codif_Time==0) {
      if (valeur_btn_recue==1) {action_encours[0]=1;action_encours[3]=1;clignTime=0;Serial.println("Btn_1");tone(Pin_Buzzer,3000,Duree_inter_cling*0.8);}
        else if (valeur_btn_recue==2) {action_encours[0]=10;seq_btn[0]=2;intseqbt=1;Btn_codif_Time=currentTime+timerseqbtn;Serial.println("Btn_2");tone(Pin_Buzzer,3000,Duree_inter_cling*0.8);}
        else if (valeur_btn_recue==3) {action_encours[0]=2;action_encours[3]=1;clignTime=0;Serial.println("Btn_3");}
        else if (valeur_btn_recue==4) {action_encours[0]=3;action_encours[3]=1;clignTime=0;Serial.println("Btn_4");}
        else if (valeur_btn_recue==5) {action_encours[0]=4;action_encours[3]=10;clignTime=0;Serial.println("Btn_5");}
        else if (valeur_btn_recue==6) {action_encours[0]=5;action_encours[3]=10;clignTime=0;Serial.println("Btn_6");}
    }
    else if (Btn_codif_Time > currentTime) {
      if (valeur_btn_recue!=0) {seq_btn[1]= valeur_btn_recue;Serial.print("Btn_");Serial.println(valeur_btn_recue);
      }
    }
    else if (Btn_codif_Time<currentTime) {seq_btn[0]=0;seq_btn[1]=0;Btn_codif_Time=0;Serial.println("Efface_seq_btn");
    }
    
    if (seq_btn[0]==2 && seq_btn[1]==5 ) {action_encours[0]=6;action_encours[3]=Angle_Virement_bord;clignTime=0;Serial.println("Virement babord");
    }
    else if (seq_btn[0]==2 && seq_btn[1]==6 ) {action_encours[0]=7;action_encours[3]=Angle_Virement_bord;clignTime=0;Serial.println("Virement tribord");
    }
  }
  valeur_btn_recue=0;
      
// FIN DE ANALYSE ET MODIFICATION LOGIQUE DES ETATS

// REALISATION DES ACTIONS 
//Serial.print("Action en cours :"); 
//Serial.println(action_encours[0]);


if (action_encours[0]!=0 && action_encours[0]!=10)  {
        //Serial.println(boucle_cpt);
        
      if (action_encours[1]==1) {
        
        //Serial.println("Action en cours _ Je vérifie aboslescence");
        //Serial.print("Heure d'obsolescence:"); 
        //Serial.println(action_encours[2]);
        //Serial.print("currenttime");
        //Serial.println(currentTime);
        //boucle_cpt=boucle_cpt+1;
        
        if (action_encours[2]<currentTime) {
          //Serial.println("Action_obsolete");
          action_encours[3]=action_encours[3]-1;
          //Serial.print("NB itterations restantes : ");
          //Serial.println(action_encours[3]);
          
          digitalWrite(pin_action[action_encours[0]],LOW);
          action_encours[1]=0;
          clignTime= currentTime + Duree_inter_cling;
          //Serial.print("Reinitialisation clignTime : ");
          //Serial.println(clignTime);
          
          if (action_encours[3]==0) {
            if (action_encours[0]==6 || action_encours[0]==7) {
              tone(Pin_Buzzer,freq_action[action_encours[0]],Duree_inter_cling*20);
            }
            action_encours[0]=0;
            action_encours[1]=0;
            action_encours[2]=0;
            action_encours[3]=0;
            
          }
        }
        }
        
        else if(action_encours[1]==0) {
          //Serial.println("Initialisation de l'action 1/2");
          //Serial.print("clignTime : ");
          //Serial.println(clignTime);
          
          //Serial.print("currentTime : ");
          //Serial.println(currentTime);
          
          if (clignTime<currentTime) {
            action_encours[2]=currentTime+Duree_pression; // fixe l'heure à laquelle l'action sera obsolète
            boucle_cpt=boucle_cpt+1;
            //Serial.println("Initialisation de l'Action 2/2");
            
            digitalWrite(pin_action[action_encours[0]],HIGH);
            tone(Pin_Buzzer,freq_action[action_encours[0]],Duree_inter_cling*0.8);
            action_encours[1]=1;
            //delay(5);
          }
        }
    }
// FIN DE REALISATION DES ACTIONS
 //delay(50);
}


int annul_actions() {
  digitalWrite(Pin_Btn_left,LOW);
  digitalWrite(Pin_Btn_Mode,LOW);
  digitalWrite(Pin_Btn_Right,LOW);
  digitalWrite(Pin_Buzzer,LOW); 
  action_encours[0]=0;
  action_encours[1]=0;
  action_encours[2]=0;
  boucle_cpt=0;
}
