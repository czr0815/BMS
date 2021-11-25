/*! @file
    @ingroup LTC68041
    Library for LTC6804-1 Multicell Battery Monitor
*/

#include "LTC6804.h"
#include "stm32f4xx.h"
#include "spi_volacq.h"
#include "delay.h"

/*!
  LTC6804 conversion command variables.会在set_adc()配置
*/
uint8_t ADCV[2]; //!< Cell Voltage conversion command.
uint8_t ADAX[2]; //!< GPIO conversion command.

/*!
 \brief 改变相关引脚，打开或关闭与指定6804的通讯(片选)
 */
void set_cs(u8 b){	
	//片选函数
	switch(b) {
		case 0:  //选中0号芯片CS
			CHIP1_OPEN;
			break;
		case 1:  //选中1号芯片CS
			CHIP2_OPEN;
			break;
		case 2:  //选中2号芯片CS
			CHIP3_OPEN;
			break;
		case 3:  //选中3号芯片CS
			CHIP4_OPEN;
			break;
		case 4:  //选中4号芯片CS
			CHIP5_OPEN;
			break;
		case 5:  //选中5号芯片CS
			CHIP6_OPEN;
			break;
		case 6:  //选中6号芯片CS
			CHIP7_OPEN;
			break;
		case 7:  //选中7号芯片CS
			CHIP8_OPEN;
			break;
		case 8:  //选中8号芯片CS
			CHIP9_OPEN;
			break;
		case 9:  //选中9号芯片CS
			CHIP10_OPEN;
			break;		
		default:
			CHIP1_CLOSE;
		  CHIP2_CLOSE;
		  CHIP3_CLOSE;
			CHIP4_CLOSE;
			CHIP5_CLOSE;
			CHIP6_CLOSE;
			CHIP7_CLOSE;
			CHIP8_CLOSE;
			CHIP9_CLOSE;
			CHIP10_CLOSE;
	}
}

/*!
  \brief This function will initialize all 6804 variables and the SPI port.

  This function will initialize the Linduino to communicate with the LTC6804 with a 1MHz SPI clock.
  The Function also intializes the ADCV and ADAX commands to convert all cell and GPIO voltages in
  the Normal ADC mode.
*/
void LTC6804_initialize()
{
  SPI1_Init();
  set_adc(MD_FILTERED,DCP_DISABLED,CELL_CH_ALL,AUX_CH_ALL);
}

/*!*******************************************************************************************************************
 \brief Maps  global ADC control variables to the appropriate control bytes for each of the different ADC commands

@param[in] uint8_t MD The adc conversion mode
@param[in] uint8_t DCP Controls if Discharge is permitted during cell conversions
@param[in] uint8_t CH Determines which cells are measured during an ADC conversion command
@param[in] uint8_t CHG Determines which GPIO channels are measured during Auxiliary conversion command

Command Code:
-------------

|command	|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|ADCV:	    |   0   |   0   |   0   |   0   |   0   |   0   |   1   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CH[2] | CH[1] | CH[0] |
|ADAX:	    |   0   |   0   |   0   |   0   |   0   |   1   |   0   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CHG[2]| CHG[1]| CHG[0]|
 ******************************************************************************************************************/
void set_adc(uint8_t MD,//ADC Mode
			 uint8_t DCP, //Discharge Permit,放电允许
			 uint8_t CH, //Cell Channels to be measured
			 uint8_t CHG //GPIO Channels to be measured
			 )
{
  uint8_t md_bits;

  md_bits = (MD & 0x02) >> 1;
  ADCV[0] = md_bits + 0x02;
  md_bits = (MD & 0x01) << 7;
  ADCV[1] =  md_bits + 0x60 + (DCP<<4) + CH;

  md_bits = (MD & 0x02) >> 1;
  ADAX[0] = md_bits + 0x04;
  md_bits = (MD & 0x01) << 7;
  ADAX[1] = md_bits + 0x60 + CHG ;

}

/*!*********************************************************************************************
  \brief Starts cell voltage conversion

  Starts ADC conversions of the LTC6804 Cpin inputs.
  The type of ADC conversion executed can be changed by setting the associated global variables:
 |Variable|Function                                      |
 |--------|----------------------------------------------|
 | MD     | Determines the filter corner of the ADC      |
 | CH     | Determines which cell channels are converted |
 | DCP    | Determines if Discharge is Permitted	     |

Command Code:
-------------

|CMD[0:1]	|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|ADCV:	    |   0   |   0   |   0   |   0   |   0   |   0   |   1   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CH[2] | CH[1] | CH[0] |
***********************************************************************************************/
void LTC6804_adcv(uint8_t chip)
{
  uint8_t cmd[4];
  uint16_t cmd_pec;

  //1
  cmd[0] = ADCV[0];
  cmd[1] = ADCV[1];

  //2
  cmd_pec = pec15_calc(2, ADCV);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);

  //3
  wakeup_idle (chip); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.
  //wakeup_idle (chip);

  //4
  //LTC6804_CS_low;
  set_cs(chip);
  spi_write_array(4,cmd);
  set_cs(0xff);
  //LTC6804_CS_high;
}
/*
  LTC6804_adcv Function sequence:

  1. Load adcv command into cmd array
  2. Calculate adcv cmd PEC and load pec into cmd array
  3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
  4. send broadcast adcv command to LTC6804 daisy chain
*/

