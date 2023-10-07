
#include <LibRobus.h> // Essentielle pour utiliser RobUS

int portBruitAmbiant = 4;
int portBruitEntendue = 5;

int portDetecteurProximVert = 48;
int portDetecteurProximRouge = 49;

int compteurDroite = 0;
int compteurGauche = 0;

int positionX = 2;   // 1 = Gauche, 2 = Millieu, 3 = Droite
int positionY = 1;   // Début = 1, Fin = 10
int orientation = 0; // Initialise l'orientation de départ, -1 = vers la gauche, 0 = tout droit, 1 = vers la droite, 2 vers le bas
                     // Le but d'utiliser un int est de faire en sorte que quand il avance la postition en x change selon l'orientation

float PID()
{

  float k1 = 0;
  float k2 = 0;
  float KP = 0.001; //0.0006
  float KI = 0.00006;

  compteurDroite += abs(ENCODER_Read(RIGHT)); // Ajoute la valeur au compteur de droite
  compteurGauche += abs(ENCODER_Read(LEFT));  // Ajoute la valeur au compteur de gauche
  
  

  k1 = 1 / 0.01 * (abs(ENCODER_ReadReset(RIGHT)) - abs(ENCODER_ReadReset(LEFT))); // Trouve le k1, différence encodeur sans cycle
  k2 = 1 / 0.01 * (abs(compteurDroite) - abs(compteurGauche));                    // Trouve le k2, différence des compteurs
  Serial.println(compteurDroite);
  Serial.println(compteurGauche);
  k1 = k1 * KP;
  k2 *= KI;

  return k1 + k2;
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

  while (compteurDroite <= 5950) // Boucle qui s'arrête selon la valeur du compteur car les encodeur sont reset à chaque utiliation du pid
  {

    MOTOR_SetSpeed(RIGHT, 0.25); // Maitre

    MOTOR_SetSpeed(LEFT, (0.25 + PID())); // Esclave

    delay(10);
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

  while (compteurDroite <= 1825) // Boucle qui s'arrête selon la valeur du compteur car les encodeur sont reset à chaque utiliation du pid
  {
    MOTOR_SetSpeed(RIGHT, -0.25); // Maitre

    MOTOR_SetSpeed(LEFT, (0.25 + PID())); // Esclave}
    delay(10);
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

  while (compteurDroite <= 1820) // Boucle qui s'arrête selon la valeur du compteur car les encodeur sont reset à chaque utiliation du pid
  {
    MOTOR_SetSpeed(RIGHT, 0.25); // Maitre

    MOTOR_SetSpeed(LEFT, (-0.25 - PID())); // Esclave}
    delay(10);
  }
  reset();
}

void detectionSifflet()
{

  float A4 = 0;
  float A5 = 0;

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
  
  return (digitalRead(portDetecteurProximVert) == 0 && digitalRead(portDetecteurProximRouge) == 0); //Renvoie vrai si il détecte quelque chose
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
  delay(50);  // Delais pour décharger le CPU
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
  //algorithme();
  while (!ROBUS_IsBumper(REAR))
  {
  }
  for(int i = 0; i < 4; i++){
    AVANCER();

  }
  delay(200);
}
