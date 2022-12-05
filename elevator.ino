#define PATRO1_TLC 1
#define PATRO2_TLC 2
#define PATRO3_TLC 3
#define PATRO4_TLC 4

#define ZAVORA1 7
#define ZAVORA2 8
#define ZAVORA3 9
#define ZAVORA4 10

#define MOTOR_IN4 11
#define MOTOR_IN3 12
#define MOTOR_ENB 13 // PWM

// knihovny pro LCD přes I2C
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Hlavicky funkci
void zastavitVytah();
void spustitVytah(int smer);
int stavZavor();
void poziceVytahuDisplej (int pozice);
void jizdaDoPatraDisplej (int aktPatro, int cilPatro);
int zjistiSmer (int aktuPatro, int cilPatro);

// Globalni promenne
// Patra - 1,2,3,4 (0 je chyba)
int aktPatro = 0;
// Pokud vytah jede => true
bool vytahJede = false;

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  // Vypis po seriovem portu
  Serial.begin(9600);
  // Inicializace displeje
  lcd.begin();
  lcd.backlight();
  lcd.print("Vytah");
  // Motor
  pinMode(MOTOR_ENB,OUTPUT);
	pinMode(MOTOR_IN3,OUTPUT);
	pinMode(MOTOR_IN4,OUTPUT);
  digitalWrite(MOTOR_IN3,LOW);
  digitalWrite(MOTOR_IN4,LOW);
  // Tlacitka
  pinMode(PATRO1_TLC,INPUT);
  pinMode(PATRO2_TLC,INPUT);
  pinMode(PATRO3_TLC,INPUT);
  pinMode(PATRO4_TLC,INPUT);
  // Zavory
  pinMode(ZAVORA1,INPUT);
  pinMode(ZAVORA2,INPUT);
  pinMode(ZAVORA3,INPUT);
  pinMode(ZAVORA4,INPUT);
  // Zjisteni aktualni pozice vytahu
  aktPatro = stavZavor();
  if (aktPatro == 0){
    chybaDisplej(0);
  }
  else{
    poziceVytahuDisplej(aktPatro);
  }
}

void loop() {
  if (aktPatro == 0){
    chybaDisplej(0);
  }
  // Logika pohybu a skenovani tlacitek
  else{
    // Promenna pro cilove patro
    int cilPatro = 0;
    // Vytah nejede
    if (vytahJede == false){
      int p1TlacitkoStav = digitalRead(PATRO1_TLC);
      int p2TlacitkoStav = digitalRead(PATRO2_TLC);
      int p3TlacitkoStav = digitalRead(PATRO3_TLC);
      int p4TlacitkoStav = digitalRead(PATRO4_TLC);
      // Pokud je tlacitko zmackle
      if (p1TlacitkoStav == HIGH){
        // Pokud uz nejsme v danem patre
        if (aktPatro != 1){
          spustitVytah(zjistiSmer(aktPatro,1));
          jizdaDoPatraDisplej(aktPatro,1);
          cilPatro = 1;
        }
      }
      if (p2TlacitkoStav == HIGH){
        if (aktPatro != 2){
          spustitVytah(zjistiSmer(aktPatro,2));
          jizdaDoPatraDisplej(aktPatro,2);
          cilPatro = 2;
        }
      }
      if (p3TlacitkoStav == HIGH){
         if (aktPatro != 3){
          spustitVytah(zjistiSmer(aktPatro,3));
          jizdaDoPatraDisplej(aktPatro,3);
          cilPatro = 3;
        }
      }
      if (p4TlacitkoStav == HIGH){
         if (aktPatro != 4){
           spustitVytah(zjistiSmer(aktPatro,4));
           jizdaDoPatraDisplej(aktPatro,4);
           cilPatro = 4;          
        }
      }
    }
    // Vytah jede 
    else {
      // Udelal bych ochranu proti stavum kdyby mohl vytah prejet pod 1 a nad 4 ...

      // Pokud jsme v cilovem patre zastav
      if (stavZavor() == cilPatro){
        zastavitVytah();
        poziceVytahuDisplej(aktPatro);
        aktPatro = cilPatro;
      }
    }
  }
}

