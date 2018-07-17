#include <HT66F70A.h>

#define	DATA_BUS _pg
#define RS  _ph0
#define RW  _ph1
#define EN  _ph2

#define SDO _pa5	//�H���s�����
#define SDI _pa6	//�H���s�����
#define SCK _pa7
#define CS  _pb5

#define SDOC _pac5	//�H���s�����
#define SDIC _pac6
#define SCKC _pac7
#define CSC  _pbc5

#define SDOPU _papu5	//���s�����
#define SDIPU _papu6
#define SCKPU _papu7
#define CSPU  _pbpu5

#define PAS2 _pas2	//pas 5 SDO
#define PAS3 _pas3	//pas 6 7 SDI SCL
#define PBS2 _pbs2	//pbs5 scs

void Delay100us(short);	
void _SPI_CS(unsigned short);
void _SPI_SCL(unsigned short);
char ADXL345_SPI_Read(char);
void ADXL345_SPI_Write(char ,char);
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
char deviceID;
short a,b,i;
char DATAX0 = 0x32;
char DATAX1 = 0x33;
char DATAY0 = 0x34;
char DATAY1 = 0x35;
char DATAZ0 = 0x36;
char DATAZ1 = 0x37;

void main()
{
	PinSet();	//�}��]�w
	Delay100us(200);
	ADXL345_SPI_Write(0x31,0x0B); //�]�w���q�d��&��Ʈ榡
	ADXL345_SPI_Write(0x2D,0x08); //�}�l���q
	//ADXL345_SPI_Write(0x2c, 0x0F);
	
	LCD_Reset();		//LCD���m			
	LCD_Cmd(0xc0);  	//LCD�Ĥ@�����
  
	while(1)
	{
		for(i=5;i>=0;i--)
		{
			*ptrRawData[i] = ADXL345_SPI_Read(i+50);
		}
		/*for(i=0;i<3;i++)
		{
			*ptrAcc[i] = ((*ptrRawData[1+i*2]<<8)&0xFF00) | (*ptrRawData[0+i*2]&0xFF);
			*ptrAcc[i] = *ptrAcc[i] * 4;
		}*/
		/*AccX = ((RawDataX[1]<<8)&0xFF00)|(RawDataX[0]&0xFF);
		AccX = AccX*4;
		AccY = ((RawDataY[1]<<8)&0xFF00)|(RawDataY[0]&0xFF);
		AccY = AccY*4;
		AccZ = ((RawDataZ[1]<<8)&0xFF00)|(RawDataZ[0]&0xFF);
		AccZ = AccZ*4;*/
		
		/*AccX = (((int)ADXL345_SPI_Read(DATAX1)&0xFF00)) | ((int)ADXL345_SPI_Read(DATAX0)&0xFF);
		AccY = (((int)ADXL345_SPI_Read(DATAY1)&0xFF00)) | ((int)ADXL345_SPI_Read(DATAY0)&0xFF);
		AccZ = (((int)ADXL345_SPI_Read(DATAZ1)&0xFF00)) | ((int)ADXL345_SPI_Read(DATAZ0)&0xFF);*/
		
		/*AccX = ((int)ADXL345_SPI_Read(DATAX1)<<8)|(int)ADXL345_SPI_Read(DATAX0);
		AccY = ((int)ADXL345_SPI_Read(DATAY1)<<8)|(int)ADXL345_SPI_Read(DATAY0);
		AccZ = ((int)ADXL345_SPI_Read(DATAZ1)<<8)|(int)ADXL345_SPI_Read(DATAZ0);*/
		
		LCD_Cmd(0x80);  //�q�Ĥ@���0��m�}�l���   C0���ĤG��
	 	PrintLCD(AccX);
	  	
	  	LCD_Cmd(0x86);	//�q�Ĥ@���5��m�}�l���
	  	PrintLCD(AccY);
		
		LCD_Cmd(0x8d);	//�q�Ĥ@���5��m�}�l���
	  	PrintLCD(AccZ);

		deviceID = ADXL345_SPI_Read(0x44);
		LCD_Cmd(0xc0);	//�q�ĤG���0��m�}�l���
	  	PrintLCD(deviceID);
	}
}
//////////////////////////////
///�}��]�w�[��
//////////////////////////////
void PinSet(void)
{
	_wdtc = 0b10101111;//�����ݪ���
	_cp0c = 0x00;	//���������\������
	_cp1c = 0x00;
	_scomen = 0;
	_smod = 0xe0;//4MHz
	/*_sdis0=0;
	_sdis1=0;
	_simen=1;  
	_simc0=0x42;
	_simc2=0x24;*/
	
	//PAS2=0x10;  //�]�w�U�Ӹ}��\��
	//PAS3=0x22;  
	//PBS2=0x10;

	
	_ifs4=0;
	
	_pac5=0; 	_pac6=1; 	_pac7=0;	CSC=0;//�]�wI/O
	_phc0 =0;   _phc1=0;    _phc2=0; //�]�mPH0�BPH1�BPH2����X   
	SCK=0;		CS=1;		_pgc = 0x00;   
}


