/******************************************************************************
*
* Main Dev Kit Application
* Revision 1.0
*
* Notes:
* Initial Software package that is developed for the Dev Kit.  07/06/16 - EBJ
*
******************************************************************************/

#include "LApp.h"

//////////////////////////// MAIN //////////////////// MAIN //////////////
int main()
{
	// Initialize System
    init_platform();  		// This initializes the platform, which is ...
	ps7_post_config();
	Xil_DCacheDisable();	//
	InitializeAXIDma();		// Initialize the AXI DMA Transfer Interface
	Xil_Out32 (XPAR_AXI_GPIO_16_BASEADDR, 16384);
	Xil_Out32 (XPAR_AXI_GPIO_17_BASEADDR , 1);
	InitializeInterruptSystem(XPAR_PS7_SCUGIC_0_DEVICE_ID);

	// Initialize buffers
	memset(RecvBuffer, '0', 32);	// Clear RecvBuffer Variable
	memset(SendBuffer, '0', 32);	// Clear SendBuffer Variable

	//*******************Setup the UART **********************//
	XUartPs_Config *Config = XUartPs_LookupConfig(UART_DEVICEID);
	if (NULL == Config) { return 1;}
	Status = XUartPs_CfgInitialize(&Uart_PS, Config, Config->BaseAddress);
	if (Status != 0){ xil_printf("XUartPS did not CfgInit properly.\n");	}

//	/* Conduct a Selftest for the UART */
//	Status = XUartPs_SelfTest(&Uart_PS);
//	if (Status != 0) { xil_printf("XUartPS failed self test.\n"); }			//handle error checks here better

	/* Set to normal mode. */
	XUartPs_SetOperMode(&Uart_PS, XUARTPS_OPER_MODE_NORMAL);
	//*******************Setup the UART **********************//

//	unsigned int *fake_event_data;
//	fake_event_data = (unsigned int *)malloc(4096 * sizeof(unsigned int));
//	if(fake_event_data == NULL)
//		xil_printf("Could not allocate fake_event_data\r\n");
	unsigned char fake_event_data[1000] = "";

	//*******************Receive and Process Packets **********************//
	Xil_Out32 (XPAR_AXI_GPIO_0_BASEADDR, 11);
	Xil_Out32 (XPAR_AXI_GPIO_1_BASEADDR, 71);
	Xil_Out32 (XPAR_AXI_GPIO_2_BASEADDR, 167);
	Xil_Out32 (XPAR_AXI_GPIO_3_BASEADDR, 1500);
/*	Xil_Out32 (XPAR_AXI_GPIO_4_BASEADDR, 12);
	Xil_Out32 (XPAR_AXI_GPIO_5_BASEADDR, 75);
	Xil_Out32 (XPAR_AXI_GPIO_6_BASEADDR, 75);
	Xil_Out32 (XPAR_AXI_GPIO_7_BASEADDR, 5);
	Xil_Out32 (XPAR_AXI_GPIO_8_BASEADDR, 25);*/
	//*******************enable the system **********************//
	Xil_Out32(XPAR_AXI_GPIO_18_BASEADDR, 1);
	//*******************Receive and Process Packets **********************//

	// *********** Setup the Hardware Reset GPIO ****************//
	GPIOConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	Status = XGpioPs_CfgInitialize(&Gpio, GPIOConfigPtr, GPIOConfigPtr ->BaseAddr);
	if (Status != XST_SUCCESS) { return XST_FAILURE; }
	XGpioPs_SetDirectionPin(&Gpio, SW_BREAK_GPIO, 1);
	// *********** Setup the Hardware Reset MIO ****************//

	// *********** Mount SD Card and Initialize Variables ****************//
	if( doMount == 0 ){			//Initialize the SD card here
		ffs_res = f_mount(0, NULL);
		ffs_res = f_mount(0, &fatfs);
		doMount = 1;
	}
//	if( f_stat( cLogFile, &fno) ){	// f_stat returns non-zero(false) if no file exists, so open/create the file
//		ffs_res = f_open(&logFile, cLogFile, FA_WRITE|FA_OPEN_ALWAYS);
//		ffs_res = f_write(&logFile, cZeroBuffer, 10, &numBytesWritten);
//		filptr_clogFile += numBytesWritten;		// Protect the first xx number of bytes to use as flags, in this case xx must be 10
//		ffs_res = f_close(&logFile);
//	}
//	else // If the file exists, read it
//	{
//		ffs_res = f_open(&logFile, cLogFile, FA_READ|FA_WRITE);	//open with read/write access
//		ffs_res = f_lseek(&logFile, 0);							//go to beginning of file
//		ffs_res = f_read(&logFile, &filptr_buffer, 10, &numBytesRead);	//Read the first 10 bytes to determine flags and the size of the write pointer
//		sscanf(filptr_buffer, "%d", &filptr_clogFile);			//take the write pointer from char -> integer so we may use it
//		ffs_res = f_lseek(&logFile, filptr_clogFile);			//move the write pointer so we don't overwrite info
//		iSprintfReturn = snprintf(cWriteToLogFile, LOG_FILE_BUFF_SIZE, "POWER RESET %f ", dTime);	//write that the system was power cycled
//		usleep(200);
//		ffs_res = f_write(&logFile, cWriteToLogFile, iSprintfReturn, &numBytesWritten);	//write to the file
//		filptr_clogFile += numBytesWritten;						//update the write pointer
//		ffs_res = f_close(&logFile);							//close the file
//	}
//
//	if( f_stat(cDirectoryLogFile0, &fnoDIR) )	//check if the file exists
//	{
//		ffs_res = f_open(&directoryLogFile, cDirectoryLogFile0, FA_WRITE|FA_OPEN_ALWAYS);	//if no, create the file
//		ffs_res = f_write(&directoryLogFile, cZeroBuffer, 10, &numBytesWritten);			//write the zero buffer so we can keep track of the write pointer
//		filptr_cDIRFile += 10;																//move the write pointer
//		ffs_res = f_write(&directoryLogFile, cLogFile, 11, &numBytesWritten);				//write the name of the log file because it was created above
//		filptr_cDIRFile += numBytesWritten;													//update the write pointer
//		snprintf(cWriteToLogFile, 10, "%d", filptr_cDIRFile);								//write formatted output to a sized buffer; create a string of a certain length
//		ffs_res = f_lseek(&directoryLogFile, (10 - LNumDigits(filptr_cDIRFile)));			// Move to the start of the file
//		ffs_res = f_write(&directoryLogFile, cWriteToLogFile, LNumDigits(filptr_cDIRFile), &numBytesWritten);	//Record the new file pointer
//		ffs_res = f_close(&directoryLogFile);												//close the file
//	}
//	else	//if the file exists, read it
//	{
//		ffs_res = f_open(&directoryLogFile, cDirectoryLogFile0, FA_READ);					//open the file
//		ffs_res = f_lseek(&directoryLogFile, 0);											//move to the beginning of the file
//		ffs_res = f_read(&directoryLogFile, &filptr_cDIRFile_buffer, 10, &numBytesWritten);	//read the write pointer
//		sscanf(filptr_cDIRFile_buffer, "%d", &filptr_cDIRFile);								//write the pointer to the relevant variable
//		ffs_res = f_close(&directoryLogFile);												//close the file
//	}
	// *********** Mount SD Card and Initialize Variables ****************//

	// ******************* POLLING LOOP *******************//
	while(1){
		XUartPs_SetOptions(&Uart_PS,XUARTPS_OPTION_RESET_RX);	// Clear UART Read Buffer
		memset(RecvBuffer, '0', 32);							// Clear RecvBuffer Variable

		//sleep(0.5);  // Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 0.5 s
		xil_printf("\n\rDevkit version 2.25 \n\r");
		xil_printf("MAIN MENU \n\r");
		xil_printf("******\n\r");
		xil_printf(" 0) Set Mode of Operation\n\r");
		xil_printf(" 1) Enable or Disable the system\n\r");
		xil_printf(" 2) Stream Processed Data\n\r");
		xil_printf("\n\r***Setup Parameters *** \n\r");
		xil_printf(" 3) Set Trigger Threshold\n\r");
		xil_printf(" 4) Set Integration Times (Number of Clock Cycles * 4ns) \n\r");
//		xil_printf("\n\r*** Additional Commands *** \n\r");
//		xil_printf(" 5) Perform a DMA transfer of Waveform Data\n\r");
//		xil_printf(" 6) Perform a DMA transfer of Processed Data\n\r");
//		xil_printf(" 7) Check the Size of the Data Buffered (Max = 4095) \n\r");
//		xil_printf(" 8) Clear the Processed Data Buffers\n\r");
//		xil_printf(" 9) *** Execute Print of Data on DRAM *** deprecated ***\n\r");
//		xil_printf("10) GUI Serial Transfer \n\r");
//		xil_printf("11) GUI Serial Change Trigger Threshold\n\r");
//		xil_printf("12) GUI Serial Change Integration Times\n\r");
//		xil_printf("13) GUI Transfer Processed Data\n\r");
//		xil_printf("14) High Voltage and Temperature Control \n\r");
//		xil_printf("15) Print Out All Files in the Directory \n\r");
		xil_printf("******\n\r");

		ReadCommandPoll();
		menusel = 99999;
		sscanf((char *)RecvBuffer,"%02u",&menusel);
		if ( menusel < 0 || menusel > 4 ) {
			xil_printf(" Invalid Command: Enter 0-4 \n\r");
			sleep(1); 			// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 1 s
		}

		switch (menusel) { // Switch-Case Menu Select

		case 0: //Set Mode of Operation
			mode = 99; //Detector->GetMode();
			xil_printf("\n\r Waveform Data: \t Enter 0 <return>\n\r");
			xil_printf(" LPF Waveform Data: \t Enter 1 <return>\n\r");
			xil_printf(" DFF Waveform Data: \t Enter 2 <return>\n\r");
			xil_printf(" TRG Waveform Data: \t Enter 3 <return>\n\r");
			xil_printf(" Processed Data: \t Enter 4 <return>\n\r");
			ReadCommandPoll();
			sscanf((char *)RecvBuffer,"%01u",&mode);

			if (mode < 0 || mode > 4 ) { xil_printf("Invalid Command\n\r"); break; }
			Xil_Out32 (XPAR_AXI_GPIO_14_BASEADDR, ((u32)mode));
			// Register 14
			if ( mode == 0 ) { xil_printf("Transfer AA Waveforms\n\r"); }
			if ( mode == 1 ) { xil_printf("Transfer LPF Waveforms\n\r"); }
			if ( mode == 2 ) { xil_printf("Transfer DFF Waveforms\n\r"); }
			if ( mode == 3 ) { xil_printf("Transfer TRG Waveforms\n\r"); }
			if ( mode == 4 ) { xil_printf("Transfer Processed Data\n\r"); }
			sleep(1); 			// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 1 s

//			dTime += 1;	//increment time for testing
//			iSprintfReturn = snprintf(cWriteToLogFile, LOG_FILE_BUFF_SIZE, "Set mode %d %f ", mode, dTime);
//
//			ffs_res = f_open(&logFile, cLogFile, FA_OPEN_ALWAYS | FA_WRITE);
//			if(ffs_res){
//				xil_printf("Could not open file %d", ffs_res);
//				break;
//			}
//			ffs_res = f_lseek(&logFile, filptr_clogFile);
//			ffs_res = f_write(&logFile, cWriteToLogFile, iSprintfReturn, &numBytesWritten);
//			filptr_clogFile += numBytesWritten;
//			snprintf(cWriteToLogFile, 10, "%d", filptr_clogFile);
//			ffs_res = f_lseek(&logFile, (10 - LNumDigits(filptr_clogFile)));	// Move to the start of the file
//			ffs_res = f_write(&logFile, cWriteToLogFile, LNumDigits(filptr_clogFile), &numBytesWritten);	//Record the new file pointer
//			ffs_res = f_close(&logFile);
			break;

		case 1: //Enable or disable the system
			xil_printf("\n\r Disable: Enter 0 <return>\n\r");
			xil_printf(" Enable: Enter 1 <return>\n\r");
			ReadCommandPoll();
			sscanf((char *)RecvBuffer,"%01u",&enable_state);
			if (enable_state != 0 && enable_state != 1) { xil_printf("Invalid Command\n\r"); break; }
			Xil_Out32(XPAR_AXI_GPIO_18_BASEADDR, ((u32)enable_state));
			// Register 18 Out enabled, In Disabled
			if ( enable_state == 1 ) { xil_printf("DAQ Enabled\n\r"); }
			if ( enable_state == 0 ) { xil_printf("DAQ Disabled\n\r"); }
			sleep(1); 			// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 1 s

			dTime += 1;
			iSprintfReturn = snprintf(cWriteToLogFile, LOG_FILE_BUFF_SIZE, "Enable system %d %f ", enable_state, dTime);

			ffs_res = f_open(&logFile, cLogFile, FA_OPEN_ALWAYS | FA_WRITE);
			if(ffs_res){
				xil_printf("Could not open file %d", ffs_res);
				break;
			}
			ffs_res = f_lseek(&logFile, filptr_clogFile);
			ffs_res = f_write(&logFile, cWriteToLogFile, iSprintfReturn, &numBytesWritten);
			filptr_clogFile += numBytesWritten;												// Add the number of bytes written to the file pointer
			snprintf(cWriteToLogFile, 10, "%d", filptr_clogFile);							// Write that to a string for saving
			ffs_res = f_lseek(&logFile, (10 - LNumDigits(filptr_clogFile)));				// Seek to the beginning of the file skipping the leading zeroes
			ffs_res = f_write(&logFile, cWriteToLogFile, LNumDigits(filptr_clogFile), &numBytesWritten); // Write the new file pointer
			ffs_res = f_close(&logFile);

			break;

		case 2: //Continuously Read of Processed Data
			ffs_res = f_open(&event_data_file, c_event_data, FA_OPEN_ALWAYS | FA_READ);	// open the data file
			if(ffs_res)
			{
				xil_printf("Could not open file %d\r\n", ffs_res);
				break;
			}

			ffs_res = f_lseek(&event_data_file, 0);	// open the data file
			if(ffs_res)
			{
				xil_printf("Could not open file %d\r\n", ffs_res);
				break;
			}

			while(1)
			{
				//read the data in from the file
				ffs_res = f_read(&event_data_file, fake_event_data, sizeof(fake_event_data), &numBytesRead);
				if(ffs_res)
				{
					xil_printf("Could not read the data %d\r\n", ffs_res);
					break;
				}

				for(i = 0; i < numBytesRead; i++)
				{
					printf("%c",fake_event_data[i]);
					if((i % 512) == 0)
					{
						//check for a 'q'
						XUartPs_Recv(&Uart_PS, (u8 *)RecvBuffer, 32);	//receive input
						if ( RecvBuffer[0] == 'q' ) { sw = 1; }		//if the first character in the buffer is 'q', set sw = 1
						if(sw)										//if sw != 0, break out of the while loop, which will return to the main menu
							break;
					}
				}

				if(numBytesRead < 1000)
				{
					ffs_res = f_lseek(&event_data_file, 0);
					if(ffs_res)
					{
						xil_printf("Could not seek to the beginning of the file\r\n");
						break;
					}
				}

				//check for a 'q'
				XUartPs_Recv(&Uart_PS, (u8 *)RecvBuffer, 32);	//receive input
				if ( RecvBuffer[0] == 'q' ) { sw = 1; }		//if the first character in the buffer is 'q', set sw = 1
				if(sw)										//if sw != 0, break out of the while loop, which will return to the main menu
					break;
			}

			sw = 0;   // broke out of the read loop, stop switch reset to 0
			break;

		case 3: //Set Threshold
			iThreshold0 = Xil_In32(XPAR_AXI_GPIO_10_BASEADDR);
			xil_printf("\n\r Existing Threshold = %d \n\r",Xil_In32(XPAR_AXI_GPIO_10_BASEADDR));
			xil_printf(" Enter Threshold (0 to 10240) <return> \n\r");
			ReadCommandPoll();
			sscanf((char *)RecvBuffer,"%05u",&iThreshold1);
			if((iThreshold1 < 0) || (iThreshold1 > 10240)) { xil_printf("Invalid command\r\n"); break; }
			Xil_Out32(XPAR_AXI_GPIO_10_BASEADDR, ((u32)iThreshold1));
			xil_printf("New Threshold = %d \n\r",Xil_In32(XPAR_AXI_GPIO_10_BASEADDR));
			sleep(1); 			// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 1 s

//			dTime += 1;
//			iSprintfReturn = snprintf(cWriteToLogFile, LOG_FILE_BUFF_SIZE, "Set trigger threshold from %d to %d %f ", iThreshold0, iThreshold1, dTime);
//
//			ffs_res = f_open(&logFile, cLogFile, FA_OPEN_ALWAYS | FA_WRITE);
//			if(ffs_res){
//				xil_printf("Could not open file %d", ffs_res);
//				break;
//			}
//			ffs_res = f_lseek(&logFile, filptr_clogFile);
//			ffs_res = f_write(&logFile, cWriteToLogFile, iSprintfReturn, &numBytesWritten);
//			filptr_clogFile += numBytesWritten;
//			snprintf(cWriteToLogFile, 10, "%d", filptr_clogFile);							// Write that to a string for saving
//			ffs_res = f_lseek(&logFile, (10 - LNumDigits(filptr_clogFile)));				// Seek to the beginning of the file skipping the leading zeroes
//			ffs_res = f_write(&logFile, cWriteToLogFile, LNumDigits(filptr_clogFile), &numBytesWritten); // Write the new file pointer
//			ffs_res = f_close(&logFile);
//			break;

		case 4: //Set Integration Times
			setIntsArray[4] = -52 + ((int)Xil_In32(XPAR_AXI_GPIO_0_BASEADDR))*4;
			setIntsArray[5] = -52 + ((int)Xil_In32(XPAR_AXI_GPIO_1_BASEADDR))*4;
			setIntsArray[6] = -52 + ((int)Xil_In32(XPAR_AXI_GPIO_2_BASEADDR))*4;
			setIntsArray[7] = -52 + ((int)Xil_In32(XPAR_AXI_GPIO_3_BASEADDR))*4;

			xil_printf("\n\r Existing Integration Times \n\r");
			xil_printf(" Time = 0 ns is when the Pulse Crossed Threshold \n\r");
			xil_printf(" Baseline Integral Window \t [-200ns,%dns] \n\r",-52 + ((int)Xil_In32(XPAR_AXI_GPIO_0_BASEADDR))*4 );
			xil_printf(" Short Integral Window \t [-200ns,%dns] \n\r",-52 + ((int)Xil_In32(XPAR_AXI_GPIO_1_BASEADDR))*4 );
			xil_printf(" Long Integral Window  \t [-200ns,%dns] \n\r",-52 + ((int)Xil_In32(XPAR_AXI_GPIO_2_BASEADDR))*4 );
			xil_printf(" Full Integral Window  \t [-200ns,%dns] \n\r",-52 + ((int)Xil_In32(XPAR_AXI_GPIO_3_BASEADDR))*4 );
			xil_printf(" Change: (Y)es (N)o <return>\n\r");
			ReadCommandPoll();
			sscanf((char *)RecvBuffer,"%c",&updateint);

			if (updateint == 'N' || updateint == 'n') {
//				dTime += 1;
//				iSprintfReturn = snprintf(cWriteToLogFile, LOG_FILE_BUFF_SIZE, "No change to integration times of %d %d %d %d %f ", setIntsArray[4], setIntsArray[5], setIntsArray[6], setIntsArray[7], dTime);
//				ffs_res = f_open(&logFile, cLogFile, FA_OPEN_ALWAYS | FA_WRITE);
//				if(ffs_res){
//					xil_printf("Could not open file %d", ffs_res);
//					break;
//				}
//				ffs_res = f_lseek(&logFile, filptr_clogFile);
//				ffs_res = f_write(&logFile, cWriteToLogFile, iSprintfReturn, &numBytesWritten);
//				filptr_clogFile += numBytesWritten;
//				snprintf(cWriteToLogFile, 10, "%d", filptr_clogFile);							// Write that to a string for saving
//				ffs_res = f_lseek(&logFile, (10 - LNumDigits(filptr_clogFile)));				// Seek to the beginning of the file skipping the leading zeroes
//				ffs_res = f_write(&logFile, cWriteToLogFile, LNumDigits(filptr_clogFile), &numBytesWritten); // Write the new file pointer
//				ffs_res = f_close(&logFile);
				break;
			}

			if (updateint == 'Y' || updateint == 'y') {
				SetIntegrationTimes(&setIntsArray, &setSamples);
//				dTime += 1;
//				iSprintfReturn = snprintf(cWriteToLogFile, LOG_FILE_BUFF_SIZE, "Set integration times to %d %d %d %d from %d %d %d %d %f ",
//						setIntsArray[0], setIntsArray[1], setIntsArray[2], setIntsArray[3], setIntsArray[4], setIntsArray[5], setIntsArray[6], setIntsArray[7], dTime);
//				ffs_res = f_open(&logFile, cLogFile, FA_OPEN_ALWAYS | FA_WRITE);
//				if(ffs_res){
//					xil_printf("Could not open file %d", ffs_res);
//					break;
//				}
//				ffs_res = f_lseek(&logFile, filptr_clogFile);
//				ffs_res = f_write(&logFile, cWriteToLogFile, iSprintfReturn, &numBytesWritten);
//				filptr_clogFile += numBytesWritten;
//				snprintf(cWriteToLogFile, 10, "%d", filptr_clogFile);							// Write that to a string for saving
//				ffs_res = f_lseek(&logFile, (10 - LNumDigits(filptr_clogFile)));				// Seek to the beginning of the file skipping the leading zeroes
//				ffs_res = f_write(&logFile, cWriteToLogFile, LNumDigits(filptr_clogFile), &numBytesWritten); // Write the new file pointer
//				ffs_res = f_close(&logFile);
			}

			sleep(1); 			// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 1 s
			break;

		case 5: //Perform a DMA transfer
			xil_printf("\n\r Perform DMA Transfer of Waveform Data\n\r");
			xil_printf(" Press 'q' to Exit Transfer  \n\r");
			Xil_Out32 (XPAR_AXI_DMA_0_BASEADDR + 0x48, 0xa000000);
			Xil_Out32 (XPAR_AXI_DMA_0_BASEADDR + 0x58 , 65536);
			sleep(1); 			// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 1 s
			PrintData();		// Display data to console.
			sw = 0;   // broke out of the read loop, stop swith reset to 0
			break;

		case 6: //Perform a DMA transfer of Processed data
			xil_printf("\n\r ********Perform DMA Transfer of Processed Data \n\r");
			xil_printf(" Press 'q' to Exit Transfer  \n\r");
			//Xil_Out32 (XPAR_AXI_GPIO_18_BASEADDR, 0);	// Disable : GPIO Reg Capture Module
			Xil_Out32 (XPAR_AXI_GPIO_15_BASEADDR, 1);	// Enable: GPIO Reg to Readout Data MUX
			//sleep(1);				// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 1 s
			Xil_Out32 (XPAR_AXI_DMA_0_BASEADDR + 0x48, 0xa000000); // Transfer from BRAM to DRAM, start address 0xa000000, 16-bit length
			Xil_Out32 (XPAR_AXI_DMA_0_BASEADDR + 0x58 , 65536);
			sleep(1); 			// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 1 s
			Xil_Out32 (XPAR_AXI_GPIO_15_BASEADDR, 0); 	// Disable: GPIO Reg turn off Readout Data MUX
			ClearBuffers();
			PrintData();								// Display data to console.
			//Xil_Out32 (XPAR_AXI_GPIO_18_BASEADDR, 1);	// Enable : GPIO Reg Capture Module
			sw = 0;   // broke out of the read loop, stop swith reset to 0
			sleep(1); 			// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 1 s
			break;

		case 7: //Check the Size of the Data Buffers
			databuff = Xil_In32 (XPAR_AXI_GPIO_11_BASEADDR);
			xil_printf("\n\r BRAM Data Buffer Size = %d \n\r",databuff);
			sleep(1); 			// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 1 s
			break;

		case 8: //Clear the processed data buffers
			xil_printf("\n\r Clear the Data Buffers\n\r");
			ClearBuffers();
			sleep(1); 			// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 1 s
			break;

		case 9: //Print DMA Data
			xil_printf("\n\r Print Data\n\r");
			PrintData();
			break;
		case 10: //Transfer data over serial port	// Skips setting integrals, threshold	//Difference is this doesn't save to sd, it prints out the data for us
			//xil_printf("321");	//echo back to the gui that the request was received
			mode = 0;
			Xil_Out32 (XPAR_AXI_GPIO_14_BASEADDR, ((u32)mode));	//sets mode to transfer AA waveforms
			sleep(1);
			enable_state = 1;
			Xil_Out32(XPAR_AXI_GPIO_18_BASEADDR, ((u32)enable_state));	//enables the system
			sleep(1);
			getWFDAQ();
			break;
		case 11: //change threshold over the serial connection
			xil_printf("Enter the new threshold: <enter>\n");
			ReadCommandPoll();
			sscanf((char *)RecvBuffer,"%04u",&iThreshold1);
			Xil_Out32(XPAR_AXI_GPIO_10_BASEADDR, ((u32)iThreshold1));
			xil_printf("New Threshold = %d \n\r",Xil_In32(XPAR_AXI_GPIO_10_BASEADDR));
			sleep(1); 			// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 1 s
			break;
		case 12: //change integrals over the serial connection
			xil_printf("Enter each integral time followed by <enter>");
			ReadCommandPoll();
			sscanf((char *)RecvBuffer,"%04u",&setBL);
			ReadCommandPoll();
			sscanf((char *)RecvBuffer,"%04u",&setSI);
			ReadCommandPoll();
			sscanf((char *)RecvBuffer,"%04u",&setLI);
			ReadCommandPoll();
			sscanf((char *)RecvBuffer,"%04u",&setFI);

			Xil_Out32 (XPAR_AXI_GPIO_0_BASEADDR, ((u32)((setBL+52)/4)));	//set baseline int time
			Xil_Out32 (XPAR_AXI_GPIO_1_BASEADDR, ((u32)((setSI+52)/4)));	//set short int time
			Xil_Out32 (XPAR_AXI_GPIO_2_BASEADDR, ((u32)((setLI+52)/4)));	//set long int time
			Xil_Out32 (XPAR_AXI_GPIO_3_BASEADDR, ((u32)((setFI+52)/4)));	//set full int time

			//echo back the changed values //for checking
			xil_printf("  Inputs Rounded to the Nearest 4 ns : Number of Samples\n\r");
			xil_printf("  Baseline Integral Window  [-200ns,%dns]: %d \n\r",-52 + ((int)Xil_In32(XPAR_AXI_GPIO_0_BASEADDR))*4, 38+Xil_In32(XPAR_AXI_GPIO_0_BASEADDR) );
			xil_printf("  Short Integral Window 	  [-200ns,%dns]: %d \n\r",-52 + ((int)Xil_In32(XPAR_AXI_GPIO_1_BASEADDR))*4, 38+Xil_In32(XPAR_AXI_GPIO_1_BASEADDR));
			xil_printf("  Long Integral Window      [-200ns,%dns]: %d \n\r",-52 + ((int)Xil_In32(XPAR_AXI_GPIO_2_BASEADDR))*4, 38+Xil_In32(XPAR_AXI_GPIO_2_BASEADDR));
			xil_printf("  Full Integral Window      [-200ns,%dns]: %d \n\r",-52 + ((int)Xil_In32(XPAR_AXI_GPIO_3_BASEADDR))*4, 38+Xil_In32(XPAR_AXI_GPIO_3_BASEADDR));
			break;
		case 13: //Transfer processed data over the serial port
			mode = 4;
			Xil_Out32 (XPAR_AXI_GPIO_14_BASEADDR, ((u32)mode));	//sets mode to processed data
			sleep(1);
			enable_state = 1;
			Xil_Out32(XPAR_AXI_GPIO_18_BASEADDR, ((u32)enable_state)); //enables the system
			sleep(1);
			getWFDAQ();
			break;
		case 14: //HV and temp control
			xil_printf("************HIGH VOLTAGE CONTROL************\n\r");
			xil_printf(" 0) Write value to potentiometer\n\r");
			xil_printf(" 1) Read value from potentiometer\n\r");
			xil_printf(" 2) Store value to EEPROM\n\r");
			xil_printf(" 3) Read value from EEPROM\n\r");
			xil_printf("*************TEMPERATURE SENSOR*************\n\r");
			xil_printf(" 4) Read temperature\n\r");

			ReadCommandPoll();
			menusel = 99999;
			sscanf((char *)RecvBuffer,"%01u",&menusel);
			if( menusel < 0 || menusel > 4) { xil_printf("Invalid command.\n\r"); sleep(1); continue; }

			switch(menusel){
			case 0:	//Write Pots
				IIC_SLAVE_ADDR=&IIC_SLAVE_ADDR1;
				cntrl = 0x1; //command 1 - write contents of serial register data to RDAC - see AD5144 datasheet pg 26
				xil_printf(" Enter a value from 1-256 taps to write to the potentiometer  \n\r");
				ReadCommandPoll();
				sscanf(RecvBuffer,"%u",&data_bits);
				if(data_bits < 1 || data_bits > 256) { xil_printf("Invalid number of taps.\n\r"); sleep(1); continue; }
				xil_printf(" Enter a number 1-4 to select a particular potentiometer to adjust or 5 for all potentiometers  \n\r");
				ReadCommandPoll();
				sscanf(RecvBuffer,"%01u",&rdac);
				if(rdac < 1 || rdac > 5) { xil_printf("Invalid pot selection.\n\r"); sleep(1); continue; }
				xil_printf("%d \t %d \n\r", data_bits, rdac);

				switch(rdac){
				case 1:
					pot_addr = 0x0;
					break;
				case 2:
					pot_addr = 0x1;
					break;
				case 3:
					pot_addr = 0x2;
					break;
				case 4:
					pot_addr = 0x3;
					break;
				case 5:
					pot_addr = 0x8;
					break;
				default:
					xil_printf("Invalid. No changes made.");
					break;
				}

				a = cntrl<<4|pot_addr;
				i2c_Send_Buffer[0] = a;
				i2c_Send_Buffer[1] = data_bits;
				Status = IicPsMasterSend(IIC_DEVICE_ID, i2c_Send_Buffer, i2c_Recv_Buffer, IIC_SLAVE_ADDR);
				break;
			case 1:	//Read Pots
				IIC_SLAVE_ADDR=&IIC_SLAVE_ADDR1;
				cntrl = 0x3; //command 3 - read back contents - see AD5144 datasheet pg 26
				xil_printf(" Enter a number 1-4 to select which potentiometer to read value  \n\r");
				ReadCommandPoll();
				sscanf(RecvBuffer,"%01d",&rdac);
				if(rdac < 1 || rdac > 4) { xil_printf("Invalid pot selection.\n\r"); sleep(1); continue; }

				switch(rdac){
				case 1:
					pot_addr = 0x0;
					break;
				case 2:
					pot_addr = 0x1;
					break;
				case 3:
					pot_addr = 0x2;
					break;
				case 4:
					pot_addr = 0x3;
					break;
				default:
					xil_printf("Invalid. No changes made.");
					break;
				}

				a = cntrl<<4|pot_addr;
				i2c_Send_Buffer[0] = a;
				i2c_Send_Buffer[1] = 0x3;
				Status = IicPsMasterSend(IIC_DEVICE_ID, i2c_Send_Buffer, i2c_Recv_Buffer, IIC_SLAVE_ADDR);
				Status = IicPsMasterRecieve(IIC_DEVICE_ID, i2c_Recv_Buffer, IIC_SLAVE_ADDR);
				xil_printf("%d \t", i2c_Recv_Buffer[0]);
				xil_printf("taps\n");
				break;
			case 2:	//Write EEPROM
				IIC_SLAVE_ADDR=&IIC_SLAVE_ADDR1;
				cntrl = 0x7; //command 9 - Copy RDAC register to EEPROM - see AD5144 datasheet pg 26
				xil_printf("Select which potentiometer value to store in EEPROM (1-4)   \n\r");
				ReadCommandPoll();
				sscanf(RecvBuffer,"%01u",&rdac);
				if(rdac < 1 || rdac > 4) { xil_printf("Invalid pot selection.\n\r"); sleep(1); continue; }
				xil_printf("%d \t %d \n\r", data_bits, rdac);

				switch(rdac){
				case 1:
					pot_addr = 0x0;
					break;
				case 2:
					pot_addr = 0x1;
					break;
				case 3:
					pot_addr = 0x2;
					break;
				case 4:
					pot_addr = 0x3;
					break;
				case 5:
					pot_addr = 0x8;
					break;
				default:
					xil_printf("Invalid. No changes made.");
					break;
				}

				a = cntrl<<4|pot_addr;
				i2c_Send_Buffer[0] = a;
				i2c_Send_Buffer[1] = 0x1;
				Status = IicPsMasterSend(IIC_DEVICE_ID, i2c_Send_Buffer, i2c_Recv_Buffer, IIC_SLAVE_ADDR);
				break;
			case 3:	//Read EEPROM
				IIC_SLAVE_ADDR=&IIC_SLAVE_ADDR1;
				cntrl = 0x3;//command 3 - Read back contents - see AD5144 datasheet pg 26
				xil_printf(" Enter a number 1-4 to select which EEPROM to read  \n\r");
				ReadCommandPoll();
				sscanf(RecvBuffer,"%01u",&rdac);
				if(rdac < 1 || rdac > 4) { xil_printf("Invalid pot selection.\n\r"); sleep(1); continue; }

				switch(rdac){
				case 1:
					pot_addr = 0x0;
					break;
				case 2:
					pot_addr = 0x1;
					break;
				case 3:
					pot_addr = 0x2;
					break;
				case 4:
					pot_addr = 0x3;
					break;
				default:
					xil_printf("Invalid. No changes made.");
					break;
				}

				//data_bits = 0x3;
				a = cntrl<<4|pot_addr;
				i2c_Send_Buffer[0] = a;
				i2c_Send_Buffer[1] = 0x1;
				Status = IicPsMasterSend(IIC_DEVICE_ID, i2c_Send_Buffer, i2c_Recv_Buffer, IIC_SLAVE_ADDR);
				Status = IicPsMasterRecieve(IIC_DEVICE_ID, i2c_Recv_Buffer, IIC_SLAVE_ADDR);
				xil_printf("%d \t", i2c_Recv_Buffer[0]);
				xil_printf("taps\n");
				break;
			case 4:	//Read Temp
				IIC_SLAVE_ADDR=&IIC_SLAVE_ADDR2;
				i2c_Send_Buffer[0] = 0x0;
				i2c_Send_Buffer[1] = 0x0;
				Status = IicPsMasterSend(IIC_DEVICE_ID, i2c_Send_Buffer, i2c_Recv_Buffer, IIC_SLAVE_ADDR);
				Status = IicPsMasterRecieve(IIC_DEVICE_ID, i2c_Recv_Buffer, IIC_SLAVE_ADDR);
				a = i2c_Recv_Buffer[0]<< 5;
				b = a | i2c_Recv_Buffer[1] >> 3;
				b /= 16;
				xil_printf("%d\xf8\x43\n", b);
				break;
			default:
				i = 1;
				break;
			}//end of case statement
			break;
		case 15:
			ffs_res = f_open(&directoryLogFile, cDirectoryLogFile0, FA_READ);
			dirSize = f_size(&directoryLogFile) - 10;
			filptr_cDIRFile = 10;
			dirFileContents = malloc(1 * dirSize + 1);
			ffs_res = f_lseek(&directoryLogFile, 10);
			ffs_res = f_read(&directoryLogFile, dirFileContents, dirSize, &numBytesRead);
			ffs_res = f_close(&directoryLogFile);
			snprintf(dirFileContents, dirSize + 1, dirFileContents + '\0');
			xil_printf(dirFileContents);
			free(dirFileContents);
			break;
		case 16:
			//DAQ minimal function
			//need to loop over this function
			// utilize DAQ function below (as opposed to WFDAQ)

		default :
			break;
		} // End Switch-Case Menu Select

	}	// ******************* POLLING LOOP *******************//

    cleanup_platform();
    return 0;
}
//////////////////////////// MAIN //////////////////// MAIN //////////////