// Vytiskne aktualni pozici vytahu na displej
void poziceVytahuDisplej (int pozice){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AKTUALNI PATRO:");
  lcd.setCursor(8, 1);
  lcd.print(pozice);
}

// Chyba v programu
// 0 - neurcita patra
void chybaDisplej (int kodChyby){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CHYBA !!!");
  lcd.setCursor(0, 1);
  if (kodChyby == 0) lcd.print("NEURCITA PATRA");
  else lcd.print("NEIDENTIFIKOVANO");
}

// Vytiskne na displej do jakeho jedeme patra
void jizdaDoPatraDisplej (int aktuPatro, int cilPatro){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("JEDU Z PATRA:");
  lcd.setCursor(13, 0);
  lcd.print(aktuPatro);
  lcd.setCursor(0, 1);
  lcd.print("CILOVE PATRO:");
   lcd.setCursor(13, 1);
  lcd.print(cilPatro);
}

// Zastavi pohyb motoru pro vytah
void zastavitVytah(){
  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, LOW);
  vytahJede = false;
}

// Vraci smer vytahu na zaklade akttualniho a cilove patra
// 0 -> dolu
// 1 -> nahoru
int zjistiSmer (int aktuPatro, int cilPatro){
  if (aktuPatro < cilPatro) return 1;
  if (aktuPatro > cilPatro) return 0;
  return -1;
}

// Spusti pohyb motoru pro vytah
// 0 -> dolu
// 1 -> nahoru
void spustitVytah (int smer){
  int rychlostVytahu = 100;
  if (smer == 0){
    analogWrite(MOTOR_ENB,rychlostVytahu);
    digitalWrite(MOTOR_IN3, HIGH);
    digitalWrite(MOTOR_IN4, LOW);
    vytahJede = true;
  }
  else if (smer == 1){
    analogWrite(MOTOR_ENB,rychlostVytahu);
    digitalWrite(MOTOR_IN3, LOW);
    digitalWrite(MOTOR_IN4, HIGH);
    vytahJede = true;
  }
  else {
    Serial.println("Zadan spatny smer!");
  }
}

// Zjisti ktera zavora je zakryta vytahem
// Vraci int hodnotu podle zavory 1,2,3,4
// Pokud vrati 0 tak je aktivovano vice zavor a nastala chyba
int stavZavor (){
  int referenceZ = 500; // Rozhodovaci hladina pro urceni stavu zavory
  // Precte hodnoty ze zavor
  int hodnotaZ1 = analogRead(ZAVORA1);
  int hodnotaZ2 = analogRead(ZAVORA2);
  int hodnotaZ3 = analogRead(ZAVORA3);
  int hodnotaZ4 = analogRead(ZAVORA4);
  // Prevede hodnoty na pravdivosti hodnoty
  bool zakrytaZ1 = false;
  bool zakrytaZ2 = false;
  bool zakrytaZ3 = false;
  bool zakrytaZ4 = false;
  if (hodnotaZ1 > referenceZ) zakrytaZ1 = true;
  if (hodnotaZ2 > referenceZ) zakrytaZ2 = true;
  if (hodnotaZ3 > referenceZ) zakrytaZ3 = true;
  if (hodnotaZ4 > referenceZ) zakrytaZ4 = true;
  // Logika a vraceni zakryte zavory
  if (zakrytaZ1 == true && zakrytaZ2 != true && zakrytaZ3 != true && zakrytaZ4 != true) return 1;
  else if (zakrytaZ1 != true && zakrytaZ2 == true && zakrytaZ3 != true && zakrytaZ4 != true) return 2;
  else if (zakrytaZ1 != true && zakrytaZ2 != true && zakrytaZ3 == true && zakrytaZ4 != true) return 3;
  else if (zakrytaZ1 != true && zakrytaZ2 != true && zakrytaZ3 != true && zakrytaZ4 == true) return 4;
  else {
    Serial.println("Je zakryta vic než jedna zavora!!!");
    return 0;
  } 
}
