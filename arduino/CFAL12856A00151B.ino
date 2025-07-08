//==============================================================================
//
//  CRYSTALFONTZ 
//
//  This code drives the CFAL12856A0-0151-B display
//  https://www.crystalfontz.com/product/cfal12856a00151b
//
//  The controller is a Solomon Systech SSD1309
//    https://www.crystalfontz.com/controllers/SolomonSystech/SSD1309/
//
//  Seeeduino v4.2, an open-source 3.3v capable Arduino clone.
//    https://www.seeedstudio.com/Seeeduino-V4.2-p-2517.html
//    https://github.com/SeeedDocument/SeeeduinoV4/raw/master/resources/Seeeduino_v4.2_sch.pdf
//
//==============================================================================
//This is free and unencumbered software released into the public domain.
//
//Anyone is free to copy, modify, publish, use, compile, sell, or
//distribute this software, either in source code form or as a compiled
//binary, for any purpose, commercial or non-commercial, and by any
//means.
//
//In jurisdictions that recognize copyright laws, the author or authors
//of this software dedicate any and all copyright interest in the
//software to the public domain. We make this dedication for the benefit
//of the public at large and to the detriment of our heirs and
//successors. We intend this dedication to be an overt act of
//relinquishment in perpetuity of all present and future rights to this
//software under copyright law.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
//OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//OTHER DEALINGS IN THE SOFTWARE.
//
//For more information, please refer to <http://unlicense.org/>
//==============================================================================
// LCD & USD control lines
//   ARD      | Port  |Display tail|  Function - 8080 Parallel   |  Function - SPI                            |	Function - I2C                  
//------------+-------+------------+-----------------------------+--------------------------------------------+
//  N/A	      |       | 23         |  POWER 12.5V                |  POWER 12V                                 |	POWER 12V  
//  3.3V      |       | 5          |  POWER 3.3V                 |  POWER 3.3V                                |	POWER 3.3V 
//  GND	      |       | 1-3, 24    |  GROUND                     |  GROUND                                    |	GROUND                
// -----------+-------+------------+-----------------------------+--------------------------------------------+
//  N/A       | N/A   | 6          |  BS1 - 3.3V                 |  BS1 - GND                                 |	BS1 - 3.3V
//  N/A       | N/A   | 7          |  BS2 - 3.3V                 |  BS2 - GND                                 |	BS2 - GND
// -----------+-------+------------+-----------------------------+--------------------------------------------+
//  A0        | PORTC | 10         |  Data/Command        (DC)   |  Data/Command (pull high for 3-wire) (DC)  |	SA0
//  A1        | PORTC | 11         |  Write               (WR)   |  N/A pull high                             |	N/A pull high    
//  A2        | PORTC | 12         |  Read                (RD)   |  N/A pull high                             |	N/A pull high     
//  D8        | PORTB | 8          |  Chip Enable Signal  (CS)   |  Chip Enable Signal                  (CS)  |	Chip Enable Signal (CS)
//  D9        | PORTB | 9          |  Reset            (RESET)   |  Reset                            (RESET)  |	Reset         (RESET)
// -----------+-------+------------+-----------------------------+--------------------------------------------+
// PARALLEL ONLY
// -----------+-------+------------+-----------------------------+--------------------------------------------+
//  D0        | PORTD | 13         |  LCD_D10 (DB0)              |                                            |
//  D1        | PORTD | 14         |  LCD_D11 (DB1)              |                                            |
//  D2        | PORTD | 15         |  LCD_D12 (DB2)              |                                            |
//  D3        | PORTD | 16         |  LCD_D13 (DB3)              |                                            |
//  D4        | PORTD | 17         |  LCD_D14 (DB4)              |                                            |
//  D5        | PORTD | 18         |  LCD_D15 (DB5)              |                                            |
//  D6        | PORTD | 19         |  LCD_D16 (DB6)              |                                            |
//  D7        | PORTD | 20         |  LCD_D17 (DB7)              |                                            |
// -----------+-------+------------+-----------------------------+--------------------------------------------+
// SPI ONLY
// -----------+-------+------------+-----------------------------+--------------------------------------------+
//  D13       | PORTD | 13         |                             |  SCLK                                      
//  D11       | PORTD | 14         |                             |  SDIN                                      
//  D2        | PORTD | 15         |                             |  No Connection                             
//  D3        | PORTD | 16         |                             |  N/A pull high                             
//  D4        | PORTD | 17         |                             |  N/A pull high                             
//  D5        | PORTD | 18         |                             |  N/A pull high                             
//  D6        | PORTD | 19         |                             |  N/A pull high                             
//  D7        | PORTD | 20         |                             |  N/A pull high                             
// -----------+-------+------------+-----------------------------+--------------------------------------------+
// I2C ONLY
// -----------+-------+------------+-----------------------------+--------------------------------------------+
//  A5        | PORTC | 13         |                             |                                            |  SCL                                      
//  A4        | PORTC | 14         |                             |                                            |  SDA                                          
//  A4        | PORTC | 15         |                             |                                            |  SDA                             
// -----------+-------+------------+-----------------------------+--------------------------------------------+
// Consult the datasheet for more interface options
//==============================================================================
//
// There are additional components that should be connected and you need an
// external 12.5v VOLED supply.
// Please consult the datasheet for information about these components.
//
// Alternatively, you can use a CFA10105 denonstration board which contains all
// the components and has a build-in VOLED supply.
//
//==============================================================================
//  BS0,BS1 interface settings:
//  
//      Interface         | BS1 | BS2 
//  ----------------------+-----+-----
//    I2C                 |  1  |  0  
//    4-wire SPI          |  0  |  0  
//    8-bit 6800 Parallel |  0  |  1  
//    8-bit 8080 Parallel |  1  |  1  
//
//  This code is demonstrated using 8080 Parallel, I2C, or 4-wire SPI
//  The CFA10105 breakout board is default SPI. To load code for the breakout
//  uncomment the define for SPI below and comment out the define for I2C.
//==============================================================================
//  Select the interface
#define SPI_4_WIRE
//#define PAR_8080	  
//#define I2C
//------------------------------------------------------------------------------
//Allow the serial debugging monitor, but only if SPI (pin conflict with parallel)
//Allow the serial debugging monitor, but only if SPI (pin conflict with parallel)
#define SER_DEBUG
// Reduce memory usage but keep essential serial communication
#define MINIMIZE_MEMORY

