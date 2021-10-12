#include "def_pinos.h"
#include "config.c"
#include <stdio.h>
#define CS P2_3
#define TECS_OPEN 21

unsigned int mem_adress = 0;
unsigned char dado = 0;
unsigned char aux = 0;
volatile unsigned char ntec = TECS_OPEN;


void delay_ms(unsigned int t) __reentrant{
    TMOD |= 0x01;
    TMOD &= ~0x02;
    for(;t>0;t--){
        TR0=0;
        TF0=0;
        TL0=0x58;
        TH0=0x9e;
        TR0=1;
        while(TF0==0);
    }
}

void putchar(unsigned char c){
	SBUF0 = c;
	while(TI0 == 0);
	TI0 = 0;
}

unsigned char le_RAM_SPI( unsigned int end ) {

	unsigned char end_L, end_H;
	
	end_L=end;
	end_H=end>>8;
	CS=0;
	SPI0DAT=0x03;
	while(!TXBMT);
	SPI0DAT=end_H;
	while(!TXBMT);
	SPI0DAT=end_L;
	while(!TXBMT);
	SPI0DAT=0x00;
	while(!TXBMT);
	SPIF=0; 
	while(!SPIF);
	SPIF=0;
	CS=1;
	return (SPI0DAT);
}

void esc_RAM_SPI(unsigned int end, unsigned char dado) {

	unsigned char end_L, end_H;

	end_L=end;
	end_H=end>>8;
	CS=0;
	SPI0DAT=0x02;
	while(!TXBMT);
	SPI0DAT=end_H;
	while(!TXBMT);
	SPI0DAT=end_L;
	while(!TXBMT);
	SPI0DAT=dado;
	while(!TXBMT);
	SPIF=0;
	while(!SPIF);
	SPIF=0;
	CS=1;
}

void int_serial(void) __interrupt 4{
if(RI0==1){
	switch(SBUF0){
			case '0':
				P3_0=0;
				delay_ms(5);
				P3_0=1;
				delay_ms(5);
				P3_0=0;
				delay_ms(150);
				P3_0=1;
				delay_ms(5);
				P3_0=0;
				delay_ms(5);
				P3_0=1;
				break;

			case '1':
				P3_1=0;
				delay_ms(5);
				P3_1=1;
				delay_ms(5);
				P3_1=0;
				delay_ms(150);
				P3_1=1;
				delay_ms(5);
				P3_1=0;
				delay_ms(5);
				P3_1=1;
				break;	
			case '2':
				P3_2=0;
				delay_ms(5);
				P3_2=1;
				delay_ms(5);
				P3_2=0;
				delay_ms(150);
				P3_2=1;
				delay_ms(5);
				P3_2=0;
				delay_ms(5);
				P3_2=1;
				break;
			case '3':
				P3_3=0;
				delay_ms(5);
				P3_3=1;
				delay_ms(5);
				P3_3=0;
				delay_ms(150);
				P3_3=1;
				delay_ms(5);
				P3_3=0;
				delay_ms(5);
				P3_3=1;
				break;
			case '4':
				P3_4=0;
				delay_ms(5);
				P3_4=1;
				delay_ms(5);
				P3_4=0;
				delay_ms(150);
				P3_4=1;
				delay_ms(5);
				P3_4=0;
				delay_ms(5);
				P3_4=1;
				break;
			case '5':
				P3_5=0;
				delay_ms(5);
				P3_5=1;
				delay_ms(5);
				P3_5=0;
				delay_ms(150);
				P3_5=1;
				delay_ms(5);
				P3_5=0;
				delay_ms(5);
				P3_5=1;
				break;
			case '6':
				P3_6=0;
				delay_ms(5);
				P3_6=1;
				delay_ms(5);
				P3_6=0;
				delay_ms(150);
				P3_6=1;
				delay_ms(5);
				P3_6=0;
				delay_ms(5);
				P3_6=1;
				break;
			case '7':
				P3_7=0;
				delay_ms(5);
				P3_7=1;
				delay_ms(5);
				P3_7=0;
				delay_ms(150);
				P3_7=1;
				delay_ms(5);
				P3_7=0;
				delay_ms(5);
				P3_7=1;
				break;
			case '8':
				P2_4=0;
				delay_ms(5);
				P2_4=1;
				delay_ms(5);
				P2_4=0;
				delay_ms(150);
				P2_4=1;
				delay_ms(5);
				P2_4=0;
				delay_ms(5);
				P2_4=1;
				break;
			case '9':
				P2_5=0;
				delay_ms(5);
				P2_5=1;
				delay_ms(5);
				P2_5=0;
				delay_ms(150);
				P2_5=1;
				delay_ms(5);
				P2_5=0;
				delay_ms(5);
				P2_5=1;
				break;
			case 'e':
				P2_6=0;
				delay_ms(5);
				P2_6=1;
				delay_ms(5);
				P2_6=0;
				delay_ms(150);
				P2_6=1;
				delay_ms(5);
				P2_6=0;
				delay_ms(5);
				P2_6=1;
				break;
			case 'c':
				P2_7=0;
				delay_ms(5);
				P2_7=1;
				delay_ms(5);
				P2_7=0;
				delay_ms(150);
				P2_7=1;
				delay_ms(5);
				P2_7=0;
				delay_ms(5);
				P2_7=1;
				break;
				
			default:
				break;
	}
	RI0=0;
	}
}

