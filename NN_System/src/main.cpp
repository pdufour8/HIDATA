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

#define MODE_MATRIX_SCAN  1
#define MODE_READ_WRITE   2
#define MODE_WRITE_SWEEP  3
#define MODE_MATRIX_SWEEP 4

/********** Declaration of global variables ********/
    /*Full test parameters*/
    //Structure of single pulse
    struct Test_param{
        struct Read_pulse_param{   //Single simple pulse (can be used for write or read)
            char Pulse_Type = 'R';    //Read type pulse
            float Vmin = 0;           //Minimum read voltage (Volts)
            float Vmax = 0.2;         //Maximum read voltage (Volts)
            int Pre_hold_Time = 2;    //Pre hold time before read pulse (microsecond)
            int Post_hold_Time = 2;   //Post hold time after read pulse (microsecond)
            int Rise_Time = 1;        //Time taken for the voltage to go from Vmin to Vmax (microsecond)
            int Fall_Time = 1;        //Time taken for the voltage to go from Vmax to Vmin (microsecond)
            int Pulse_Time = 4;       //Time during the pulse is at Vmax (microsecond)
            int Meas_Wait = 1;        //Wait in us before measurements are taken (us)
            int Meas_Stop = 1;        //Stopping measurements #us before pulse end
            int Meas_type = 3;        //Type of measurement: 0 - No measurements / 1 - One measurement in middle / 2 - Measurement every #us
            int APMU_Pulse = 8;       //APMU used to Read the specific point
            int APMU_GND = 7;         //APMU used to ground the specific point
        }Read;

        struct Write_pulse_param{   //Single simple pulse (can be used for write or read)
            char Pulse_Type = 'W';    //Write type pulse
            float Vmin = 0;           //Minimum read voltage (Volts)
            float Vmax = 4;           //Maximum read voltage (Volts)
            int Pre_hold_Time = 1;    //Pre hold time before read pulse (microsecond)
            int Post_hold_Time = 1;   //Post hold time after read pulse (microsecond)
            int Rise_Time = 1;        //Time taken for the voltage to go from Vmin to Vmax (microsecond)
            int Fall_Time = 1;        //Time taken for the voltage to go from Vmax to Vmin (microsecond)
            int Pulse_Time = 2;       //Time during the pulse is at Vmax (microsecond)
            int Meas_Wait = 0;        //Wait in us before measurements are taken (us)
            int Meas_Stop = 0;        //Stopping measurements #us before pulse end
            int Meas_type = 0;        //Type of measurement: 0 - No measurements / 1 - One measurement in middle / 2 - Measurement every #us
            int APMU_Pulse = 8;       //APMU used to Read the specific point
            int APMU_GNG = 7;         //APMU used to connect the opposite node of the point
        }Write;

        struct Sampling_pulse_param{   //Single simple pulse (can be used for write or read)
            char Pulse_Type = 'S';    //Sampling type pulse
            float Vmin = 0;           //Minimum read voltage (Volts)
            float Vmax = 4;           //Maximum read voltage (Volts)
            int Rise_Time = 2;        //Time taken for the voltage to go from Vmin to Vmax (microsecond)
            int Fall_Time = 3;        //Time taken for the voltage to go from Vmax to Vmin (microsecond)
            int Pulse_Time = 3;       //Time during the pulse is at Vmax (microsecond)
            int APMU_Pulse = 8;       //APMU used to Read the specific point
            int APMU_GNG = 7;         //APMU used to connect the opposite node of the point
        }Sampling;
        
        struct Matrix_scan{
            // Matrix_scan is used to scan a crossbar array by reading with the
            // Read_pulse defined above, every point of the crossbar, one by one.
            //Faire avec un seul APMU ou temps est critique?
            //Reset la matrice et commencer les test au points 1 avec APMU choisi
            int Num_Line = 2;         //Number of line to scan
            int Num_column = 2;       //Number of column to scan
            int APMU_pulse = 7;       //APMU used to send read pulse
            int APMU_read = 6;        //APMU used to read the current      
        };

        //Default matrix setup
                                             /*----------- 0-15 ------------*/    /*---------- 16-31 -----------*/    /*---------- 32-47 -------------*/    /*---------- 48-64 -----------*/
         short matrix_set[8][64] = /*IN 0*/{  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                  /*IN 1*/   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                  /*IN 2*/   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                  /*IN 3*/   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                  /*IN 4*/   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                  /*IN 5*/   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                  /*IN 6*/   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0/*1*/,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                  /*IN 7*/   0,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0};

        bool TestFinish = false;
        int Test_Mode = MODE_MATRIX_SCAN;   //Mode used for tests
    };
    
    struct Pulse_param{   //Single simple pulse (can be used for write or read)
      char Pulse_Type = 'R';    //Type of pulse
      float Vmin = 0;           //Minimum read voltage (Volts)
      float Vmax = 0.2;         //Maximum read voltage (Volts)
      int Pre_hold_Time = 1;    //Pre hold time before read pulse (microsecond)
      int Post_hold_Time = 1;   //Post hold time after read pulse (microsecond)
      int Rise_Time = 5;        //Time taken for the voltage to go from Vmin to Vmax (microsecond)
      int Fall_Time = 5;        //Time taken for the voltage to go from Vmax to Vmin (microsecond)
      int Pulse_Time = 10;       //Time during the pulse is at Vmax (microsecond)
      int Meas_Wait = 0;        //Wait in us before measurements are taken (us)
      int Meas_Stop = 0;        //Stopping measurements #us before pulse end
      int Meas_type = 0;        //Type of measurement: 0 - No measurements / 1 - One measurement in middle / 2 - Measurement every #s
      int APMU_Pulse = 8;        //APMU used to Read the specific point
      int APMU_GNG = 7;         //APMU used to connect the opposite node of the point
      int Total_Time = 0;       //Number of windows needed for pulse
    };

    //Matrix configuration
    //.... Connections

    /*
    //General test parameters
    struct Test_param{
      bool Pulse_rep_mode = false;    //If you want to repeat single pulse a certain number of times
        int Number_pulse = 0;         //The number of pulse you want to do

      bool V_increase_mode = false;   //If you want to increase or decrease voltage every pulse
        float V_inc = 0;              //The increment you want to add (can be positive or negative)

      // .... parameters
      //Number of repetitions, Resistance to reach
    };
    */
  
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
  
  /*Funtion's global variables*/
    int bit_10_read_DAC[10];  //Port C - DAC values (PC12-PC23)
    int bit_10_read_ADC[10];  //Port C - ADC values
    int Calibre_3bit = 0;   //Current range choice
    byte port1 = 0;   //Port 1 of IO expender
    byte port2 = 0;   //Port 2 of IO expender