/*!******************************************************************************************************
 \brief Start an GPIO Conversion

  Starts an ADC conversions of the LTC6804 GPIO inputs.
  The type of ADC conversion executed can be changed by setting the associated global variables:
 |Variable|Function                                      |
 |--------|----------------------------------------------|
 | MD     | Determines the filter corner of the ADC      |
 | CHG    | Determines which GPIO channels are converted |


Command Code:
-------------

|CMD[0:1]	|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|ADAX:	    |   0   |   0   |   0   |   0   |   0   |   1   |   0   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CHG[2]| CHG[1]| CHG[0]|
*********************************************************************************************************/
void LTC6804_adax(uint8_t chip)
{
  uint8_t cmd[4];
  uint16_t cmd_pec;

  cmd[0] = ADAX[0];
  cmd[1] = ADAX[1];
  cmd_pec = pec15_calc(2, ADAX);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
  wakeup_idle (chip);
  //wakeup_idle (chip); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.

//   LTC6804_CS_low;
  set_cs(chip);
  spi_write_array(4,cmd);
  set_cs(0xff);
//   LTC6804_CS_high;
}
/*
  LTC6804_adax Function sequence:

  1. Load adax command into cmd array
  2. Calculate adax cmd PEC and load pec into cmd array
  3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
  4. send broadcast adax command to LTC6804 daisy chain
*/



/***********************************************//**
 \brief Reads and parses the LTC6804 cell voltage registers.

 The function is used to read the cell codes of the LTC6804.
 This function will send the requested read commands parse the data
 and store the cell voltages in cell_codes variable.

 @param[in] uint8_t reg; This controls which cell voltage register is read back.

          0: Read back all Cell registers

          1: Read back cell group A

          2: Read back cell group B

          3: Read back cell group C

          4: Read back cell group D

 @param[in] uint8_t total_ic; This is the number of ICs in the daisy chain(-1 only)

 @param[out] uint16_t cell_codes[]; An array of the parsed cell codes from lowest to highest. The cell codes will
  be stored in the cell_codes[] array in the following format:
  |  cell_codes[0][0]| cell_codes[0][1] |  cell_codes[0][2]|    .....     |  cell_codes[0][11]|  cell_codes[1][0] | cell_codes[1][1]|  .....   |
  |------------------|------------------|------------------|--------------|-------------------|-------------------|-----------------|----------|
  |IC1 Cell 1        |IC1 Cell 2        |IC1 Cell 3        |    .....     |  IC1 Cell 12      |IC2 Cell 1         |IC2 Cell 2       | .....    |

  @return int8_t, PEC Status.

		0: No PEC error detected

		-1: PEC error detected, retry read


 *************************************************/
