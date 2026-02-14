#include <reg51.h>
#include <string.h>
#include <stdio.h>
#define lcdport P1

sbit rs = P1 ^ 0;
sbit rw = P1 ^ 1;
sbit en = P1 ^ 2;

char i, rx_data[50];
char rfid[12], ch = 0;
char pass[4];

void delay(int m)
{
   int i, j;
   for (i = 0; i < m; i++)
   {
      for (j = 0; j < 1275; j++)
      {
      }
   }
}

void dataen()
{
   rs = 1;
   rw = 0;
   en = 1;
   delay(10);
   en = 0;
}

void lcddata(unsigned char ch)
{
   lcdport = ch & 0xf0;
   dataen();
   lcdport = (ch << 4) & 0xf0;
   dataen();
}

void command()

{
   rs = 0;
   rw = 0;
   en = 1;
   delay(10);
   en = 0;
}

void lcdcmd(unsigned char ch)
{
   lcdport = ch & 0xf0;
   command();
   lcdport = (ch << 4) & 0xf0;
   command();
}

void lcdstring(char *str)
{
   while (*str)
   {
      lcddata(*str);
      str++;
   }
}

void lcd_init(void)

{
   delay(10);
   lcdcmd(0x02);    // Back to first
   delay(10);
   lcdcmd(0x28);    // 4-bit mode
   delay(10);
   lcdcmd(0x0e);    // display/cursor ON
   delay(10);
   lcdcmd(0x01);    // Clear screen
   delay(10);
}

void uart()
{
   TMOD = 0x20;
   SCON = 0x50;
   TH1 = 0xfd;    //baud rate: 9600
   TR1 = 1;
}

char rxdata()
{
   while (!RI);
   ch = SBUF;
   RI = 0;
   return ch;
}

void accept()
{
   lcdcmd(1);
   lcdstring("Welcome");
   lcdcmd(192);   // to second line
   delay(200);
}

void wrong()
{
   lcdcmd(1);
   lcdstring("Wrong Card");
   lcdcmd(192);
   lcdstring("Access Denied");
   delay(1000);
}

void main()

{
   uart();
   lcd_init();
   lcdstring("  RFID Based    ");
   lcdcmd(0xc0);
   lcdstring("Security system ");
   delay(400);

   while (1)
   {
      lcdcmd(1);
      lcdstring("Place Your Card:");
      lcdcmd(0xc0);
      
      i = 0;
      for (i = 0; i < 12; i++)
      {

         rfid[i] = rxdata();
      }
      rfid[i] = '\0';

      lcdcmd(1);
      lcdstring("Your ID No. is:");
      lcdcmd(0xc0);

      for (i = 0; i < 12; i++)
      {
         lcddata(rfid[i]);
      }

      delay(100);
      if (strncmp(rfid, "E2700213A122", 12) == 0)
      {
         accept();
         lcdstring("Access Granted");
         delay(300);
      }
      else
         wrong();
   }
}