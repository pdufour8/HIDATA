//Include libraries
#include <Wire.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
extern TwoWire Wire1;

/*
  Neural network characterization system source code
  Name: NN_system
  Purpose: Arduino Due code for the PCB NN board

  @author Patrick Dufour
  @version 1.0 01/09/2019
*/

/********** Declaration of global variables ********/
  /*Full test parameters*/
    //Structure of single pulse
    struct Read_pulse_param{   //Single simple pulse (can be used for write or read)
      float Vmin = 0;             //Minimum read voltage (Volts)
      float Vmax = 1;             //Maximum read voltage (Volts)
      int Pre_hold_Time = 1;    //Pre hold time before read pulse (microsecond)
      int Post_hold_Time = 1;   //Post hold time after read pulse (microsecond)
      int Rise_Time = 1;        //Time taken for the voltage to go from Vmin to Vmax (microsecond)
      int Fall_Time = 1;        //Time taken for the voltage to go from Vmax to Vmin (microsecond)
      int Pulse_Time = 10;      //Time during the pulse is at Vmax (microsecond)
      int Meas_Wait = 10;       //Wait in us before measurements are taken (us)
      int Meas_Stop = 10;       //Stopping measurements #us before pulse end
      int Meas_type = 2;        //Type of measurement: 0 - No measurements / 1 - One measurement in middle / 2 - Measurement every #s
    };

    //Structure of complex write pulse
    struct Write_pulse_param{
       //Vcentral, Vmin/Vmax,Every timing, etc
    };

    //Matrix configuration
    //.... Connections

    //General test parameters
    struct Test_param{
      bool Pulse_rep_mode = false;    //If you want to repeat single pulse a certain number of times
        int Number_pulse = 0;         //The number of pulse you want to do

      bool V_increase_mode = false;   //If you want to increase or decrease voltage every pulse
        float V_inc = 0;              //The increment you want to add (can be positive or negative)

      // .... parameters
      //Number of repetitions, Resistance to reach
    };

    //General test parameters
    struct Full_param{
      
    };
  
  /*Address setup*/
    //Main Board
      //IOexpender
        int IOexpender_write = 32;  //IOexpender I2C address for writting
        //int IOexpender_read = 65;  //IOexpender I2C address for reading - NOT TESTED
    //APMU Board
      //DAC I2C
        int DAC_I2C_write = 96;   //DAC I2C address for writting operation
        int DAC_I2C_read = 192;   //DAC I2C address for reading operation
      //Switch matrix chips (decoder 4:16)
        int Matrix_Select_EN = 6;   //Enable of decoder for selection of switch matrix
        int Matrix_Select_LE = 55;  //Latch Enable of decoder for selection of switch matrix
        int Matrix_Switch_HL = 10;  //State of switch wanted (HIGH/ON or LOW/OF)
        int Matrix_Strobe = 24;     //Strobe control of switch array
        int Matrix_Reset = 28;      //RESET of selected switch matrix
  
  /*Arduino PIN maping*/
    //Main Board
      //Counter
        int Counter_CLK = 53;     //Counter for memory addresses
        int Counter_CLR = 23;     //RESET pin of the counter
      //Decoder 3-8
        int APMU_select_EN = 11;  //Decoder 3-8 for APMU's activation
    //APMU Board
      //DAC
        int DAC_CS = 26;          //Chip select of selected APMU DAC
        int DAC_RW = 30;          //Read/write of selected APMU DAC
        int DAC_B0 = 50;          //Selected APMU DAC Bits - 10 Bits
        int DAC_B1 = 49;
        int DAC_B2 = 48;                        /*....*/
        int DAC_B3 = 47;
        int DAC_B4 = 46;          //////////////////////////////////
        int DAC_B5 = 45;
        int DAC_B6 = 44;
        int DAC_B7 = 9;                         /*.....*/
        int DAC_B8 = 8;
        int DAC_B9 = 7;           //End of DAC 10 bits
      //DAC SRAM memory
        int DAC_mem_CS = 29;      //Chip select of selected APMU DAC memory with APMU_select_EN
        int DAC_mem_WE = 27;      //Write Enable of selected APMU DAC memory with APMU_select_EN
        int DAC_memALL_CS = 25;   //Chip select connected to all APMU DAC SRAM
        int DAC_memALL_OE = 31;   //Output enable connected to all APMU DAC SRAM
      //ADC
        int ADC_EN = 22;          //ADC Enable
        int ADC_CLK = 13;         //ADC Clock to enable every convertions
        int ADC_B0 = 33;          //Selected APMU ADC Bits - 10 Bits
        int ADC_B1 = 34;
        int ADC_B2 = 35;                       /*....*/
        int ADC_B3 = 36;
        int ADC_B4 = 37;          //////////////////////////////////
        int ADC_B5 = 38;
        int ADC_B6 = 39;
        int ADC_B7 = 40;                       /*....*/
        int ADC_B8 = 41;
        int ADC_B9 = 51;          //End of ADC 10 bits
      //ADC SRAM Memory
        int ADC_mem_CS = 42;      //Chip select of selected APMU ADC memory with APMU_select_EN
        int ADC_mem_WE = 52;      //Write Enable of selected APMU ADC memory with APMU_select_EN
        int ADC_memALL_OE = 43;   //Output enable connected to all APMU ADC SRAM
      //Current Range Multiplexer
        int Calibre_B0 = 2;       //Bit0 of current range choice
        int Calibre_B1 = 3;       //Bit1 of current range choice
        int Calibre_B2 = 4;       //Bit2 of current range choice
        int Calibre_CLK = 5;      //Latch of selected current range
        int Calibre_EN = 54;      //Activation of multiplexer
  
  /*----------- 0-15 ------------*/    /*---------- 16-31 -----------*/    /*---------- 32-47 -------------*/    /*---------- 48-64 -----------*/
  short matrix_set[8][64] = /*IN 0*/ {  0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            /*IN 1*/    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            /*IN 2*/    0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            /*IN 3*/    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            /*IN 4*/    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            /*IN 5*/    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            /*IN 6*/    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            /*IN 7*/    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                                     };
  
  /*Funtion's global variables*/
    int bit_10_read_DAC[10];  //Port C - DAC values (PC12-PC23)
    int bit_10_read_ADC[10];  //Port C - ADC values
    int Calibre_3bit = 0;   //Current range choice
    byte port1 = 0;   //Port 1 of IO expender
    byte port2 = 0;   //Port 2 of IO expender
