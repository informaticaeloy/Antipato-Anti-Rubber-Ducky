#include "usbhub.h"
#include "pgmstrings.h"


/// LCD
#include <Wire.h>
#include "LCD.h"
#include "LiquidCrystal_I2C.h"

#define I2C_ADDR    0x27

LiquidCrystal_I2C             lcd(I2C_ADDR,2, 1, 0, 4, 5, 6, 7);
/// FIN LCD


/// BOTONES
    int boton1 = LOW;
    int boton2 = LOW;
/// FIN BOTONES

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB     Usb;
//USBHub  Hub1(&Usb);
//USBHub  Hub2(&Usb);
//USBHub  Hub3(&Usb);
//USBHub  Hub4(&Usb);
//USBHub  Hub5(&Usb);
//USBHub  Hub6(&Usb);
//USBHub  Hub7(&Usb);

void PrintAllAddresses(UsbDevice *pdev)
{
  UsbDeviceAddress adr;
  adr.devAddress = pdev->address.devAddress;

}

void PrintAddress(uint8_t addr)
{
  UsbDeviceAddress adr;
  adr.devAddress = addr;

}

void setup()
{
  /// LCD
       lcd.begin (20,4);    // Inicializar el display con 20 caraceres 4 lineas
       lcd.setBacklightPin(3,POSITIVE);
       lcd.setBacklight(HIGH);
  /// FIN LCD

  /// BOTONES
      pinMode(5,INPUT);
      pinMode(6,INPUT);
  /// FIN BOTONES

  /// RELAY 4 
      pinMode(14, OUTPUT);
      pinMode(15, OUTPUT);
      pinMode(16, OUTPUT);
      pinMode(17, OUTPUT);
  /// FIN RELAY
  

  //Serial.begin( 184320 );
  Serial.begin( 115200 );
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  //// Serial.println("Espera, voy a ver que has enchufado ...");
       lcd.home ();                   // go home
       lcd.print("Duck HW Hunter v0.4a");
       lcd.setCursor ( 0, 1 );        // go to the 2nd line
       lcd.print("  (spaghetti code)");
       lcd.setCursor ( 0, 3 );        // go to the 2nd line
       lcd.print("Esperando...");

  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");

  delay( 200 );
}

uint8_t getdevdescr( uint8_t addr, uint8_t &num_conf );

void PrintDescriptors(uint8_t addr)
{
  uint8_t rcode = 0;
  uint8_t num_conf = 0;

  rcode = getdevdescr( (uint8_t)addr, num_conf );
  if ( rcode )
  {
    printProgStr(Gen_Error_str);
    print_hex( rcode, 8 );
  }
 

  for (int i = 0; i < num_conf; i++)
  {
    rcode = getconfdescr( addr, i );                 // get configuration descriptor
    if ( rcode )
    {
      printProgStr(Gen_Error_str);
      print_hex(rcode, 8);
    }
   
  }
}

void PrintAllDescriptors(UsbDevice *pdev)
{
  
  PrintDescriptors( pdev->address.devAddress );
}

void loop()
{
  Usb.Task();

  if ( Usb.getUsbTaskState() == USB_STATE_RUNNING )
  {
    Usb.ForEachUsbDevice(&PrintAllDescriptors);
    Usb.ForEachUsbDevice(&PrintAllAddresses);

    while ( 1 ) { // stop
#ifdef ESP8266
        yield(); // needed in order to reset the watchdog timer on the ESP8266
#endif
    }
  }
}

uint8_t getdevdescr( uint8_t addr, uint8_t &num_conf )
{
  USB_DEVICE_DESCRIPTOR buf;
  uint8_t rcode;
  rcode = Usb.getDevDescr( addr, 0, 0x12, ( uint8_t *)&buf );
  if ( rcode ) {
    return ( rcode );
  }

  num_conf = buf.bNumConfigurations;
  return ( 0 );
}

void printhubdescr(uint8_t *descrptr, uint8_t addr)
{
  HubDescriptor  *pHub = (HubDescriptor*) descrptr;
  uint8_t        len = *((uint8_t*)descrptr);



  for (uint8_t i = 7; i < len; i++)
    print_hex(descrptr[i], 8);

  //for (uint8_t i=1; i<=pHub->bNbrPorts; i++)
  //    PrintHubPortStatus(&Usb, addr, i, 1);
}

