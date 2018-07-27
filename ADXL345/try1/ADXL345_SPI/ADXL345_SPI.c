#include <HT66F70A.h>

#define	DATA_BUS _pg
#define RS  _ph0
#define RW  _ph1
#define EN  _ph2

#define SDO _pa5	//以盛群為基準
#define SDA _pa6	//以盛群為基準
#define SCK _pa7
#define CS  _pb5

#define SDOC _pac5	//以盛群為基準
#define SDAC _pac6
#define SCKC _pac7
#define CSC  _pbc5

#define SDOPU _papu5	//盛群為基準
#define SDAPU _papu6
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
void ADXL345_SPI_Write(unsigned char,unsigned char);
void LCD_Cmd(unsigned char data);
void LCD_Senddata(unsigned char data);
void LCD_Reset(void);
void PinSet(void);
void DataFormat(void);
void PrintLCD(unsigned char);
volatile unsigned char datax;
int RawDataX[2],RawDataY[2],RawDataZ[2];
int* ptrRawData[] = {&RawDataX[0],&RawDataX[1],&RawDataY[0],&RawDataY[1],&RawDataZ[0],&RawDataZ[1]};
int AccX,AccY,AccZ;
int* ptrAcc[] = {&AccX,&AccY,&AccZ};
unsigned short j;

void main()
{
	PinSet();	//腳位設定
	delay_ms(2);
	ADXL345_SPI_Write(0x31,0x0B); //設定測量範圍&資料格式
	ADXL345_SPI_Write(0x2D,0x08); //開始測量
	//ADXL345_SPI_Write(0x2E,0x80);
	
	LCD_Reset();		//LCD重置
	delay_ms(250);		//延遲20毫秒
	LCD_Cmd(0xc0);  	//LCD第一行顯示
	delay_ms(250);            
	
	while(1)
	{
		
		AccX = ((ADXL345_SPI_Read(0x32)<<8)&0xFF00) | (ADXL345_SPI_Read(0x33)&0xFF);
		AccX = AccX*4;
		AccY = ((ADXL345_SPI_Read(0x34)<<8)&0xFF00) | (ADXL345_SPI_Read(0x35)&0xFF);
		AccY = AccY*4;
		AccZ = ((ADXL345_SPI_Read(0x36)<<8)&0xFF00) | (ADXL345_SPI_Read(0x37)&0xFF);
		AccZ = AccZ*4;
		//datax = ADXL345_SPI_Read(0x32);
		LCD_Cmd(0xC0);
		PrintLCD(AccX);
		LCD_Cmd(0xC6);
		PrintLCD(AccY);
		LCD_Cmd(0xCD);
		PrintLCD(AccZ);
		_wdtc = 0xa8;
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
	//_smod = 0xe0;//4MHz
	_sdis0=0;
	_sdis1=0;
	_simc0=0x00;
	_simc2=0x0C;
	_simen=1;  
	
	_adoff=1;
	PAS2=0x10;  //設定各個腳位功能
	PAS3=0x22;
	PBS2=0x10;

	
	_ifs4=0;
	
	SDOC=0; 	SDAC=1; 	SCKC=0;		CSC=0;		//設定I/O
	_phc0 = 0; 	_phc1 = 0;	_phc2 = 0; 	_pgc = 0x00;   //設置PH0、PH1、PH2為輸出   
	SDAPU=1;	SDOPU=1;	SCKPU=1;	CSPU=1;				
	SCK=0;		CS=1;		//數據初始
}


//////////////////////////////
///SPI讀取涵式
//////////////////////////////
unsigned char ADXL345_SPI_Read(unsigned char Address)
{
	unsigned char TempSend=0;
	unsigned char TempRead =0;
	do
	{
		_wcol=0;
		TempSend = Address|0x80;
		_simd = TempSend;
	}while(_wcol == 1);
	while(_trf == 0)
	{
		_wdtc = 0xaF;
	}
	_trf = 0;
	TempRead = _simd;
	return TempRead;
}

//////////////////////////////
///LCD顯示涵式
//////////////////////////////
void PrintLCD(unsigned char data)
{
	LCD_Senddata((data/1000)%10+0x30); 
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
}

//////////////////////////////
///SCL腳電位涵式
//////////////////////////////
void _SPI_SCL(unsigned short bLevel)
{
    SCK=bLevel;
}

//////////////////////////////
///SPI寫入涵式
//////////////////////////////
void ADXL345_SPI_Write(unsigned char Address, unsigned char WriteData)
{
	unsigned char AddressSend;
	unsigned char WriteDataSend;
	
	do
	{
		_wcol=0;
		AddressSend = Address & 0x7F;
		_simd = AddressSend;
	}while(_wcol == 1);
	while(_trf == 0)
	{
		_wdtc = 0xa8;
	}
	_trf = 0;

	do
	{
		_wcol=0;
  		WriteDataSend = WriteData;
  		_simd = WriteDataSend;
	}while(_wcol == 1);
	while(_trf == 0)
	{
  		_wdtc = 0xa8;
	}
	_trf = 0;
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