//******************************************//



//********** Function's prototypes ********//
  // Hardware functions for PCB
    //Mainboard
      //IOexpender
        void IOexpender_OUTPUT_SETUP();
        byte IOexpender_READ(int port);
        void IOexpender_WRITE(int data1, int data2);

  // Function for test preparation
    int Security_check(struct Full_param Test);
    void Get_needed_samp(struct Pulse_param *Pulse_Param);
    float** Get_pulse(struct Pulse_param Pulse_Param);
    void Pulse_setting(struct Test_param Test, struct Pulse_param *ptr_Read, struct Pulse_param *ptr_Write, struct Pulse_param *ptr_Sampling);

  // Function for tests
    void Test_pulse(float **Pulse, struct Pulse_param Pulse_Param);

  // Function of different test execution types
    int Simple_Read(struct Full_param Test);
    int Simple_Write(struct Full_param Test);
    int Simple_Sampling(struct Full_param Test);
    int Matrix_Scan(struct Full_param Test, struct Pulse_param *Read);
    int Read_Write(struct Full_param Test);
    int Write_Sweep(struct Full_param Test);
    int Matrix_Sweep(struct Full_param Test);
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
  
  Serial.begin(9600);             // Uart communication setup

  delay(500);
}
//***********************************//



//********* Main loop code ***********//
void loop() {
 //__asm__ __volatile__("NOPX"); // The preprocessor replaces this line with 119ns delay

  float **Read;     // Vector of Read pulse
  float **Write;    // Vector of Write pulse
  float **Sampling; // Vector of Sampling pulse


  struct Test_param Test;   // Full test initial parameters

  // Creation of Read and Write pulses objects
  struct Pulse_param Read_pulse;        // Read pulse parameters
  struct Pulse_param Write_pulse;       // Write pulse parameters
  struct Pulse_param Sampling_pulse;    // Sampling pulse parameters
  
  // Copy of setting pulses to test pulses
  Pulse_setting(Test, &Read_pulse, &Write_pulse, &Sampling_pulse); 

  // Setting needed number of windows
  Get_needed_samp(&Read_pulse);
  Get_needed_samp(&Write_pulse);
  Get_needed_samp(&Sampling_pulse);

  // Getting vector for Read and Write pulses
  Read = Get_pulse(Read_pulse);
  Write = Get_pulse(Write_pulse);
  Sampling = Get_pulse(Sampling_pulse);
  
  // Test part of the Get_pulse() function
  Test_pulse(Read, Read_pulse);
  Test_pulse(Write, Write_pulse);
  Test_pulse(Sampling, Sampling_pulse);

    while(1){

    if(!Test.TestFinish){
        // Switch case for test type
        switch(Test.Test_Mode){
            case MODE_MATRIX_SCAN:
            //Fonction de matrix scan
            break;
            
            case MODE_READ_WRITE:
            //Fonction de Read write
            break;

            case MODE_WRITE_SWEEP:
            //Fonction de write sweep
            break;

            case MODE_MATRIX_SWEEP:
            //Fonction de Matrix sweep:
            break;
        }
    }  
    
  free(Read);
  free(Write);


    }
  
  while (1){}
}
//***********************************//



