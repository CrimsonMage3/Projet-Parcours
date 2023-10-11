
#include <LibRobus.h> // Essentielle pour utiliser RobUS

int portBruitAmbiant = 4;
int portBruitEntendue = 5;

int portDetecteurProximVert = 48;
int portDetecteurProximRouge = 49;

int compteurDroite = 0;
int compteurGauche = 0;
double derniereErreur = 0;

int positionX = 2;   // 1 = Gauche, 2 = Millieu, 3 = Droite
int positionY = 1;   // Début = 1, Fin = 10
int orientation = 0; // Initialise l'orientation de départ, -1 = vers la gauche, 0 = tout droit, 1 = vers la droite, 2 vers le bas
                     // Le but d'utiliser un int est de faire en sorte que quand il avance la postition en x change selon l'orientation
double erreurKP = 0;
double erreurKI = 0;

long millisDebut = 0;
long millisFin = 0;

double KP = 0.00005; // Robot B : 0.00005
double KI = 0.0000018; // Robot B : 0.0000018

int valeurAvancer = 6740; // Robot A : 6682 Robot B : 6740
int valeurTourner = 2013; // Robot A : 1985 Robot B : 2015
float distanceRestante = 0;
float MOTORSPEED = 0;

double ValPID = 0;

double PID()
{

  millisFin = millis();

  if ((millisFin - millisDebut) >= 50.0)
  {
     Serial.println("debut");
    int valeurEncodeurDroit = abs(ENCODER_Read(RIGHT));
    int valeurEncodeurGauche = abs(ENCODER_Read(LEFT));

    erreurKP = valeurEncodeurDroit - valeurEncodeurGauche;
    erreurKI += erreurKP;

    ValPID = (1 / 0.05) * (erreurKP * KP + erreurKI * KI);

    Serial.println(erreurKP);
    Serial.println(ValPID, 10);
    Serial.println("");
    Serial.println(abs(ENCODER_Read(RIGHT)));
    Serial.println(abs(ENCODER_Read(LEFT)));
    Serial.println("");

    millisDebut = millis();
  }

  // Serial.println(ValPID);
  return ValPID;
  
}

void AVANCER()
{
  reset();
  if (orientation != 2)
  {
    positionX += orientation;
  }

  if (orientation == 0)
  {
    positionY++;
  }
  else if (orientation == 2)
  {

    positionY--;
  }
  MOTOR_SetSpeed(RIGHT,0.25);
  MOTOR_SetSpeed(LEFT,0.25);

  delay(100);

  while ((abs(ENCODER_Read(RIGHT))) <= valeurAvancer) // Boucle qui s'arrête selon la valeur du compteur car les encodeur sont reset à chaque utiliation du pid
  { 
    if ((abs(ENCODER_Read(RIGHT))) <= valeurAvancer * 0.75)
    {
      MOTOR_SetSpeed(RIGHT, 0.35);          // Maitre
      MOTOR_SetSpeed(LEFT, (0.35+0.01 + PID())); // Esclave
    }
    else
    {
      MOTOR_SetSpeed(RIGHT, 0.20);          // Maitre
      MOTOR_SetSpeed(LEFT, (0.20+0.01 + PID())); // Esclave
    }
  

  }
  reset();
}
void TOURNERDROITE()
{

  if (orientation == 0) // Si le robot est orienté vers le nord il aura une orientation vers la droite
  {
    orientation = 1;
  }
  else if (orientation == -1) // Sinon il est orienté vers la gauche et reviens avec un orientation vers le nord
  {
    orientation = 0;
  }
  else if (orientation == 2)
  {
    orientation = -1;
  }
  else
  {

    orientation = 2;
  }

  while (abs(ENCODER_Read(RIGHT)) <= valeurTourner) // Boucle qui s'arrête selon la valeur du compteur car les encodeur sont reset à chaque utiliation du pid
  {
    if ((abs(ENCODER_Read(RIGHT))) <= valeurTourner * 0.60)
    {
      MOTOR_SetSpeed(RIGHT, -0.35);         // Maitre
      MOTOR_SetSpeed(LEFT, (0.35 + PID())); // Esclave
    }
    else
    {
      distanceRestante = 1 - (abs(ENCODER_Read(RIGHT)) / valeurTourner);
      MOTORSPEED = (-0.625 * distanceRestante) + 0.725;
      MOTOR_SetSpeed(RIGHT, -(MOTORSPEED)); // Maitre
      MOTOR_SetSpeed(LEFT, MOTORSPEED + PID());
    }
  }
  reset();
}

