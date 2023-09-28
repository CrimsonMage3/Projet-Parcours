/*
Projet: Le nom du script
Equipe: Votre numero d'equipe
Auteurs: Les membres auteurs du script
Description: Breve description du script
Date: Derniere date de modification
*/

/* ****************************************************************************
Inclure les librairies de functions que vous voulez utiliser
**************************************************************************** */

#include <LibRobus.h> // Essentielle pour utiliser RobUS

int portBruitAmbiant = 4;
int portBruitEntendue = 5;
int compteur = 0;

void setup()
{
  BoardInit();

  pinMode(portBruitAmbiant, INPUT);
  pinMode(portBruitEntendue, INPUT);

  Serial.begin(9600);

  detectionSifflet();
}

/* ****************************************************************************
Fonctions de boucle infini (loop())
**************************************************************************** */
// -> Se fait appeler perpetuellement suite au "setup"

void loop()
{

  PID();
  delay(10); // Delais pour décharger le CPU
}

void AVANCER()
{

  while (ENCODER_Read(RIGHT) <= 50000)
  {
    MOTOR_SetSpeed(RIGHT, 0.5); // Maitre

    MOTOR_SetSpeed(LEFT, (0.5 + PID())); // Esclave}
    delay(500);
  }
  int compteur = 0; // Reset compteur et encodeurs
  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
}

double PID()
{

  double k1 = 0;
  double k2 = 0;
  float KP = 0.001;
  float KI = 0.00002;

  
  k1 = abs(ENCODER_Read(RIGHT)) - abs(ENCODER_Read(LEFT));

  k1 = k1 * KP;
  //Serial.write();
  return k1 * k2;
}

void detectionSifflet()
{
  int i = 0;

  while (i <= 20) // Détection sifflet
  {

    float A4 = analogRead(portBruitAmbiant);
    float A5 = analogRead(portBruitEntendue);
/*
    if ((A5 - A4) <= 5000)
    {
      i++;
    }
    else
    {
      i = 0;
    }

    delay(100);*/
  }
  
}