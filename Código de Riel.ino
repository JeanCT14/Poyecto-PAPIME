  // Los comandos que agregaré para conectar a internet los colocaré entre asteríscos "*" "*"
  // #include "comandos.h"  // *Agrega los comandos necesarios para la conexión a internet*
  
  #define DIR  26        // (15) Pin de DIR del driver
  #define STEP 27        // (4) Pin STEP del driver
  #define LED_UV 12      // (Aquí debemos agregar los leds) Pin del LED UV
  #define LED_Azul 14    // (Aquí debemos agregar los leds) Pin del LED Azul
  #define Switch 25      // (Aquí no sé ) Switch que limita el desplazamiento del deslizador
  
  //*************** Variables para definir las instrucciones ******************************************************
  
  String entrada = String(6);   // Variable de entrada: Mnemónico y argumento del mnemónico (en caso de ser necesario.)
  String ar = String(3);        // Argumento asociados al mnemónico (Solo MX tiene argumento)
  char com;                     // Comando del mnemónico (MN), es la segunda letra del mnemómico
  long num;                     // Argumento del mnemónico pasado a variable tipo long
  boolean EstadoAzul;           // Variable booleana que indica el estado del LED Azul (prendido "true" o apagado "false")
  boolean EstadoUV;             // Variable booleana que indica el estado del LED UV (prendido "true" o a pagado "false")
  boolean errFlag;              // Bandera de error (true si encuentra un error)

  // **************************************************************************************************************
  
  /****************************************************************************************************************
    - SE NECESITAN 260 PASOS PARA RECORRER EL RIEL COMPLETO
    - EL RIEL MIDE 390 mm
    - HAY (390 mm)/(50 mm) = [7.8] = 8 intervalos que miden 50 mm
    - CADA INTERVALO TIENE (260  PASOS)/8 = [32.5] = 33 PASOS
  *****************************************************************************************************************/
  
  int microPausa = 5;    // Pausa entre cada paso que da el motor (asociada a la velocidad de desplazamiento)
  long dist;             // Variable que indica la cantidad de intervalos de 0.5 cm que avanza o retrocede el motor
  long Pasos;            // Es la cantidad de pasos que da el motor 66 pasos por cm (66,6 pasos)
  long Posicion;         // Es la posición en la cual se encuentra el deslizador a lo largo del riel (num entre 0 y 7)
  
  const int Minimo = 0;  // Minimo valor que puede tomar la variable Posición
  const int Maximo = 7;  // Máximo valor que puede tomar la variable Posición

  String valores [] = {"-3.5", "-3.0", "-2.5", "-2.0", "-1.5", "-1.0", "-0.5", "0.0", "0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "3.5"}; // Lista de valores que puede tomar la variable Posicion
  
  /****************************************************************************************************************
     Comandos del riel de Jean:
  
     MN | Comando(com) | Argumanto(ar)| Ejemplo    | Return                          | Mensaje de error
        |              |              |            |                                 |
     MX | Mov en  x    | long         | MX -4      | Posición                        | Error: comando no válido. (MX)
     LU | Prende LED UV| ----         | LU         | LED UV encendido                | Error: comando no válido. (LU)
     LA | Prende LED Az| ----         | LA         | LED Azul encendido              | Error: comando no válido. (LA)
     LF | Apaga LEDS   | ----         | LF         | LEDs apagados                   | Error: comando no válido. (LF)
     ST | Imprime Pos  | ----         | ST         | Posición: (posición) cm, Led    | 
        | y estado LEDs|              |            |                                 | 
    
     Sin espacios antes del mnenónico (MN)
     Un espacio entre el nemónico y el argumento
     No se tiene que poner espacio después de los mnemónicos: LU ,LA ,LF, ST
     Las variables de posición son de tipo long
     La variable Posicion es incremental
  
     Cualquier otro mnemónico marcará error        Error entrada no válida
  *****************************************************************************************************************/
  
  void setup(){
      
      pinMode(STEP, OUTPUT);      // Define STEP como salida
      pinMode(DIR, OUTPUT);       // Define DIR como salida
      pinMode(LED_UV, OUTPUT);    // Define LED_UV como salida
      pinMode(LED_Azul, OUTPUT);  // Define AZUL como salida
      pinMode(Switch, INPUT);     // Define Switch como salida
      Serial.begin(9600);         // Velocidad serial
  //  beginRcv();                 // *Comando de la librería que nos pasaron (no sé qué hace)*  

      // Sencuencia para ir al inicio
    
      if(digitalRead(Switch) == 1){
        digitalWrite(DIR, LOW);   // Indica la dirección del motor (LOW es para retroceder)
        while(digitalRead(Switch) == 1) {  // El deslizador retrocede hasta que el switch se active
              digitalWrite(STEP, HIGH);    
              delay(microPausa);           // Pausa entre casa paso
              digitalWrite(STEP, LOW);
              delay(microPausa);
        }
      }
      
      // El riel ya debe estar en la posición cero, esto se debe hacer solo al iniciar el programa y no se debe repetir// 

      // Condiciones iniciales

      Posicion = 0; // Posición del deslizador en el origen del riel
      num = 0;             // Se asigna el valor inicial del argumento de MX
      EstadoAzul = false;  // LED azul apagado
      EstadoUV = false;    // LED UV apagado

    // Terminan las condiciones iniciales
  
      Serial.print("Posición: "); // Imprime en pantalla la posición a la que se encuentra el deslizador 
      Serial.print(Posicion);
      Serial.print(" cm"); 
      Serial.print('\n');
  }
  
  void loop(){
  
    if(Serial.available()){
      
      errFlag = false;  // Se de clara la bandera de error (true solo cuando ocurre un error)
      
      //entrada = rcv();  // *Aquí es donde se mandan los coma ndos de la página de internet y se guardan como entrada (en lugar de la Serial.readString)*      
      entrada = Serial.readString();    // Entrada serial que contiene al nemónico y los argumentos Ej: "MX -4"
      entrada.toUpperCase();            // Se converten los caracteres alfabéticos en mayúsculas 
      com = entrada.charAt(1);          // Segundo caracter del string "entrada" (Segundo caracter del mnemónico)
  
      // ****** Código para extraer los argumentos del mnemóminco *******
      if(entrada.length() > 2){                   // Bloque de código que corre solo si la entrada tiene argumentos (Solo para el mnemónico MX) 
        ar = entrada.substring(3);                // Crea un string con los argumentos del mnémonico (substring comienza desde el 4to elemento del string)
        verificacionArgumento(ar);                // Variable booleana que indica si el argumento del mnenómico es válido o no
        convertir(verificacionArgumento(ar));     // Convierte el argumento a long y lo duplica; en caso de error manda false en la variable errFlag       
      }
      //******* Termina codigo para extraer los argumentos **************
  
      switch(com){ // Switch que ejecuta el bloque de código dependiendo del mnemónico de entrada.
  
            case 'T' : // Caso que imprime en pantalla el estado del dispositivo: posición y estado de los LEDs (prendido/apagado)
            //  snd(ok); //*Creo que confirma al servidor si le llego el mensaje al ESP32
                 if(entrada.length()<=2){
                   if(EstadoAzul == false; & EstadoUV == false){ // Bloque que se corre si ambos LEDs están apagados
                     Serial.print("Posición: ");
                     Serial.print(Posicion);
                     Serial.print(" cm. ");
                     Serial.print("LEDs apagados.")
                     Serial.print("\n");   // Se imprime en pantalla la posición del delizador en cm y el estado de los LEDs
                   }
                   else if(EstadoAzul == false; & EstadoUV == true){ // Bloque que corre si solo el LED UV está encendido 
                     Serial.print("Posición: ");
                     Serial.print(Posicion);
                     Serial.print(" cm. ");
                     Serial.print("LED Azul: Apagado, Led UV: Apagado.")
                     Serial.print("\n"); // Se imprime en pantalla la posición del delizador en cm y el estado de los LEDs
                   }
                   else if(EstadoAzul == true; & EstadoUV == false){ // Bloque que se corre corre si el LED azul está encendido
                     Serial.print("Posición: ");
                       Serial.print(Posicion);
                     Serial.print(" cm. ");
                     Serial.print("LED Azul: Prendido, Led UV: Apagado.")
                     Serial.print("\n"); // Se imprime en pantalla la posición del delizador en cm y el estado de los LEDs
                   } 
                }
                break;
            
            case 'A' : // Caso que corre el bloque para encender el LED azul
                //  snd(ok); // *Creo que confi
                if(entrada.length() <= 2){
                  // Bloque de codigo para encender el LED azul
                  digitalWrite(LED_Azul, HIGH);        // Se enciende solamente el LED azul
                  EstadoAzul = true; 
                  digitalWrite(LED_UV, LOW);
                  EstadoUV = false;
                  Serial.print("LED azul encendido."); // Se imprime en pantalla el aviso que el LED azul está encendido    
                  Serial.print('\n');
                }
                else{
                    Serial.print("Error: Comando no válido. (Led A) ");
                    Serial.print('\n');
                    errFlag = true; // Se activa la bandera de error
                }
                break;
      
            case 'U' : // Caso que corre el bloque para encender el LED UV
                //  snd(ok); //*Creo que confi
                if(entrada.length() <= 2){
                  // Bloque de código para encender el LED UV
                  digitalWrite(LED_Azul, LOW);
                  EstadoAzul = false;
                  digitalWrite(LED_UV, HIGH);         // Se enciende solamente el LED UV
                  EstadoUV = true;
                  Serial.print("LED UV encendido.");  // Se imprime en pantalla el aviso que el LED UV está encendido
                  Serial.print('\n');
                }
                else{
                    Serial.print("Error: comando no válido. (LU)");
                    Serial.print('\n');
                    errFlag = true; // Se activa la bandera de error
                }
                break;
      
            case 'F' : // Caso que corre el bloque para apagar ambos LEDS
                //  snd(ok); //*Creo que confi
                if(entrada.length() <= 2){
                  // Bloque de código para apaga ambos LEDS
                  digitalWrite(LED_Azul, LOW);    // Se apagan ambos LEDS
                  EstadoAzul = false;
                  digitalWrite(LED_UV, LOW); 
                  EstadoUV = false;
                  Serial.print("LEDS apagados."); // Se imprime en pantalla el aviso que ambos están apagados
                  Serial.print('\n');
                }
                else{
                  Serial.print("Error: comando no válido. (LF)");
                  Serial.print('\n');
                  errFlag = true;
                }
                break;
      
            case 'X' : // Caso que corre el bloque que mueve el deslizador
                //  snd(ok); //*Creo que confi
                if(entrada.charAt(2) == ' '){ // Se debe poner espacio entre el MN y el argumento o el código marca error.
                  dist = num;                 // Se extra el argumento de MX y se 
                  num = long(2 * ar.toFloat());
                  
                  // Bloque de código que mueve al deslizador      
                  if(dist > 0){ // Condición para que el deslizador avance
                    if((Minimo <= Posicion + dist) && (Posicion + dist <= Maximo)) { // Condición para saber si la entrada permite que el deslizador esté en el rango permitido
                      Posicion = Posicion + dist; // Este va a marcar la posición en el riel en la que debe estar el deslizador [0,8], solo la marca, pero no avanza el deslizador
                      Serial.print("Posición: ");
                      Serial.print(float(Posicion)/2.0,1); 
                      Serial.print(" cm");
                      Serial.print('\n');
                      Pasos = abs(dist) * 33; // Los pasos que le faltan por recorrer para llegar a la posición deseada considerando la posición en la que se encuentra ya
                      MovRiel(HIGH, Pasos);
                    } // Termina condición para saber si el deslizador se encontrará en el rango permitido
                    else{ // Bloque de código para que el deslizador avance lo máximo posible en caso de que el valor de entrada sea mayor al permitido
                        Serial.print("Error: fuera de rango");
                        Serial.print('\n');
                        errFlag = true;
                    } // Acaba el else
                  } // *Acaba el if dist > 0
        
                  if(dist < 0){ // Condición para que el deslizador retroceda
                    if((Minimo <= Posicion + dist) && (Posicion + dist <= Maximo)){ // Condición para saber si la entrada permite que el deslizador esté en el rango permitido
                      Posicion = Posicion + dist; // Este va a marcar la posición en el riel en la que debe estar el deslizador [0,8], solo la marca, pero no retrocede el deslizador
                      Serial.print("Posición: ");
                      Serial.print(float(Posicion)/2.0,1); //
                      Serial.print(" cm");
                      Serial.print('\n');
                      Pasos = abs(dist) * 33; //Pasos que le faltan por recorrer para llegar a la posición deseada considerando la posición en la que se encuentra ya
                      MovRiel(LOW, Pasos);
                    } // Termina condición para saber si el deslizador se encontrará en el rango permitido
                    else{
                        Serial.print("Error: fuera del rango");
                        Serial.print('\n');
                        errFlag = true;        
                    } //Acaba el else
                  } // *Acaba el if dist<0
                // Termina bloque de código que mueve el deslizador
               
                } // Termina if num != 0
                else{
                    Serial.print("Error: comando no válido. (MX)");
                    Serial.print('\n');
                    errFlag = true;
                }
                break;
      
            default : // Código que se corre en caso de que la entrada no sea válida
                   errFlag = true;
                   Serial.print("Error entrada no válida"); // Manda un mensaje al puerto serial indicando que la entrada no es válida.
                   Serial.print('\n');
      } // Termina switch
      if((not errFlag)){
        Serial.print("Entrada ");
        Serial.print(entrada);
        Serial.print(" aceptada.");
        Serial.print("\n");
      }
      else{
          errFlag = false;
      }
    } //  Termina if(Serial.available)
  } // Termina void loop
  
  
  //********************************************* Función para mover el riel *********************************************
  void MovRiel(bool Sentido, int Steps){
      digitalWrite(DIR, Sentido); // Indica la dirección del motor (LOW es para retroceder HIGH avanzar )
      for(int x = 0; x < Steps; x++){ // Se da una cantidad N de pasos, N=Steps
          digitalWrite(STEP, HIGH);
          delay(microPausa); // Pausa entre casa paso
          digitalWrite(STEP, LOW); 
          delay(microPausa); // Pausa entre cada paso
      }
  }
  //**********************************************************************************************************************

  // **************************************** Función para validar string ************************************************
  bool verificacionArgumento(String argumento){
     for(int i = 0; i < 14; i++){  // 
        if(argumento.equals(valores[i])){ // Se compara el argumento con cada uno de los valores de la lista "Valores"
          return true; // Si se encuentra una coincidencia, devuelve true
        }
     }
     return false; // Si no se encuentra ninguna coincidencia devuelve false
  }
  // ***********************************************************************************************************************

  // ********************************************* Función para convertir **************************************************
  void convertir(bool entradaValida){
      if(entradaValida){ // Se cumple solo si la entrada está compuesta por dígitos (el primer caracter puede ser "-")
        num = long(2 * ar.toFloat()); // Convierte la entrada en una variable en tipo long y la duplica (es el número de posiciones que se desplaza)
      }
      else{
          errFlag = true;
      }
  }
  // ************************************************************************************************************************