uint8_t getconfdescr( uint8_t addr, uint8_t conf )
{
  uint8_t buf[ BUFSIZE ];
  uint8_t* buf_ptr = buf;
  uint8_t rcode;
  uint8_t descr_length;
  uint8_t descr_type;
  uint16_t total_length;
  rcode = Usb.getConfDescr( addr, 0, 4, conf, buf );  //get total length
  LOBYTE( total_length ) = buf[ 2 ];
  HIBYTE( total_length ) = buf[ 3 ];
  if ( total_length > 256 ) {   //check if total length is larger than buffer
    printProgStr(Conf_Trunc_str);
    total_length = 256;
  }
  rcode = Usb.getConfDescr( addr, 0, total_length, conf, buf ); //get the whole descriptor
  while ( buf_ptr < buf + total_length ) { //parsing descriptors
    descr_length = *( buf_ptr );
    descr_type = *( buf_ptr + 1 );
    switch ( descr_type ) {
      case ( USB_DESCRIPTOR_CONFIGURATION ):
        printconfdescr( buf_ptr );
        break;
      case ( USB_DESCRIPTOR_INTERFACE ):
        printintfdescr( buf_ptr );
        break;
      case ( USB_DESCRIPTOR_ENDPOINT ):
        printepdescr( buf_ptr );
        break;
      case 0x29:
        printhubdescr( buf_ptr, addr );
        break;
      default:
        printunkdescr( buf_ptr );
        break;
    }//switch( descr_type
    buf_ptr = ( buf_ptr + descr_length );    //advance buffer pointer
  }//while( buf_ptr <=...
  return ( rcode );
}
/* prints hex numbers with leading zeroes */
// copyright, Peter H Anderson, Baltimore, MD, Nov, '07
// source: http://www.phanderson.com/arduino/arduino_display.html
void print_hex(int v, int num_places)
{
  int mask = 0, n, num_nibbles, digit;

  for (n = 1; n <= num_places; n++) {
    mask = (mask << 1) | 0x0001;
  }
  v = v & mask; // truncate v to specified number of places

  num_nibbles = num_places / 4;
  if ((num_places % 4) != 0) {
    ++num_nibbles;
  }
  do {
    digit = ((v >> (num_nibbles - 1) * 4)) & 0x0f;
    Serial.print(digit, HEX);
  }
  while (--num_nibbles);
}
/* function to print configuration descriptor */
void printconfdescr( uint8_t* descr_ptr )
{
  USB_CONFIGURATION_DESCRIPTOR* conf_ptr = ( USB_CONFIGURATION_DESCRIPTOR* )descr_ptr;

  return;
}
/* function to print interface descriptor */
void printintfdescr( uint8_t* descr_ptr )
{
  USB_INTERFACE_DESCRIPTOR* intf_ptr = ( USB_INTERFACE_DESCRIPTOR* )descr_ptr;
Serial.println(intf_ptr->bInterfaceProtocol);

  /*if (intf_ptr->bInterfaceProtocol == 2){
     Serial.println("Identificado ratón USB");}
  else if (intf_ptr->bInterfaceProtocol == 1){
     Serial.println("Identificado teclado USB / Pato");}
  else if(intf_ptr->bInterfaceProtocol == 80) ///// 50 en decimal
  {
     Serial.println("Identificado Almacenamiento USB");}
  else   
     {Serial.println("Dispositivo USB no identificado");};*/

       lcd.home ();                   // go home
       lcd.clear ();

  if (intf_ptr->bInterfaceProtocol == 2){
     lcd.print("RATON USB");}
  else if (intf_ptr->bInterfaceProtocol == 1){
     lcd.print("Teclado USB");}
  else if (intf_ptr->bInterfaceProtocol == 0){
     lcd.print("Pato CUAK CUAK");}
  else if(intf_ptr->bInterfaceProtocol == 80) ///// 50 en decimal
  {
     lcd.print("Pendrive");}
  else   
     {lcd.print("Desconocido");};

       lcd.setCursor ( 0, 1 );        // go to the 2nd line
       lcd.print("To PC? <SI> <NO>");

while ( (boton1 == LOW) && (boton2 == LOW)){
      boton1 = digitalRead(5);
      boton2 = digitalRead(6);

      if (boton1 == HIGH){
        lcd.home();
        lcd.clear();
        lcd.print("siiiiiiiiii");
        activar_relay();
        delay(5000);
        desactivar_relay();
      }
      if (boton2 == HIGH){
        lcd.home();
        lcd.clear();
        lcd.print("noooooooooo");
      }
} /// fin del while


  return;
}
/* function to print endpoint descriptor */
void printepdescr( uint8_t* descr_ptr )
{


  return;
}
/*function to print unknown descriptor */
void printunkdescr( uint8_t* descr_ptr )
{
  uint8_t length = *descr_ptr;
  uint8_t i;

  descr_ptr += 2;
  for ( i = 0; i < length; i++ ) {
    
    descr_ptr++;
  }
}


/* Print a string from Program Memory directly to save RAM */
void printProgStr(const char* str)
{
  char c;
  if (!str) return;
  while ((c = pgm_read_byte(str++)))
    Serial.print(c);
}



void desactivar_relay()
{
  digitalWrite(14, HIGH);
  digitalWrite(15, HIGH);
  digitalWrite(16, HIGH);
  digitalWrite(17, HIGH);
}


void activar_relay()
{
  digitalWrite(14, LOW);
  digitalWrite(15, LOW);
  digitalWrite(16, LOW);
  digitalWrite(17, LOW);
}