#ifdef PAR_8080
  #undef SER_DEBUG
#endif // PAR_8080
//==============================================================================
#define CLR_CS     (PORTB &= ~(0x01)) //pin #8  - Chip Enable Signal
#define SET_CS     (PORTB |=  (0x01)) //pin #8  - Chip Enable Signal
#define CLR_RESET  (PORTB &= ~(0x02)) //pin #12 - Reset
#define SET_RESET  (PORTB |=  (0x02)) //pin #12 - Reset
#define CLR_DC     (PORTC &= ~(0x01)) //pin #9  - Data/Instruction
#define SET_DC     (PORTC |=  (0x01)) //pin #9  - Data/Instruction
#define CLR_WR	   (PORTC &= ~(0x02)) //pin #10 - Write
#define SET_WR	   (PORTC |=  (0x02)) //pin #10 - Write
#define CLR_RD	   (PORTC &= ~(0x04)) //pin #11 - Read
#define SET_RD	   (PORTC |=  (0x04)) //pin #11 - Read
#define CLR_DBG	   (PORTC &= ~(0x08)) //pin #12 - Debug
#define SET_DBG	   (PORTC |=  (0x08)) //pin #12 - Debug

#include <avr/io.h>
#include <avr/pgmspace.h>
#ifdef SPI_4_WIRE
  #include <SPI.h>
#endif
#ifdef I2C
  #include <Wire.h>
#endif
// Replace with much smaller logo/animation data
// Store in PROGMEM to save RAM
#include "font5x8.h" // Moved font to separate header file to save memory
#include "CFAL12856A0_0151_B_Splash.h" // Include splash images (Aiming, Eagle, etc.)
//================================================================================
#define MAX_BRIGHT (0x8F)
//================================================================================
#ifdef SPI_4_WIRE
//================================================================================
void writeCommand(uint8_t command)
  {
  // Select the LCD's command register
  CLR_DC;
  // Select the LCD controller
  CLR_CS;

  //Send the command via SPI:
  SPI.transfer(command);
  //deselect the controller
  SET_CS;
  }
//================================================================================
void writeData(uint8_t data)
  {
  //Select the LCD's data register
  SET_DC;
  //Select the LCD controller
  CLR_CS;
  //Send the command via SPI:
  SPI.transfer(data);

  // Deselect the LCD controller
  SET_CS;
  }
#endif
//================================================================================

#ifdef I2C
//================================================================================
void writeCommand(uint8_t command)
{
	Wire.beginTransmission(0x3C); //send start & Slave address
	Wire.write(0x00);		          //Control Byte - Command
	Wire.write(command);	        //payload
	Wire.endTransmission();

}
//============================================================================
void writeData(uint8_t data)
{
	Wire.beginTransmission(0x3C);	//send start & Slave address
	Wire.write(0xC0);		          //Control Byte - Data (non-continued)
	Wire.write(data);          //payload
	Wire.endTransmission();
}
#endif
//================================================================================

#ifdef PAR_8080
void writeCommand(uint8_t command)
  {
  //select the LCD's command register
  CLR_DC;
  //select the LCD controller
  CLR_CS;
  //send the data via parallel
  PORTD = command;
  //clear the write register
  CLR_WR;
  //set the write register
  SET_WR;
  //deselct the LCD controller
  SET_CS;
  }
//================================================================================
void writeData(uint8_t data)
  {
  //select the LCD's data register
  SET_DC;
  //select the LCD controller
  CLR_CS;
  //send the data via parallel
  PORTD = data;
  //clear the write register
  CLR_WR;
  //set the write register
  SET_WR;
  //deselct the LCD controller
  SET_CS;
  }
