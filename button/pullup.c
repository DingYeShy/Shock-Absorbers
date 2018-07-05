#include <HT66F70A.h>
#define		SEG_Port	_pc			//PC
#define		SEG_PortC	_pcc		//PCC
#define		SW0		   	_pe0		//PE0
#define		SW0C		_pec0		//PEC0
#define		SW0PU		_pepu0		//PEPU0
typedef	unsigned char	u8;
typedef	char			s8;
typedef	unsigned short	u16;
typedef	short			s16;
typedef	unsigned long	u32;
typedef	long			s32;
const u8 SEG_TAB[] = { 							//七段顯示器顯示碼(共陰)
				0x3F,0x06,0x5B,0x4F,0x66,
				0x6D,0x7D,0x07,0x7F,0x67};
void Delay100us(u16);							//函式原型宣告
void main()
{	u8 i=0;
	_wdtc=0b10101111;							//關閉看們狗計時器
	SEG_PortC=0x00;								//規劃SEG_Port為輸出模式
	SW0C=1; SW0PU=1;                      		//規劃SW0接腳為輸入並啟用提升電阻
	while(1)
	{	SEG_Port=SEG_TAB[i++];					//查表
		wait:
		while(SW0);						        //等待按下按鍵
		Delay100us(1200);						//延遲 120 mSec
		if(SW0) goto wait; 		            	//判斷彈跳是否結束
		if(i>9) i=0;							//若已顯示至9則指標i歸零
	}
}
void Delay100us(u16 del)						//延遲del*200指令週期
{	u16 i,j;									//@fSYS=8MH,延遲del*100us
	for(i=0;i<del;i++)
		for(j=0;j<=25;j++) GCC_NOP();
}