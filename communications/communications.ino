/*
  Communications v5

  Transmits data using a white LED and recieves it using a photoresistor

*/

#define STX 0x70
#define ETX 0x71

char txButton, txTilt, txPot, txA, txB, txC, txD;
char txBuffer[8] = {0,0,0,0,0,0,0,ETX};
char rxBuffer[7];
char rxButton, rxTilt, rxPot, rxA, rxB, rxC, rxD;
int  rx_index;

void readInputs()
{
  // Reads the inputs in the mini-projects
  // Uses txButton, txTilt, txPot, txA, txB, txC, txD;
  int buttonState = digitalRead(2);
  int potValue = analogRead(A1);
  // Serial.println(potValue);
  if (buttonState == 1){
    txButton = 1;
  }
  
}

void writeOutputs()
{
  // Writes the outputs in the mini-projects
  // Uses rxButton, rxTilt, rxPot, rxA, rxB, rxC, rxD;
  
  if (rxButton == 1){
    tone(13, 1000)
  }
}



int ledState = LOW;             // ledState used to set the LED


char encrypt(char in_char)
{
  char out_char;
  
  out_char = in_char;
  
  return out_char;
}


char decrypt(char in_char)
{
  char out_char;
  
  out_char = in_char;
  
  return out_char;
}



// the setup routine runs once when you press reset:
void setup()
{
  // set the digital pin as output:
  pinMode(3, OUTPUT); //TRANSMIT LED OUT
  pinMode(2, INPUT); //BUTTON IN
  pinMode(4, OUTPUT); //LED OUT

  pinMode(5, OUTPUT); // rightBottom
  pinMode(6, OUTPUT); // decimalPlace
  pinMode(7, OUTPUT); // bottom
  pinMode(8, OUTPUT); // leftBottom

  pinMode(9, OUTPUT); // leftTop
  pinMode(10, OUTPUT); // centre
  pinMode(11, OUTPUT); //rightTop
  pinMode(12, OUTPUT); //top 

  pinMode(13, OUTPUT); // Buzzer
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

void SDDclearDisplay(){
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);

  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
}

void SDDdisplayOne(){
  digitalWrite(9, HIGH);
  digitalWrite(8, LOW);
}

void SDDdisplayTwo(){
  digitalWrite(12, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(7, HIGH);
}

const long txInterval = 200;              // interval at which to tx bit (milliseconds)
int tx_state = 0;
char tx_char = 'H';
char chr;
unsigned long previousTxMillis = 0;        // will store last time LED was updated

char tx_string[] = "Hello World";
#define TX_START_OF_TEXT  -1
int tx_string_state = TX_START_OF_TEXT;



char getTxChar()
{
  char chr;
  
  switch (tx_string_state)
  {
    case TX_START_OF_TEXT:
    tx_string_state = 0;
    txBuffer[0] = txButton;
    txBuffer[1] = txTilt;
    txBuffer[2] = txPot;
    txBuffer[3] = txA;
    txBuffer[4] = txB;
    txBuffer[5] = txC;
    txBuffer[6] = txD;
    return STX;
    break;
    
    default:
    chr = txBuffer[tx_string_state];
    tx_string_state++;
    if (chr == ETX)  /* End of string? */
    {
      tx_string_state = TX_START_OF_TEXT;  /* Update the tx string state to start sending the string again */
      return ETX;  /* Send End of Text character */
    }
    else
    {
      return chr;  /* Send a character in the string */
    }
    break;
  }
}

void txChar()
{
  unsigned long currentTxMillis = millis();

  if (currentTxMillis - previousTxMillis >= txInterval)
  {
    // save the last time you blinked the LED (improved)
    previousTxMillis = previousTxMillis + txInterval;  // this version catches up with itself if a delay was introduced

    switch (tx_state)
    {
      case 0:
        chr = encrypt(getTxChar());
        digitalWrite(3, HIGH);  /* Transmit Start bit */
        tx_state++;
        break;

      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
        if ((chr & 0x40) != 0)   /* Transmit each bit in turn */
        {
          digitalWrite(3, HIGH);
        }
        else
        {
          digitalWrite(3, LOW);
        }
        chr = chr << 1;  /* Shift left to present the next bit */
        tx_state++;
        break;

      case 8:
        digitalWrite(3, HIGH);  /* Transmit Stop bit */
        tx_state++;
        break;

      default:
        digitalWrite(3, LOW);
        tx_state++;
        if (tx_state > 20) tx_state = 0;  /* Start resending the character */
        break;
    }
  }
}



const long rxInterval = 20;              // interval at which to read bit (milliseconds)
int rx_state = 0;
char rx_char;
unsigned long previousRxMillis = 0;        // will store last time LED was updated
int rx_bits[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


void rxChar()
{
  unsigned long currentRxMillis = millis();
  int sensorValue;
  int i;

  if (currentRxMillis - previousRxMillis >= rxInterval)
  {
    // save the last time you read the analogue input (improved)
    previousRxMillis = previousRxMillis + rxInterval;  // this version catches up with itself if a delay was introduced

    sensorValue = analogRead(A0);
    // Serial.println(sensorValue);
    //Serial.println(rx_state);

    switch (rx_state)
    {
      case 0:
        if (sensorValue >= 900)
        {
          rx_bits[0]++;
          rx_state++;
        }
        break;

      case 100:
        if ((rx_bits[0] >= 6) && (rx_bits[8] >= 6))  /* Valid start and stop bits */
        {
          rx_char = 0;

          for (i = 1; i < 8; i++)
          {
            rx_char = rx_char << 1;
            if (rx_bits[i] >= 6) rx_char = rx_char | 0x01;
          }
          rx_char = decrypt(rx_char);

          Serial.println(rx_char + 0);

          switch (rx_char)
          {
            case STX:
            rx_index = 0;
            break;
            
            case ETX:
            rxButton = rxBuffer[0];
            rxTilt = rxBuffer[1];
            rxPot = rxBuffer[2];
            rxA = rxBuffer[3];
            rxB = rxBuffer[4];
            rxC = rxBuffer[5];
            rxD = rxBuffer[6];
            rx_index = 0;
            break;
            
            default:
            rxBuffer[rx_index] = rx_char;
            rx_index++;
            break;
          }
        }
        else
        {
          Serial.println("Rx error");
        }
       for (i = 0; i < 10; i++)  /* Print the recieved bit on the monitor - debug purposes */
       {
         Serial.println(rx_bits[i]);
       }
        for (i = 0; i < 10; i++)
        {
          rx_bits[i] = 0;
        }
        rx_state = 0;
        break;

      default:
        if (sensorValue >= 900)
        {
          rx_bits[rx_state / 10]++;
        }
        rx_state++;
        break;
    }
  }

}



// the loop routine runs over and over again forever:
void loop()
{
  readInputs();
  txChar();
  rxChar();
  writeOutputs();
}