//////////////////////////// InitializeAXIDma////////////////////////////////
// Sets up the AXI DMA
int InitializeAXIDma(void) {
	u32 tmpVal_0 = 0;

	tmpVal_0 = Xil_In32(XPAR_AXI_DMA_0_BASEADDR + 0x30);

	tmpVal_0 = tmpVal_0 | 0x1001; //<allow DMA to produce interrupts> 0 0 <run/stop>

	Xil_Out32 (XPAR_AXI_DMA_0_BASEADDR + 0x30, tmpVal_0);
	Xil_In32(XPAR_AXI_DMA_0_BASEADDR + 0x30);	//what does the return value give us? What do we do with it?

	return 0;
}
//////////////////////////// InitializeAXIDma////////////////////////////////

//////////////////////////// InitializeInterruptSystem////////////////////////////////
int InitializeInterruptSystem(u16 deviceID) {
	int Status;

	GicConfig = XScuGic_LookupConfig (deviceID);

	if(NULL == GicConfig) {

		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(&InterruptController, GicConfig, GicConfig->CpuBaseAddress);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;

	}

	Status = SetUpInterruptSystem(&InterruptController);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;

	}

	Status = XScuGic_Connect (&InterruptController,
			XPAR_FABRIC_AXI_DMA_0_S2MM_INTROUT_INTR,
			(Xil_ExceptionHandler) InterruptHandler, NULL);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;

	}

	XScuGic_Enable(&InterruptController, XPAR_FABRIC_AXI_DMA_0_S2MM_INTROUT_INTR );

	return XST_SUCCESS;

}
//////////////////////////// InitializeInterruptSystem////////////////////////////////