//================================================================================
#endif

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Instruction Setting
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Start_Column(uint8_t d)
  {
  writeCommand(0x00 + d % 16);		// Set Lower Column Start Address for Page Addressing Mode
            //   Default => 0x00
  writeCommand(0x10 + d / 16);		// Set Higher Column Start Address for Page Addressing Mode
            //   Default => 0x10
  }
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Column_Address(uint8_t a, uint8_t b)
  {
  writeCommand(0x21);			// Set Column Address
  writeCommand(a);			//   Default => 0x00 (Column Start Address)
  writeCommand(b);			//   Default => 0x7F (Column End Address)
  }
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Page_Address(uint8_t a, uint8_t b)
  {
  writeCommand(0x22);			// Set Page Address
  writeCommand(a);			//   Default => 0x00 (Page Start Address)
  writeCommand(b);			//   Default => 0x07 (Page End Address)
  }
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Start_Page(uint8_t d)
  {
  writeCommand(0xB0 | d);			// Set Page Start Address for Page Addressing Mode
            //   Default => 0xB0 (0x00)
  }
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen) - ultra optimized for memory
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_RAM(uint8_t Data)
  {
  for (uint8_t i = 0; i < 8; i++)
    {
    Set_Start_Page(i);
    Set_Start_Column(0x00);
    for (uint8_t j = 0; j < 128; j++)
      {
      writeData(Data);
      }
    }
  }
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_RAM_CheckerBoard(void)
  {
  uint8_t
    page;
  uint8_t
    column;

  for(page = 0; page < 8; page++)
    {
    Set_Start_Page(page);
    Set_Start_Column(0x00);

    for (column= 0; column < 128; column++)
      {
      if(0 == (column&0x01))
        {
        writeData(0x55);
        }
      else
        {
        writeData(0xAA);
        }
      }
    }
  }
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define XLevelL    0x00
#define XLevelH    0x10
#define XLevel     ((XLevelH&0x0F)*16+XLevelL)
#define HRES       128
#define VRES       56
#define Brightness 0xBF

void OLED_Init()
  {
  //The CFA10105 has a power-on reset circuit, 
  //you can use the following code if you are using GPIO for reset
  CLR_RESET;
  delay(1);
  SET_RESET;
  delay(120);

  writeCommand(0xFD);	// Set Command Lock
  writeCommand(0X12);	//   Default => 0x12
                      //     0x12 => Driver IC interface is unlocked from entering command.
                      //     0x16 => All Commands are locked except 0xFD.

  writeCommand(0XAE);	// Set Display On/Off
                      //   Default => 0xAE
                      //     0xAE => Display Off
                      //     0xAF => Display On

                            
  writeCommand(0xD5);	// Set Display Clock Divide Ratio / Oscillator Frequency
//  writeCommand(0XA0);	// Set Clock as 116 Frames/Sec
  writeCommand(0X30);  // Set Clock as 116 Frames/Sec
                      //   Default => 0x70
                      //     D[3:0] => Display Clock Divider
                      //     D[7:4] => Oscillator Frequency

  writeCommand(0xA8);	// Set Multiplex Ratio
  writeCommand(0X37); //   Default => 0x3F (1/56 Duty)

  writeCommand(0xD3);	// Set Display Offset
  writeCommand(0X08);	//   Default => 0x00

  writeCommand(0x40); // Set Mapping RAM Display Start Line (0x00~0x3F)
                      //   Default => 0x40 (0x00)

  //writeCommand(0xD8);	// Set Low Power Display Mode (0x04/0x05)
  //writeCommand(0x05);	//   Default => 0x04 (Normal Power Mode)

  writeCommand(0xA1); // Set SEG/Column Mapping (0xA0/0xA1)
                      //   Default => 0xA0
                      //     0xA0 => Column Address 0 Mapped to SEG0
                      //     0xA1 => Column Address 0 Mapped to SEG127

  writeCommand(0xC8); // Set COM/Row Scan Direction (0xC0/0xC8)
                      //   Default => 0xC0
                      //     0xC0 => Scan from COM0 to 63
                      //     0xC8 => Scan from COM63 to 0

  writeCommand(0xDA); // Set COM Pins Hardware Configuration
  writeCommand(0x12); //   Default => 0x12
                      //     Alternative COM Pin Configuration
                      //     Disable COM Left/Right Re-Map

  writeCommand(0x81); // Set SEG Output Current
  writeCommand(0x8F); // Set Contrast Control for Bank 0

  writeCommand(0xD9); // Set Pre-Charge as 2 Clocks & Discharge as 5 Clocks
  writeCommand(0x25); //   Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
                      //     D[3:0] => Phase 1 Period in 1~15 Display Clocks
                      //     D[7:4] => Phase 2 Period in 1~15 Display Clocks
  
  writeCommand(0xDB); // Set VCOMH Deselect Level
  writeCommand(0x34); //   Default => 0x34 (0.78*VCC)

  writeCommand(0xA4); // Set Entire Display On / Off
                      //   Default => 0xA4
                      //     0xA4 => Normal Display
                      //     0xA5 => Entire Display On

  writeCommand(0xA6); // Set Inverse Display On/Off
                      //   Default => 0xA6
                      //     0xA6 => Normal Display
                      //     0xA7 => Inverse Display On

  Fill_RAM(0x00);			// Clear Screen

  writeCommand(0XAF); // Display On (0xAE/0xAF)
  }
