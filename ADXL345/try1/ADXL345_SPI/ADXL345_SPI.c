#include <HT66F70A.h>

#define	DATA_BUS _pg
#define RS  _ph0
#define RW  _ph1
#define EN  _ph2

#define SDO _pa5	//以盛群為基準
#define SDI _pa6	//以盛群為基準
#define SCK _pa7
#define CS  _pb5

#define SDOC _pac5	//以盛群為基準
#define SDIC _pac6
#define SCKC _pac7
#define CSC  _pbc5

#define SDOPU _papu5	//盛群為基準
#define SDIPU _papu6
#define SCKPU _papu7
#define CSPU  _pbpu5

void Delay100us(short);	
void _SPI_CS(unsigned short);
void _SPI_SCL(unsigned short);
unsigned char ADXL345_SPI_Read(unsigned char);
void ADXL345_SPI_Write(unsigned char,unsigned char);
void LCD_Cmd(unsigned char data);
void LCD_Senddata(unsigned char data);
void LCD_Reset(void);
void PinSet(void);
void PrintLCD(unsigned char);

int RawDataX[2],RawDataY[2],RawDataZ[2];
int* ptrRawData[] =
{&RawDataX[0],&RawDataX[1],&RawDataY[0],&RawDataY[1],&RawDataZ[0],&RawDataZ[1]};
int AccX,AccY,AccZ;
int* ptrAcc[] = {&AccX,&AccY,&AccZ};
unsigned int deviceID;
short a,b,i;
unsigned int DATAX0;
unsigned int DATAX1;
unsigned int DATAY0;
unsigned int DATAY1;
unsigned int DATAZ0;
unsigned int DATAZ1;

void main()
{
	PinSet();	//腳位設定
	Delay100us(20);
	ADXL345_SPI_Write(0x31,0x0B); //設定測量範圍&資料格式
	ADXL345_SPI_Write(0x2D,0x08); //開始測量
	//ADXL345_SPI_Write(0x2c,0x0F);
	
	LCD_Reset();		//LCD重置			
  
	while(1)
	{
		/*for(i=5;i>=0;i--)
		{
			*ptrRawData[i] = (int)ADXL345_SPI_Read(i+50);
		}
		for(i=0;i<3;i++)
		{
			*ptrAcc[i] = ((*ptrRawData[1+i*2]<<8)&0xFF00) | (*ptrRawData[0+i*2]&0xFF);
			*ptrAcc[i] = *ptrAcc[i] * 4;
		}
		AccX = ((RawDataX[1]<<8)&0xFF00)|(RawDataX[0]&0xFF);
		AccX = AccX*4;
		AccY = ((RawDataY[1]<<8)&0xFF00)|(RawDataY[0]&0xFF);
		AccY = AccY*4;
		AccZ = ((RawDataZ[1]<<8)&0xFF00)|(RawDataZ[0]&0xFF);
		AccZ = AccZ*4;*/
		
		AccX = (ADXL345_SPI_Read(0x33)&0xFF00) | (ADXL345_SPI_Read(0x32)&0xFF);
		AccX = AccX * 4;
		AccY = (ADXL345_SPI_Read(0x35)&0xFF00) | (ADXL345_SPI_Read(0x34)&0xFF);
		AccY = AccY * 4;
		AccZ = (ADXL345_SPI_Read(0x37)&0xFF00) | (ADXL345_SPI_Read(0x36)&0xFF);
		AccZ = AccZ * 4;
		
		/*AccX = ((int)ADXL345_SPI_Read(DATAX1)<<8)|(int)ADXL345_SPI_Read(DATAX0);
		AccY = ((int)ADXL345_SPI_Read(DATAY1)<<8)|(int)ADXL345_SPI_Read(DATAY0);
		AccZ = ((int)ADXL345_SPI_Read(DATAZ1)<<8)|(int)ADXL345_SPI_Read(DATAZ0);*/
		/*Delay100us(200);
		LCD_Cmd(0x80);  //從第一行第0位置開始顯示   C0為第二行
	 	PrintLCD(AccX);
	  	Delay100us(200);
	  	LCD_Cmd(0x86);	//從第一行第5位置開始顯示
	  	PrintLCD(AccY);
		Delay100us(200);
		LCD_Cmd(0x8d);	//從第一行第5位置開始顯示
	  	PrintLCD(AccZ);*/

		deviceID = (unsigned int)ADXL345_SPI_Read(0x00);
		LCD_Cmd(0xc0);	//從第二行第0位置開始顯示
	  	PrintLCD(deviceID);
	  	
	  	/*DATAZ0 = (int)ADXL345_SPI_Read(0x36);
		LCD_Cmd(0xc6);	//從第二行第0位置開始顯示
	  	PrintLCD(DATAZ0);
	  	
	  	DATAZ1 = (int)ADXL345_SPI_Read(0x37);
		LCD_Cmd(0xcd);	//從第二行第0位置開始顯示
	  	PrintLCD(DATAZ1);*/
	  	
	  	Delay100us(100);
	}
}
//////////////////////////////
///腳位設定涵式
//////////////////////////////
void PinSet(void)
{
	_wdtc = 0b10101111;//關閉看門狗
	_cp0c = 0x00;	//類比比較器功能關閉
	_cp1c = 0x00;
	_scomen = 0;
	_smod = 0xe0;//4MHz
	
	SDOC=0; 	SDIC=1; 	SCKC=0;	    CSC=0;//設定I/O
	_phc0 =0;   _phc1=0;    _phc2=0;    //設置PH0、PH1、PH2為輸出 
	SCK=0;	CS=1;     //腳位初始狀態   
	_pgc = 0x00;   
}