//////////////////////////// Interrupt Handler////////////////////////////////
void InterruptHandler (void ) {

	u32 tmpValue;
	tmpValue = Xil_In32(XPAR_AXI_DMA_0_BASEADDR + 0x34);
	tmpValue = tmpValue | 0x1000;
	Xil_Out32 (XPAR_AXI_DMA_0_BASEADDR + 0x34, tmpValue);

	global_frame_counter++;
}
//////////////////////////// Interrupt Handler////////////////////////////////


//////////////////////////// SetUp Interrupt System////////////////////////////////
int SetUpInterruptSystem(XScuGic *XScuGicInstancePtr) {
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, XScuGicInstancePtr);
	Xil_ExceptionEnable();
	return XST_SUCCESS;

}
//////////////////////////// SetUp Interrupt System////////////////////////////////

//////////////////////////// ReadCommandPoll////////////////////////////////
// Function used to clear the read buffer
// Read In new command, expecting a <return>
// Returns buffer size read
int ReadCommandPoll() {
	u32 rbuff = 0;			// read buffer size returned

	XUartPs_SetOptions(&Uart_PS,XUARTPS_OPTION_RESET_RX);	// Clear UART Read Buffer
	memset(RecvBuffer, '0', 32);	// Clear RecvBuffer Variable
	while (!(RecvBuffer[rbuff-1] == '\n' || RecvBuffer[rbuff-1] == '\r'))
	{
		rbuff += XUartPs_Recv(&Uart_PS, &RecvBuffer[rbuff],(32 - rbuff));
		sleep(0.1);			// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 0.1 s
	}
	return rbuff;
}
//////////////////////////// ReadCommandPoll////////////////////////////////


