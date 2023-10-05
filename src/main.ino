
#include <LibRobus.h> // Essentielle pour utiliser RobUS

int portBruitAmbiant = 4;
int portBruitEntendue = 5;

int portDetecteurProximVert = 48;
int portDetecteurProximRouge = 49;

int compteurDroite = 0;
int compteurGauche = 0;

int positionX = 2;    // 1 = Gauche, 2 = Millieu, 3 = Droite
int positionY = 1;    // Début = 1, Fin = 10
int orientation = 0; // Initialise l'orientation de départ, -1 = vers la gauche, 0 = tout droit, 1 = vers la droite, 2 vers le bas
                      // Le but d'utiliser un int est de faire en sorte que quand il avance la postition en x change selon l'orientation

float PID()
{

  float k1 = 0;
  float k2 = 0;
  float KP = 0.0006;
  float KI = 0.00002;

  compteurDroite += abs(ENCODER_Read(RIGHT)); // Ajoute la valeur au compteur de droite
  compteurGauche += abs(ENCODER_Read(LEFT));  // Ajoute la valeur au compteur de gauche

  k1 = 1 / 0.1 * (abs(ENCODER_ReadReset(RIGHT)) - abs(ENCODER_ReadReset(LEFT))); // Trouve le k1, différence encodeur sans cycle
  k2 = 1 / 0.1 * (abs(compteurDroite) - abs(compteurGauche));                    // Trouve le k2, différence des compteurs

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
  } else if(orientation == 2){

    positionY--;
  }

  while (compteurDroite <= 6700) // Boucle qui s'arrête selon la valeur du compteur car les encodeur sont reset à chaque utiliation du pid
  {

    MOTOR_SetSpeed(RIGHT, 0.25); // Maitre

    MOTOR_SetSpeed(LEFT, (0.25 + PID())); // Esclave

    delay(100);
  }
}

void TOURNERDROITE()
{

  if (orientation == 0) // Si le robot est orienté vers le nord il aura une orientation vers la droite
  {
    orientation = 1;
  }
  else if( orientation == -1) // Sinon il est orienté vers la gauche et reviens avec un orientation vers le nord
  {
    orientation = 0;
  } else {

    orientation = -1;

  }

  while (compteurDroite <= 1900) // Boucle qui s'arrête selon la valeur du compteur car les encodeur sont reset à chaque utiliation du pid
  {
    MOTOR_SetSpeed(RIGHT, -0.2); // Maitre

    MOTOR_SetSpeed(LEFT, (0.2 + PID())); // Esclave}
    delay(100);
  }
}

void TOURNERGAUCHE()
{

  if (orientation == 0) // Si le robot est orienté vers le nord il aura une orientation vers la droite
  {
    orientation = -1;
  }
  else if( orientation == 1) // Sinon il est orienté vers la gauche et reviens avec un orientation vers le nord
  {
    orientation = 0;

  } else {

    orientation = 1;

  }

  while (compteurDroite <= 1900) // Boucle qui s'arrête selon la valeur du compteur car les encodeur sont reset à chaque utiliation du pid
  {
    MOTOR_SetSpeed(RIGHT, 0.2); // Maitre

    MOTOR_SetSpeed(LEFT, (-0.2 - PID())); // Esclave}
    delay(100);
  }
}

void detectionSifflet()
{

  float A4 = 0;
  float A5 = 0;

  while ((A5 - A4) < 10)
  {
    Serial.println(A5-A4);
    A4 = analogRead(portBruitAmbiant);
    A5 = analogRead(portBruitEntendue);
  }

  delay(2000);
}

bool PasDetection()
{
 bool vert = digitalRead(portDetecteurProximRouge);
 bool rouge = digitalRead(portDetecteurProximVert);
if (rouge == 1){
  if(vert == 1)
  Serial.println("Faux");
  return true;
}
Serial.println("True");
  return false;

}

void algorithme()
{

  if (positionY != 10)
  { // Condition finale du robot, c'est à dire la postion 10 est la fin du labyrinthe

    switch (positionX)
    {
    case 1: // À gauche dans le labyrinthe

      if ((PasDetection() && (orientation == -1)) || (!PasDetection() && (orientation == 0))) // Si il regarde à gauche et il y a rien ou si il reguarde tout droit et voit quelque chose
      {
        TOURNERDROITE();
        AVANCER();
      }
      else
      {
        AVANCER();
      }

      break;

    case 2: // Au milieu dans le labyrinthe

      if (PasDetection())
      {

        AVANCER();
      }
      else
      {

        TOURNERDROITE();
    delay(1000);
        if (PasDetection())
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

      if ((PasDetection() && (orientation == 1)) || (!PasDetection() && (orientation == 0))) // Si il regarde à gauche et il y a rien ou si il reguarde tout droit et voit quelque chose
      {
        TOURNERGAUCHE();
        AVANCER();
      }
      else
      {
        AVANCER();
      }

      break;
    }
  }
}

void reset()
{

  MOTOR_SetSpeed(RIGHT, 0.0);
  MOTOR_SetSpeed(LEFT, 0.0);
  compteurDroite = 0;
  compteurGauche = 0;
}

void setup()
{
  BoardInit();

  pinMode(portBruitAmbiant, INPUT);
  pinMode(portBruitEntendue, INPUT);

  pinMode(portDetecteurProximVert, INPUT);
  pinMode(portDetecteurProximRouge, INPUT);

  Serial.begin(115200);
  // Serial.write("Début");
   //detectionSifflet(); // Attend le son du siflet
  // AVANCER();
 while(!ROBUS_IsBumper(REAR)){}
  // AVANCER();
}

void loop()
{

  algorithme();
//asDetection();
  reset();
  delay(50); // Delais pour décharger le CPU
}