uint8_t LTC6804_rdcv(uint8_t chip,
                     uint8_t reg, // Controls which cell voltage register is read back.//选择读取的寄存器
					 uint8_t total_ic, // the number of ICs in the system
					 uint16_t cell_codes[][12] // Array of the parsed cell codes读取电压存储的地方
					 )
{

  const uint8_t NUM_RX_BYT = 8;
  const uint8_t BYT_IN_REG = 6;
  const uint8_t CELL_IN_REG = 3;
	
	uint8_t cell_reg;
	uint8_t current_ic;
	uint8_t current_cell;
	

  uint8_t cell_data[120];
  uint8_t pec_error = 0;
  uint16_t parsed_cell;
  uint16_t received_pec;
  uint16_t data_pec;
  uint8_t data_counter=0; //data counter
  //cell_data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));8*15
  //1.a
  if (reg == 0)
  {
    //a.i
    for( cell_reg = 1; cell_reg<5; cell_reg++)         			 			//executes once for each of the LTC6804 cell voltage registers
    {
      data_counter = 0;
      LTC6804_rdcv_reg(chip, cell_reg, total_ic,cell_data);								//Reads a single Cell voltage register

      for ( current_ic = 0 ; current_ic < total_ic; current_ic++) 			// executes for every LTC6804 in the daisy chain
      {																 	  			// current_ic is used as the IC counter

        //a.ii
		for( current_cell = 0; current_cell<CELL_IN_REG; current_cell++)	 	// This loop parses the read back data into cell voltages, it
                 {														   		  			// loops once for each of the 3 cell voltage codes in the register

                     parsed_cell = cell_data[data_counter] + (cell_data[data_counter + 1] << 8);//Each cell code is received as two bytes and is combined to
																					 // create the parsed cell voltage code

                      cell_codes[current_ic][current_cell  + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;
                      data_counter = data_counter + 2;											 //Because cell voltage codes are two bytes the data counter
																					//must increment by two for each parsed cell code
                   }
		//a.iii
                received_pec = (cell_data[data_counter] << 8) + cell_data[data_counter+1]; //The received PEC for the current_ic is transmitted as the 7th and 8th
                                                                                                                                                                           //after the 6 cell voltage data bytes
                data_pec = pec15_calc(BYT_IN_REG, &cell_data[current_ic * NUM_RX_BYT]);
                if(received_pec != data_pec)
                {
                  pec_error = 1;															//The pec_error variable is simply set negative if any PEC errors
                                                                                                                                                                                //are detected in the serial data
                }
                data_counter=data_counter+2;												//Because the transmitted PEC code is 2 bytes long the data_counter
																					//must be incremented by 2 bytes to point to the next ICs cell voltage data
      }
    }
  }
 //1.b
  else
  {
	//b.i
    LTC6804_rdcv_reg(chip, reg, total_ic,cell_data);
    for ( current_ic = 0 ; current_ic < total_ic; current_ic++) 				// executes for every LTC6804 in the daisy chain
    {																 	  			// current_ic is used as the IC counter
		//b.ii
		for( current_cell = 0; current_cell < CELL_IN_REG; current_cell++)   // This loop parses the read back data into cell voltages, it
              {														   		  			// loops once for each of the 3 cell voltage codes in the register

			parsed_cell = cell_data[data_counter] + (cell_data[data_counter+1]<<8); //Each cell code is received as two bytes and is combined to
																					// create the parsed cell voltage code

			cell_codes[current_ic][current_cell + ((reg - 1) * CELL_IN_REG)] = 0x0000FFFF & parsed_cell;
			data_counter= data_counter + 2;     									//Because cell voltage codes are two bytes the data counter
																					//must increment by two for each parsed cell code
		}
		//b.iii
	    received_pec = (cell_data[data_counter] << 8 )+ cell_data[data_counter + 1]; //The received PEC for the current_ic is transmitted as the 7th and 8th
																				     //after the 6 cell voltage data bytes
            data_pec = pec15_calc(BYT_IN_REG, &cell_data[current_ic * NUM_RX_BYT]);
		if(received_pec != data_pec)
		{
			pec_error = 1;															//The pec_error variable is simply set negative if any PEC errors
																					//are detected in the serial data
		}
		data_counter= data_counter + 2; 											//Because the transmitted PEC code is 2 bytes long the data_counter
																					//must be incremented by 2 bytes to point to the next ICs cell voltage data
	}
  }

 //2
 //free(cell_data);
return(pec_error);
}
/*
	LTC6804_rdcv Sequence

	1. Switch Statement:
		a. Reg = 0
			i. Read cell voltage registers A-D for every IC in the daisy chain
			ii. Parse raw cell voltage data in cell_codes array
			iii. Check the PEC of the data read back vs the calculated PEC for each read register command
		b. Reg != 0
			i.Read single cell voltage register for all ICs in daisy chain
			ii. Parse raw cell voltage data in cell_codes array
			iii. Check the PEC of the data read back vs the calculated PEC for each read register command
	2. Return pec_error flag
*/



/***********************************************//**
 \brief Read the raw data from the LTC6804 cell voltage register

 The function reads a single cell voltage register and stores the read data
 in the *data point as a byte array. This function is rarely used outside of
 the LTC6804_rdcv() command.

 @param[in] uint8_t reg; This controls which cell voltage register is read back.

          1: Read back cell group A

          2: Read back cell group B

          3: Read back cell group C

          4: Read back cell group D

 @param[in] uint8_t total_ic; This is the number of ICs in the daisy chain(-1 only)

 @param[out] uint8_t *data; An array of the unparsed cell codes

Command Code:
-------------

|CMD[0:1]	|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|RDCVA:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   0   |
|RDCVB:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   0   |
|RDCVC:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   0   |   0   |
|RDCVD:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   1   |   0   |

 *************************************************/
void LTC6804_rdcv_reg(uint8_t chip,
                      uint8_t reg, //Determines which cell voltage register is read back
					  uint8_t total_ic, //the number of ICs in the
					  uint8_t *data //An array of the unparsed cell codes
					  )
{
  const uint8_t REG_LEN = 8; //number of bytes in each ICs register + 2 bytes for the PEC
  uint8_t cmd[4];
  uint16_t cmd_pec;

  //1
  if (reg == 1)     //1: RDCVA
  {
    cmd[1] = 0x04;
    cmd[0] = 0x00;
  }
  else if(reg == 2) //2: RDCVB
  {
    cmd[1] = 0x06;
    cmd[0] = 0x00;
  }
  else if(reg == 3) //3: RDCVC
  {
    cmd[1] = 0x08;
    cmd[0] = 0x00;
  }
  else if(reg == 4) //4: RDCVD
  {
    cmd[1] = 0x0A;
    cmd[0] = 0x00;
  }

  //2
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);

  //3
  wakeup_idle (chip); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.
  //wakeup_idle (chip);
  //wakeup_idle (chip);
  //wakeup_idle (chip);
  //wakeup_idle (chip);


  //4
  //LTC6804_CS_low;
  set_cs(chip);
  spi_write_read(cmd,4,data,(REG_LEN*total_ic));
  set_cs(0xff);
  //LTC6804_CS_high;

}
/*
  LTC6804_rdcv_reg Function Process:
  1. Determine Command and initialize command array
  2. Calculate Command PEC
  3. Wake up isoSPI, this step is optional
  4. Send Global Command to LTC6804 daisy chain
*/



/***********************************************************************************//**
 \brief Reads and parses the LTC6804 auxiliary registers.

 The function is used
 to read the  parsed GPIO codes of the LTC6804. This function will send the requested
 read commands parse the data and store the gpio voltages in aux_codes variable

@param[in] uint8_t reg; This controls which GPIO voltage register is read back.

          0: Read back all auxiliary registers

          1: Read back auxiliary group A

          2: Read back auxiliary group B


@param[in] uint8_t total_ic; This is the number of ICs in the daisy chain(-1 only)


 @param[out] uint16_t aux_codes[][6]; A two dimensional array of the gpio voltage codes. The GPIO codes will
 be stored in the aux_codes[][6] array in the following format:
 |  aux_codes[0][0]| aux_codes[0][1] |  aux_codes[0][2]|  aux_codes[0][3]|  aux_codes[0][4]|  aux_codes[0][5]| aux_codes[1][0] |aux_codes[1][1]|  .....    |
 |-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|---------------|-----------|
 |IC1 GPIO1        |IC1 GPIO2        |IC1 GPIO3        |IC1 GPIO4        |IC1 GPIO5        |IC1 Vref2        |IC2 GPIO1        |IC2 GPIO2      |  .....    |

@return  int8_t, PEC Status

  0: No PEC error detected

 -1: PEC error detected, retry read
 *************************************************/