//////////////////////////// Set Integration Times ////////////////////////////////
// wfid = 	0 for AA
//			1 for DFF
//			2 for LPF
// At the moment, the software is expecting a 5 char buffer.  This needs to be fixed.
void SetIntegrationTimes(int * setIntsArray, u32 * setSamples){
	//int setIntsArray[8] = {};
	//u32 setsamples[8] = {};

	xil_printf("Enter Baseline Stop Time in ns: -52 to 0 <return> \r\n");
	ReadCommandPoll();
	sscanf((char *)RecvBuffer,"%d",&setIntsArray[0]);

	xil_printf("\n\rEnter Short Integral Stop Time in ns: -52 to 8000 <return> \n\r");
	ReadCommandPoll();
	sscanf((char *)RecvBuffer,"%d",&setIntsArray[1]);

	xil_printf("\n\rEnter Long Integral Stop Time in ns: -52 to 8000 <return> \n\r");
	ReadCommandPoll();
	sscanf((char *)RecvBuffer,"%d",&setIntsArray[2]);

	xil_printf("\n\rEnter Full Integral Stop Time in ns: -52 to 8000 <return> \n\r");
	ReadCommandPoll();
	sscanf((char *)RecvBuffer,"%d",&setIntsArray[3]);

	setSamples[0] = ((u32)((setIntsArray[0]+52)/4));
	setSamples[1] = ((u32)((setIntsArray[1]+52)/4));
	setSamples[2] = ((u32)((setIntsArray[2]+52)/4));
	setSamples[3] = ((u32)((setIntsArray[3]+52)/4));

	Xil_Out32 (XPAR_AXI_GPIO_0_BASEADDR, setSamples[0]);
	Xil_Out32 (XPAR_AXI_GPIO_1_BASEADDR, setSamples[1]);
	Xil_Out32 (XPAR_AXI_GPIO_2_BASEADDR, setSamples[2]);
	Xil_Out32 (XPAR_AXI_GPIO_3_BASEADDR, setSamples[3]);

	xil_printf("\n\rInputs Rounded to the Nearest 4 ns : Number of Samples\n\r");
	xil_printf("Baseline Integral Window  [-200ns,%dns]: %d \n\r",-52 + ((int)Xil_In32(XPAR_AXI_GPIO_0_BASEADDR))*4, 38+Xil_In32(XPAR_AXI_GPIO_0_BASEADDR) );
	xil_printf("Short Integral Window 	  [-200ns,%dns]: %d \n\r",-52 + ((int)Xil_In32(XPAR_AXI_GPIO_1_BASEADDR))*4, 38+Xil_In32(XPAR_AXI_GPIO_1_BASEADDR));
	xil_printf("Long Integral Window      [-200ns,%dns]: %d \n\r",-52 + ((int)Xil_In32(XPAR_AXI_GPIO_2_BASEADDR))*4, 38+Xil_In32(XPAR_AXI_GPIO_2_BASEADDR));
	xil_printf("Full Integral Window      [-200ns,%dns]: %d \n\r",-52 + ((int)Xil_In32(XPAR_AXI_GPIO_3_BASEADDR))*4, 38+Xil_In32(XPAR_AXI_GPIO_3_BASEADDR));

	xil_printf("value of gpio 3: %d\r\n", Xil_In32(XPAR_AXI_GPIO_3_BASEADDR));

	return;
}
//////////////////////////// Set Integration Times ////////////////////////////////

