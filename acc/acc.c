// PROGRAM	: 2-4.c								2014.0101
// FUNCTION	: STM PWM Servo Control with 		By Steven
#include <HT66F70A.h>
#define	 SW_DutyUpPU	_pepu6		//PEPU.6
#define	 SW_DutyUp		_pe6		//PE.6
#define	 SW_DutyUpC		_pec6		//PEC.6
#define	 SW_DutyDnPU	_pepu7		//PEPU.7
#define	 SW_DutyDn		_pe7		//PE.7
#define	 SW_DutyDnC		_pec7		//PEC.7
typedef	unsigned char	u8;
typedef	char			s8;
typedef	unsigned short	u16;
typedef	short			s16;
typedef	unsigned long	u32;
typedef	long			s32;

void Delay100us(u16);							//函式原型宣告		 
void main()
{	s16 temp,Index=0;
	_wdtc=0b10101111;							//關閉看們狗計時器
	SW_DutyUpC=1; SW_DutyUpPU=1;			   	//規劃按鍵腳位為輸入屬性並啟用提升電阻
	SW_DutyDnC=1; SW_DutyDnPU=1;		   	
	_psc0=0x01; _tbc0=0x85;_tb0e=1;				//fTB=fSYS/4,週期=8192*fTB,致能TB0中斷	
	_pcs1=0x10; _pcs2=0x01;						//設定PC3/PC4為TP2功能
	_tm2c0=0b00000000;							//fINT=fSYS(8MHz)/4 (0.5us)
	_tm2c1=0b10101000;							//PWM/Active High/TM2R控制週期
	_tm2rp=160;									//週期=256*160*0.5us=20.48ms
	_t2on=1; _emi=1;							//起始TM1計數並致能EMI
	while(1)
	{	temp=Index*20+1000;						//轉換為佔空比參數
		_tm2al=temp%256;						//設定佔空比參數
		_tm2ah=temp/256;					
		temp>>=1;								//轉換為Ton時間
		Delay100us(12);						//延遲120ms
		while(1)
		{	if(!SW_DutyUp)
			{	Index=200;		//若按下遞增鍵,指標遞增(上限200) 
				break;   	
			}
			if(!SW_DutyDn) 						//若按下遞減鍵,指標遞減(下限0) 
			{	Index=0;
				break;   	
			}
		}
	}
}
void Delay100us(u16 del)						//延遲del*200指令週期
{	u16 i,j;									//@fSYS=8MH,延遲del*100us
	for(i=0;i<del;i++)
		for(j=0;j<=25;j++) GCC_NOP();
}