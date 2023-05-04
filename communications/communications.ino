/*
  Communications v4

  Transmits data using a white LED and recieves it using a photoresistor

*/


int ledState = LOW;             // ledState used to set the LED

#define STX 0x70
#define ETX 0x71
#define TX_START_OF_TEXT  -1

int tx_buffer_state = TX_START_OF_TEXT;
const long txInterval = 200;              // interval at which to tx bit (milliseconds)
int tx_state = 0;
char chr;
unsigned long previousTxMillis = 0;        // will store last time LED was updated

char tx_buffer[8] = {0,0,0,0,0,0,0,ETX};
char rx_buffer[7];
int rx_index;
char txButton, txTilt, txPot, txA, txB, txC, txD;
char rxTilt, rxPot, rxA, rxB, rxC, rxD;
char rxButton = 0;
const int buttonPin = 2;

const long rxInterval = 20;              // interval at which to read bit (milliseconds)
int rx_state = 0;
char rx_char;
unsigned long previousRxMillis = 0;        // will store last time LED was updated
int rx_bits[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void readInputs(){
    int buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH)
    {
      txButton = 1;
      //Serial.println("Button Press");
    }

    
  
}

void setOutputs(){
  if (rxButton == 1)
  {
    digitalWrite(4, HIGH);
  }
  Serial.print(rxButton);
  
}

char encrypt(char in_char)
{
  char out_char;
  
  out_char = XOREncryptDecrypt(in_char);
  
  return out_char;
}

char decrypt(char in_char)
{
  char out_char;
  
  out_char = XOREncryptDecrypt(in_char);
  
  return out_char;
}

char XOREncryptDecrypt(char in_char)
{
    char xorKey = 'a';
    char out = in_char ^ xorKey;
    return out;
}

// the setup routine runs once when you press reset:
void setup()
{
  // set the digital pin as output:
  pinMode(3, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(4, OUTPUT);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

char getTxChar()
{
  char chr;
  
  switch (tx_buffer_state)
  {
    case TX_START_OF_TEXT:
    //Serial.println("Button: " + txButton);
    tx_buffer_state = 0;
    tx_buffer[0] = txButton;
    tx_buffer[1] = txTilt;
    tx_buffer[2] = txPot;
    tx_buffer[3] = txA;
    tx_buffer[4] = txB;
    tx_buffer[5] = txC;
    tx_buffer[6] = txD;
    return STX;
    break;
    
    default:
    chr = tx_buffer[tx_buffer_state];
    tx_buffer_state++;
    if (chr == ETX)  /* End of string? */
    {
      tx_buffer_state = TX_START_OF_TEXT;  /* Update the tx string state to start sending the string again */
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
          switch (rx_char)
          {
            case STX:
            rx_index = 0;
            Serial.println("New Transmission");
            break;
            
            case ETX:
            rxButton = rx_buffer[0];
            //Serial.println("rxButton: " + rxButton);
            rxTilt = rx_buffer[1];
            rxPot = rx_buffer[2];
            rxA = rx_buffer[3];
            rxB = rx_buffer[4];
            rxC = rx_buffer[5];
            rxD = rx_buffer[6];
            rx_index = 0;
            break;
            
            default:
            rx_buffer[rx_index] = rx_char;
            rx_index++;
            break;
          }
        }
        else
        {
          Serial.println("Rx error");
        }
      //  for (i = 0; i < 10; i++)  /* Print the recieved bit on the monitor - debug purposes */
      //  {
      //    Serial.println(rx_bits[i]);
      //  }
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
    setOutputs();
}