//////////////////////////////
/////////SPI讀取涵式//////////
//////////////////////////////
unsigned char ADXL345_SPI_Read(unsigned char Address)
{
  unsigned char ReadData=0;
  unsigned char tempSDO;
  _SPI_CS(0);
  a=7;
  do{
    // F-Edge
    /*LCD_Cmd(0x80);
    PrintLCD(a);*/
    _SPI_SCL(1);
    SDO = 0x1 & ((0x80 | Address) >> a);
    _SPI_SCL(0);
    --a;
  }while(a!=0); 
  //===========================
  _SPI_SCL(1);
  //===========================
  a=7;
  do{
    // R-Edge
    _SPI_SCL(0);
    _SPI_SCL(1);
    tempSDO = SDI; // Read bit
    ReadData |= tempSDO << a;
  }while(a--!=0);
 
  _SPI_CS(1);
  return ReadData & 0xFF;
}

//////////////////////////////
///LCD顯示涵式
//////////////////////////////
void PrintLCD(unsigned char data)
{
	LCD_Senddata((data/100)%10+0x30);
	LCD_Senddata((data/10)%10+0x30);
	LCD_Senddata((data/1)%10+0x30);
}

//////////////////////////////
///Delay100us涵式
//////////////////////////////
void Delay100us(short del)						//延遲del*200指令週期
{	unsigned short k,l;							//@fSYS=8MH,延遲del*100us
	for(k=0;k<del;k++)
		for(l=0;l<=25;l++)GCC_NOP();
}	

//////////////////////////////
///CS腳電位涵式
//////////////////////////////
void _SPI_CS(unsigned short isSelect)
{
    CS=isSelect;
    GCC_NOP();
}

//////////////////////////////
///SCL腳電位涵式
//////////////////////////////
void _SPI_SCL(unsigned short bLevel)
{
    SCK=bLevel;
    GCC_NOP();
}

//////////////////////////////
///SPI寫入涵式
//////////////////////////////
void ADXL345_SPI_Write(unsigned char Address,unsigned char WriteData)
{
  _SPI_CS(0);
  b=7;
  do{
    // F-Edge
    _SPI_SCL(1);
    SDO = 0x1 & ((0x7F & Address) >> b);
    _SPI_SCL(0);
    --b;
  }while(b!=0);
  
  b=7;
  do{
    // F-Edge
    _SPI_SCL(1);
    SDO = 0x1 & ((WriteData) >> b);
    _SPI_SCL(0);
    --b;
  }while(b!=0);
  _SPI_CS(1);
}

//---傳送命令到LCD
void LCD_Cmd(unsigned char data)
{
    DATA_BUS = data;  //命令送到BUS
    RS=0; RW=0; EN=1; //命令到LCD內 
    Delay100us(20);
    EN=0;           //禁能LCD
}
//---傳送資料到LCD 
void LCD_Senddata(unsigned char data)
{
    DATA_BUS = data;  //資料送到BUS
    RS=1; RW=0; EN=1; //資料到LCD內
	Delay100us(20);
    EN=0;           //禁能LCD
}
//---LCD的啟始程式  
void LCD_Reset(void)
{
    LCD_Cmd(0b00111000);//8bit傳輸,顯示2行,5*7字型
                        //bit4:DL=1,8bit傳輸,
                        //bit3:N=1,顯示2行
                        //bit2:F=0,5*7字型                        
    LCD_Cmd(0b00001100);//顯示幕ON,不顯示游標,游標不閃爍
                        //bit2:D=1,顯示幕ON
                        //bit1:C=0,不顯示游標                      
    LCD_Cmd(0b00000110);//顯示完游標右移,游標移位禁能 
                        //bit1:I/D=1,顯示完游標右移
                        //bit0:S=0,游標移位禁能                                   
    LCD_Cmd(0b00000001); //清除顯示幕
    LCD_Cmd(0b00000010); //游標回原位 
}