//********** Declaration of Functions ********//

/*  Security_check(
     struct Test_param)      Structure with all needed parameters to generate a pulse

    @Return int Error_code    The value coresponding to a certain error
     This function verifies if the parameters are correct
*/
int Security_check(struct Test_param Test){    //Possiblement mettre switch case dependament du mode choisi
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
     struct *Pulse_param)      Structure pointer with all needed parameters to generate a pulse

    @Return int Total_Time
     This function calculates the number of time windows needed
*/
void Get_needed_samp(struct Pulse_param *Pulse_Param){
  int Total_Time = 0;
  
  Total_Time += (Pulse_Param->Rise_Time + Pulse_Param->Fall_Time);

  // Counting the number of windows needed
  if(Pulse_Param->Pulse_Time != 0){
    Total_Time++;
  }
  if(Pulse_Param->Pre_hold_Time != 0){
      Total_Time++;
  }
  if(Pulse_Param->Post_hold_Time != 0){
      Total_Time++;  
  }

  if((Pulse_Param->Pulse_Type == 'R') || (Pulse_Param->Pulse_Type == 'W')){ 
    if(Pulse_Param->Meas_Wait != 0){
        Total_Time++;
    }
    if(Pulse_Param->Meas_Stop != 0){
        Total_Time++;  
    }
  }
    
  Pulse_Param->Total_Time = Total_Time;
}