int8_t LTC6804_rdaux(uint8_t chip,
                     uint8_t reg, //Determines which GPIO voltage register is read back.
					 uint8_t total_ic,//the number of ICs in the system
					 uint16_t aux_codes[][6]//A two dimensional array of the gpio voltage codes.
					 )
{


  const uint8_t NUM_RX_BYT = 8;
  const uint8_t BYT_IN_REG = 6;
  const uint8_t GPIO_IN_REG = 3;
   
	uint8_t gpio_reg;
	uint8_t current_ic;
	uint8_t current_gpio;
	
	
	
  uint8_t data[120];
  uint8_t data_counter = 0;
  int8_t pec_error = 0;
  uint16_t parsed_aux;
  uint16_t received_pec;
  uint16_t data_pec;
 // data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));
  //1.a
  if (reg == 0)
  {
	//a.i
    for( gpio_reg = 1; gpio_reg<3; gpio_reg++)		 	   		 			//executes once for each of the LTC6804 aux voltage registers
    {
      data_counter = 0;
      LTC6804_rdaux_reg(chip, gpio_reg, total_ic,data);									//Reads the raw auxiliary register data into the data[] array

      for ( current_ic = 0 ; current_ic < total_ic; current_ic++) 			// executes for every LTC6804 in the daisy chain
      {																 	  			// current_ic is used as the IC counter

        //a.ii
		for( current_gpio = 0; current_gpio< GPIO_IN_REG; current_gpio++)	// This loop parses the read back data into GPIO voltages, it
        {														   		  			// loops once for each of the 3 gpio voltage codes in the register

		  parsed_aux = data[data_counter] + (data[data_counter+1]<<8);              //Each gpio codes is received as two bytes and is combined to
																				    // create the parsed gpio voltage code

          aux_codes[current_ic][current_gpio +((gpio_reg-1)*GPIO_IN_REG)] = parsed_aux;
          data_counter=data_counter+2;												//Because gpio voltage codes are two bytes the data counter
																					//must increment by two for each parsed gpio voltage code

        }
		//a.iii
        received_pec = (data[data_counter]<<8)+ data[data_counter+1]; 				 //The received PEC for the current_ic is transmitted as the 7th and 8th
																				     //after the 6 gpio voltage data bytes
        data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);
        if(received_pec != data_pec)
        {
          pec_error = -1;															//The pec_error variable is simply set negative if any PEC errors
																					//are detected in the received serial data
        }

        data_counter=data_counter+2;												//Because the transmitted PEC code is 2 bytes long the data_counter
																					//must be incremented by 2 bytes to point to the next ICs gpio voltage data
      }


    }

  }
  else
  {
		
	//b.i
    LTC6804_rdaux_reg(chip, reg, total_ic, data);
    for ( current_ic = 0 ; current_ic < total_ic; current_ic++) 			  		// executes for every LTC6804 in the daisy chain
    {							   									          		// current_ic is used as an IC counter

		//b.ii
		for( current_gpio = 0; current_gpio<GPIO_IN_REG; current_gpio++)  	 	// This loop parses the read back data. Loops
		{						 											  		// once for each aux voltage in the register

			parsed_aux = (data[data_counter] + (data[data_counter+1]<<8));    		//Each gpio codes is received as two bytes and is combined to
																					// create the parsed gpio voltage code
			aux_codes[current_ic][current_gpio +((reg-1)*GPIO_IN_REG)] = parsed_aux;
			data_counter=data_counter+2;									 		//Because gpio voltage codes are two bytes the data counter
																					//must increment by two for each parsed gpio voltage code
		}
		//b.iii
		received_pec = (data[data_counter]<<8) + data[data_counter+1]; 				 //The received PEC for the current_ic is transmitted as the 7th and 8th
																				     //after the 6 gpio voltage data bytes
        data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);
        if(received_pec != data_pec)
        {
          pec_error = -1;													   		//The pec_error variable is simply set negative if any PEC errors
																					//are detected in the received serial data
        }

		data_counter=data_counter+2;												//Because the transmitted PEC code is 2 bytes long the data_counter
																					//must be incremented by 2 bytes to point to the next ICs gpio voltage data
	}
  }
 // free(data);
  return (pec_error);
}
/*
	LTC6804_rdaux Sequence

	1. Switch Statement:
		a. Reg = 0
			i. Read GPIO voltage registers A-D for every IC in the daisy chain
			ii. Parse raw GPIO voltage data in cell_codes array
			iii. Check the PEC of the data read back vs the calculated PEC for each read register command
		b. Reg != 0
			i.Read single GPIO voltage register for all ICs in daisy chain
			ii. Parse raw GPIO voltage data in cell_codes array
			iii. Check the PEC of the data read back vs the calculated PEC for each read register command
	2. Return pec_error flag
*/


/***********************************************//**
 \brief Read the raw data from the LTC6804 auxiliary register

 The function reads a single GPIO voltage register and stores thre read data
 in the *data point as a byte array. This function is rarely used outside of
 the LTC6804_rdaux() command.

 @param[in] uint8_t reg; This controls which GPIO voltage register is read back.

          1: Read back auxiliary group A

          2: Read back auxiliary group B


@param[in] uint8_t total_ic; This is the number of ICs in the daisy chain

@param[out] uint8_t *data; An array of the unparsed aux codes



Command Code:
-------------

|CMD[0:1]	    |  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
|---------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|RDAUXA:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   0   |   0   |
|RDAUXB:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   1   |   0   |

 *************************************************/
