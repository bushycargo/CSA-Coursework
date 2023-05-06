/*
  Communications v5

  Transmits data using a white LED and recieves it using a photoresistor

*/

#define STX 0x70
#define ETX 0x71

char txButton, txTilt, txPot, txA, txB, txC, txD;
char txBuffer[8] = {0, 0, 0, 0, 0, 0, 0, ETX};
char rxBuffer[7];
char rxButton, rxTilt, rxPot, rxA, rxB, rxC, rxD;
int rx_index;
int rightBottom, decimalPlace, bottom, leftBottom, leftTop, centre, rightTop, top;
int trigPin = 17;
int echoPin = 18;

    
void readInputs()
{
  // Reads the inputs in the mini-projects
  // Uses txButton, txTilt, txPot, txA, txB, txC, txD;
  int buttonState = digitalRead(2);
  int potValue = analogRead(A1);
  int tiltState = digitalRead(1);

  digitalWrite(trigPin, LOW);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  int duration = pulseIn(echoPin, HIGH);
  int distance = (duration*0.0343)/2;

  txA = distance;

  // Due to it being a 1 digit display get the first digit and use that assuming all digits in 100s
  if (potValue > 999) // if the potValue is 1000 or higher assume it is 999
  {
    txPot = 9;
  }
  else
  {
    while (potValue >= 10) // Gets just the first digit of potValue
    {
      potValue /= 10;
    }
    txPot = potValue;
  }

  if (buttonState == HIGH)
  {
    txButton = 1;
  }
  if (tiltState == HIGH)
  {
    txTilt = 1;
  }
}

void writeOutputs()
{
  // Writes the outputs in the mini-projects
  // Uses rxButton, rxTilt, rxPot, rxA, rxB, rxC, rxD;
  SDDclearDisplay();
  int distance = rxA;
  if (rxA < 5)
  {
    digitalWrite(16, HIGH);
  }
  else
  {
    digitalWrite(16, LOW);
  }
  
  if (rxButton == 1)
  {
    tone(13, 1000);
  }
  if (rxTilt == 1)
  {
    digitalWrite(4, HIGH);
  }

  switch (rxPot)
  {
  case 1:
    SDDdisplayOne();
    break;

  case 2:
    SDDdisplayTwo();
    break;

  case 3:
    SDDdisplayThree();
    break;

  case 4:
    SDDdisplayFour();
    break;

  case 5:
    SDDdisplayFive();
    break;

  case 6:
    SDDdisplaySix();
    break;

  case 7:
    SDDdisplaySeven();
    break;

  case 8:
    SDDdisplayEight();
    break;

  case 9:
    SDDdisplayNine();
    break;

  default:
    SDDdisplayZero();
    break;
  }
}

int ledState = LOW; // ledState used to set the LED

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
  pinMode(3, OUTPUT);         // TRANSMIT LED OUT
  pinMode(2, INPUT);          // BUTTON IN
  pinMode(4, OUTPUT);         // LED OUT
  pinMode(1, INPUT);          // Tilt Switch
  pinMode(13, OUTPUT);        // Buzzer
  pinMode(16, OUTPUT);        // Motor
  pinMode(trigPin, OUTPUT);    // TRIG
  pinMode(echoPin, INPUT); // ECHO

  rightBottom = 5;
  decimalPlace = 6;
  bottom = 7;
  leftBottom = 8;
  leftTop = 9;
  centre = 10;
  rightTop = 11;
  top = 12;

  for (int i = 5; i < 13; i++) // sets pinMode to output for pins 5-12
  {
    pinMode(i, OUTPUT);
  }

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

void SDDclearDisplay()
{
  digitalWrite(rightBottom, LOW);
  digitalWrite(decimalPlace, LOW);
  digitalWrite(bottom, LOW);
  digitalWrite(leftBottom, LOW);

  digitalWrite(leftTop, LOW);
  digitalWrite(centre, LOW);
  digitalWrite(rightTop, LOW);
  digitalWrite(rightBottom, LOW);
}

void SDDdisplayZero()
{
  digitalWrite(leftBottom, HIGH);
  digitalWrite(leftTop, HIGH);
  digitalWrite(top, HIGH);
  digitalWrite(rightBottom, HIGH);
  digitalWrite(rightTop, HIGH);
  digitalWrite(bottom, HIGH);
}

void SDDdisplayOne()
{
  digitalWrite(leftBottom, HIGH);
  digitalWrite(leftTop, HIGH);
}

