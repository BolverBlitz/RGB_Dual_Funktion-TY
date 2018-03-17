#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define LEDS2 10
#define PACKET_SZ ( (LEDS2 * 3) + 3 )

//Dual
unsigned long timestamp;
unsigned long deltaForF2= 50;  // Zeit Funktionsjump (ms)
//Visualizer
unsigned char serial_buffer[PACKET_SZ];
unsigned int head = 0;
unsigned int start;
unsigned int checksum_1;
unsigned int checksum_0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS2, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.show();
  while (!Serial) {
  }
}


void WaveRLLR(byte red, byte green, byte blue, byte red2, byte green2, byte blue2, int Size, int LaufSpeed, int AnderSpeed, int Diff){

//WelleRechts-Links-Links-Rechts
  for(int i = 0; i < LEDS2-Size-2; i++) {
    strip.setPixelColor(i, red/Diff, green/Diff, blue/Diff);
    for(int j = 1; j <= Size; j++) {
      strip.setPixelColor(i+j, red, green, blue);
    }
    strip.show(); 
    delay(LaufSpeed);
  }

  delay(AnderSpeed);

  for(int i = LEDS2-Size-2; i > 0; i--) {
    strip.setPixelColor(i, red2/Diff, green2/Diff, blue2/Diff);
    for(int j = 1; j <= Size; j++) {
      strip.setPixelColor(i+j, red2, green2, blue2);
    }
    strip.setPixelColor(i+Size+1, red2/Diff, green2/Diff, blue2/Diff);
    strip.show(); 
    delay(LaufSpeed);
  }
  delay(AnderSpeed);
}

void EinFarbig(byte red, byte green, byte blue)
{


  for(int i=0;i<LEDS2;i++){
    strip.setPixelColor(i, strip.Color(red,green,blue)); 
  }
      strip.show(); 
     
}
//////////////////////////////////////////////////////////////////////////////
void NebenEffenkt(){
  //EinFarbig(250,50,50); //R,G,B
  WaveRLLR(255,0,255,0,255,0, 10, 10, 0, 5); //WaveRLLR(R,G,B,R,G,B, AugenGröße, LaufGeschwindikeit, WarteZeit, HintergrundHellikeit);
  }
//////////////////////////////////////////////////////////////////////////////
void Visualizer()
{
  if( Serial.available() )
  {
    serial_buffer[head] = Serial.read();
  
    if( head >= (PACKET_SZ - 1) )
    {
      start = 0;
      checksum_1 = head;
      checksum_0 = head - 1;
      head = 0;
    }
    else
    {
      start = head + 1;
      checksum_1 = head;
      if( head == 0 )
      {
        checksum_0 = PACKET_SZ - 1;
      }
      else
      {
        checksum_0 = head - 1;
      }
      head++;
    }
  
    if( serial_buffer[start] == 0xAA )
    {
      unsigned short sum = 0;

      for( int i = 0; i < checksum_0; i++ )
      {
        sum += serial_buffer[i];
      }

      if( start > 0 )
      {
        for( int i = start; i < PACKET_SZ; i++ )
        {
          sum += serial_buffer[i];
        }
      }
      
      //Test if valid write packet
      if( ( ( (unsigned short)serial_buffer[checksum_0] << 8 ) | serial_buffer[checksum_1] ) == sum )
      {
        noInterrupts();
        for( int i = 0; i < LEDS2; i++ )
        {
          int idx = start + 1 + ( 3 * i );
  
          if( idx >= (PACKET_SZ - 1) )
          {
            idx = idx - PACKET_SZ;
          }
          
          strip.setPixelColor(i, strip.Color(serial_buffer[idx], serial_buffer[idx+1], serial_buffer[idx+2]));
        }

        
        strip.show();
        interrupts();
      }
    }
  }
}





void loop() {

  if (Serial.available()>0) {
    while (Serial.available()>0) {
      Visualizer();
    }
    timestamp= millis();
  } else {
    // no serial input available
    if ((millis()-timestamp)>deltaForF2) {
      NebenEffenkt();
    }
  }

}
