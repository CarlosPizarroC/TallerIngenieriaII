//#include <LiquidCrystal.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>



//// Inicialización pantalla ////

/* - Código para LCD sin I2C */
// LiquidCrystal lcd(7, 6, 5, 4, 3, 2, 1);   // -

/* + Código para LCD con I2C */
LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, D4, D5, D6, D7 // +

//// Declaración de pines ////
int botonCancelar = 8;
int botonArriba = 9;
int botonSeleccionar = 10;
int botonAbajo = 11;

int interruptorMaestro = 12;

//// Variables auxiliares ////
char auxCadena[16];
int isEnvasadoEnable = 0;           //Esta variable cambiaría su valor con base al inicio o final de un ciclo de envasado.

//// Variables del sistema ////
int velocidadDeLlenado = 5;
int envasesLlenados = 0;            //Esta variable cambiaría su valor con base al desarrollo que se presente en los demás bloques.
int gramosLlenados = 0;             //Esta variable cambiaría su valor con base al desarrollo que se presente en los demás bloques.

int PESOSMAXIMOS[3] = {179, 282, 330};
int PESOSMINIMOS[3] = {31, 40, 50};

int pesos[3] = {179, 282, 330};    // Pesos del envase 1, 2 y 3 respectivamente (incluyendo el peso de la lata vacía).
                                  // NOTA: el peso 3 no se ha definido aún, por lo que por ahora se define (arbitrariamente) como 330.


/* Variables de Menú */
char MENU[3][16] = {"1-Vel.Llenado(5)", "2-CambiarPesos", "3-Estadísticas"}; // Menú inicial
unsigned int seccionMenu = 0;
unsigned int opcion = 0;
int mostrando = 0;

/// Variables para el uso de millis()
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
const long interval = 500;

void setup() {

  // Setup pinMode
  pinMode(botonCancelar, INPUT);
  pinMode(botonArriba, INPUT);
  pinMode(botonSeleccionar, INPUT);
  pinMode(botonAbajo, INPUT);
  pinMode(interruptorMaestro,INPUT);

  // Setup LCD
  lcd.begin(16, 2);
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciada :D");


}

void loop() { 

  while(digitalRead(isEnvasadoEnable) == 0){
    MenuPrincipal();
  } 
  // ... ejecución del ciclo de envasado...
}

void MenuPrincipal(){

  titilarSeleccion(seccionMenu, opcion);
  
  if (digitalRead(botonArriba) == HIGH && opcion != 0){
    opcion--;
  }
  else if(digitalRead(botonAbajo) == HIGH && opcion != 2){
    opcion++;
  }

  if (opcion <=1){
    seccionMenu = 0;
  }
  else if (opcion >= 2){
    seccionMenu = 1;
  }

  if (digitalRead(botonSeleccionar) == HIGH){
    switch (opcion){
      case 0:
        SeleccionDeVelocidad();
        break;
      case 1:
        configPesos();
        break;
      case 2:
        EnvasesLlenados();
        break;
    }
  }
}

void titilarSeleccion(int seccion, int opcion){
  lcd.clear();
  int mostrando = 0;

  currentMillis = millis();
  if (currentMillis - previousMillis >= interval){
    switch (seccion){

      case 0:
        switch (mostrando){
          case 0:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(MENU[0]);
            lcd.setCursor(0, 1);
            lcd.print(MENU[1]);
            mostrando = 1;
            break;

          case 1:
            lcd.clear();
            lcd.setCursor(0, !opcion);
            lcd.print(MENU[!opcion]);

            mostrando = 0;
            break;
        }
      case 1:
        switch (mostrando){
          case 0:
            lcd.setCursor(0, 0);
            lcd.print(MENU[2]);

            mostrando = 1;
            break;

          case 1:
            lcd.clear();
            
            mostrando = 0;
            break;
        }
    }
    previousMillis = currentMillis;
  }
}
void EnvasesLlenados(){
  int aux = 0;

  while(1){  

    if (currentMillis - previousMillis >= 3000){
      if (aux == 0){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Gramos");
        lcd.setCursor(0, 1);
        sprintf(auxCadena, "envasados: %d", gramosLlenados);
        lcd.print(auxCadena);

        aux = 1;
        previousMillis = currentMillis;
      }
      else{
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Envases");
        lcd.setCursor(0, 1);
        sprintf(auxCadena, "llenados: %d", envasesLlenados);
        lcd.print(auxCadena);

        aux = 0;
        previousMillis = currentMillis;
      }

      if (digitalRead(botonCancelar) == HIGH || digitalRead(botonSeleccionar) == HIGH){
        break;
      }
    }
    
  }
  
}

void SeleccionDeVelocidad(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Velocidad de");
  int vel_actual = velocidadDeLlenado;

  while(1){
    if (digitalRead(botonArriba) == HIGH && velocidadDeLlenado < 5){
      velocidadDeLlenado++;

    }
    else if(digitalRead(botonAbajo) == HIGH && velocidadDeLlenado > 1){
      velocidadDeLlenado--;
    }
    lcd.setCursor(0, 1);
    sprintf(auxCadena, "llenado: %d", velocidadDeLlenado);
    lcd.print(auxCadena);   

    if (digitalRead(botonCancelar) == HIGH){
      velocidadDeLlenado = vel_actual;

      sprintf(auxCadena, "1-Vel.Llenado(%d)", velocidadDeLlenado);  
      strcpy(MENU[0], auxCadena);

      break;
    }
    else if(digitalRead(botonSeleccionar) == HIGH){
      break;
    } 
  }
  
}

void configPesos(){
  int aux = 1;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tipo de envase:");

  while(1){
    lcd.setCursor(0, 1);
    lcd.print(aux);

    if (digitalRead(botonArriba) == HIGH && aux != 3){
      aux++;
    }
    else if (digitalRead(botonAbajo) == HIGH && aux != 1){
      aux--;
    }

    if (digitalRead(botonSeleccionar) == HIGH){
      CambiarPeso(aux);
    }
    else if(digitalRead(botonCancelar) == HIGH){
      break;
    }
  }

}

void CambiarPeso(int tipoDeEnvase){
  int aux = pesos[tipoDeEnvase];
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Seleccione el");
  lcd.setCursor(0, 1);
  lcd.print("nuevo peso: ");

  while(1){
    lcd.setCursor(0, 12);
    lcd.print(aux);

    if (digitalRead(botonArriba) == HIGH && aux < PESOSMAXIMOS[tipoDeEnvase]){

      if (aux + 10 > PESOSMAXIMOS[tipoDeEnvase]){
        aux = PESOSMAXIMOS[tipoDeEnvase];
      }
      else{
        aux = aux + 10;
      }
      
    }
    else if (digitalRead(botonAbajo) == HIGH && aux > PESOSMINIMOS[tipoDeEnvase]){
      if (aux - 10 < PESOSMINIMOS[tipoDeEnvase]){
        aux = PESOSMINIMOS[tipoDeEnvase];
      }
      else{
        aux = aux - 10;
      }
    }

    if (digitalRead(botonCancelar) == HIGH){
      break;
    }
    else if(digitalRead(botonSeleccionar) == HIGH){
      pesos[tipoDeEnvase] = aux;
      break;
    } 
  }
}