//================================================================================
// Draw a simple logo instead of loading a bitmap from flash
//================================================================================
void drawSimpleLogo() {
  Fill_RAM(0x00);
  
  // Draw "OLED" in large letters centered on screen
  displayText("OLED DISPLAY", 20, 16);
  displayText("READY", 45, 32);
  
  // Draw a border
  for (uint8_t x = 0; x < 128; x++) {
    Set_Start_Page(0);
    Set_Start_Column(x);
    writeData(0x01);
    
    Set_Start_Page(6);
    Set_Start_Column(x);
    writeData(0x80);
  }
  
  for (uint8_t y = 0; y < 7; y++) {
    Set_Start_Page(y);
    Set_Start_Column(0);
    writeData(0xFF);
    
    Set_Start_Page(y);
    Set_Start_Column(127);
    writeData(0xFF);
  }
}
  
// Draw the Omnia logo programmatically instead of using a large bitmap
void drawOmniaLogo() {
  Fill_RAM(0x00);
  
  // Draw "OMNIA" text
  displayText("OMNIA", 40, 10);
  
  // Draw a simple border
  for (uint8_t y = 0; y < 7; y++) {
    Set_Start_Page(y);
    Set_Start_Column(20);
    writeData(0xFF);
    
    Set_Start_Page(y);
    Set_Start_Column(108);
    writeData(0xFF);
  }
  
  for (uint8_t x = 20; x < 109; x++) {
    Set_Start_Page(0);
    Set_Start_Column(x);
    writeData(0x01);
    
    Set_Start_Page(6);
    Set_Start_Column(x);
    writeData(0x80);
  }
  
  // Draw a simple icon
  displayText("AI", 50, 25);
}
  
// Function to display bitmap images from the splash header file
void displayBitmap(const uint8_t bitmap[][128]) {
  for (uint8_t page = 0; page < 7; page++) {
    Set_Start_Page(page);
    Set_Start_Column(0x00);
    
    for (uint8_t column = 0; column < 128; column++) {
      writeData(pgm_read_byte(&bitmap[page][column]));
    }
  }
}

// Display the Aiming reticle image
void displayAiming() {
  Fill_RAM(0x00);
  displayBitmap(Aiming);
  Serial.println(F("Aiming reticle displayed"));
}

// Display the Eagle image
void displayEagle() {
  Fill_RAM(0x00);
  displayBitmap(Eagle);
  Serial.println(F("Eagle image displayed"));
}
  
// Forward declarations to fix compilation errors
void drawCharacter(char c, int x, int y);
void simpleScroll(const char* text, uint8_t y);
void resetDisplay();
void simpleAnimation();
void showSymbolGuide();

// Memory-efficient text display function with bitmap font support using char arrays
void displayText(const char* text) {
  displayText(text, 0, 0); // Default to top-left position
}

void displayText(const char* text, int startX, int startY) {
  // Optimized version with auto-wrapping and boundary checking
  int x = startX;
  int y = startY;
  char c;
  const uint8_t lineHeight = 8; // Character height
  const uint8_t charSpacing = 1; // Space between characters
  const uint8_t charWidth = 5;   // Character width
  const uint8_t totalCharWidth = charWidth + charSpacing;
  
  // Ensure startY is aligned to prevent half-characters at top
  y = (y / 8) * 8;
  
  while ((c = *text++)) {
    // Handle newline character
    if (c == '\n') {
      x = startX;
      y += lineHeight + 2; // Move to next line with spacing
      if (y >= 56) break; // Don't go beyond screen height
      continue;
    }
    
    // Handle carriage return
    if (c == '\r') {
      x = startX;
      continue;
    }
    
    // Auto-wrap text when approaching screen edge
    if (x + totalCharWidth >= 128) {
      x = startX;
      y += lineHeight + 2; // Add spacing between lines
      
      // Stop if we've reached the bottom of the screen
      if (y >= 56) {
        break;
      }
      
      // Skip leading spaces at line start after wrap
      if (c == ' ') {
        continue;
      }
    }
    
    // Draw the character
    drawCharacter(c, x, y);
    x += totalCharWidth;
  }
}

