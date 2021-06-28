// Pond management - this manages a barrel with an input valve (24VAC), a discharge pump (120VAC), a high water float sensor (circuit open when barrel full),
// and a low water float sensor (circuit open when barrel not empty).
// Commands come from connect Raspberry Pi, either
// drain - run pump for time limit or until barrel empty
// fill - fill barrel until time limit or barrel full

// Relay pins IN1, IN2, IN3, IN4
int pins[] = { 4, 5, 6, 7 };

// Index assignments for relay pins
const int rly_pump = 3;
const int rly_valve = 0;

// Input pins. These are wired with a loop from +5V to the pin and on the pin side, through a 10k resistor to ground
const int input_high = 8;
const int input_low = 9;

unsigned long last_time = 0;
unsigned long mode_start = 0;
unsigned long edge_start = 0;
int last_high_state = -1;
int last_low_state = -1;
unsigned long last_active_display = 0;
int force_display = 0;
int force_pin_high = -1;

int mode = 0; // 0 = idle, 1 = filling, 2 = draining; OR with 0x80 when edge reached

void setup() {
  Serial.begin( 9600 );
  while (!Serial)
    ; // Wait for serial port to connect

  pinMode(input_high, INPUT);
  pinMode(input_low, INPUT);

  pinMode(pins[rly_pump], OUTPUT);
  pinMode(pins[rly_valve], OUTPUT);
  digitalWrite(pins[rly_pump], HIGH);
  digitalWrite(pins[rly_valve], HIGH);
  Serial.println("Setup complete");
}

void loop() {
  unsigned long curTime = millis();
  int elapsed_s = (curTime - last_time) / 1000;
  if (last_time != 0 && curTime >= last_time && elapsed_s < 1)
  {
    return;
  }
  last_time = curTime;
  int high_state = digitalRead(input_high);
  int low_state = digitalRead(input_low);
  // 1 = open, 0 = closed
  if (force_display != 0 || last_high_state != high_state || last_low_state != low_state)
  {
    force_display = 0;
    Serial.println("@" + String(curTime/1000) + " M:" + String(mode) + " H:" + String(high_state) + " L:" + String(low_state));
    last_high_state = high_state;
    last_low_state = low_state;
  }
  else if (force_pin_high >= 0)
  {
    Serial.println("FPH: " + String(force_pin_high));
    delay(50);
    digitalWrite(pins[(force_pin_high+3)%4], HIGH);
    delay(50);
    digitalWrite(pins[(force_pin_high+2)%4], HIGH);
    delay(50);
    digitalWrite(pins[(force_pin_high+1)%4], HIGH);
    delay(50);
    digitalWrite(pins[(force_pin_high+0)%4], HIGH);
    force_pin_high = -1;
  }
  int input_chars = Serial.available();
  if (input_chars > 0)
  {
    int last_mode = mode;
    while (input_chars > 0)
    {
      input_chars--;
      int cmdIn = Serial.read();
      switch (cmdIn)
      {
        case 'f':
          Serial.println("Starting fill");
          mode = 1;
          mode_start = millis();
          break;
        case 'd':
          Serial.println("Starting drain");
          mode = 2;
          mode_start = millis();
          break;
        case 'c':
          Serial.println("Cancelling");
          mode = 0;  
          force_display = 1;
          delay(50);
          digitalWrite(pins[rly_pump], HIGH);
          delay(50);
          digitalWrite(pins[rly_valve], HIGH);
          break;
        case '\r':
        case '\n':
        case ' ':
        case '\t':
          // Silently ignore
          break;
        default:
          Serial.println("Ignoring: " + cmdIn);
          break;
      }
    }
    if (mode != last_mode)
    {
      last_active_display = millis();
      switch (mode)
      {
        case 1:
          delay(50);
          digitalWrite(pins[rly_pump], LOW);
          break;
        case 2:
          delay(50);
          digitalWrite(pins[rly_valve], LOW);
          break;
      }
    }
  }
  if (mode == 0)
  {
    return;
  }
  long mode_elapsed = millis() - mode_start;
  switch (mode)
  {
    case 1: // Filling, edge not reached
      if (high_state == 1)
      {
        mode |= 0x80;
        edge_start = millis();
        Serial.println("HE @" + String(mode_elapsed / 1000));
        last_active_display = edge_start;
      }
      // Maximum 8 minutes to fill
      else if (mode_elapsed < 0 || mode_elapsed >= 480000)
      {
        Serial.println("FTO elapsed " + String(mode_elapsed / 1000));
        delay(150);
        digitalWrite(pins[rly_valve], HIGH);
        delay(250);
        digitalWrite(pins[rly_valve], HIGH);
        force_pin_high = rly_valve;
        last_active_display = millis();
        mode = 0;
        force_display = 1;
      }
      break;
    case 2: // Draining, edge not reached
      if (low_state == 0)
      {
        mode |= 0x80;
        edge_start = millis();
        Serial.println("LE @" + String(mode_elapsed / 1000));
        last_active_display = edge_start;
      }
      // Maximum 7 minutes to drain.
      else if (mode_elapsed < 0 || mode_elapsed >= 420000)
      {
        Serial.println("DTO elapsed " + String(mode_elapsed / 1000));
        delay(150);
        digitalWrite(pins[rly_pump], HIGH);
        delay(250);
        digitalWrite(pins[rly_pump], HIGH);
        force_pin_high = rly_pump;
        last_active_display = millis();
        mode = 0;
        force_display = 1;
      }
      break;
  }
  long edge_elapsed = millis() - edge_start;
  switch (mode)
  {
    case 0x81: // Filling, edge reached
      if (edge_elapsed < 0 || edge_elapsed >= 10000)
      {
        Serial.println("FILCOMP @" + String(mode_elapsed / 1000) + "," + String(edge_elapsed / 1000));
        last_active_display = millis();
        delay(150);
        digitalWrite(pins[rly_valve], HIGH);
        delay(250);
        digitalWrite(pins[rly_valve], HIGH);
        force_pin_high = rly_valve;
        mode = 0;
        force_display = 1;
      }
      break;
    case 0x82: // Draining, edge reached
      if (edge_elapsed < 0 || edge_elapsed >= 20000)
      {
        Serial.println("DRNCOMP @" + String(mode_elapsed / 1000) + "," + String(edge_elapsed / 1000));
        last_active_display = millis();
        delay(150);
        digitalWrite(pins[rly_pump], HIGH);
        delay(250);
        digitalWrite(pins[rly_pump], HIGH);
        force_pin_high = rly_pump;
        mode = 0;
        force_display = 1;
      }
  }
  // Provide an update every 30s while filling / draining
  if (millis() - last_active_display >= 30000)
  {
    Serial.println("@" + String(mode_elapsed / 1000) + " M:" + String(mode) + " H:" + String(high_state) + " L:" + String(low_state));
    last_active_display = millis();
  }
}