void SDDdisplayTwo()
{
  digitalWrite(top, HIGH);
  digitalWrite(rightTop, HIGH);
  digitalWrite(centre, HIGH);
  digitalWrite(leftBottom, HIGH);
  digitalWrite(bottom, HIGH);
}

void SDDdisplayThree()
{
  digitalWrite(top, HIGH);
  digitalWrite(centre, HIGH);
  digitalWrite(bottom, HIGH);
  digitalWrite(rightBottom, HIGH);
  digitalWrite(rightTop, HIGH);
}

void SDDdisplayFour()
{
  digitalWrite(leftTop, HIGH);
  digitalWrite(rightTop, HIGH);
  digitalWrite(rightBottom, HIGH);
  digitalWrite(centre, HIGH);
}

void SDDdisplayFive()
{
  digitalWrite(top, HIGH);
  digitalWrite(leftTop, HIGH);
  digitalWrite(centre, HIGH);
  digitalWrite(rightBottom, HIGH);
  digitalWrite(bottom, HIGH);
}

void SDDdisplaySix()
{
  digitalWrite(top, HIGH);
  digitalWrite(leftTop, HIGH);
  digitalWrite(leftBottom, HIGH);
  digitalWrite(centre, HIGH);
  digitalWrite(rightBottom, HIGH);
  digitalWrite(bottom, HIGH);
}

void SDDdisplaySeven()
{
  digitalWrite(top, HIGH);
  digitalWrite(rightBottom, HIGH);
  digitalWrite(rightTop, HIGH);
}

void SDDdisplayEight()
{
  digitalWrite(rightBottom, HIGH);
  digitalWrite(bottom, HIGH);
  digitalWrite(leftBottom, HIGH);
  digitalWrite(leftTop, HIGH);
  digitalWrite(centre, HIGH);
  digitalWrite(rightTop, HIGH);
  digitalWrite(rightBottom, HIGH);
}

void SDDdisplayNine()
{
  digitalWrite(top, HIGH);
  digitalWrite(leftTop, HIGH);
  digitalWrite(centre, HIGH);
  digitalWrite(rightBottom, HIGH);
  digitalWrite(rightTop, HIGH);
}

const long txInterval = 200; // interval at which to tx bit (milliseconds)
int tx_state = 0;
char tx_char = 'H';
char chr;
unsigned long previousTxMillis = 0; // will store last time LED was updated

char tx_string[] = "Hello World";
#define TX_START_OF_TEXT -1
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
    if (chr == ETX) /* End of string? */
    {
      tx_string_state = TX_START_OF_TEXT; /* Update the tx string state to start sending the string again */
      return ETX;                         /* Send End of Text character */
    }
    else
    {
      return chr; /* Send a character in the string */
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
    previousTxMillis = previousTxMillis + txInterval; // this version catches up with itself if a delay was introduced

    switch (tx_state)
    {
    case 0:
      chr = encrypt(getTxChar());
      digitalWrite(3, HIGH); /* Transmit Start bit */
      tx_state++;
      break;

    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
      if ((chr & 0x40) != 0) /* Transmit each bit in turn */
      {
        digitalWrite(3, HIGH);
      }
      else
      {
        digitalWrite(3, LOW);
      }
      chr = chr << 1; /* Shift left to present the next bit */
      tx_state++;
      break;

    case 8:
      digitalWrite(3, HIGH); /* Transmit Stop bit */
      tx_state++;
      break;

    default:
      digitalWrite(3, LOW);
      tx_state++;
      if (tx_state > 20)
        tx_state = 0; /* Start resending the character */
      break;
    }
  }
}

const long rxInterval = 20; // interval at which to read bit (milliseconds)
int rx_state = 0;
char rx_char;
unsigned long previousRxMillis = 0; // will store last time LED was updated
int rx_bits[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void rxChar()
{
  unsigned long currentRxMillis = millis();
  int sensorValue;
  int i;

  if (currentRxMillis - previousRxMillis >= rxInterval)
  {
    // save the last time you read the analogue input (improved)
    previousRxMillis = previousRxMillis + rxInterval; // this version catches up with itself if a delay was introduced

    sensorValue = analogRead(A0);
    // Serial.println(sensorValue);
    // Serial.println(rx_state);

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
      if ((rx_bits[0] >= 6) && (rx_bits[8] >= 6)) /* Valid start and stop bits */
      {
        rx_char = 0;

        for (i = 1; i < 8; i++)
        {
          rx_char = rx_char << 1;
          if (rx_bits[i] >= 6)
            rx_char = rx_char | 0x01;
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
      for (i = 0; i < 10; i++) /* Print the recieved bit on the monitor - debug purposes */
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
