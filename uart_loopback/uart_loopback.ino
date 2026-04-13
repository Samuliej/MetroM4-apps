#include <cstdint>
#include <cassert>

constexpr bool USE_GCLK5 = true; // This can be flipped to change between GCLK0 and GCLK5
// We're using a 12MHz clock, so we divide the 120MHz by 10.
constexpr float DIVISOR = 10.0f;
constexpr float BASE_FREQ = 120000000.0f;
// GCLK0 is the main clock which runs at 120MHz. Should be the same for the FDPLL200M0 oscillator
constexpr float F_REF = USE_GCLK5 ? BASE_FREQ / DIVISOR : BASE_FREQ;
constexpr float F_BAUD = 9600.0f;         // Wanted baud rate
constexpr float REGISTER_SIZE = 65536.0f; // Register size from the equation
constexpr uint8_t GCLK_SERCOM1_CORE = 8;  // Page 155, PCHCTRLm mapping SERCOM1 => index(m) 8
constexpr uint32_t SERCOM1_CLOCK_ENABLE_BIT = (0x1 << 13);

// Calculate the Baud rate
// We want to use GCLK5 for practise and change the frequency.
// Since no dynamic values are needed in the calculation, we calculate the baud rate already in compile time
constexpr uint16_t BAUD_RATE = (uint16_t)(REGISTER_SIZE * (1.0f - 16.0f * (F_BAUD / F_REF))); // Page 853

void setup() {
  Serial.begin(9600);
  
  // We're using pins D13 and D12 on the metro m4, which correspond to SERCOMS
  // 1 or 3, we're choosing 1.
  // Table for pins page 32
  // Page 48, product memory mapping overview
  // Pages 164 & 173 for specific bits. bit for enabling the clock for SERCOM1 is the 13'th bit in the APBAMASK
  MCLK->APBAMASK.reg |= SERCOM1_CLOCK_ENABLE_BIT; // Unmask the clock in the MCLK register, which wakes up the SERCOM module.
                                                  // Using CMSIS MCLK_APBAMASK_SERCOM1 basically does the same thing

  configureClocks();
  configurePins();
  initUsart();
}


void loop() {
  transmitByte();
  receiveByte();
  delay(1000);
}


void transmitByte() {
  while (!(SERCOM1->USART.INTFLAG.bit.DRE));
  // Write an character to the DATA register
  SERCOM1->USART.DATA.reg = 'A';

  // These would work also
  // SERCOM1->USART.DATA.reg = 65;
  // SERCOM1->USART.DATA.reg = 0x41;
}


void receiveByte() {
  if (!SERCOM1->USART.INTFLAG.bit.RXC) return;

  // Read from the DATA register, cast it to a character and print it
  char receivedChar = (char)SERCOM1->USART.DATA.reg;
  Serial.println(receivedChar);
}


void configureClocks() {
  if (USE_GCLK5) initGCLK5();
  else initGCLK0();
}


void initGCLK0() {
  // Easy way, use the main GCLK0 clock
  // Configure the GCLK to feed a known clock source to the SERCOM1 core. GCLK0 = 120MHz
  //                                   GCLK_PCHCTRL_GEN_GCLK0 does 0x0 << 0, so that means it's on by default,
  //                                   meaning it would be enough just to enable the peripheral channel
  GCLK->PCHCTRL[GCLK_SERCOM1_CORE].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
}