//******************************************//



//********** Function's prototypes ********//
int Security_check(struct Full_param Test);
int Get_needed_samp(struct Read_pulse_param Read_pulse);
float **Get_read_pulse(struct Read_Pulse_param Read_pulse);
//******************************************//



//********* Initialisation of hardware ***********//
void setup() {
  /*Arduino Pin Mode setup*/
    //Mainboard
      //Counter
        pinMode(Counter_CLR, OUTPUT);
        pinMode(Counter_CLK, OUTPUT);
      //Switch matrix (Decoder 4:16)
        pinMode(Matrix_Select_EN, OUTPUT);
        pinMode(Matrix_Select_LE, OUTPUT);
        pinMode(Matrix_Switch_HL, OUTPUT);
        pinMode(Matrix_Reset, OUTPUT);
        pinMode(Matrix_Strobe, OUTPUT);
    //APMU board
      //DAC
        pinMode(DAC_CS, OUTPUT);
        pinMode(DAC_RW, OUTPUT);
        pinMode(DAC_B0, OUTPUT);
        pinMode(DAC_B1, OUTPUT);
        pinMode(DAC_B2, OUTPUT);
        pinMode(DAC_B3, OUTPUT);
        pinMode(DAC_B4, OUTPUT);
        pinMode(DAC_B5, OUTPUT);
        pinMode(DAC_B6, OUTPUT);
        pinMode(DAC_B7, OUTPUT);
        pinMode(DAC_B8, OUTPUT);
        pinMode(DAC_B9, OUTPUT);
      //DAC SRAM Memory
        pinMode(DAC_mem_WE, OUTPUT);
        pinMode(DAC_mem_CS, OUTPUT);
        pinMode(DAC_memALL_CS, OUTPUT);
        pinMode(DAC_memALL_OE, OUTPUT);
      //ADC
        pinMode(ADC_EN, OUTPUT);
        pinMode(ADC_CLK, OUTPUT);
        pinMode(ADC_B0, INPUT);
        pinMode(ADC_B1, INPUT);
        pinMode(ADC_B2, INPUT);
        pinMode(ADC_B3, INPUT);
        pinMode(ADC_B4, INPUT);
        pinMode(ADC_B5, INPUT);
        pinMode(ADC_B6, INPUT);
        pinMode(ADC_B7, INPUT);
        pinMode(ADC_B8, INPUT);
        pinMode(ADC_B9, INPUT);
      //ADC SRAM Memory
        pinMode(ADC_mem_WE, OUTPUT);
        pinMode(ADC_mem_CS, OUTPUT);
        pinMode(ADC_memALL_OE, OUTPUT);
      //Current range multiplexer
        //pinMode(Calibre_B0, OUTPUT);
        //pinMode(Calibre_B1, OUTPUT);
        //pinMode(Calibre_B2, OUTPUT);    //NOT TESTED
        //pinMode(Calibre_CLK, OUTPUT);
        //pinMode(Calibre_EN, OUTPUT);

//  /*Hardware setup*/
//    //Arduino general setup
//      WDT->WDT_MR = WDT_MR_WDDIS;     // Watchdog disable
//      Wire1.begin();
//      Serial.begin(9600);             // Uart communication setup                 
//    //Mainboard
//      //IO expender setup
//        pinMode(APMU_select_EN, OUTPUT);
//        IOexpender_OUTPUT_SETUP();    //Beginning of transmission
//      //Counter setup
//        Counter_CLEAR(); //Reset of memory address counter
//    //APMU Board
//      //APMU default selection
//        APMU_SELECT(0); //Selection of first APMU
//        //Vref default setup at 10V
//        //DAC_I2C_EPROM(250,10); //Vref = 9.99906V - EPROM writting should be used only one time per board
//        DAC_I2C_OUTPUT(25, 16);   //Vref = 1V
//      //Output 0V of selected APMU DAC  - SECURITY Precautions
//        REG_PIOC_ODSR = 0x800000; //Send 0V to DAC
//        DAC_OUTPUT();             //Output 0V with DAC
//      //Setup Calibre
//        //Calibre_SET(0x8); //Choose 100ohms range - NOT TESTED
//
//  REG_PIOC_OWDR = 0xFFFFFFFF; //Reset le registre de masquage
//  REG_PIOC_OWER = 0xEFF000;   //Active PC13 à PC19 et PC21 à PC23 pour écriture
//  digitalWrite(DAC_mem_WE, HIGH);
//  digitalWrite(DAC_mem_CS, HIGH);
//  digitalWrite(DAC_memALL_OE, HIGH);
//  REG_PIOB_CODR = 0x1 << 26; // 22 (ADC_EN) LOW - PB26 = Activate ADC

  __asm__ __volatile__(  //Assembly macro for 119 ns delay (10clock/84MHz)
    ".macro NOPX                  \n\t"
    ".rept 5\                   \n\t"   // adjust the number here
    "NOP                          \n\t"
    ".endr                        \n\t"   // End of Repeat
    ".endm                        \n\t"   // End of macro
    );
  
  delay(500);
}
//***********************************//