//////////////////////////// PrintData ////////////////////////////////
int PrintData( ){
	u32 data;
	int dram_addr;
	int dram_base = 0xa000000;
	int dram_ceiling = 0xA004000; //read out just adjacent average (0xA004000 - 0xa000000 = 16384)

	Xil_DCacheInvalidateRange(0x00000000, 65536);

	for (dram_addr = dram_base; dram_addr <= dram_ceiling; dram_addr+=4){
		if (!sw) { sw = XGpioPs_ReadPin(&Gpio, SW_BREAK_GPIO); } //read pin
		data = Xil_In32(dram_addr);
		xil_printf("%d\r\n",data);				//is there a better place to put this? We need to transfer it over USB, but can we do that rather than printing it?
		XUartPs_Recv(&Uart_PS, (u8 *)RecvBuffer, 32);
		if ( RecvBuffer[0] == 'q' ) { sw = 1;  }
		if(sw) { return sw; }
	}

	return sw;
}
//////////////////////////// PrintData ////////////////////////////////

//////////////////////////// Clear Processed Data Buffers ////////////////////////////////
void ClearBuffers() {
	Xil_Out32(XPAR_AXI_GPIO_9_BASEADDR,1);
	Xil_Out32(XPAR_AXI_GPIO_9_BASEADDR,0);
}
//////////////////////////// Clear Processed Data Buffers ////////////////////////////////