//////////////////////////////
///SPIŪ���[��
//////////////////////////////
char ADXL345_SPI_Read(char Address)
{
  char ReadData=0;
  char tempSDO;
  _SPI_CS(0);
  a=7;
  do{
    // F-Edge
    /*LCD_Cmd(0x80);
    PrintLCD(a);*/
    _SPI_SCL(1);
    SDO = 0x1 & ((0x80 | Address) >> a);
    _SPI_SCL(0);
    a--;
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
    a--;
  }while(a>=0);
 
  _SPI_CS(1);
  return ReadData & 0xFF;
}

//////////////////////////////
///LCD��ܲ[��
//////////////////////////////
void PrintLCD(unsigned char data)
{
	LCD_Senddata((data/100)%10+0x30);
	LCD_Senddata((data/10)%10+0x30);
	LCD_Senddata((data/1)%10+0x30);
}

//////////////////////////////
///Delay100us�[��
//////////////////////////////
void Delay100us(short del)						//����del*200���O�g��
{	unsigned short k,l;									//@fSYS=8MH,����del*100us
	for(k=0;k<del;k++)
		for(l=0;l<=25;l++)GCC_NOP();
}	

//////////////////////////////
///CS�}�q��[��
//////////////////////////////
void _SPI_CS(unsigned short isSelect)
{
    CS=isSelect;
    _nop();
}

//////////////////////////////
///SCL�}�q��[��
//////////////////////////////
void _SPI_SCL(unsigned short bLevel)
{
    SCK=bLevel;
    _nop();
}

//////////////////////////////
///SPI�g�J�[��
//////////////////////////////
void ADXL345_SPI_Write(char Address, char WriteData)
{
  _SPI_CS(0);
  for(b=7;b>=0;b--)
  {
    // F-Edge
    _SPI_SCL(1);
    SDI = 0x1 & ((0x7F & Address) >> b);
    _SPI_SCL(0);
  }
  
  for(b=7;b>=0;b--)
  {
    // F-Edge
    _SPI_SCL(1);
    SDI = 0x1 & ((WriteData) >> b);
    _SPI_SCL(0);
  }

  _SPI_CS(1);
}

//---�ǰe�R�O��LCD
void LCD_Cmd(unsigned char data)
{
    DATA_BUS = data;  //�R�O�e��BUS
    RS=0; RW=0; EN=1; //�R�O��LCD�� 
    Delay100us(20);
    EN=0;           //�T��LCD
}
//---�ǰe��ƨ�LCD 
void LCD_Senddata(unsigned char data)
{
    DATA_BUS = data;  //��ưe��BUS
    RS=1; RW=0; EN=1; //��ƨ�LCD��
	Delay100us(20);
    EN=0;           //�T��LCD
}
//---LCD���ҩl�{��  
void LCD_Reset(void)
{
    LCD_Cmd(0b00111000);//8bit�ǿ�,���2��,5*7�r��
                        //bit4:DL=1,8bit�ǿ�,
                        //bit3:N=1,���2��
                        //bit2:F=0,5*7�r��                        
    LCD_Cmd(0b00001100);//��ܹ�ON,����ܴ��,��Ф��{�{
                        //bit2:D=1,��ܹ�ON
                        //bit1:C=0,����ܴ��                      
    LCD_Cmd(0b00000110);//��ܧ���Хk��,��в���T�� 
                        //bit1:I/D=1,��ܧ���Хk��
                        //bit0:S=0,��в���T��                                   
    LCD_Cmd(0b00000001); //�M����ܹ�
    LCD_Cmd(0b00000010); //��Ц^��� 
}