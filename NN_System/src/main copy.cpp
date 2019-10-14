
/*  Get_pulse(
     struct Pulse_param)      Structure with all needed parameters to generate a Pulse_Param

    @Return float Pulse[2][Total_Time]
     This function generate an array of all needed parameter to generate a Pulse Pulse_Param
*/
float** Get_pulse(struct Pulse_param Pulse_Param){
  int Total_Time = 0;
  
  // Counting the number of windows needed
  if(Pulse_Param.Pre_hold_Time != 0){
    Total_Time++;
  }
  if(Pulse_Param.Post_hold_Time != 0){
    Total_Time++;  
  }
  if(Pulse_Param.Meas_Wait != 0){
    Total_Time++;
  }
  if(Pulse_Param.Meas_Stop != 0){
    Total_Time++;  
  }
  if(Pulse_Param.Pulse_Time != 0){
    Total_Time++;
  }
  Total_Time += (Pulse_Param.Rise_Time + Pulse_Param.Fall_Time);
      
  float Rise_deltaV = 0; 
  float Fall_deltaV = 0; 
  
  Rise_deltaV = (Pulse_Param.Vmax - Pulse_Param.Vmin)/(Pulse_Param.Rise_Time+1);
  Fall_deltaV = (Pulse_Param.Vmax - Pulse_Param.Vmin)/(Pulse_Param.Fall_Time+1);
  
  float **Pulse;
  Pulse = (float**)malloc(sizeof(float*) * Total_Time);
  
  for(int i = 0; i < 3; i++) {
      Pulse[i] = (float*)malloc(sizeof(float*) * Total_Time);
  }
  
  //memset( Pulse, 0, 2*Total_Time*sizeof(float) );  // Initialise Pulse array with 0
  int off = 0;    //offset position
  int off_post = 0;  //offset reinitialiser
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
    //Pulse_Param Time: Waiting for measurements
    else if(((Pulse_Param.Meas_Wait != 0) && (i<(off + Pulse_Param.Rise_Time + 1))) && (off < 2)){
      Pulse[0][i] = Pulse_Param.Vmax;
      Pulse[1][i] = Pulse_Param.Meas_Wait; 
      Pulse[2][i] = 0;   // No measurements during wait
      off++;
    }
    //Pulse_Param Time: measurements
    else if(i<(off + Pulse_Param.Rise_Time + 1)){
      Pulse[0][i] = Pulse_Param.Vmax;
      Pulse[1][i] = Pulse_Param.Pulse_Time; 
      Pulse[2][i] = Pulse_Param.Meas_type;   // Measurement type choosen
    }
    //Pulse_Param Time: Stop measurements
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

  int i = 0;
  int j = 0;
  
  int windows = Pulse_Param.Rise_Time + Pulse_Param.Fall_Time + 5;
  
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
    
    for(i=0;i<(Pulse_Param.Rise_Time + Pulse_Param.Fall_Time + 5);i++){
      Serial.print(Pulse[j][i]);
      Serial.print('\t');
    }
    Serial.println();
  } 

  return Pulse;
  
}
//***********************************//