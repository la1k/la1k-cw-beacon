/*
    Simple mixed mode beacon sending PI4, CW callsign and locator and carrier
    for use with the Aurdino Uno platform.
    
    GPS, or another time provider, is not need as it would be in real life.
    To improve timr precission interrupt timing shoudl be implementes.
    
    The LED connected to pin 13 flashed with transmission.

    Environment
    - IDE: Arduino IDE 1.0.1
    - Platform: Arduino Uno LED on pin 13 (built in on Arduino Uno)
    - Hardware: Speaker connected on pin 8

    By Bo, OZ2M - www.rudius.net/oz2m
*/

// Hardware parameters
const int PinSpeaker=8;         // The pin where the speaker is connected
const int PinLED=13;            // Tee pin where the LED is connected

// Software variables
//                     _123456789_123456789_123456789_1234567
const char PI4Chars[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ /";
const char PI4Vector[]={0,0,1,0,0,1,1,1,1,0,1,0,1,0,1,0,0,1,0,0,0,1,0,0,0,1,1,0,0,1,
                        1,1,1,0,0,1,1,1,1,1,0,0,1,1,0,1,1,1,1,0,1,0,1,1,0,1,1,0,1,0,
                        0,0,0,0,1,1,1,1,1,0,1,0,1,0,0,0,0,0,1,1,1,1,1,0,1,0,0,1,0,0,
                        1,0,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,1,1,1,
                        0,1,1,1,0,1,1,0,1,0,1,0,1,0,0,0,0,1,1,1,0,0,0,0,1,1};
const int PI4MaxInfoLength=8;
const int PI4Symbols=146;       // The number of symbols in the PI4 transmission
int Symbols[PI4Symbols];        // The PI4 symbols
int PI4Tones[4];                // The PI4 tones

int CWCallsign[8];
int CWCallsignLength;
int CWLocator[6];
int CWLocatorLength;

// User variables
char PI4Message[9]="OZ7IGY  ";  // Length of PI4 Message must be 8 charcters. If need pad with spaces
char Callsign[9]="OZ7IGY";      // Max length of callsign is 8 charcters
char Locator[7]="JO55WM";       // Max length of locator is 6 charcters
const int CWTone=800;

int Parity(long int Value)
{
  int Even=0;
  int BitNo;
  for (BitNo=0;BitNo<=31;BitNo++)
    if (((Value >> BitNo) & 0x01) != 0)
      Even=1-Even;

  return Even;
}

void PI4MakeMessage(char *Msg)
{
  int I,J;
  int BitNo;
  
  // Source encoding
  unsigned long long SourceEnc;
  
  SourceEnc=0;
  for (I=0;I<PI4MaxInfoLength;I++)
    SourceEnc=SourceEnc*38+(unsigned long long) (strchr(PI4Chars,Msg[I])-PI4Chars);
  
  // Convolutional Encoding
  const unsigned long Poly1=0xF2D05351;
  const unsigned long Poly2=0xE4613C47;
  unsigned long N=0;
  char ConvEnc[PI4Symbols]={0};

  I=0;
  for (J=0;J<PI4Symbols/2;J++)
  {
    N <<= 1;
    if((SourceEnc & 0x20000000000LLU) != 0)
      N |= 1;
    SourceEnc <<= 1;

    ConvEnc[I++]=Parity(N & Poly1);
    ConvEnc[I++]=Parity(N & Poly2);
  } 

  // Interleaving
  int P=0;
  int R=0;
  int Interleaved[PI4Symbols]={0};
  
  for (I=0;I<=255;I++)
  {
    for (BitNo=0;BitNo<=7;BitNo++)
    {
      if (((I >> BitNo) & 0x01)==0x01)
	R |= 1 << (7-BitNo);
      else
	R &= ~(1Lu << (7-BitNo));
    }

    if ((P<PI4Symbols) && (R<PI4Symbols))
      Interleaved[R]=ConvEnc[P++];
  }

  //  Merge With Sync Vector
  for (I=0;I<PI4Symbols;I++)
    Symbols[I]=PI4Vector[I]+(2*Interleaved[I]);
}

int CharToMorse(char Ch)
{
  const char Letters[]={0x42,0x84,0xA4,0x83,0x01,0x24,0xC3,0x04,0x02,0x74,0xA3,0x44,0xC2,0x82,0xE3,0x64,0xD4,0x43,0x03,0x81,0x23,0x14,0x63,0x94,0xB4,0xC4};
  const char Numbers[]={0x95,0xFD,0x7D,0x3D,0x1D,0x0D,0x05,0x85,0xC5,0xE5,0xF5};
	
  if (Ch==0)
    return(0);                                          // Not a valid morse character
  else
    if (Ch<'A')                                         // Get then Morse pattern
      return(Numbers[Ch-'/']);
    else
      return(Letters[Ch-'A']);
}

void SendMorse(const int *Info, const int Length)
{
  const int CWDotDuration=100;                          // 100 ms = 60 LPM = 12 WPM
  int I,J;
  int Morse;
  int MorseLength;
  
  for (I=0;I<Length;I++)
  {
    Morse=Info[I];
    MorseLength=Morse & 0x07;                           // Bit2 to Bit0 of Morse[I] is the length

    for (J=0;J<MorseLength;J++)
    {
      tone(PinSpeaker,CWTone);  
      digitalWrite(PinLED,HIGH);                        // Turn LED on
      
      if ((Morse & 0x80)==0x80)                         // If MSB 0 = dot, 1 = dash, 
        delay(3*CWDotDuration);                         // It is a dash, so wait 3 dot durations
      else 
        delay(CWDotDuration);                           // It is a dot, so wait 1 dot duration
		  
      noTone(PinSpeaker);  
      digitalWrite(PinLED,LOW);                         // Turn LED off
      delay(CWDotDuration);
      Morse=Morse <<1;                                  // Point to next bit 
    }
    delay(CWDotDuration << 1);                          // Inter morse character pause, 1 dot duration already "gone" so only 2 left
  } 
}

void setup()
{
  int I;
  
  pinMode(PinLED,OUTPUT);                               // Enable LID on pin 13
  digitalWrite(PinLED,LOW);                             // Turn LED off
  
  // Prepare PI4
  PI4MakeMessage(PI4Message);                           // Convert the PI4 message to symbols
  
  PI4Tones[0]=CWTone- (float) 20*12000/2048+0.5;        // Calculate the PI4 tones with K=40
  PI4Tones[1]=CWTone+ (float) 20*12000/2048+0.5;
  PI4Tones[2]=CWTone+ (float) 60*12000/2048+0.5;
  PI4Tones[3]=CWTone+ (float) 100*12000/2048+0.5;
  
  // Prepare CW Callsign
  CWCallsignLength=strlen(Callsign);                    // Find the length of the now encoded CW callsign
  for (I=0;I<CWCallsignLength;I++)                                                                     // Put the EEPROM CW Callsign into the active CW Callsign
    CWCallsign[I]=CharToMorse(Callsign[I]);             // Encode the callsign character
   
  // Prepare CW Locator
  CWLocatorLength=strlen(Locator);                      // Find the length of the now encoded CW callsign
  for (I=0;I<CWLocatorLength;I++)                                                                     // Put the EEPROM CW Callsign into the active CW Callsign
    CWLocator[I]=CharToMorse(Locator[I]);               // Encode the locator character
}

void loop()
{
  int I;
  
  // Wait until 59 s with carrier it real beacon program
  tone(PinSpeaker,CWTone);
  digitalWrite(PinLED,HIGH);                            // Set LED high to indicate carrier
  delay(20000);                                         // Delay "until" sec 59 is reached
  delay(300);                                           // Delay 500 ms incl GPS intrepretation time

  noTone(PinSpeaker);                                   // Speaker off
  digitalWrite(PinLED,LOW);                             // Set LED high to indicate carrier
  delay(500);                                           // Delay between carrier and PI4
  
  // Send PI4 message
  for (I=0;I<PI4Symbols;I++)
  {
    tone(PinSpeaker,PI4Tones[Symbols[I]]);
    digitalWrite(PinLED,!digitalRead(PinLED));          // Toggle LED on/off

    delay(167);                                         // Symbol width 166,667 ms
  }
  noTone(PinSpeaker);                                   // Speaker off
  
  delay(667);                                           // Pause between PI4 message and CW callsign
 
  // Send CW callsign
  SendMorse(CWCallsign,CWCallsignLength);
  delay(500);                                           // Pause between CW callsign and locator
  
  // Send CW locator
  SendMorse(CWLocator,CWLocatorLength);
  delay(500);                                           // Pause between CW locator and carrier
}