//////////////////////////// DAQ ////////////////////////////////
int DAQ(){
	int buffsize; 	//BRAM buffer size
	int dram_addr;	// DRAM Address
	static int filesWritten = 0;

	XUartPs_SetOptions(&Uart_PS,XUARTPS_OPTION_RESET_RX);

	Xil_Out32 (XPAR_AXI_DMA_0_BASEADDR + 0x48, 0xa000000); 		// DMA Transfer Step 1
	Xil_Out32 (XPAR_AXI_DMA_0_BASEADDR + 0x58 , 65536);			// DMA Transfer Step 2
	sleep(1);						// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 1 s
	ClearBuffers();												// Clear Buffers.
	// Capture garbage in DRAM
	for (dram_addr = 0xa000000; dram_addr <= 0xA004000; dram_addr+=4){Xil_In32(dram_addr);}

	while(1){
		buffsize = Xil_In32 (XPAR_AXI_GPIO_11_BASEADDR);
		if (!sw) { sw = XGpioPs_ReadPin(&Gpio, SW_BREAK_GPIO); } //read pin
		XUartPs_Recv(&Uart_PS, (u8 *)RecvBuffer, 32);
		if ( RecvBuffer[0] == 'q' ) { sw = 1; }
		if(sw) { return sw;	}

		if(buffsize >= 4095)
		{
			Xil_Out32 (XPAR_AXI_GPIO_15_BASEADDR, 1);
			Xil_Out32 (XPAR_AXI_DMA_0_BASEADDR + 0x48, 0xa000000);
			Xil_Out32 (XPAR_AXI_DMA_0_BASEADDR + 0x58 , 65536);
			sleep(1); 			// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 1 s
			Xil_Out32 (XPAR_AXI_GPIO_15_BASEADDR, 0);

			ClearBuffers();
			sw = ReadDataIn(filesWritten, &directoryLogFile);
			++filesWritten;
		}
	}

	return sw;
}
//////////////////////////// DAQ ////////////////////////////////

