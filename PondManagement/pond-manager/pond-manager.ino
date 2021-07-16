// Pond management - this manages a barrel with an input valve (24VAC), a discharge pump (120VAC), a high water float sensor (circuit open when barrel full),
// and a low water float sensor (circuit open when barrel not empty). An additional 24VAC circuit (AUX1) can be used to control a turntable motor
// for an automated fish feeder. The other auxiliary circuit controls 120VAC (AUX2) and is currently unused.
// Commands come from connect Raspberry Pi, either
// drain - run pump for time limit or until barrel empty
// fill - fill barrel until time limit or barrel full

// Relay pins IN1, IN2, IN3, IN4
int pins[] = { 4, 5, 6, 7 };

// Index assignments for relay pins
const int rly_pump = 3;
const int rly_aux2 = 2; // Unused 120VAC
const int rly_valve = 0;
const int rly_aux1 = 1; // 24VAC for fish feeder

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

// Line input buffer
char line_buffer[16];
int line_length = 0;

// Option values
int opt1_FTO = 540; // Was 480s
int opt2_FPOST = 10; // Unchanged
int opt3_DTO = 540; // Was 420s
int opt4_DPOST = 20; // Unchanged
int opt5_AUX1LEN = 2000; // ms
int opt6_AUX2LEN = 2000; // ms

int mode = 0; // 0 = idle, 1 = filling, 2 = draining, 3 = aux1 active, 4 = aux2 active; OR with 0x80 when edge reached

void setup() {
  Serial.begin( 9600 );
  while (!Serial)
    ; // Wait for serial port to connect

  pinMode(input_high, INPUT);
  pinMode(input_low, INPUT);

  pinMode(pins[rly_pump], OUTPUT);
  pinMode(pins[rly_aux1], OUTPUT);
  pinMode(pins[rly_valve], OUTPUT);
  pinMode(pins[rly_aux2], OUTPUT);
  digitalWrite(pins[rly_pump], HIGH);
  digitalWrite(pins[rly_aux1], HIGH);
  digitalWrite(pins[rly_valve], HIGH);
  digitalWrite(pins[rly_aux2], HIGH);
  Serial.println("Setup complete");
}

void loop() {
  // Check for available characters
  unsigned long curTime = millis();
  int input_chars = Serial.available();
  int line_complete = 0;
  if (input_chars > 0)
  {
    while (input_chars > 0 && line_length < 15 && !line_complete)
    {
      input_chars--;
      int cmdIn = Serial.read();
      switch (cmdIn)
      {
        case 0x08:
          if (line_length > 0) {
            line_length--;
            line_buffer[line_length] = 0;
          }
          break;
        case '\r':
        case ' ':
        case '\t':
          // Silently ignore
          break;
        case '\n':
          line_complete = 1;
          break;
        default:
          line_buffer[line_length] = cmdIn;
          line_length++;
          line_buffer[line_length] = 0;
          break;
      }
    }
    //Serial.println("Complete:" + String(line_complete) + " len:" + String(line_length) + " >" + line_buffer);
  } // Input available

  int elapsed_m = curTime - last_time;
  int elapsed_s = elapsed_m / 1000;
  // Process only on second elapsed unless mode 3 or greater
  if (last_time != 0 && curTime >= last_time && elapsed_s < 1 && line_complete == 0 && (mode & 0x7f) < 3)
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
    Serial.println("@" + String(curTime / 1000) + " M:" + String(mode) + " H:" + String(high_state) + " L:" + String(low_state));
    last_high_state = high_state;
    last_low_state = low_state;
  }
  else if (force_pin_high >= 0)
  {
    Serial.println("FPH: " + String(force_pin_high));
    delay(50);
    digitalWrite(pins[(force_pin_high + 3) % 4], HIGH);
    delay(50);
    digitalWrite(pins[(force_pin_high + 2) % 4], HIGH);
    delay(50);
    digitalWrite(pins[(force_pin_high + 1) % 4], HIGH);
    delay(50);
    digitalWrite(pins[(force_pin_high + 0) % 4], HIGH);
    force_pin_high = -1;
  }
  if (line_complete && line_length > 0)
  {
    int last_mode = mode;
    // Commands are f, d, c, 1, 2, s
    // Options are '$' <n> '=' <value>
    switch (line_buffer[0]) {
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
      case '1':
          Serial.println("Starting aux1");
          mode = 3;
          force_display = 1;
          mode_start = millis();
          break;
      case '2':
          Serial.println("Starting aux2");
          mode = 4;
          force_display = 1;
          mode_start = millis();
          break;
      case 's':
          force_display = 1;
          break;
      // Handle options
      case '$':
          if (line_length < 5)
          {
            Serial.println("Invalid opt fmt");
          }
          else
          {
            // One might thing that &line_buffer[3] would be the string starting from the number,
            // but not so...
            switch (line_buffer[1]) {
              case '1': // FTO
                opt1_FTO = String(line_buffer).substring(3).toInt();
                break;
              case '2': // FPOST
                opt2_FPOST = String(line_buffer).substring(3).toInt();
                break;
              case '3': // DTO
                opt3_DTO = String(line_buffer).substring(3).toInt();
                break;
              case '4': // DPOST
                opt4_DPOST = String(line_buffer).substring(3).toInt();
                break;
              case '5': // AUX1LEN
                opt5_AUX1LEN = String(line_buffer).substring(3).toInt();
                Serial.println("AUX1 len " + String(opt5_AUX1LEN) + " <" + line_buffer);
                break;
              case '6': // AUX2LEN
                opt6_AUX2LEN = String(line_buffer).substring(3).toInt();
                break;
              default:
                Serial.println("Invalid opt");
                break;
            }
          }
          break;
      default:
        Serial.println("Invalid cmd");
        break;
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
        case 3:
          delay(50);
          digitalWrite(pins[rly_aux1], LOW);
          break;
        case 4:
          delay(50);
          digitalWrite(pins[rly_aux2], LOW);
          break;
      }
    }
  }
  line_complete = 0;
  line_length = 0;
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
      else if (mode_elapsed < 0 || mode_elapsed >= opt1_FTO * 1000)
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
      else if (mode_elapsed < 0 || mode_elapsed >= opt3_DTO * 1000)
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
    case 3: // AUX1 
      if (mode_elapsed >= opt5_AUX1LEN)
      {
         Serial.println("AUX1 complete " + String(mode_elapsed / 1000) + " L:" + String(opt5_AUX1LEN));
         delay(150);
         digitalWrite(pins[rly_aux1], HIGH);
         delay(250);
         digitalWrite(pins[rly_aux1], HIGH);
         force_pin_high = rly_aux1;
         last_active_display = millis();
         mode = 0;
         force_display = 1;
      }
      break;
    case 4: // AUX2
      if (mode_elapsed >= opt6_AUX2LEN)
      {
         Serial.println("AUX2 complete " + String(mode_elapsed / 1000));
         delay(150);
         digitalWrite(pins[rly_aux2], HIGH);
         delay(250);
         digitalWrite(pins[rly_aux2], HIGH);
         force_pin_high = rly_aux2;
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
      if (edge_elapsed < 0 || edge_elapsed >= opt2_FPOST * 1000)
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
      if (edge_elapsed < 0 || edge_elapsed >= opt4_DPOST * 1000)
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