void TOURNERGAUCHE()
{

  if (orientation == 0) // Si le robot est orienté vers le nord il aura une orientation vers la droite
  {
    orientation = -1;
  }
  else if (orientation == -1) // Sinon il est orienté vers la gauche et reviens avec un orientation vers le nord
  {
    orientation = 2;
  }
  else if (orientation == 2)
  {
    orientation = 1;
  }
  else
  {

    orientation = 0;
  }

  while (abs(ENCODER_Read(RIGHT)) <= valeurTourner) // Boucle qui s'arrête selon la valeur du compteur car les encodeur sont reset à chaque utiliation du pid
  {
    if ((abs(ENCODER_Read(RIGHT))) <= valeurTourner * 0.60)
    {
      MOTOR_SetSpeed(RIGHT, 0.35);             // Maitre
      MOTOR_SetSpeed(LEFT, (-(0.35 + PID()))); // Esclave
    }
    else
    {
      distanceRestante = 1 - (abs(ENCODER_Read(RIGHT)) / valeurTourner);
      MOTORSPEED = (-0.625 * distanceRestante) + 0.725;
      MOTOR_SetSpeed(RIGHT, MOTORSPEED); // Maitre
      MOTOR_SetSpeed(LEFT, -(MOTORSPEED + PID()));
    }
  }
  reset();
}

void detectionSifflet()
{

  double A4 = 0;
  double A5 = 0;
  for (int i = 0; i < 500; i++)
  {
    while ((A5 - A4) < 110) // Robot A:220 Robot B : 110
    {
      Serial.println(A5 - A4);
      A4 = analogRead(portBruitAmbiant);
      A5 = analogRead(portBruitEntendue);
      delay(1);
      // if(A5 - A4)
    }
  }

  delay(1500) ;
}

bool Detection()
{

  return (digitalRead(portDetecteurProximVert) == 0 && digitalRead(portDetecteurProximRouge) == 0); // Renvoie vrai si il détecte quelque chose
}

void algorithme()
{

  if (positionY != 10)
  { // Condition finale du robot, c'est à dire la postion 10 est la fin du labyrinthe

    switch (positionX)
    {
    case 1: // À gauche dans le labyrinthe

      if (orientation == -1 || (Detection() && (orientation == 0))) // Si il regarde à gauche et il y a rien ou si il reguarde tout droit et voit quelque chose
      {
        TOURNERDROITE();
      }
      else // Si il regarde vers l'avant et pas de mur, ou il regarde à droite
      {
        AVANCER();
      }

      break;

    case 2: // Au milieu dans le labyrinthe

      if (!Detection())
      {
        AVANCER();
      }
      else
      {

        TOURNERDROITE();

        if (!Detection())
        {

          AVANCER();
        }
        else
        {
          TOURNERGAUCHE();

          TOURNERGAUCHE();

          AVANCER();
        }
      }

      break;

    case 3: // À droite dans le labyrinthe

      if (orientation == 1 || (Detection() && (orientation == 0))) // Si il regarde à gauche et il y a rien ou si il regarde tout droit et voit quelque chose
      {
        TOURNERGAUCHE();
      }
      else
      {
        AVANCER(); // Si il regarde vers l'avant et pas de mur, ou il regarde à gauche
      }

      break;
    }
  }
}

void reset()
{

  MOTOR_SetSpeed(RIGHT, 0.0);
  MOTOR_SetSpeed(LEFT, 0.0);
  // Delais pour décharger le CPU
  delay(150); //old value : 250
  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
}

void setup()
{
  BoardInit();

  pinMode(portBruitAmbiant, INPUT);
  pinMode(portBruitEntendue, INPUT);

  pinMode(portDetecteurProximVert, INPUT);
  pinMode(portDetecteurProximRouge, INPUT);

  Serial.begin(9600);
 
  delay(100);
  detectionSifflet(); // Attend le son du siflet
  //while (!ROBUS_IsBumper(REAR))
  //{
  //}
  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
}

void loop()
{

  //AVANCER();
  algorithme();
  //while (!ROBUS_IsBumper(REAR))
  //{
  //}

}