// New process special char function that works with char arrays directly
// Returns processed string in outBuf
void processSpecialChars(const char* inText, char* outBuf, uint8_t maxLen) {
  uint8_t inPos = 0;
  uint8_t outPos = 0;
  char c;
  
  // Safety check
  if (!inText || !outBuf || maxLen < 1) return;
  
  while ((c = inText[inPos]) != 0 && outPos < maxLen-1) {
    // Check for escape sequence
    if (c == '\\' && inText[inPos+1] != 0) {
      // Process escape sequence
      inPos++; // Skip backslash
      
      switch (inText[inPos]) {
        case 'u': // \up
          if (inText[inPos+1] == 'p') {
            outBuf[outPos++] = 127; // Up arrow
            inPos += 2; // Skip "up"
          } else {
            outBuf[outPos++] = '\\'; // Not recognized
            outBuf[outPos++] = inText[inPos++];
          }
          break;
          
        case 'd': // \down or \deg
          if (strncmp(&inText[inPos], "down", 4) == 0) {
            outBuf[outPos++] = 128; // Down arrow
            inPos += 4; // Skip "down"
          } 
          else if (strncmp(&inText[inPos], "deg", 3) == 0) {
            outBuf[outPos++] = 176; // Degree symbol
            inPos += 3; // Skip "deg"
          }
          else {
            outBuf[outPos++] = '\\'; // Not recognized
            outBuf[outPos++] = inText[inPos++];
          }
          break;
          
        case 'l': // \left
          if (strncmp(&inText[inPos], "left", 4) == 0) {
            outBuf[outPos++] = 129; // Left arrow
            inPos += 4; // Skip "left"
          } else {
            outBuf[outPos++] = '\\'; // Not recognized
            outBuf[outPos++] = inText[inPos++];
          }
          break;
          
        case 'r': // \right
          if (strncmp(&inText[inPos], "right", 5) == 0) {
            outBuf[outPos++] = 130; // Right arrow
            inPos += 5; // Skip "right"
          } else {
            outBuf[outPos++] = '\\'; // Not recognized
            outBuf[outPos++] = inText[inPos++];
          }
          break;
          
        case 'n': // \n newline
          outBuf[outPos++] = '\n';
          inPos++;
          break;
          
        case 'x': // \xHH hex character
          if (isxdigit(inText[inPos+1]) && isxdigit(inText[inPos+2])) {
            char hex[3] = {inText[inPos+1], inText[inPos+2], 0};
            outBuf[outPos++] = (char)strtol(hex, NULL, 16);
            inPos += 3; // Skip 'x' and two hex digits
          } else {
            outBuf[outPos++] = '\\'; // Not recognized
            outBuf[outPos++] = inText[inPos++];
          }
          break;
          
        default:
          // Unknown escape sequence
          outBuf[outPos++] = '\\';
          outBuf[outPos++] = inText[inPos++];
          break;
      }
    } else {
      // Regular character, copy as-is
      outBuf[outPos++] = inText[inPos++];
    }
    
    // Ensure we don't overflow
    if (outPos >= maxLen-1) break;
  }
  
  // Null terminate the output
  outBuf[outPos] = 0;
}

// Ultra-optimized helper function to draw a character with boundary checking
void drawCharacter(char c, int x, int y) {
  // Skip drawing if completely out of bounds
  if (x < 0 || x >= 128 || y < 0 || y >= 56) {
    return;
  }
  
  // Get character value as unsigned for proper handling of extended chars
  unsigned char uc = (unsigned char)c;
  
  // Calculate font index with inline logic to save memory
  int fontIndex;
  if (uc >= 32 && uc <= 126) {
    fontIndex = uc - 32; // Standard ASCII
  } else if (uc >= 127 && uc <= 131) {
    fontIndex = 95 + (uc - 127); // Map 127-131 to 95-99
  } else if (uc == 176) {
    fontIndex = 99; // Degree symbol
  } else {
    fontIndex = '?' - 32; // Unknown characters
  }
  
  // Calculate page boundaries correctly to avoid cut-off
  uint8_t startPage = y / 8;
  uint8_t endPage = (y + 7) / 8;
  
  // Make sure we don't exceed display boundaries
  if (startPage >= 8) return;
  if (endPage >= 8) endPage = 7;
  
  // Draw the character column by column with boundary checking
  for (uint8_t col = 0; col < 5; col++) {
    // Skip columns outside display area
    if (x + col >= 128) continue;
    
    uint8_t fontCol = pgm_read_byte(&font5x8[fontIndex][col]);
    
    for (uint8_t page = startPage; page <= endPage; page++) {
      Set_Start_Page(page);
      Set_Start_Column(x + col);
      
      int8_t yOffset = y - (page * 8);
      uint8_t displayByte = (yOffset >= 0) ? 
                            (fontCol << yOffset) : 
                            (fontCol >> (-yOffset));
      
      writeData(displayByte);
    }
  }
}

// Ultra-minimal test function
void testArrows() {
  Fill_RAM(0x00);
  drawCharacter((char)127, 10, 10); // Up
  drawCharacter((char)128, 30, 10); // Down
  drawCharacter((char)129, 50, 10); // Left
  drawCharacter((char)130, 70, 10); // Right
  drawCharacter((char)176, 90, 10); // Degree
  
  // Print debug info to serial
  Serial.println("Testing arrow characters:");
  Serial.println("UP(127), DOWN(128), LEFT(129), RIGHT(130), DEG(176)");
}

// Ultra-minimal degree symbol test
void testDegreeSymbol() {
  Fill_RAM(0x00);
  displayText("72", 50, 20);
  drawCharacter((char)176, 62, 20); // Degree symbol
  displayText("F", 70, 20);
  
  // Print debug info
  Serial.println("Testing degree symbol (char 176)");
  Serial.println("Should show: 72°F");
}