void LTC6804_rdaux_reg(uint8_t chip,
                       uint8_t reg, //Determines which GPIO voltage register is read back
					   uint8_t total_ic, //The number of ICs in the system
					   uint8_t *data //Array of the unparsed auxiliary codes
					   )
{
  const uint8_t REG_LEN = 8; // number of bytes in the register + 2 bytes for the PEC
  uint8_t cmd[4];
  uint16_t cmd_pec;

  //1
  if (reg == 1)			//Read back auxiliary group A
  {
    cmd[1] = 0x0C;
    cmd[0] = 0x00;
  }
  else if(reg == 2)		//Read back auxiliary group B
  {
    cmd[1] = 0x0e;
    cmd[0] = 0x00;
  }
  else					//Read back auxiliary group A
  {
     cmd[1] = 0x0C;
     cmd[0] = 0x00;
  }
  //2
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);

  //3
  wakeup_idle (chip);
  //wakeup_idle (chip);
  //wakeup_idle (chip);
  //wakeup_idle (chip); //This will guarantee that the LTC6804 isoSPI port is awake, this command can be removed.


  //4
  //LTC6804_CS_low;
  set_cs(chip);
  spi_write_read(cmd,4,data,(REG_LEN*total_ic));
  set_cs(0xff);
  //LTC6804_CS_high;

}
/*
  LTC6804_rdaux_reg Function Process:
  1. Determine Command and initialize command array
  2. Calculate Command PEC
  3. Wake up isoSPI, this step is optional
  4. Send Global Command to LTC6804 daisy chain
*/

/********************************************************//**
 \brief Clears the LTC6804 cell voltage registers

 The command clears the cell voltage registers and intiallizes
 all values to 1. The register will read back hexadecimal 0xFF
 after the command is sent.


Command Code:
-------------

|CMD[0:1]	    |  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
|---------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|CLRCELL:	    |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   1   |   0   |   0   |   0   |   1   |   0   |   0   |   0   |   1   |
************************************************************/
void LTC6804_clrcell(uint8_t chip)
{
  uint8_t cmd[4];
  uint16_t cmd_pec;

  //1
  cmd[0] = 0x07;
  cmd[1] = 0x11;

  //2
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec );

  //3
  wakeup_idle (chip); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.
  wakeup_idle (chip);


  //4
  //LTC6804_CS_low;
  set_cs(chip);
  spi_write_read(cmd,4,0,0);
  set_cs(0xff);
  //LTC6804_CS_high;
}
/*
  LTC6804_clrcell Function sequence:

  1. Load clrcell command into cmd array
  2. Calculate clrcell cmd PEC and load pec into cmd array
  3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
  4. send broadcast clrcell command to LTC6804 daisy chain
*/


/***********************************************************//**
 \brief Clears the LTC6804 Auxiliary registers

 The command clears the Auxiliary registers and intiallizes
 all values to 1. The register will read back hexadecimal 0xFF
 after the command is sent.


Command Code:
-------------

|CMD[0:1]	    |  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
|---------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|CLRAUX:	    |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   1   |   0   |   0   |   0   |   1   |   0   |   0   |   2   |   0   |
***************************************************************/
void LTC6804_clraux(uint8_t chip)
{
  uint8_t cmd[4];
  uint16_t cmd_pec;

  //1
  cmd[0] = 0x07;
  cmd[1] = 0x12;

  //2
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);

  //3
  wakeup_idle (chip);
  wakeup_idle (chip); //This will guarantee that the LTC6804 isoSPI port is awake.This command can be removed.

  //4
  //LTC6804_CS_low;
  set_cs(chip);
  spi_write_read(cmd,4,0,0);
  set_cs(0xff);
  //LTC6804_CS_high;
}
/*
  LTC6804_clraux Function sequence:

  1. Load clraux command into cmd array
  2. Calculate clraux cmd PEC and load pec into cmd array
  3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
  4. send broadcast clraux command to LTC6804 daisy chain
*/


