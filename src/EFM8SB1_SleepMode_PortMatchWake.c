//-----------------------------------------------------------------------------
// SB1_SleepMode_PortMatchWake.c
//-----------------------------------------------------------------------------
// Copyright 2014 Silicon Laboratories, Inc.
// http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
//
// Program Description:
//
// This program flashes LED0 on the EFM8SB1 target board using the
// interrupt handler for Timer2 when the MCU is awake.
//
// Pressing the switch on PB1  will place the device in a low power mode.
// Pressing the switch on PB0  will wake up the device using Port Match.
//
// How To Test:
// 1) Place the switch in "AEM" mode.
// 2) Connect the EFM8SB1 STK board to a PC using a mini USB cable.
// 3) Compile and download code to the EFM8SB1 STK board.
//    In Simplicity Studio IDE, select Run -> Debug from the menu bar,
//    click the Debug button in the quick menu, or press F11.
// 4) Run the code.
//    In Simplicity Studio IDE, select Run -> Resume from the menu bar,
//    click the Resume button in the quick menu, or press F8.
// 5) Press PB0 to place the device in sleep mode.  LED0 will stay lit
//    while the device is sleeping.
//    Press PB1 to wake it back up. The LED0 will blink when
//    the device is awake.
//
//
// Target:         EFM8SB1
// Tool chain:     Silicon Labs Studio / Keil C51 9.51
// Command Line:   None
//
// Release 1.0 (BL)
//    - Initial Release
//    - 9 JAN 2015
//
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include <si_toolchain.h>
#include <SI_EFM8SB1_Register_Enums.h>                  // SI_SFR declarations
#include "InitDevice.h"
#include "smartclock.h"                // RTC Functionality
#include "power.h"                     // Power Management Functionality

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------
SI_LOCATED_VARIABLE_NO_INIT (reserved, uint8_t, SI_SEG_XDATA, 0x0000);

#define SDA_H       	SDA=1
#define SDA_L       	SDA=0
#define SCL_H       	SCL=1
#define SCL_L      		SCL=0
#define SMBUS_SDA_PIN 	SDA
#define ACK 			0
#define NACK 			1
#define SA 				0x00                                   //Slave address 单个 MLX90615 时地址为 0x00,多个时地址默0x5b
#define RAM_ACCESS		0x20                           //RAM access command
#define EEPROM_ACCESS 	0x10                        //EEPROM access command
#define RAM_TOBJ1 		0x07                            //To1 address in the eeprom 物体温度
SI_SBIT(SDA, SFR_P1, 7);
SI_SBIT(SCL, SFR_P1, 6);

SI_SBIT(UART_TX, SFR_P0, 4);
SI_SBIT(UART_RX, SFR_P0, 5);

#define uart_tx()   UART_TX = 0
#define uart_rx()   UART_RX = 0


void usleep(int us)
{
	volatile int i =800*us;
	while(i-- > 0);
}
void msleep(int ms)
{
       volatile int i = ms;
       while(i-- > 0)usleep(1);
}
void SMBus_Delay(uint16_t time)
{
	/*uint16_t i, j;
	for (i=0; i<4; i++)
	{
		for (j=0; j<time; j++);
	}*/
	if(time==0){
	}
	else if((time==1)||(time==10)||(time==25)||(time==40)||(time==50)||(time==60)||(time==100)){
		timer_sleepus_select(time,1);
	}
	else if(time<10){
			timer_sleepus_select(1,time);
	}
	else if(time<1000){
		timer_sleepus_select(10,time/10);
		timer_sleepus_select(1,time-time/10*10);
	}
	else{
		timer_sleepus_select(10,time/10);
	}
}
void Mlx96015_Configuration(void)
{
    SDA_H;  //拉高SDAL和SCL
    SCL_H; //因为 SMBus 是下降沿触发
}