// Removed helper functions to save memory
//================================================================================
void setup() {
  //Set up port directions
  DDRD = 0xff;
  DDRC = 0xff;
  DDRB = 0x03;
  //Idle the lines in a reasonable state
  PORTD = 0xff;
  SET_RD;
  SET_WR;
  SET_CS;

  // Initialize Serial with minimal settings
  Serial.begin(9600);
  delay(50);
  
  // Clear any garbage from the serial buffer
  while (Serial.available()) {
    Serial.read();
  }
  
  Serial.println(F("Serial ready at 9600 baud"));

#ifdef SPI_4_WIRE
  //SPI begin transactions
  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
#endif
			  
#ifdef I2C
  Wire.begin();
  TWBR = 12; // 400KHz
#endif
 
  //OLED_Init
  OLED_Init();
  
  // Show startup message
  Fill_RAM(0x00);
  displayText("OLED READY", 30, 16);
  displayText("Type 'help'", 30, 32);
  Serial.println(F("Display ready"));
}
//================================================================================
// Define a static buffer for message processing (to avoid local String allocations)
static char msgBuffer[64];

void loop() {
  // Check for serial commands - with feedback for debugging
  if (Serial.available()) {
    // Ultra-memory efficient command reading
    char cmdBuffer[32]; // Fixed size buffer to save memory
    byte bufPos = 0;
    
    // Read characters with timeout - very minimal implementation
    unsigned long startTime = millis();
    while ((millis() - startTime) < 200 && bufPos < sizeof(cmdBuffer) - 1) {
      if (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
          break;
        }
        cmdBuffer[bufPos++] = c;
        startTime = millis();
      }
      delay(1);
    }
    cmdBuffer[bufPos] = '\0'; // Null-terminate
    
    // Minimal debug
    Serial.print(F("Cmd: "));
    Serial.println(cmdBuffer);

    // Simple command checking using strcmp/strncmp for minimal memory usage
    if (strcmp(cmdBuffer, "show_logo") == 0 || strcmp(cmdBuffer, "logo") == 0) {
      drawOmniaLogo();
      Serial.println(F("Logo displayed"));
    } else if (strcmp(cmdBuffer, "aiming") == 0) {
      displayAiming();
    } else if (strcmp(cmdBuffer, "eagle") == 0) {
      displayEagle();
    } else if (strcmp(cmdBuffer, "show_index") == 0 || strcmp(cmdBuffer, "index") == 0) {
      // Show a simple index display
      Fill_RAM(0x00);
      displayText("INDEX", 45, 0);
      displayText("1. Demo", 5, 16);
      displayText("2. Help", 5, 24);
      displayText("3. Text", 5, 32);
      displayText("4. Symbols", 5, 40);
      Serial.println(F("Index displayed"));
    } 
    else if (strcmp(cmdBuffer, "clear") == 0) {
      Fill_RAM(0x00);
      Serial.println(F("Display cleared"));
    } 
    else if (strncmp(cmdBuffer, "text:", 5) == 0) {
      // Process special characters
      processSpecialChars(cmdBuffer + 5, msgBuffer, sizeof(msgBuffer));
      
      // Clear screen first for full-screen text
      Fill_RAM(0x00);
      displayText(msgBuffer);
      Serial.println(F("Text displayed"));
    } 
    else if (strncmp(cmdBuffer, "textxy:", 7) == 0) {
      // Format: textxy:x,y,message (e.g., "textxy:10,20,Hello World")
      char *xStr = cmdBuffer + 7;
      char *yStr = strchr(xStr, ',');
      
      if (yStr) {
        *yStr++ = '\0'; // Null-terminate x string and move to y
        char *msgStr = strchr(yStr, ',');
        
        if (msgStr) {
          *msgStr++ = '\0'; // Null-terminate y string and move to message
          
          // Convert coordinates
          int x = atoi(xStr);
          int y = atoi(yStr);
          
          // Process special characters
          processSpecialChars(msgStr, msgBuffer, sizeof(msgBuffer));
          
          // Clear screen and display
          Fill_RAM(0x00);
          displayText(msgBuffer, x, y);
          Serial.println(F("Positioned text displayed"));
        } else {
          Serial.println(F("Invalid format. Use: textxy:x,y,message"));
        }
      } else {
        Serial.println(F("Invalid format. Use: textxy:x,y,message"));
      }
    } 
    else if (strncmp(cmdBuffer, "multiline:", 10) == 0) {
      // Process special characters (including \n)
      processSpecialChars(cmdBuffer + 10, msgBuffer, sizeof(msgBuffer));
      
      // Clear screen first
      Fill_RAM(0x00);
      displayText(msgBuffer);
      Serial.println(F("Multi-line text displayed"));
    } 
    else if (strncmp(cmdBuffer, "scroll:", 7) == 0) {
      // Process special characters
      processSpecialChars(cmdBuffer + 7, msgBuffer, sizeof(msgBuffer));
      
      // Simple scrolling implementation
      simpleScroll(msgBuffer, 20);
      Serial.println(F("Scrolling text displayed"));
    } 
    else if (strcmp(cmdBuffer, "animate") == 0) {
      // Simple animation without large bitmaps
      simpleAnimation();
      Serial.println(F("Animation complete"));
    } 
    else if (strcmp(cmdBuffer, "help") == 0 || strcmp(cmdBuffer, "?") == 0 || strcmp(cmdBuffer, "commands") == 0) {
      // Memory-efficient way to print help info using F() macro
      Serial.println(F("\n===== OLED COMMANDS ====="));
      Serial.println(F("GENERAL:"));
      Serial.println(F("  help - Show this help"));
      Serial.println(F("  reset - Reset display"));
      Serial.println(F("  clear - Clear display"));
      
      Serial.println(F("\nTEXT DISPLAY:"));
      Serial.println(F("  text:message - Show text"));
      Serial.println(F("  textxy:x,y,message - Positioned text"));
      Serial.println(F("  multiline:text\\ntext - Multi-line"));
      Serial.println(F("  scroll:message - Scrolling text"));
      
      Serial.println(F("\nDEMOS:"));
      Serial.println(F("  demo - Basic demo"));
      Serial.println(F("  directions - Show arrows"));
      Serial.println(F("  weather - Show weather"));
      Serial.println(F("  test_arrows - Test arrows"));
      Serial.println(F("  test_degree - Test degree"));
      
      Serial.println(F("\nSPECIAL SYMBOLS:"));
      Serial.println(F("  \\up \\down \\left \\right - Arrows"));
      Serial.println(F("  \\deg - Degree symbol"));
      Serial.println(F("  \\n - New line"));
      
      // Show help on display
      Fill_RAM(0x00);
      displayText("Help info sent", 0, 0);
      displayText("to Serial Monitor", 0, 16);
      Serial.println("\n============= OLED COMMANDS =============");
      Serial.println("GENERAL:");
      Serial.println("  help, ? - Show this help");
      Serial.println("  reset, restart - Reset the display if errors occur");
      Serial.println("  clear - Clear the display");
      
      Serial.println("\nIMAGES:");
      Serial.println("  show_logo - Display logo");
      Serial.println("  aiming - Display aiming reticle");
      Serial.println("  eagle - Display eagle image");
      Serial.println("  show_index - Display index");
      Serial.println("  animate - Show animation");
      
      Serial.println("\nTEXT DISPLAY:");
      Serial.println("  text:Your message - Show text");
      Serial.println("  textxy:x,y,Your message - Show text at position");
      Serial.println("  multiline:Line1\\nLine2 - Show multi-line text");
      Serial.println("  scroll:Your scrolling message - Scrolling text");
      Serial.println("  scrollxy:y,delay,Your message - Custom scroll");
      
      Serial.println("\nDEMOS:");
      Serial.println("  demo - Basic demo");
      Serial.println("  text_demo - Text wrapping and scrolling demo");
      Serial.println("  directions - Show direction arrows");
      Serial.println("  weather - Show weather with degree symbol");
      Serial.println("  test_arrows - Test arrow symbols");
      Serial.println("  test_degree - Test degree symbol");
      Serial.println("  symbols - Show available symbols");
      
      Serial.println("\nSPECIAL SYMBOLS:");
      Serial.println("  \\up \\down \\left \\right - Arrow symbols");
      Serial.println("  \\deg - Degree symbol (°)");
      Serial.println("  \\x## - Hex character code (example: \\x7F)");
      
      Serial.println("\nEXAMPLES:");
      Serial.println("  text:Temp 72\\deg F");
      Serial.println("  scroll:Long message with arrows \\up \\down");
      Serial.println("  multiline:Line 1\\nLine 2\\nLine 3");
      Serial.println("==========================================");
    } else if (strcmp(cmdBuffer, "demo") == 0) {
      // Ultra-minimal demo to save memory
      Fill_RAM(0x00);
      displayText("ABCDEFG", 0, 0);
      displayText("0123456", 0, 10);
      Serial.println(F("Demo done"));
    } else if (strcmp(cmdBuffer, "directions") == 0) {
      // Ultra-minimal directions demo to save memory
      Fill_RAM(0x00);
      displayText("DIRECTIONS", 0, 0);
      drawCharacter((char)127, 10, 20); // Up
      drawCharacter((char)128, 30, 20); // Down
      drawCharacter((char)129, 50, 20); // Left
      drawCharacter((char)130, 70, 20); // Right
      Serial.println(F("Demo done"));
    } else if (strcmp(cmdBuffer, "weather") == 0) {
      // Ultra-minimal weather demo to save memory
      Fill_RAM(0x00);
      displayText("TEMP: 72", 0, 10);
      drawCharacter((char)176, 60, 10); // Degree
      displayText("F", 70, 10);
      Serial.println(F("Demo done"));
    } else if (strcmp(cmdBuffer, "test_arrows") == 0) {
      // Run arrow symbol diagnostic test
      Serial.println(F("Running arrow symbol test..."));
      testArrows();
      Serial.println(F("Arrow test complete"));
    } else if (strcmp(cmdBuffer, "test_degree") == 0) {
      // Run degree symbol diagnostic test
      Serial.println(F("Running degree symbol test..."));
      testDegreeSymbol();
      Serial.println(F("Degree symbol test complete"));
    } else if (strcmp(cmdBuffer, "usage_guide") == 0 || strcmp(cmdBuffer, "symbols") == 0) {
      // Show usage guide for special symbols
      Serial.println(F("Displaying symbols guide..."));
      showSymbolGuide();
      Serial.println(F("Symbols guide displayed"));
    } else if (strcmp(cmdBuffer, "text_demo") == 0) {
      // Simple text demo
      Fill_RAM(0x00);
      displayText("Text Demo", 0, 0);
      displayText("Wrapping text test", 0, 16);
      delay(1000);
      simpleScroll("This is scrolling text with arrows and symbols", 32);
      Serial.println(F("Text demo complete"));
    } else if (strcmp(cmdBuffer, "reset") == 0 || strcmp(cmdBuffer, "restart") == 0) {
      // Reset the display if things go wrong
      Serial.println(F("Resetting display..."));
      resetDisplay();
      Serial.println(F("Display reset complete"));
    } else {
      Serial.println(F("Command not recognized. Type 'help' for commands."));
    }

    return; // Skip the default animation if a command was handled
  }

  // Default state - just wait for commands
  static bool init_done = false;
  if (!init_done) {
    Fill_RAM(0x00);
    init_done = true;
    Serial.println(F("Ready for commands. Type 'help' for commands."));
  }
  delay(100);
}