/*****************************************************//**
 \brief Write the LTC6804 configuration register

 This command will write the configuration registers of the LTC6804-1s
 connected in a daisy chain stack. The configuration is written in descending
 order so the last device's configuration is written first.

 @param[in] uint8_t total_ic; The number of ICs being written to.

 @param[in] uint8_t config[][6] is a two dimensional array of the configuration data that will be written, the array should contain the 6 bytes for each
 IC in the daisy chain. The lowest IC in the daisy chain should be the first 6 byte block in the array. The array should
 have the following format:
 |  config[0][0]| config[0][1] |  config[0][2]|  config[0][3]|  config[0][4]|  config[0][5]| config[1][0] |  config[1][1]|  config[1][2]|  .....    |
 |--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|-----------|
 |IC1 CFGR0     |IC1 CFGR1     |IC1 CFGR2     |IC1 CFGR3     |IC1 CFGR4     |IC1 CFGR5     |IC2 CFGR0     |IC2 CFGR1     | IC2 CFGR2    |  .....    |

 The function will calculate the needed PEC codes for the write data
 and then transmit data to the ICs on a daisy chain.


Command Code:
-------------
|               |							CMD[0]                              |                            CMD[1]                             |
|---------------|---------------------------------------------------------------|---------------------------------------------------------------|
|CMD[0:1]	    |  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
|---------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|WRCFG:	        |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |
********************************************************/
void LTC6804_wrcfg(uint8_t chip,
                   uint8_t total_ic, //The number of ICs being written to
				   uint8_t config[][6] //A two dimensional array of the configuration data that will be written
				   )
{
  const uint8_t BYTES_IN_REG = 6;
  const uint8_t CMD_LEN = 4+(8*total_ic);
  uint8_t cmd[124];
  uint16_t cmd_pec, cfg_pec;
  uint8_t cmd_index; //command counter
	
	uint8_t current_ic;
  uint8_t current_byte;
  //cmd = (uint8_t *)malloc(CMD_LEN*sizeof(uint8_t));

  //1
  cmd[0] = 0x00;
  cmd[1] = 0x01;
	cmd_pec = (uint16_t)pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
  //cmd[2] = 0x3d;
  //cmd[3] = 0x6e;

  //2
  cmd_index = 4;
  for ( current_ic = total_ic; current_ic > 0; current_ic--) 			// executes for each LTC6804 in daisy chain, this loops starts with
  {																				// the last IC on the stack. The first configuration written is
																				// received by the last IC in the daisy chain
    for ( current_byte = 0; current_byte < BYTES_IN_REG; current_byte++) // executes for each of the 6 bytes in the CFGR register
    {																			// current_byte is the byte counter

      cmd[cmd_index] = config[current_ic-1][current_byte]; 						//adding the config data to the array to be sent
      cmd_index = cmd_index + 1;
    }
	//3
    cfg_pec = (uint16_t)pec15_calc(BYTES_IN_REG, &config[current_ic-1][0]);		// calculating the PEC for each ICs configuration register data
    cmd[cmd_index] = (uint8_t)(cfg_pec >> 8);
    cmd[cmd_index + 1] = (uint8_t)cfg_pec;
    cmd_index = cmd_index + 2;
  }

  //4
  wakeup_idle (chip);
  wakeup_idle (chip); //This will guarantee that the LTC6804 isoSPI port is awake.This command can be removed.

  //5
  //LTC6804_CS_low;
  //delayMicroseconds(3);
  set_cs(chip);
  spi_write_array(CMD_LEN, cmd);
  set_cs(0xff);
  //LTC6804_CS_high;
  //free(cmd);
}
/*
	WRCFG Sequence:

	1. Load cmd array with the write configuration command and PEC
	2. Load the cmd with LTC6804 configuration data
	3. Calculate the pec for the LTC6804 configuration data being transmitted
	4. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
	5. Write configuration data to the LTC6804 daisy chain

*/