//////////////////////////// getWFDAQ ////////////////////////////////

int getWFDAQ(){
	int buffsize; 	//BRAM buffer size
	//int dram_addr;	// DRAM Address

	XUartPs_SetOptions(&Uart_PS,XUARTPS_OPTION_RESET_RX);

	Xil_Out32 (XPAR_AXI_DMA_0_BASEADDR + 0x48, 0xa000000); 		// DMA Transfer Step 1
	Xil_Out32 (XPAR_AXI_DMA_0_BASEADDR + 0x58 , 65536);			// DMA Transfer Step 2
	sleep(1);						// Built in Latency ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 1 s
	ClearBuffers();
	//for (dram_addr = 0xa000000; dram_addr <= 0xA004000; dram_addr+=4){Xil_In32(dram_addr);}

	while(1){
		if (!sw) { sw = XGpioPs_ReadPin(&Gpio, SW_BREAK_GPIO); } //read pin
		XUartPs_Recv(&Uart_PS, (u8 *)RecvBuffer, 32);
		if ( RecvBuffer[0] == 'q' ) { sw = 1; }
		if(sw) { return sw;	}

		buffsize = Xil_In32 (XPAR_AXI_GPIO_11_BASEADDR);	// AA write pointer // tells how far the system has read in the AA module
		if(buffsize >= 4095)
		{
			Xil_Out32 (XPAR_AXI_GPIO_15_BASEADDR, 1);				// init mux to transfer data between integrater modules to DMA
			Xil_Out32 (XPAR_AXI_DMA_0_BASEADDR + 0x48, 0xa000000);
			Xil_Out32 (XPAR_AXI_DMA_0_BASEADDR + 0x58 , 65536);
			sleep(1); 												// this will change
			Xil_Out32 (XPAR_AXI_GPIO_15_BASEADDR, 0);

			ClearBuffers();
			PrintData();
		}
	}
	return sw;
}

//////////////////////////// getWFDAQ ////////////////////////////////