void SMBus_StartBit(void)
{
	SDA_H; // Set SDA line
    SMBus_Delay(1);   // Wait a few microseconds
    SCL_H; // Set SCK line
    SMBus_Delay(5);   // Generate bus free time between Stop
    SDA_L; // Clear SDA line
    SMBus_Delay(10);  // Hold time after (Repeated) Start
                      // Condition. After this period, the first clock is generated.
                      //(Thd:sta=4.0us min)
    SCL_L;            // Clear SCK line
    SMBus_Delay(2);   // Wait a few microseconds
}
void SMBus_StopBit(void)
{
	SCL_L; // Clear SCK line
    SMBus_Delay(5); // Wait a few microseconds
    SDA_L; // Clear SDA line
    SMBus_Delay(5); // Wait a few microseconds
    SCL_H; // Set SCK line
    SMBus_Delay(10); // Stop condition setup time(Tsu:sto=4.0us min)
    SDA_H; // Set SDA line
}
void SMBus_SendBit(uint8_t bit_out)
{
	if(bit_out==0)
		{
			SDA_L;
		}
		else
			{
				SDA_H;
			}
			SMBus_Delay(2); // Tsu:dat = 250ns minimum
			SCL_H; // Set SCK line
			SMBus_Delay(10); // High Level of Clock Pulse
			SCL_L; // Clear SCK line
			SMBus_Delay(10); // Low Level of Clock Pulse
          //SMBUS_SDA_H(); // Master release SDA line ,
			return;
}
uint8_t SMBus_ReceiveBit(void)
{
	uint8_t Ack_bit;
	SDA_H; //引脚靠外部电阻上拉， 当作输入
	SCL_H; // Set SCL line
	SMBus_Delay(2); // High Level of Clock Pulse
	if (SMBUS_SDA_PIN)
		{
			Ack_bit=1;
		}
		else
			{
				Ack_bit=0;
			}
			SCL_L; // Clear SCL line
			SMBus_Delay(4); // Low Level of Clock Pulse
			return Ack_bit;
}
uint8_t SMBus_SendByte(uint8_t Tx_buffer)
{
	uint8_t Bit_counter;
    uint8_t Ack_bit;
    uint8_t bit_out;
	for(Bit_counter=8; Bit_counter; Bit_counter--)
	{
		if (Tx_buffer&0x80)
			{
				bit_out=1; // If the current bit of Tx_buffer is 1 set bit_out
			}
			else
				{
					bit_out=0; // else clear bit_out
				}
				SMBus_SendBit(bit_out); // Send the current bit on SDA
				Tx_buffer<<=1; // Get next bit for checking
				}
	Ack_bit=SMBus_ReceiveBit(); // Get acknowledgment bit
				return Ack_bit;
}