//'e' como 10 e 'c' como 11 
unsigned char le_tec(void){
	unsigned char vp2, vp3, i=0, j=8;

	vp2 = ~P2;
	vp3 = ~P3;
	if (vp3 == 0 & vp2 == 0){
		return TECS_OPEN;
	} 

	if (vp3 != 0) {
		while ((vp3 & 0x01) ==0) {
		vp3 = vp3 >> 1;
		i++;
		}
		return i;
	}
	
	else{
		vp2 = vp2 >> 4;
		while ((vp2 & 0x01) ==0) {
		vp2 = vp2 >> 1;
		j++;
		}
		return j;
	}

}

void ISR_ext_tc2(void) __interrupt 5{
	static unsigned char tecla_ant, tecla_atu;
	TF2 = 0;
	tecla_ant = tecla_atu;
	tecla_atu = le_tec();

	if (tecla_ant==TECS_OPEN && tecla_atu!=TECS_OPEN) {
		ntec = tecla_atu;
	}
}

void main(void) {
	
	Init_Device();
    SFRPAGE = LEGACY_PAGE;
    printf_fast_f("--- ESCRITA (0) E LEITURA (1) ---\n");
    while(1){
    	if(ntec!=TECS_OPEN) {
    		switch(ntec){
    			case 0:
	    			printf_fast_f("--- OPERACAO DE ESCRITA SELECIONADA, (e): prosseguir, (c):cancelar ---\n");
	    			printf_fast_f("Digite o endereco para realizar a escrita:\n");
	    			ntec = TECS_OPEN;
	
					while (1){
						if(ntec == 11){
							printf_fast_f("OPERACAO CANCELADA.\n");
							ntec = TECS_OPEN;
							mem_adress=0;
							break;
						}

						if(ntec!=TECS_OPEN && ntec<10){
							mem_adress = mem_adress*10 + ntec;
							printf_fast_f("Endereco: %d\n",mem_adress);
							ntec = TECS_OPEN;
						}
						if(ntec == 10){
							ntec = TECS_OPEN;
							if (mem_adress<8192) {
		    				printf_fast_f("Digite o dado:\n");
			    				while(1){
			    					if(ntec == 11){
										ntec = TECS_OPEN;
										printf_fast_f("OPERACAO CANCELADA.\n");
										mem_adress = 0;
										break;
									}
									if(ntec!=TECS_OPEN && ntec<10){
										dado = dado*10 + ntec;
										printf_fast_f("Dado a ser armazenado: %u\n",dado);
										ntec = TECS_OPEN;
									}
									
									if(ntec == 10){
										ntec = TECS_OPEN;
										esc_RAM_SPI(mem_adress,dado);
			    						printf_fast_f("Dado escrito com sucesso.\n");
			    						dado = 0;
			    						mem_adress = 0;
			    						break;
									}
			    				}
			    				break;		    				
			    			}
			    			else {
			    				printf_fast_f("Endereco maior que o permitido, operacao cancelada.\n");
			    				mem_adress = 0;
			    				break;
		    				}							
						}
					}
	    			break;
    			case 1:
	    			printf_fast_f("--- OPERACAO DE LEITURA SELECIONADA, (e): prosseguir, (c):cancelar ---\n");
	    			printf_fast_f("Digite o endereco para realizar a leitura:\n");
	    			ntec = TECS_OPEN;
	
					while (1){
						if(ntec == 11){
							printf_fast_f("OPERACAO CANCELADA.\n");
							ntec = TECS_OPEN;
							mem_adress = 0;
							break;
						}

						if(ntec!=TECS_OPEN && ntec<10){
							mem_adress = mem_adress*10 + ntec;
							printf_fast_f("Endereco: %d\n",mem_adress);
							ntec = TECS_OPEN;
						}
						if(ntec == 10){
							ntec = TECS_OPEN;
							if (mem_adress<8192) {
		    					aux = le_RAM_SPI(mem_adress);
		    					printf_fast_f("Dado lido do endereco %d: %u\n",mem_adress, aux);
		    					mem_adress = 0;
		    					break;
			    			}
			    			else {
			    				printf_fast_f("Endereco maior que o permitido, operacao cancelada.\n");
			    				mem_adress = 0;
			    				break;
		    				}							
						}
					}

	    			break;
	    		default:
					break;
    		}
    		ntec = TECS_OPEN;
    		printf_fast_f("--- ESCRITA (0) E LEITURA (1) ---\n");
    	}
    }

}