// Define debug macros that always allow essential serial output
// but skip verbose debugging information
#ifdef MINIMIZE_MEMORY
#define DEBUG_PRINT(x) 
#define DEBUG_PRINTLN(x) 
#else
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#endif

void showSymbolGuide() {
  // Clear the display
  Fill_RAM(0x00);
  
  // Show minimal guide on screen with no String objects
  displayText("SYMBOLS", 40, 0);
  
  // Pre-defined strings to avoid String objects
  displayText("\\deg = ", 0, 15);
  drawCharacter((char)176, 42, 15); // Degree symbol
  
  displayText("\\up = ", 0, 25);
  drawCharacter((char)127, 42, 25); // Up arrow
  
  displayText("\\down = ", 0, 35);
  drawCharacter((char)128, 48, 35); // Down arrow
  
  displayText("\\left = ", 64, 15);
  drawCharacter((char)129, 110, 15); // Left arrow
  
  displayText("\\right = ", 64, 25);
  drawCharacter((char)130, 116, 25); // Right arrow
  
  // Minimal serial output
  Serial.println(F("SYMBOL GUIDE"));
}

// Removed Plot_Pixel function to save memory

// Function to reset the display if something goes wrong
void resetDisplay() {
  Serial.println(F("Resetting display..."));
  
  // Reset the display
  CLR_RESET;
  delay(10);
  SET_RESET;
  delay(120);
  
  // Re-initialize
  OLED_Init();
  
  // Clear display
  Fill_RAM(0x00);
  
  // Show ready message
  displayText("READY", 45, 24);
  
  Serial.println(F("Display reset complete"));
}