//********* Main loop code ***********//
void loop() {

  //Structure with every parameters of the test
  struct Full_param{  //Faire des define pour tous les parametres
    byte mode = 0;        //Measurement mode -> 0 = Single read pulse

    struct Test_param Test;  
    
  };
  //Single read operation

 //__asm__ __volatile__("NOPX"); // The preprocessor replaces this line with 119ns delay


  while (1){}
}
//***********************************//



//********** Declaration of Functions ********//

/*  Security_check(
     struct Test_param)      Structure with all needed parameters to generate a pulse

    @Return int Error_code    The value coresponding to a certain error
     This function verifies if the parameters are correct
*/
int Security_check(struct Full_param Test){    //Possiblement mettre switch case dependament du mode choisi
  //Security check of pulse
    //Faire une "base de donnee" qui dit chaque erreure c'est quoi
    //Rajouter chaque error check possible
      //Vmin Vmax trop bas/haut
      //Un des params est < 1
      //Delay en % >0 et <100
      //Voir nombre de samp pour que les temps en chaque samp soit egaux
    //if((Read_pulse.Vmin < || Read_pulse.Vmax)
    
    return 0; //Success! 
}

/*  Get_needed_samp(
     struct Pulse_param)      Structure with all needed parameters to generate a pulse

    @Return int Total_Time
     This function calculates the number of time windows needed
*/
int Get_needed_samp(struct Read_pulse_param Read_pulse){
  
}


