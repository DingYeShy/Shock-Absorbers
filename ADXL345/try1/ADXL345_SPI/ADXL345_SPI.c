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

#define PAS2 _pas2	//pas 5 SDO
#define PAS3 _pas3	//pas 6 7 SDI SCL
#define PBS2 _pbs2	//pbs5 scs

void Delay100us(short);	
void _SPI_CS(unsigned short);
void _SPI_SCL(unsigned short);
unsigned char ADXL345_SPI_Read(unsigned char);
void delay_ms(unsigned char);
void ADXL345_SPI_Write(unsigned char , unsigned char);
void LCD_Cmd(unsigned char data);
void LCD_Senddata(unsigned char data);
void LCD_Reset(void);
void PinSet(void);
void DataFormat(void);
void PrintLCD(unsigned char);
unsigned char DATAX0 = 0x32;
unsigned char DATAX1 = 0x33;
unsigned char DATAY0 = 0x34;
unsigned char DATAY1 = 0x35;
unsigned char DATAZ0 = 0x36;
unsigned char DATAZ1 = 0x37;
char i;
double x[10];
double y[10];
double z[10];
int RawDataX[2],RawDataY[2],RawDataZ[2];
int* ptrRawData[] =
{&RawDataX[0],&RawDataX[1],&RawDataY[0],&RawDataY[1],&RawDataZ[0],&RawDataZ[1]};
int AccX,AccY,AccZ;
int* ptrAcc[] = {&AccX,&AccY,&AccZ};
char deviceID;

void main()
{
	PinSet();	//腳位設定
	delay_ms(2);
	ADXL345_SPI_Write(0x31, 0x0B); //設定測量範圍&資料格式
	ADXL345_SPI_Write(0x2D, 0x08); //開始測量
	ADXL345_SPI_Write(0x2E, 0x80);
	ADXL345_SPI_Write(0x2c, 0x0F);
	
	LCD_Reset();		//LCD重置
	delay_ms(250);		//延遲20毫秒 				
	LCD_Cmd(0xc0);  	//LCD第一行顯示
	delay_ms(250);   
	

	while(1)
	{/*
		delay_ms(250);
		for(i=5;i>=0;i--)
		{
			*ptrRawData[i] = (int)ADXL345_SPI_Read(i+50);
		}
		for(i=0;i<3;i++)
		{
			*ptrAcc[i] = ((*ptrRawData[1+i*2]<<8)&0xFF00) | (*ptrRawData[0+i*2]&0xFF);
			*ptrAcc[i] = *ptrAcc[i] * 4;
		}*/
		/*
		Delay100us(100);
		LCD_Cmd(0x80);  //從第一行第0位置開始顯示   C0為第二行
		delay_ms(150);
	 	PrintLCD(*ptrAcc[0]);
	  	delay_ms(50);
	  	
	  	Delay100us(100);
	  	LCD_Cmd(0x86);	//從第一行第5位置開始顯示
	 	delay_ms(150);
	  	PrintLCD(*ptrAcc[1]);
		delay_ms(50);
		
		Delay100us(100);
		LCD_Cmd(0x8d);	//從第一行第5位置開始顯示
	 	delay_ms(150);
	  	PrintLCD(*ptrAcc[2]);
		delay_ms(50);
		*/
		delay_ms(255);
		deviceID = ADXL345_SPI_Read(0x00);
		Delay100us(100);
		LCD_Cmd(0xc0);	//從第一行第5位置開始顯示
	 	delay_ms(255);
	  	PrintLCD(deviceID);
		delay_ms(50);
	}
}
//////////////////////////////
///腳位設定涵式
//////////////////////////////
void PinSet(void)
{
	_wdtc = 0xa8;//關閉看門狗
	_cp0c = 0x00;	//類比比較器功能關閉
	_cp1c = 0x00;
	_scomen = 0;
	_smod = 0xe0;//4MHz
	_sdis0=0;
	_sdis1=0;
	_simen=1;  
	_simc0=0x42;
	_simc2=0x24;
	
	//PAS2=0x10;  //設定各個腳位功能
	//PAS3=0x22;  
	//PBS2=0x10;

	
	_ifs4=0;
	
	_pac5=0; 	_pac6=1; 	_pac7=0;		CSC=0;//設定I/O
	_phc0 = 0; _phc1 = 0; _phc2 = 0; //設置PH0、PH1、PH2為輸出   
	SDIPU=1;	SDOPU=1;	SCKPU=1;	CSPU=1;		SCK=0;		CS=1;		_pgc = 0x00;   
}


//////////////////////////////
///SPI讀取涵式
//////////////////////////////
unsigned char ADXL345_SPI_Read(unsigned char Address)
{
  unsigned char ReadData=0;
  unsigned char tempSDO;
  
  short i;
 
  _SPI_CS(0);
  
  for(i = 7; i >= 0; i-- )
  {
    // F-Edge
    _SPI_SCL(1);
    SDO = 0x1 & ((0x80 | Address) >> i);
    _SPI_SCL(0);
  }
 
  //===========================
  _SPI_SCL(1);
  //===========================
 
  for(i = 7; i >= 0; i-- )
  {
    // R-Edge
    _SPI_SCL(0);
    _SPI_SCL(1);
    tempSDO = SDI; // Read bit
 
    ReadData |= tempSDO << i;
  }
 
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
{	short i,j;									//@fSYS=8MH,延遲del*100us
	for(i=0;i<del;i++)
		for(j=0;j<=25;j++) GCC_NOP();
}	

//////////////////////////////
///CS腳電位涵式
//////////////////////////////
void _SPI_CS(unsigned short isSelect)
{
    CS=isSelect;
    _nop();
}

//////////////////////////////
///SCL腳電位涵式
//////////////////////////////
void _SPI_SCL(unsigned short bLevel)
{
    SCK=bLevel;
    _nop();
}

//////////////////////////////
///SPI寫入涵式
//////////////////////////////
void ADXL345_SPI_Write(unsigned char Address, unsigned char WriteData)
{
  char i;
 
  _SPI_CS(0);
 
  for(i=7; i >= 0; i--)
  {
    // F-Edge
    _SPI_SCL(1);
    SDO = 0x1 & ((0x7F & Address) >> i);
    _SPI_SCL(0);
  }
 
  for(i=7; i >= 0; i--)
  {
    // F-Edge
    _SPI_SCL(1);
    SDO = 0x1 & ((WriteData) >> i);
    _SPI_SCL(0);
  }
 
  _SPI_CS(1);
}

//---傳送命令到LCD
void LCD_Cmd(unsigned char data)
{
    DATA_BUS = data;  //命令送到BUS
    RS=0; RW=0; EN=1; //命令到LCD內 
    delay_ms(1);
    EN=0;           //禁能LCD
}
//---傳送資料到LCD 
void LCD_Senddata(unsigned char data)
{
    DATA_BUS = data;  //資料送到BUS
    RS=1; RW=0; EN=1; //資料到LCD內
    delay_ms(1);
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

//////////////////////////////
///Delay毫秒涵式
//////////////////////////////
void delay_ms(unsigned char del)
{	
	unsigned char i;
	for(i=0;i<del;i++) GCC_DELAY(2000);		//內建函式，延遲2000指令週期
}