// Use a custom GCLK5 clock
void initGCLK5() {
  // Pages 143-145
  GCLK->PCHCTRL[GCLK_SERCOM1_CORE].bit.CHEN = 0;     // First disable peripheral clock from the SERCOM1 during the switch
  while (GCLK->PCHCTRL[GCLK_SERCOM1_CORE].bit.CHEN); // Wait until the peripheral clock is disabled

  GCLK->GENCTRL[5].bit.DIVSEL = 0; // We don't want exponential division and the CMSIS structure GCLK_GENCTRL_DIVSEL would enable that.
  GCLK->GENCTRL[5].bit.DIV = 0xA;  // We can control the clock frequency by setting a value in DIV,
                                   // which then divides the frequency according to DIVSEL

  GCLK->GENCTRL[5].reg |= GCLK_GENCTRL_SRC(7) // Set the source as FDPLL200M0, and enable the clock generator. Page 152 
                                              // Since the max speed for this chip is 120MHz, I'm assuming that the FDPLL 
                                              // oscillator is running at the same frequency, since it's defined between 90-200MHz. Page 4
                       | GCLK_GENCTRL_GENEN;  // Enable the generator

  // We're enabling non-default clock generator so we must wait until the switch is complete page 144
  while (GCLK->SYNCBUSY.bit.GENCTRL5); // SYNCBUSY register will remain '1' until the switch operation is completed

  GCLK->PCHCTRL[GCLK_SERCOM1_CORE].reg = GCLK_PCHCTRL_GEN_GCLK5 // Set the GCLK5 as the GCLK for SERCOM1. Page 155
                                        | GCLK_PCHCTRL_CHEN;     // Enable the GCLK

  // Wait until the peripheral clock is enabled for the SERCOM1 Page 145
  // We need to wait because the CHEN bit must be synchronized to the generic clock domain
  while (!(GCLK->PCHCTRL[GCLK_SERCOM1_CORE].bit.CHEN));
}


void configurePins() {
  // group[0] = Pins PAn
  // Set the PINCFGs in group[0] for TX and RX pins to enable PMUXEN bit
  // We're basically telling the processor that we will be using these 
  // pins for other than their default function
  PORT->Group[0].PINCFG[16].bit.PMUXEN = 1;
  PORT->Group[0].PINCFG[17].bit.PMUXEN = 1;

  const uint8_t muxIndex = 8;  // 16 / 2 = 8 & floor(17 / 2) = 8

  // Page 844
  // Write to the PMUX register to assign the correct multiplexing function
  // 0x2 = C, since SERCOM1.PAD[0] and SERCOM1.PAD[1] are in the C column on table on page 32.
  PORT->Group[0].PMUX[muxIndex].bit.PMUXE = 0x2; // Pin PA16 (even) PAD[0]
  PORT->Group[0].PMUX[muxIndex].bit.PMUXO = 0x2; // Pin PA17 (odd)  PAD[1]
}


void initUsart() {
  // SERCOM1->USART.CTRLA.bit.MODE = 0x1;
  // SERCOM1->USART.CTRLA.bit.TXPO = 0x0; 
  // SERCOM1->USART.CTRLA.bit.RXPO = 0x1; 


  SERCOM1->USART.CTRLA.reg |= (0x1 << 2)   // Set Internal Asynchronous mode (0x1) to use the internal clock.
                                           // This opts out of the shared (synchronous) clock, allowing the USART 
                                           // to run on its own frequency without being in sync with an external signal. Page 852

                            // Set the corresponding bits for TXPO (transmit) and RXPO (receive) bits in the USART CTRLA. Page 879
                            | (0x0 << 16)  // Output transmit, sets the PA16 (PAD[0]) as TXPO
                            | (0x1 << 20)  // Input receive, sets the PA17 (PAD[1]) as RXPO

                            | (0x1 << 30); // Set data order Least Significant Bit first. Page 878
                                           // Could not find the CMSIS structs for these.
  

  
  // SERCOM1->USART.CTRLA.bit.DORD = 1;

  // Enable the receiver / will be enabled when the USART is enabled. page 882
  SERCOM1->USART.CTRLB.bit.RXEN = 1;

  // Enable the transmitter / will be enabled when the USART is enabled. Page 883
  SERCOM1->USART.CTRLB.bit.TXEN = 1;
  
  SERCOM1->USART.CTRLB.bit.CHSIZE = 0x0; // Set the character size to 8bits. Page 884
  SERCOM1->USART.BAUD.reg = BAUD_RATE;   // Write the calculated baud rate to the register. Page 887

  // Finally, Enable USART in the SERCOM
  SERCOM1->USART.CTRLA.bit.ENABLE = 1;
  while (SERCOM1->USART.SYNCBUSY.bit.ENABLE);  // Wait until the USART is enabled
                                               // SYNCBUSY.ENABLE is cleared when the operation is complete. Page 880
}