/*!******************************************************
 \brief Reads configuration registers of a LTC6804 daisy chain

@param[in] uint8_t total_ic: number of ICs in the daisy chain

@param[out] uint8_t r_config[][8] is a two dimensional array that the function stores the read configuration data. The configuration data for each IC
is stored in blocks of 8 bytes with the configuration data of the lowest IC on the stack in the first 8 bytes
block of the array, the second IC in the second 8 byte etc. Below is an table illustrating the array organization:

|r_config[0][0]|r_config[0][1]|r_config[0][2]|r_config[0][3]|r_config[0][4]|r_config[0][5]|r_config[0][6]  |r_config[0][7] |r_config[1][0]|r_config[1][1]|  .....    |
|--------------|--------------|--------------|--------------|--------------|--------------|----------------|---------------|--------------|--------------|-----------|
|IC1 CFGR0     |IC1 CFGR1     |IC1 CFGR2     |IC1 CFGR3     |IC1 CFGR4     |IC1 CFGR5     |IC1 PEC High    |IC1 PEC Low    |IC2 CFGR0     |IC2 CFGR1     |  .....    |


@return int8_t, PEC Status.

	0: Data read back has matching PEC

	-1: Data read back has incorrect PEC


Command Code:
-------------

|CMD[0:1]		|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
|---------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|RDCFG:	        |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   0   |   1   |   0   |
********************************************************/
int8_t LTC6804_rdcfg(uint8_t chip,
                     uint8_t total_ic, //Number of ICs in the system
				     uint8_t r_config[][8] //A two dimensional array that the function stores the read configuration data.
					 )
{
  const uint8_t BYTES_IN_REG = 8;

  uint8_t cmd[4];
  uint8_t rx_data[20];
  int8_t pec_error = 0;
  uint16_t data_pec;
  uint16_t received_pec;
	
	uint8_t current_ic;
  uint8_t current_byte;
  //rx_data = (uint8_t *) malloc((8*total_ic)*sizeof(uint8_t));

  //1
  cmd[0] = 0x00;
  cmd[1] = 0x02;
  cmd[2] = 0x2b;
  cmd[3] = 0x0A;

  //2
  wakeup_idle (chip);
  wakeup_idle (chip); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.

  //3
  //LTC6804_CS_low;
  set_cs(chip);
  spi_write_read(cmd, 4, rx_data, (BYTES_IN_REG*total_ic));         //Read the configuration data of all ICs on the daisy chain into
  set_cs(0xff);
  //LTC6804_CS_high;													//rx_data[] array

  for ( current_ic = 0; current_ic < total_ic; current_ic++) 			//executes for each LTC6804 in the daisy chain and packs the data
  { 																			//into the r_config array as well as check the received Config data
																				//for any bit errors
	//4.a
    for ( current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
    {
      r_config[current_ic][current_byte] = rx_data[current_byte+ (current_ic*BYTES_IN_REG)];// 
    }
    //4.b
    received_pec = (r_config[current_ic][6]<<8) + r_config[current_ic][7];
    data_pec = pec15_calc(6, &r_config[current_ic][0]);
    if(received_pec != data_pec)
    {
      pec_error = -1;
    }
  }

  //free(rx_data);
  //5
  return(pec_error);
}
/*
	RDCFG Sequence:

	1. Load cmd array with the write configuration command and PEC
	2. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
	3. Send command and read back configuration data
	4. For each LTC6804 in the daisy chain
	  a. load configuration data into r_config array
	  b. calculate PEC of received data and compare against calculated PEC
	5. Return PEC Error

*/
void delayMicroseconds(unsigned int x)
{
	
	unsigned int i,j;
#if IS_USE_RTOS
	vTaskDelay(x);
#endif
#if !IS_USE_RTOS
	for(j=0;j<500;j++) ;
#endif
	
}
/*!****************************************************
  \brief Wake isoSPI up from idle state
 Generic wakeup commannd to wake isoSPI up out of idle
 *****************************************************/
void wakeup_idle(uint8_t chip)
{
  //LTC6804_CS_low;
  set_cs(chip);
  delayMicroseconds(1); //Guarantees the isoSPI will be in ready mode
  set_cs(0xff);
  //LTC6804_CS_high;
}

/*!****************************************************
  \brief Wake the LTC6804 from the sleep state

 Generic wakeup commannd to wake the LTC6804 from sleep
 *****************************************************/
void wakeup_sleep(uint8_t chip)
{
  //LTC6804_CS_low;
  set_cs(chip);
  delayMicroseconds(5); // Guarantees the LTC6804 will be in standby
  set_cs(0xff);
  //LTC6804_CS_high;
}
/*!**********************************************************
 \brief calaculates  and returns the CRC15

  @param[in] uint8_t len: the length of the data array being passed to the function

  @param[in] uint8_t data[] : the array of data that the PEC will be generated from


  @returns The calculated pec15 as an unsigned int
***********************************************************/
uint16_t pec15_calc(uint8_t len, //Number of bytes that will be used to calculate a PEC
					uint8_t *data //Array of data that will be used to calculate  a PEC
					)
{
	uint16_t remainder,addr;
	uint8_t i;

	remainder = 16;//initialize the PEC
	for( i = 0; i<len;i++) // loops for each byte in data array
	{
		addr = ((remainder>>7)^data[i])&0xff;//calculate PEC table address
		remainder = (remainder<<8)^crc15Table[addr];
	}
	return(remainder*2);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}


/*!
 \brief Writes an array of bytes out of the SPI port

 @param[in] uint8_t len length of the data array being written on the SPI port
 @param[in] uint8_t data[] the data array to be written on the SPI port

*/
void spi_write_array(uint8_t len, // Option: Number of bytes to be written on the SPI port
					 uint8_t data[] //Array of bytes to be written on the SPI port
					 )
{
	uint8_t i;
  for( i = 0; i < len; i++)
  {
     SPI1_ReadWriteByte((int8_t)data[i]);
    // spi2_read((int8_t)data[i]);
  }
}

/*!
 \brief Writes and read a set number of bytes using the SPI port.

@param[in] uint8_t tx_data[] array of data to be written on the SPI port
@param[in] uint8_t tx_len length of the tx_data array
@param[out] uint8_t rx_data array that read data will be written too.
@param[in] uint8_t rx_len number of bytes to be read from the SPI port.

*/

void spi_write_read(uint8_t tx_Data[],//array of data to be written on SPI port
					uint8_t tx_len, //length of the tx data arry
					uint8_t *rx_data,//Input: array that will store the data read by the SPI port
					uint8_t rx_len //Option: number of bytes to be read from the SPI port
					)
{
	uint8_t i;
  for( i = 0; i < tx_len; i++)
  {
  SPI1_ReadWriteByte(tx_Data[i]);
//spi2_read(tx_Data[i]);
  }

  for( i = 0; i < rx_len; i++)
  {
    rx_data[i] = (uint8_t)SPI1_ReadWriteByte(0x00);
  }

}

/*自测试模式 */
void LTC6804_cvst(uint8_t chip)
{

  uint8_t cmd[4];
  uint16_t cmd_pec;

  //1
  cmd[0] = 0x02;
  cmd[1] = 0xa7;

  //2
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);

  //3
  wakeup_idle (chip); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.
   wakeup_idle (chip);

  //4
  //LTC6804_CS_low;
  set_cs(chip);
  spi_write_array(4,cmd);
  set_cs(0xff);
  //LTC6804_CS_high;

}

void LTC6804_ADSTAT(uint8_t chip)//启动状态组ADC转换，滤波模式
{

  uint8_t cmd[4];
  uint16_t cmd_pec;

  //1
  cmd[0] = 0x05;
  cmd[1] = 0xE8;

  //2
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);

  //3
  wakeup_idle (chip); //This will guarantee that the LTC6804 isoSPI port is awake. This command can be removed.
  wakeup_idle (chip);

  //4
  //LTC6804_CS_low;
  set_cs(chip);
  spi_write_array(4,cmd);
  set_cs(0xff);
  //LTC6804_CS_high;

}

