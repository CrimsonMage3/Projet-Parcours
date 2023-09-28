
#include <LibRobus.h> // Essentielle pour utiliser RobUS

int portBruitAmbiant = 4;
int portBruitEntendue = 5;

int portDetecteurProximVert = 6;
int portDetecteurProximRouge = 7;

int compteurDroite = 0;
int compteurGauche = 0;

void setup()
{
  BoardInit();

  pinMode(portBruitAmbiant, INPUT);
  pinMode(portBruitEntendue, INPUT);

  pinMode(portDetecteurProximVert, INPUT);
  pinMode(portDetecteurProximRouge, INPUT);

  Serial.begin(9600);

  detectionSifflet(); // Attend le son du siflet
}

void loop()
{

  if (digitalRead(portDetecteurProximRouge) == digitalRead(portDetecteurProximVert) == 1) // Si rien devant avance
  {
    AVANCER();
  }
  else
  {
    // Tourner droite
    if (digitalRead(portDetecteurProximRouge) == digitalRead(portDetecteurProximVert) == 1) // Si après tourner à droite rien devant avance
    {
      AVANCER();
    }
    else
    {
      // Tourner gauche 2 fois / tourner 180 degré
      if (digitalRead(portDetecteurProximRouge) == digitalRead(portDetecteurProximVert) == 1) // Si apès tourner à gauche deux fois rien devant avance
      {
        AVANCER();
      }
      else
      {              // FIN
        while (true) // Gay baby jail
        {
        }
      }
    }
  }
  int compteur = 0; // Reset compteur

  delay(10); // Delais pour décharger le CPU
}

void AVANCER()
{

  while (compteurDroite <= 50000) // Boucle qui s'arrête selon la valeur du compteur car les encodeur sont reset à chaque utiliation du pid
  {
    MOTOR_SetSpeed(RIGHT, 0.5); // Maitre

    MOTOR_SetSpeed(LEFT, (0.5 + PID())); // Esclave}
    delay(500);
  }
}

void TOURNERDROITE()
{
  while (compteurDroite <= 50000) // Boucle qui s'arrête selon la valeur du compteur car les encodeur sont reset à chaque utiliation du pid
  {
    MOTOR_SetSpeed(RIGHT, -0.5); // Maitre

    MOTOR_SetSpeed(LEFT, (0.5 + PID())); // Esclave}
    delay(500);
  }
}

void TOURNER180()
{
  while (compteurDroite <= 50000) // Boucle qui s'arrête selon la valeur du compteur car les encodeur sont reset à chaque utiliation du pid
  {
    MOTOR_SetSpeed(RIGHT, 0.5); // Maitre

    MOTOR_SetSpeed(LEFT, (-0.5 - PID())); // Esclave}
    delay(500);
  }
}

double PID()
{

  float k1, k2 = 0;
  float KP = 0.001;
  float KI = 0.00002;
  compteurDroite += ENCODER_Read(RIGHT); // Ajoute la valeur au compteur de droite
  compteurGauche += ENCODER_Read(LEFT);  // Ajoute la valeur au compteur de gauche

  k1 = abs(ENCODER_ReadReset(RIGHT)) - abs(ENCODER_ReadReset(LEFT)); // Trouve le k1, différence encodeur sans cycle
  k2 = abs(compteurDroite) - abs(compteurGauche);                    // Trouve le k2, différence des compteurs

  k1 *= KP;
  k2 *= KI;

  return k1 + k2;
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