
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
double erreur = 0;

long millisDebut = 0;
long millisFin = 0;

double KP = 0.0035; // 0.0006
double KI = 0.00009;

int valeurAvancer = 7300;

double ValPID = 0;

double PID()
{
  
  millisFin = millis();

  if ((millisFin - millisDebut) >= 50.0)
  {
    int valeurEncodeurDroit = abs(ENCODER_Read(RIGHT));
    int valeurEncodeurGauche = abs(ENCODER_Read(LEFT));

    erreur = valeurEncodeurDroit - valeurEncodeurGauche;

    ValPID = (erreur * KP + erreur * 0.05 * KI);
    
    Serial.println(erreur);
    Serial.println(ValPID,10);
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

  while (abs(ENCODER_Read(RIGHT)) <= valeurAvancer) // Boucle qui s'arrête selon la valeur du compteur car les encodeur sont reset à chaque utiliation du pid
  {
    if (compteurDroite <= valeurAvancer * 0.90)
    {
      MOTOR_SetSpeed(RIGHT, 0.25);                   // Maitre
      MOTOR_SetSpeed(LEFT, (0.25  + PID())); // Esclave}
      
    }
    else
    {

      MOTOR_SetSpeed(RIGHT, 0.1); // Maitre
      MOTOR_SetSpeed(LEFT, (0.1 + PID()));
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
  else
  {
    orientation = -1;
  }

  while (abs(ENCODER_Read(RIGHT)) <= 1940) // Boucle qui s'arrête selon la valeur du compteur car les encodeur sont reset à chaque utiliation du pid
  {
    MOTOR_SetSpeed(RIGHT, -0.15); // Maitre

    MOTOR_SetSpeed(LEFT, (0.15 + 0.0057 + PID())); // Esclave}
  }
  reset();
}

void TOURNERGAUCHE()
{

  if (orientation == 0) // Si le robot est orienté vers le nord il aura une orientation vers la droite
  {
    orientation = -1;
  }
  else if (orientation == 1) // Sinon il est orienté vers la gauche et reviens avec un orientation vers le nord
  {
    orientation = 0;
  }
  else
  {
    orientation = 1;
  }

  while (abs(ENCODER_Read(RIGHT)) <= 1940) // Boucle qui s'arrête selon la valeur du compteur car les encodeur sont reset à chaque utiliation du pid
  {

    MOTOR_SetSpeed(RIGHT, 0.15);                    // Maitre
    MOTOR_SetSpeed(LEFT, (-(0.1557 + PID()))); // Esclave
  }
  reset();
}

void detectionSifflet()
{

  double A4 = 0;
  double A5 = 0;

  while ((A5 - A4) < 10)
  {
    Serial.println(A5 - A4);
    A4 = analogRead(portBruitAmbiant);
    A5 = analogRead(portBruitEntendue);
  }

  delay(2000);
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

      if (orientation == 1 || (Detection() && (orientation == 0))) // Si il regarde à gauche et il y a rien ou si il reguarde tout droit et voit quelque chose
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
  delay(100); // Delais pour décharger le CPU
  compteurDroite = 0;
  compteurGauche = 0;
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

  Serial.begin(115200);

  // detectionSifflet(); // Attend le son du siflet

  while (!ROBUS_IsBumper(REAR))
  {
  }

  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
}

void loop()
{


 algorithme();
}
