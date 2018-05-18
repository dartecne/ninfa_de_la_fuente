// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.


#include <Servo.h> 

#define RIGHT  true
#define LEFT  false
 
#define L_GREEN  107
#define ZERO_GREEN  87  /* En frio 87, en caliente 85 */
#define R_GREEN  67
#define PIN_GREEN  9

#define R_ORANGE  35
#define ZERO_ORANGE  15   /* En frio 15, en caliente 14 */
#define L_ORANGE  -5
#define PIN_ORANGE  10

#define R_LED  6
#define L_LED  5
#define MICRO  5
#define BUTTON  13

Servo servo_green;  
Servo servo_orange;  

#define IDLE  0  // Waiting for button
#define SLOW  1  // 1- Lento hacia fuerta
                // 2- Lento hacia dentro
#define HIP_HOP  2  // 3- Entrecortados uniforme hacia fuera (1 giro completo)
#define RANDOM  3 //   4- Entrecortados aleatorio


int state = IDLE; 
boolean button = false;

void setup() { 
  
  pinMode( R_LED, OUTPUT );
  pinMode( L_LED, OUTPUT );
  
  randomSeed( analogRead(0) );

  Serial.begin( 9600 );
  Serial.print( "Init green = " );
  Serial.println( servo_green.read() );
  Serial.print( "Init orange = " );
  Serial.println( servo_orange.read() );
  
  delay(1000);
  
/*  for( unsigned int i = 0; i < 10; i++ ) 
    hip_hop();
  */
} 

void enable_servos() {
  servo_green.attach( PIN_GREEN );
  servo_orange.attach( PIN_ORANGE );
  servo_green.write( ZERO_GREEN );
  servo_orange.write( ZERO_ORANGE );

}

void disable_servos() {
  servo_green.detach();
  servo_orange.detach();
  
}

void slow() {
  move_in( 200, 0 );  // Lento hacia fuera
  delay( 5000 );
  move_out( 200, 0 );  // Lento hacia fuera
  delay( 5000 );
}

void hip_hop_rand() {
  long v_g = random(10);
  long p_g = random(500);
  long v_o = random(10);
  long p_o = random(500);

  v_g = map( v_g, 0, 10, L_GREEN, R_GREEN );
  v_o = map( v_o, 0, 10, L_ORANGE, R_ORANGE );
  servo_green.write( v_g );
  servo_orange.write( v_o ); 
  analogWrite( R_LED, random(255) ); 
  analogWrite( L_LED, random(255)); 
  if( (p_g != 0) || ( p_o != 0)  ) {
    delay( p_g );
    analogWrite( R_LED, 0 ); 
    analogWrite( L_LED, 0 ); 
    pause_servos( p_o );
  }
  check_button();
}

void hip_hop() {
  for( unsigned int i = 0; i < 10 ; i++ ) {
    move_out( 200, 500 );  // Lento hacia fuera
  }
  for( unsigned int i = 0; i < 10 ; i++ ) {
    move_in( 200, 500 );  // Lento hacia dentro
  }
}

void check_button() {
  long init_time = millis();
  while( ( millis() - init_time ) < 200 ) {
    if( digitalRead( BUTTON ))  {
      button = !button;
      delay(1000);
      return;
    }
  }
}

int r_led_value = 0;  // for right led
int l_led_value = 0;  // for left led
int led_value = 0;  // for both
int tau = 1;
int t = 0;
unsigned long T = 20 / tau;

void loop() {
  check_button();
  modify_led_values();
  Serial.print( "button = " ); Serial.println( button );
  Serial.print( "state = " ); Serial.println( state );
  if( state == IDLE && button )  {
    enable_servos();
    state = SLOW;
  } else if( (state == IDLE) && !button ) {
    analogWrite( R_LED, led_value );
    analogWrite( L_LED, led_value );
  } else if( (state == SLOW) && button ) {
    slow();
    state = HIP_HOP;
  } else if( (state == SLOW) && !button ) {
    state = IDLE;
    disable_servos();
  } else if( (state == HIP_HOP) && button ) {
    hip_hop();
    state = RANDOM;
  } else if( (state == HIP_HOP) && !button ) {
    state = IDLE;
    disable_servos();
  } else if( (state == RANDOM) && button ) {
    hip_hop_rand();
  } else if( (state == RANDOM) && !button ) {
    state = IDLE;
    disable_servos();
  }
} 

void modify_led_values() {
  led_value = 255 * ( 1 + sin( 2 * PI * t / T) ) / 2;
  t += tau; // count time passing
  if( t/T >= 1 )  // handle if time pass one period and if so initialize time
    t = 0;
}

void pause_servos( int dur ) {
 disable_servos(); 
 delay( dur ); 
 enable_servos();
 servo_green.write( ZERO_GREEN );
 servo_orange.write( ZERO_ORANGE );
}

/* move from 0 to 10 */
void move_out( int pos, int pause ) {
    int g = map( pos, 0, 1000, ZERO_GREEN, R_GREEN );
    int o = map( pos, 0, 1000, ZERO_ORANGE, L_ORANGE );
    int l = map( pos, 0, 1000, 0, 255  );
    servo_green.write( g );
    servo_orange.write( o );
    analogWrite( R_LED, l );
    analogWrite( L_LED, l );
    if( pause != 0 ) {
      delay( pause );
      stop_all();
      delay( pause );
    }
}

/* move from 0 to 10 */
void move_in( int pos, int pause ) {
    int g = map( pos, 0, 1000, ZERO_GREEN, L_GREEN );
    int o = map( pos, 0, 1000, ZERO_ORANGE, R_ORANGE );
    int l = map( pos, 0, 1000, 0, 255  );
    servo_green.write( g );
    servo_orange.write( o );
    analogWrite( R_LED, l );
    analogWrite( L_LED, l );
    if( pause != 0 ) {
      delay( pause );
      stop_all();
      delay( pause );
    }
}

void vibra( int n, int dur, boolean mirror ) {
  for( unsigned int i = 0; i < n; i++ ) {  
    servo_green.write( L_GREEN );
    if(mirror) servo_orange.write( R_ORANGE );
    else servo_orange.write( L_ORANGE );
    digitalWrite( R_LED, HIGH );
    digitalWrite( L_LED, HIGH );
    delay( dur );
    servo_green.write( R_GREEN );
    if(mirror) servo_orange.write( L_ORANGE );
    else servo_orange.write( R_ORANGE );
    digitalWrite( R_LED, LOW );
    digitalWrite( L_LED, LOW );
    delay( dur );
  }
}

void stop_all() {
 servo_green.write( ZERO_GREEN );
 servo_orange.write( ZERO_ORANGE );
 blink( 100 );
 blink( 200 );
}

void blink( int del ) {
 digitalWrite( R_LED, HIGH );
 digitalWrite( L_LED, HIGH );
 delay( del );
 digitalWrite( R_LED, LOW );
 digitalWrite( L_LED, LOW );
  
}
