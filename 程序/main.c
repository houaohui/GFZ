#include <stc12c5a60s2.h>
sbit k1=P1^5;
unsigned int index=0;
unsigned char blockbuff[16]={
	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00
};

unsigned char sw=0,g=4;
unsigned char blockcode1[]={0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x00};
unsigned char blockcode2[]={0x00,0x01,0x03,0x06,0x0c,0x18,0x30,0x60,0xc0,0x80,0x00};
unsigned char blockcode3[]={0x00,0x01,0x03,0x07,0x0e,0x1c,0x38,0x70,0xe0,0xc0,0x80,0x00};
unsigned char blockcode4[]={0x00,0x01,0x03,0x07,0x0f,0x1e,0x3c,0x78,0xf0,0xe0,0xc0,0x80,0x00};
unsigned char i=0;
unsigned char speed=200;
bit en=0,rl=0,clear=0,keysta=1;//向右

void Timer0Init();
void changeblock();
void swingblock();
void keyaction();
void blockscan();
void keyscan();
void keydriver();


void main()
{
	unsigned char j=0;
	EA=1;
	Timer0Init();
	
	while(1)
{
	if(clear)
	{
		clear=0;
		speed=200;
		i=0;
		sw=0;
		g=4;
		rl=0;
		en=0;
		for(j=0;j<16;j++)
		{
			blockbuff[j]=0x00;
		}
	}
	keydriver();
}
	
}

void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;
}

void T0inter() interrupt 1
{
	TL0 = 0x66;
	TH0 = 0xFC;
	blockscan();
	keyscan();
	index++;
	if(index==speed)
	{
		index=0;
		swingblock();
	}
}


void changeblock()
{
	switch(blockbuff[i-1])//看是什么形状
	{
		case 0x0f:sw=4;g=4;break;
		case 0x1e:sw=5;g=4;break;
		case 0x3c:sw=6;g=4;break;
		case 0x71:sw=7;g=4;break;
		case 0xf0:sw=8;g=4;break;
		
		case 0x07:sw=3;g=3;break;
		case 0x0e:sw=4;g=3;break;
		case 0x1c:sw=5;g=3;break;
		case 0x38:sw=6;g=3;break;
		case 0x70:sw=7;g=3;break;
		case 0xe0:sw=8;g=3;break;
		
		case 0x03:sw=2;g=2;break;
		case 0x06:sw=3;g=2;break;
		case 0x0c:sw=4;g=2;break;
		case 0x18:sw=5;g=2;break;
		case 0x30:sw=6;g=2;break;
		case 0x60:sw=7;g=2;break;
		case 0xc0:sw=8;g=2;break;
		
		case 0x01:sw=1;g=1;break;
		case 0x02:sw=2;g=1;break;
		case 0x04:sw=3;g=1;break;
		case 0x08:sw=4;g=1;break;
		case 0x10:sw=5;g=1;break;
		case 0x20:sw=6;g=1;break;
		case 0x40:sw=7;g=1;break;
		case 0x80:sw=8;g=1;break;
	}
	
}

//
void swingblock()
{
	if(en)
	{
		en=0;
		changeblock();
	}
	switch(g)
	{
		case 4:blockbuff[i]=blockcode4[sw];break;
		case 3:blockbuff[i]=blockcode3[sw];break;
		case 2:blockbuff[i]=blockcode2[sw];break;
		case 1:blockbuff[i]=blockcode1[sw];break;
	}
	if(blockbuff[i]==0) 
	{
		rl=~rl;
	}
	if(rl)
	{
		sw++;
	}
	else
	{
		sw--;
	}
}

//
void keyaction()
{
	speed-=10;
	if(i==0)
	{
		blockbuff[0]=blockcode4[sw];
		i++;
	}
	else
	{
		if(rl==1)
		{
			switch(g)
			{
				case 4:blockbuff[i]=blockcode4[sw-1]&blockbuff[i-1];break;
				case 3:blockbuff[i]=blockcode3[sw-1]&blockbuff[i-1];break;
				case 2:blockbuff[i]=blockcode2[sw-1]&blockbuff[i-1];break;
				case 1:blockbuff[i]=blockcode1[sw-1]&blockbuff[i-1];break;
			}
		}
		else
		{
			switch(g)
			{
				case 4:blockbuff[i]=blockcode4[sw+1]&blockbuff[i-1];break;
				case 3:blockbuff[i]=blockcode3[sw+1]&blockbuff[i-1];break;
				case 2:blockbuff[i]=blockcode2[sw+1]&blockbuff[i-1];break;
				case 1:blockbuff[i]=blockcode1[sw+1]&blockbuff[i-1];break;
			}
		}
		i++;
		if(blockbuff[i-1]==0)
		{
			clear=1;
		}
		else
		{
			en=1;
		}
	}
	
}
	
void blockscan()
{
	static unsigned char i=0;
	P2=i;
	P0=~blockbuff[i];
	i++;
	if(i==17)i=0;
}
	
void keyscan()
{
	static unsigned char keybuff=0xff;
	keybuff=(keybuff<<1)|k1;
	if((keybuff&0x0f)==0x00)
	{
		keysta=0;
	}
	else
	{
		keysta=1;
	}
}

void keydriver()
{
	static bit backup=1;
	if(backup!=keysta)
	{
		if(backup==1	)
		{
			keyaction();
		}
		backup=keysta;
	}
}