/*  Get_pulse(
     struct Pulse_param)      Structure with all needed parameters to generate a Pulse_Param

    @Return float Pulse[2][Total_Time]
     This function generate an array of all needed parameter to generate a Pulse Pulse_Param
*/
float** Get_pulse(struct Pulse_param Pulse_Param){
  float Rise_deltaV = 0; 
  float Fall_deltaV = 0; 

  int Total_Time = Pulse_Param.Total_Time;
  
  Rise_deltaV = (Pulse_Param.Vmax - Pulse_Param.Vmin)/(Pulse_Param.Rise_Time+1);
  Fall_deltaV = (Pulse_Param.Vmax - Pulse_Param.Vmin)/(Pulse_Param.Fall_Time+1);
  
  float **Pulse;    // Array that will contain every parameter for the generation of a pulse
  Pulse = (float**)malloc(sizeof(float*) * Total_Time);
  
  for(int i = 0; i < 3; i++) {
      Pulse[i] = (float*)malloc(sizeof(float*) * Total_Time);
  }
  
  //memset( Pulse, 0, 2*Total_Time*sizeof(float) );  // Initialise Pulse array with 0
  int off = 0;    //offset position
  int off_post = 0;  //offset reinitialiser

  if((Pulse_Param.Pulse_Type == 'R') || (Pulse_Param.Pulse_Type == 'W')){
    for(int i=0; i<Total_Time; i++){
        //Pre Hold Time
        if((Pulse_Param.Pre_hold_Time != 0) && (i < 1)){
        Pulse[0][i] = Pulse_Param.Vmin;
        Pulse[1][i] = Pulse_Param.Pre_hold_Time;
        Pulse[2][i] = 0;
        off++;
        }
        //Rise Time
        else if(i < (off + Pulse_Param.Rise_Time)){
          if((Pulse_Param.Pre_hold_Time == 0)&&(i < 1)){  
              Pulse[0][i] = Rise_deltaV;
          }
          else{
              Pulse[0][i] = Pulse[0][i-1]+Rise_deltaV;
          }
          Pulse[1][i] = 1;
          Pulse[2][i] = 0;
        }
        //Waiting for measurements
        else if(((Pulse_Param.Meas_Wait != 0) && (i<(off + Pulse_Param.Rise_Time + 1))) && (off < 2)){
          Pulse[0][i] = Pulse_Param.Vmax;
          Pulse[1][i] = Pulse_Param.Meas_Wait; 
          Pulse[2][i] = 0;   // No measurements during wait
          off++;
        }
        //Measurements
        else if(i<(off + Pulse_Param.Rise_Time + 1)){
          Pulse[0][i] = Pulse_Param.Vmax;
          Pulse[1][i] = Pulse_Param.Pulse_Time; 
          Pulse[2][i] = Pulse_Param.Meas_type;   // Measurement type choosen
        }
        //Stop measurements
        else if((Pulse_Param.Meas_Stop != 0) && (i<(off + Pulse_Param.Rise_Time + 2))){
          Pulse[0][i] = Pulse_Param.Vmax;
          Pulse[1][i] = Pulse_Param.Meas_Stop; 
          Pulse[2][i] = 0;   // No measurements during stop
        }
        //Fall Time
        else if(i<(Total_Time - 1)){
          Pulse[0][i] = Pulse[0][i-1]-Fall_deltaV;
          Pulse[1][i] = 1; 
          Pulse[2][i] = 0;   // No measurements during stop
        }
        //Post Hold Time
        else if(i<(Total_Time)){
          Pulse[0][i] = Pulse_Param.Vmin;
          Pulse[1][i] = Pulse_Param.Post_hold_Time; 
          Pulse[2][i] = 0;   // No measurements during stop
        }    
    }
  }
  else if(Pulse_Param.Pulse_Type == 'S'){
      for(int i=0; i<Total_Time; i++){
        //Pre Hold Time
        if((Pulse_Param.Pre_hold_Time != 0) && (i < 1)){  //If there is a pre and is not considered yet
            Pulse[0][i] = Pulse_Param.Vmin;
            Pulse[1][i] = Pulse_Param.Pre_hold_Time;
            Pulse[2][i] = 2;
            off++;
        }
        //Rise Time
        else if(i < (off + Pulse_Param.Rise_Time)){ //Else if under offset + rise time
            if((Pulse_Param.Pre_hold_Time == 0)&&(i < 1)){  //There was no pre and you start on a rise
                Pulse[0][i] = Rise_deltaV;
            }
            else{
                Pulse[0][i] = Pulse[0][i-1]+Rise_deltaV;  //If there was a pre
            }
            Pulse[1][i] = 1;
            Pulse[2][i] = 2;
        }
        //Measurements
        else if(i<(off + Pulse_Param.Rise_Time + 1)){
            Pulse[0][i] = Pulse_Param.Vmax;
            Pulse[1][i] = Pulse_Param.Pulse_Time; 
            Pulse[2][i] = 2;   
        }
        //Fall Time
        else if(i<(Total_Time - 1)){
            Pulse[0][i] = Pulse[0][i-1]-Fall_deltaV;
            Pulse[1][i] = 1; 
            Pulse[2][i] = 2;   
        }
        //Post Hold Time
        else if(i<(Total_Time)){
            Pulse[0][i] = Pulse_Param.Vmin;
            Pulse[1][i] = Pulse_Param.Post_hold_Time; 
            Pulse[2][i] = 2;  
        }    
    }
  }

  return Pulse;
}