uint8_t SMBus_ReceiveByte(uint8_t ack_nack)
{
	uint8_t RX_buffer;
	uint8_t Bit_Counter;
	for(Bit_Counter=8; Bit_Counter; Bit_Counter--)
	{
		if(SMBus_ReceiveBit()) // Get a bit from the SDA line
			{
				RX_buffer <<= 1; // If the bit is HIGH save 1 in RX_buffer
				RX_buffer |=0x01;
			}
			else
				{
					RX_buffer <<= 1; // If the bit is LOW save 0 in RX_buffer
					RX_buffer &=0xfe;
	            }
	}
	SMBus_SendBit(ack_nack); // Sends acknowledgment bit
	return RX_buffer;
}
uint8_t PEC_Calculation(uint8_t pec[])
{
	uint8_t crc[6];
	uint8_t BitPosition=47;
	uint8_t shift;
	uint8_t i;
	uint8_t j;
	uint8_t temp;
	do
	{ /*Load pattern value 0x000000000107*/
		crc[5]=0;
		crc[4]=0;
		crc[3]=0;
		crc[2]=0;
		crc[1]=0x01;
		crc[0]=0x07; /*Set maximum bit position at 47 ( six bytes byte5...byte0,MSbit=47)*/
		BitPosition=47; /*Set shift position at 0*/
		shift=0; /*Find first "1" in the transmited message beginning from the MSByte byte5*/
		i=5;
		j=0;
		while((pec[i]&(0x80>>j))==0 && i>0)
			{
				BitPosition--;
				if(j<7)
					{
						j++;
					}
					else
						{
							j=0x00;
							i--;
						}
			}/*End of while */
			/*Get shift value for pattern value*/
			shift=BitPosition-8;
			/*Shift pattern value */
			while(shift)
				{
					for(i=5; i<0xFF; i--)
					{
						if((crc[i-1]&0x80) && (i>0))
							{
								temp=1;
							}
							else
								{
									temp=0;
								}
								crc[i]<<=1;
								crc[i]+=temp;
					}/*End of for*/
					shift--;
				}/*End of while*/ /*Exclusive OR between pec and crc*/
				for(i=0; i<=5; i++)
				{
					pec[i] ^=crc[i];
				}/*End of for*/
	}
	while(BitPosition>8); /*End of do-while*/
	return pec[0];
}
/*int int_to_string(unsigned char *buffer, int  h )
{
	int len=0;
	if(h>=10000){
		len=5;
		buffer[0]=h/10000 + '0';
		buffer[1]=(h%10000)/1000 + '0';
		buffer[2]=(h%1000)/100 + '0';
		buffer[3]=(h%100)/10 + '0';
		buffer[4]=(h%10) + '0';

	}
	else if(h>=1000){
		len=4;
		buffer[0]=h/1000 + '0';
		buffer[1]=(h%1000)/100 + '0';
		buffer[2]=(h%100)/10 + '0';
		buffer[3]=(h%10) + '0';

	}
	else if(h>=100){
		len=3;
		buffer[0]=h/100 + '0';
		buffer[1]=(h%100)/10 + '0';
		buffer[2]=(h%10) + '0';

	}
	else if(h>=10){
		len=2;
		buffer[0]=h/10 + '0';
		buffer[1]=h%10 + '0';

	}
	else{
		len=1;
		buffer[0]=h + '0';
	}

	 return len;
}*/
/*int 型，限制长度8位*/
int int_to_string(unsigned char *buffer, unsigned int  h )
{
	int len=0;
	int i;
	unsigned int  t=h;
	while(t>0){
		t=t/10;
		len++;
	}
	t=h;
	for( i=0;i<len;i++){
		buffer[len-1-i]=t%10+'0';
		t=t/10;
	}
	return len;
}
uint16_t SMBus_ReadMemory(uint8_t slaveAddress, uint8_t command)
{
	char buf1[8]={0};
	int len=0;
	uint16_t tempdata;// Data storage (DataH:DataL)
	uint8_t Pec; // PEC byte storage
	uint8_t DataL=0; // Low data byte storage
	uint8_t DataH=0; // High data byte storage
	uint8_t arr[6]; // Buffer for the sent bytes
	uint8_t PecReg; // Calculated PEC byte storage
	uint8_t ErrorCounter; // Defines the number of the attempts for communicationwith MLX90615
	ErrorCounter=0x00; // Initialising of ErrorCounter
	slaveAddress <<= 1; //2-7 位表示从机地址
	do
	{
		repeat: SMBus_StopBit(); //If slave send NACK stop comunication
		--ErrorCounter; //Pre-decrement ErrorCounter
		if(!ErrorCounter) //ErrorCounter=0?
		{
			break; //Yes,go out from do-while{}
		}
		SMBus_StartBit(); //Start condition
		if(SMBus_SendByte(slaveAddress))//Send SlaveAddress 最低位 Wr=0 表示接下来写命令
		{
			goto repeat; //Repeat comunication again
		}
		if(SMBus_SendByte(command)) //Send command
		{
			goto repeat; //Repeat comunication again
		}
		SMBus_StartBit(); //Repeated Start condition
		if(SMBus_SendByte(slaveAddress+1)) //Send SlaveAddress 最低位 Rd=1 表示接下来读数据
			{
				goto repeat; //Repeat comunication again
			}
			DataL = SMBus_ReceiveByte(ACK); //Read low data,master must send ACK
			DataH = SMBus_ReceiveByte(ACK); //Read high data,master must send ACK
			Pec = SMBus_ReceiveByte(NACK); //Read PEC byte, master must send NACK
			SMBus_StopBit(); //Stop condition
			arr[5] = slaveAddress;
			arr[4] = command;
			arr[3] = slaveAddress+1; //Load array arr
			arr[2] = DataL;
			arr[1] = DataH;
			arr[0] = 0;
			PecReg=PEC_Calculation(arr); //Calculate CRC
		}
		while(PecReg != Pec); //If received and calculated CRC are equal go out fromdo-while{}
			//DataH=54;
			//DataL=96;//tempdata=13920;//5.25du
			len=int_to_string(buf1, DataH);
			uart_send("DataH:", sizeof("DataH:")-1);
			uart_send(buf1, len);//uart_send(buf1, sizeof(buf1));
			uart_send("\r\n", sizeof( "\r\n")-1);
			len=int_to_string(buf1, DataL);
			uart_send("DataL:", sizeof("DataL:")-1);
			uart_send(buf1, len);//uart_send(buf1, sizeof(buf1));
			uart_send("\r\n", sizeof( "\r\n")-1);

			tempdata = (DataH<<8) | DataL; //data=DataH:DataL
			len=int_to_string(buf1, tempdata);
			uart_send("tempdata:", sizeof("tempdata:")-1);
			uart_send(buf1, len);//uart_send(buf1, sizeof(buf1));
			uart_send("\r\n", sizeof( "\r\n")-1);
		return tempdata;
}

int SMBus_ReadTemp(void)
{
		float real_temp;
		uint16_t tempdata;
		tempdata=SMBus_ReadMemory(SA, RAM_ACCESS|RAM_TOBJ1);
		real_temp=tempdata*0.02-273.15;
		return real_temp*100;
}

int main (void)
{
	char buf1[8]={0};
	int len=0;
	int temp_100bei;

	enter_DefaultMode_from_RESET();
	while(1)
	{
		temp_100bei=SMBus_ReadTemp();
		len=int_to_string(buf1, temp_100bei);
		uart_send("objtemp:", sizeof("objtemp:")-1);
		uart_send(buf1, len);//uart_send(buf1, sizeof(buf1));
		uart_send("\r\n", sizeof( "\r\n")-1);
		msleep(500);
	}
}