/*读状态寄存器*/
int8_t LTC6804_rdstat(uint8_t chip,
                     uint8_t reg, //Determines which GPIO voltage register is read back.
					 uint8_t total_ic,//the number of ICs in the system
					 uint16_t stat_codes[][6]//A two dimensional array of the gpio voltage codes.
					 )
{


  const uint8_t NUM_RX_BYT = 8;
  const uint8_t BYT_IN_REG = 6;
  const uint8_t GPIO_IN_REG = 3;

	uint8_t gpio_reg;
	uint8_t current_ic;
	uint8_t current_gpio;
	
  uint8_t data[30];
  uint8_t data_counter = 0;
  int8_t pec_error = 0;
  uint16_t parsed_aux;
  uint16_t received_pec;
  uint16_t data_pec;
 // data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));
  //1.a
  if (reg == 0)
  {
	//a.i
    for( gpio_reg = 1; gpio_reg<3; gpio_reg++)		 	   		 			//executes once for each of the LTC6804 aux voltage registers
    {
      data_counter = 0;
      LTC6804_rdstat_reg(chip, gpio_reg, total_ic,data);									//Reads the raw auxiliary register data into the data[] array

      for ( current_ic = 0 ; current_ic < total_ic; current_ic++) 			// executes for every LTC6804 in the daisy chain
      {																 	  			// current_ic is used as the IC counter

        //a.ii
		for( current_gpio = 0; current_gpio< GPIO_IN_REG; current_gpio++)	// This loop parses the read back data into GPIO voltages, it
        {														   		  			// loops once for each of the 3 gpio voltage codes in the register

		  parsed_aux = data[data_counter] + (data[data_counter+1]<<8);              //Each gpio codes is received as two bytes and is combined to
																				    // create the parsed gpio voltage code

          stat_codes[current_ic][current_gpio +((gpio_reg-1)*GPIO_IN_REG)] = parsed_aux;
          data_counter=data_counter+2;												//Because gpio voltage codes are two bytes the data counter
																					//must increment by two for each parsed gpio voltage code

        }
		//a.iii
        received_pec = (data[data_counter]<<8)+ data[data_counter+1]; 				 //The received PEC for the current_ic is transmitted as the 7th and 8th
																				     //after the 6 gpio voltage data bytes
        data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);
        if(received_pec != data_pec)
        {
          pec_error = -1;															//The pec_error variable is simply set negative if any PEC errors
																					//are detected in the received serial data
        }

        data_counter=data_counter+2;												//Because the transmitted PEC code is 2 bytes long the data_counter
																					//must be incremented by 2 bytes to point to the next ICs gpio voltage data
      }


    }

  }
  else
  {
	//b.i
    LTC6804_rdstat_reg(chip, reg, total_ic, data);
    for ( current_ic = 0 ; current_ic < total_ic; current_ic++) 			  		// executes for every LTC6804 in the daisy chain
    {							   									          		// current_ic is used as an IC counter

		//b.ii
		for( current_gpio = 0; current_gpio<GPIO_IN_REG; current_gpio++)  	 	// This loop parses the read back data. Loops
		{						 											  		// once for each aux voltage in the register

			parsed_aux = (data[data_counter] + (data[data_counter+1]<<8));    		//Each gpio codes is received as two bytes and is combined to
																					// create the parsed gpio voltage code
			stat_codes[current_ic][current_gpio +((reg-1)*GPIO_IN_REG)] = parsed_aux;
			data_counter=data_counter+2;									 		//Because gpio voltage codes are two bytes the data counter
																					//must increment by two for each parsed gpio voltage code
		}
		//b.iii
		received_pec = (data[data_counter]<<8) + data[data_counter+1]; 				 //The received PEC for the current_ic is transmitted as the 7th and 8th
																				     //after the 6 gpio voltage data bytes
        data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);
        if(received_pec != data_pec)
        {
          pec_error = -1;													   		//The pec_error variable is simply set negative if any PEC errors
																					//are detected in the received serial data
        }

		data_counter=data_counter+2;												//Because the transmitted PEC code is 2 bytes long the data_counter
																					//must be incremented by 2 bytes to point to the next ICs gpio voltage data
	}
  }
 // free(data);
  return (pec_error);
}


void LTC6804_rdstat_reg(uint8_t chip,
                       uint8_t reg, //Determines which GPIO voltage register is read back
					   uint8_t total_ic, //The number of ICs in the system
					   uint8_t *data //Array of the unparsed auxiliary codes
					   )
{
  const uint8_t REG_LEN = 8; // number of bytes in the register + 2 bytes for the PEC
  uint8_t cmd[4];
  uint16_t cmd_pec;

  //1
  if (reg == 1)			//Read back auxiliary group A
  {
    cmd[1] = 0x10;
    cmd[0] = 0x00;
  }
  else if(reg == 2)		//Read back auxiliary group B
  {
    cmd[1] = 0x12;
    cmd[0] = 0x00;
  }
  else					//Read back auxiliary group A
  {
     cmd[1] = 0x10;
     cmd[0] = 0x00;
  }
  //2
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);

  //3
  wakeup_idle (chip);
  wakeup_idle (chip); //This will guarantee that the LTC6804 isoSPI port is awake, this command can be removed.

  //4
  //LTC6804_CS_low;
  set_cs(chip);
  spi_write_read(cmd,4,data,(REG_LEN*total_ic));
  set_cs(0xff);
  //LTC6804_CS_high;

}

void LTC6804_adcvax(uint8_t chip)//启动组合电池电压以及GPIO1、GPIO2转换和轮询状态
{
		uint8_t cmd[4];
		uint16_t cmd_pec;

		//1
		cmd[0] = 0x05;//滤波模式，一个转换要270ms
		//cmd[1] = 0x6F;// MD[1：0]=01:27KHz快速   10:7KHz标准  11:26Hz滤波模式
    cmd[1] = 0xeF;
		//2
		cmd_pec = pec15_calc(2, cmd);
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);

		//3
		wakeup_idle (chip);//This will guarantee that the LTC6804 isoSPI port is awake. 
		wakeup_idle (chip);//This command can be removed.
		wakeup_idle (chip);//5句wakeup_idle ()可保证LTC6804 isoSP 接口唤醒。
		wakeup_idle (chip);
		wakeup_idle (chip);

		//4
	  //以下指令详见LTC6804-1说明书P53页,与“起动电池电压ADC转换”说明类似
		//LTC6804_CS_low;  //把CSB信号拉低  
        set_cs(chip);
		spi_write_array(4,cmd);//发送命令
        set_cs(0xff);
		//LTC6804_CS_high; //把CSB信号拉高

}