/*  Pulse_setting(
      struct Test_param Test,             Structure with all parameters needed for the test execution
      struct Pulse_param *ptr_Read,       Pointer to the Read pulse (empty) array
      struct Pulse_param *ptr_Write,      Pointer to the Write pulse (empty) array
      struct Pulse_param *ptr_Sampling)   Pointer to the Sampling pulse (empty) array

    @Return void
     This function receive a default pulse for every pulse type and give the right parameters to the Read, Write and Sampling pulses
*/
void Pulse_setting(struct Test_param Test, struct Pulse_param *ptr_Read, struct Pulse_param *ptr_Write, struct Pulse_param *ptr_Sampling){
  struct Test_param *ptr_Test = &Test;
  
  // Copy of initial parameters of Read pulse
  ptr_Read->Pulse_Type = ptr_Test->Read.Pulse_Type;
  ptr_Read->Vmin = ptr_Test->Read.Vmin;
  ptr_Read->Vmax = ptr_Test->Read.Vmax;
  ptr_Read->Pre_hold_Time = ptr_Test->Read.Pre_hold_Time;
  ptr_Read->Post_hold_Time = ptr_Test->Read.Post_hold_Time;
  ptr_Read->Rise_Time = ptr_Test->Read.Rise_Time;
  ptr_Read->Fall_Time = ptr_Test->Read.Fall_Time;
  ptr_Read->Pulse_Time = ptr_Test->Read.Pulse_Time;
  ptr_Read->Meas_Wait = ptr_Test->Read.Meas_Wait;
  ptr_Read->Meas_Stop = ptr_Test->Read.Meas_Stop;
  ptr_Read->Meas_type = ptr_Test->Read.Meas_type;

  // Copy of initial parameters of Write pulse
  ptr_Write->Pulse_Type = ptr_Test->Write.Pulse_Type;
  ptr_Write->Vmin = ptr_Test->Write.Vmin;
  ptr_Write->Vmax = ptr_Test->Write.Vmax;
  ptr_Write->Pre_hold_Time = ptr_Test->Write.Pre_hold_Time;
  ptr_Write->Post_hold_Time = ptr_Test->Write.Post_hold_Time;
  ptr_Write->Rise_Time = ptr_Test->Write.Rise_Time;
  ptr_Write->Fall_Time = ptr_Test->Write.Fall_Time;
  ptr_Write->Pulse_Time = ptr_Test->Write.Pulse_Time;
  ptr_Write->Meas_Wait = ptr_Test->Write.Meas_Wait;
  ptr_Write->Meas_Stop = ptr_Test->Write.Meas_Stop;
  ptr_Write->Meas_type = ptr_Test->Write.Meas_type;

  // Copy of initial parameters of Sampling pulse
  ptr_Sampling->Pulse_Type = ptr_Test->Sampling.Pulse_Type;
  ptr_Sampling->Vmin = ptr_Test->Sampling.Vmin;
  ptr_Sampling->Vmax = ptr_Test->Sampling.Vmax;
  ptr_Sampling->Rise_Time = ptr_Test->Sampling.Rise_Time;
  ptr_Sampling->Fall_Time = ptr_Test->Sampling.Fall_Time;
  ptr_Sampling->Pulse_Time = ptr_Test->Sampling.Pulse_Time;
}

/*  Test_pulse(
      float **Pulse,                    Pulse array to test
      struct Pulse_param Pulse_Param)   Structure of parameters of the pulse tested

    @Return void
     This function is used to test a pulse array by displaying all its values to see if the pulse has been created correctly
*/
void Test_pulse(float **Pulse, struct Pulse_param Pulse_Param){
  int i = 0;
  int j = 0;

  Serial.println("Test de Get_pulse(), On doit voir un tableau avec les bonnes valeurs");
  for(j = 0; j<3; j++){
    if(j == 0){
      Serial.print("Voltage:");
      Serial.print("\t");
    }
    else if(j == 1){
      Serial.print("Time:");
      Serial.print("\t\t");
    }
    else if(j == 2){
      Serial.print("Measure:");
      Serial.print("\t");
    }
    
    for(i=0;i<Pulse_Param.Total_Time;i++){
      Serial.print(Pulse[j][i]);
      Serial.print('\t');
    }
    Serial.println();
  }
  Serial.println(); 
}

/*  Matrix_Scan(
      struct Full_param Test)

    @Return int   Return value if an error occured
     This function is used to scan a crossbar array by reading the value of every point
*/
/*
int Matrix_Scan(struct Full_param Test, struct Pulse_param *Read){
  
}
*/

//***********************************//