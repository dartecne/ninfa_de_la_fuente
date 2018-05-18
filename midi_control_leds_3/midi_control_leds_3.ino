/**
@brief 

 */

#include <PololuLedStrip.h>

#define LED_COUNT 60

#define CONTROL_CHANGE  0xB0
#define CC  CONTROL_CHANGE
#define NOTE_ON  0x90
#define NOTE_OFF  0x80
#define BPM  0xC0

#define SWITCH  7
#define COIN 0
#define LED 13

#define NUM_TRACKS  7

#define AMBIENT  0
#define TRACK  1
#define TRACK_INTERRUPT  2


int track[NUM_TRACKS];
unsigned long times[NUM_TRACKS];
unsigned long init_time;
int threshold;
int state = 0;
int song_index = 0;

PololuLedStrip<12> ledStrip;
rgb_color colors[LED_COUNT];
rgb_color color;
rgb_color ray;

/**
 \brief Each time Switch is on send a MIDI msg for playing the next track
*/
void config_midi() {
  for( unsigned int i = 0; i < NUM_TRACKS; i++ ) {
    while( !test_switch() )
      play_track( i );
    delay(1000);
  }
}


void test_tracks() {
  for( unsigned int i = 0; i < NUM_TRACKS; i++ ) {
    play_track( i );
    delay(3000);
  }
}

void setup() {
  //  Set MIDI baud rate:
  Serial.begin(31250);

  pinMode( SWITCH, INPUT );
  pinMode( LED, OUTPUT );
  
  times[0] = 1000;  /* duration of track_ambiente. No se usa */
  times[1] = 149000;  /* duration of track-1 : 2m29s */
  times[2] = 88000;  /* duration of track-2 : 1m28s */
  times[3] = 121000;  /* duration of track-3 : 2m01s */
  times[4] = 127000;  /* duration of track-4 : 2m07s */
  times[5] = 115000;  /* duration of track-5 : 1m55s */
  times[6] = 102000;  /* duration of track-6 : 1m42s  */
  for( unsigned int i = 0; i < NUM_TRACKS; i++ ) {
    track[i] = (i + 1) * 10;
//    times[i] = (i + 1) * 1000;
  } 
  
  init_colors();
  
  ray.red = 0;
  ray.green = 0;
  ray.blue = 0;

    
  state = AMBIENT;
  
    delay(1000);
    Serial.print( "state = " ); Serial.print( state );
    Serial.println( "  Playing track 0" );
    
    play_track( 0 );
//    test_tracks();
    delay(1000); 
    
//  config_midi();
//    while(1);
}

boolean switch_on = false;
boolean coin_in = false;  

int Ar = 248 / LED_COUNT;  
int Ag = 84 / LED_COUNT;
int Ab = 220 / LED_COUNT;



void loop() {

    switch_on = test_switch();
    coin_in = new_coin_handle();

    if( (state == AMBIENT) & ( switch_on | coin_in ) ) {
//    if( (state == AMBIENT) & switch_on  ) {
//    if( (state == AMBIENT) & coin_in  ) {
      state = TRACK;
      play_track( 0 );
      init_time = millis();
      switch_on = false;
      coin_in = false;
      
    } else if( (state == TRACK) & test_time() ) {
      state = AMBIENT;
      song_index ++;
      if( song_index >= NUM_TRACKS ) song_index = 1;
      play_track( song_index );
    }

    // Write the colors to the LED strip.
    ledStrip.write(colors, LED_COUNT);
    shift_colors();

    delay(40);
}

boolean test_switch() {
  if( digitalRead( SWITCH ) ) {
    digitalWrite( LED, HIGH );
    delay(100);
    digitalWrite( LED, LOW );
    return true;
  }
  else
     return false;     
}

int valOld = 0;

boolean new_coin_handle() {
  int val = analogRead( COIN );
//  unsigned int thres = analogRead( A4 );
//  threshold = thres / 32;
  threshold = 20;
  
  if( abs( val - valOld) >= threshold ) {
    digitalWrite( LED, HIGH );
    /**/
    for( unsigned int i = 0; i < 20; i++ ) {
      set_ray();
      ledStrip.write(colors, LED_COUNT);
      delay(20);
    }
   /* */
//    delay(1000);
    digitalWrite( LED, LOW );
    init_colors();
    ledStrip.write(colors, LED_COUNT);
    valOld = analogRead( COIN );
    return true;
  }
  valOld = val;
  return false;
}

void shift_colors() {
  colors[LED_COUNT-1] = colors[0];
  for(uint16_t i = 0; i < (LED_COUNT-1); i++)  {
    colors[i] = colors[i+1];
  }  
}

void set_ray() {
  
  for(uint16_t i = 0; i < LED_COUNT; i++)  {
    if(random(2))
      colors[i] = ray;
    else
      colors[i] = (rgb_color){255, 255, 255 };  
  }  
}

void init_colors() {
  for(uint16_t i = 0; i < LED_COUNT; i++)  {
    color.red = Ar * i;
    if(i < LED_COUNT/2) 
        color.green = 255* 4 * i / LED_COUNT;
      else if( i < (LED_COUNT * 3 / 4))
        color.green = - 255 * 8 * i / (3 * LED_COUNT) + 255;
      else if( i < LED_COUNT )
        color.green = 84 * 8 * i / LED_COUNT;          

    color.blue = 220;
    colors[i] = color;
  }  
}

boolean test_time() {
   if( ( millis() - init_time ) > times[song_index] ) return true;
   else return false; 
}


void play_track( unsigned int id ) {
    Serial.write( 1 );
    Serial.write( NOTE_ON );
    Serial.write( track[id] );
}


// Converts a color from HSV to RGB.
// h is hue, as a number between 0 and 360.
// s is the saturation, as a number between 0 and 255.
// v is the value, as a number between 0 and 255.
rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return (rgb_color){r, g, b};
}