/*  Get_read_pulse(
     struct Pulse_param)      Structure with all needed parameters to generate a pulse

    @Return float Read[2][Total_Time]
     This function generate an array of all needed parameter to generate a read pulse
*/
float **Get_read_pulse(struct Read_pulse_param Read_pulse){
  int Total_Time = 0;
  
  Total_Time =  Read_pulse.Pre_hold_Time + Read_pulse.Post_hold_Time + Read_pulse.Rise_Time + \
                Read_pulse.Fall_Time + Read_pulse.Pulse_Time;     //Sum number of voltage windows for the pulse
      
  if(Total_Time > 200){  //Changer valeur pour plus grand array 
    return 0;
  }

  float Rise_deltaV = (Read_pulse.Vmax - Read_pulse.Vmin)/(Read_pulse.Rise_Time);
  float Fall_deltaV = (Read_pulse.Vmax - Read_pulse.Vmin)/(Read_pulse.Fall_Time);

  float **Read;
  Read = (float**)malloc(sizeof(float*) * Total_Time);

  for(int i = 0; i < 2; i++) {
      Read[i] = (float*)malloc(sizeof(float*) * Total_Time);
  }
  
  //memset( Read, 0, 2*Total_Time*sizeof(float) );  // Initialise Read array with 0
  
  for(int i=0; i<Total_Time; i++){
    
    //Pre Hold Time
    if(i < Read_pulse.Post_hold_Time){
      Read[0][0] = Read_pulse.Vmin;
      Read[1][0] = Read_pulse.Pre_hold_Time;
      Read[2][0] = 0;
    }  
    //Rise Time
    else if(i<(Read_pulse.Pre_hold_Time + Read_pulse.Rise_Time)){
      if((Read_pulse.Pre_hold_Time == 0)&&(i == 0)){  
        Read[0][i] = Rise_deltaV;
      }
      else{
        Read[0][i] = Read[0][i-1]+Rise_deltaV;
      }
      Read[1][i] = 1;
      Read[2][i] = 0;
    }
    //Pulse Time: Waiting for measurements
    else if(i<(Read_pulse.Pre_hold_Time + Read_pulse.Rise_Time + Read_pulse.Meas_Wait)){
      Read[0][i] = Read_pulse.Vmax;
      Read[1][i] = Read_pulse.Meas_Wait; 
      Read[2][i] = 0;   // No measurements during wait
    }
    //Pulse Time: measurements
    else if(i<(Read_pulse.Pre_hold_Time + Read_pulse.Rise_Time + Read_pulse.Pulse_Time - Read_pulse.Meas_Stop)){
      Read[0][i] = Read_pulse.Vmax;
      Read[1][i] = Read_pulse.Pulse_Time; 
      Read[2][i] = Read_pulse.Meas_type;   // Measurement type choosen
    }
    //Pulse Time: Stop measurements
    else if(i<(Read_pulse.Pre_hold_Time + Read_pulse.Rise_Time + Read_pulse.Pulse_Time)){
      Read[0][i] = Read_pulse.Vmax;
      Read[1][i] = Read_pulse.Meas_Stop; 
      Read[2][i] = 0;   // No measurements during stop
    }
    //Fall Time
    else if(i<(Total_Time - Read_pulse.Post_hold_Time)){
      Read[0][i] = Read[0][i-1]-Fall_deltaV;
      Read[1][i] = 1; 
      Read[2][i] = 0;   // No measurements during stop
    }
    //Post Hold Time
    else if(i<Total_Time){
      Read[0][i] = Read_pulse.Vmin;
      Read[1][i] = Read_pulse.Post_hold_Time; 
      Read[2][i] = 0;   // No measurements during stop
    }    
  }
  
  return Read;
}
//***********************************//