// Ultra memory-efficient scrolling function that avoids String objects
void simpleScroll(const char* text, uint8_t y) {
  if (!text || !*text) return; // Empty text check
  
  // Calculate text width (approximate)
  uint8_t textLen = 0;
  while (text[textLen]) textLen++;
  uint8_t textWidth = textLen * 6;
  
  // Simple scrolling with minimal memory usage
  for (int16_t startX = 128; startX > -textWidth; startX -= 3) {
    Fill_RAM(0x00);
    
    // Draw characters that are in view
    const char* ptr = text;
    uint8_t charPos = 0;
    while (*ptr) {
      int16_t charX = startX + (charPos * 6);
      if (charX > -6 && charX < 128) { // Only draw visible characters
        drawCharacter(*ptr, charX, y);
      }
      ptr++;
      charPos++;
    }
    
    // Simple delay - most memory efficient
    delay(50);
  }
}

// Simple animation function that doesn't use large bitmap arrays
void simpleAnimation() {
  char buffer[10]; // Small buffer for number display
  
  // Draw 5 simple animation frames with minimal memory usage
  for (uint8_t i = 1; i <= 5; i++) {
    Fill_RAM(0x00);
    
    // Draw frame number
    strcpy(buffer, "Frame ");
    buffer[6] = '0' + i;
    buffer[7] = 0;
    displayText(buffer, 10 + (i*6), 10);
    
    // Draw some animated element
    for (uint8_t j = 0; j < i; j++) {
      drawCharacter('>', 64 + (j*6), 32);
    }
    
    delay(200);
  }
  
  // Final frame
  Fill_RAM(0x00);
  displayText("Animation", 30, 10);
  displayText("Complete!", 30, 30);
}

// Function to show a basic text demo with minimal memory usage
void showTextDemo() {
  Fill_RAM(0x00);
  displayText("Text Demo", 0, 0);
  displayText("Wrapping text test", 0, 16);
  delay(1000);
  simpleScroll("This is scrolling text with arrows and symbols", 32);
}

// Alias for showSymbolGuide to maintain backward compatibility
void showUsageGuide() {
  showSymbolGuide();
}