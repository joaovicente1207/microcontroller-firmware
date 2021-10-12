//Joao Vicente de Oliveira Neto
//P2 de MICAP

#include "def_pinos.h"
#include "config.c"
#include <stdio.h>

#define TECS_OPEN 44
#define tecla_I 21
#define tecla_T 22
#define tecla_S 23
#define tecla_P 24
#define tecla_V 25

#define SCL P0_7
#define SDA P0_6
#define R 1
#define W 0

#define G05 	6
#define G1 		0
#define G2 		1
#define G4 		2
#define G8 		3
#define G16 	4
#define AIN0_0 		0
#define AIN0_1 		1
#define AIN0_2 		2
#define AIN0_3 		3
#define HVDA   		4
#define AGND   		5
#define P3EVEN 		6
#define P3ODD  		7
#define TEMP_SENSOR 8

volatile unsigned char lido_teclado = TECS_OPEN;
volatile unsigned int contador_int = 0;
int ultimo_end = 1;
unsigned int intervalo = 0;
volatile unsigned int flag_leitura_periodica = 0;
int end_para_eeprom = 1;

void putchar(unsigned char c){
	SBUF0 = c;
	while(TI0 == 0);
	TI0 = 0;
}

void int_serial(void) __interrupt 4{
if(RI0==1){
	switch(SBUF0){
			case '0':
				lido_teclado = 0;
				break;
			case '1':
				lido_teclado = 1;
				break;	
			case '2':
				lido_teclado = 2;
				break;
			case '3':
				lido_teclado = 3;
				break;	
			case '4':
				lido_teclado = 4;
				break;
			case '5':
				lido_teclado = 5;
				break;	
			case '6':
				lido_teclado = 6;
				break;
			case '7':
				lido_teclado = 7;
				break;	
			case '8':
				lido_teclado = 8;
				break;
			case '9':
				lido_teclado = 9;
				break;	
			case 'i':
				lido_teclado = tecla_I;
				break;
			case 't':
				lido_teclado = tecla_T;
				break;
			case 's':
				lido_teclado = tecla_S;
				break;
			case 'p':
				lido_teclado = tecla_P;
				break;
			case 'v':
				lido_teclado = tecla_V;
				break;				
			default:
				break;
	}
	RI0=0;
	}
}


unsigned char esc_byte_contr(unsigned char end_disp, __bit rw){
	STA=1;
	SI=0;
	while(SI==0);

	if(SMB0STA != 0x08 && SMB0STA != 0x10)
		return SMB0STA;

	SMB0DAT = (end_disp & 0xFE) | rw;
	STA=0;
	SI=0;
	while(SI==0);

	if(rw == W)
		if(SMB0STA != 0x18)
			return SMB0STA;
	else if(rw == R)
		if(SMB0STA != 0x40)
			return SMB0STA;
	
	return 0;
}

unsigned char esc_byte_dado(unsigned char dado){
	SMB0DAT = dado;
	SI=0;
	while(SI==0);
	if(SMB0STA != 0x28)
		return SMB0STA;

	return 0;
}

int esc_eeprom(unsigned char end_disp, unsigned char end, unsigned char dado){
	unsigned char erro;

	erro = esc_byte_contr(end_disp, W);

	if(erro != 0) return -erro;

	erro = esc_byte_dado(end);

	if(erro != 0) return -erro;

	erro = esc_byte_dado(dado);

	if(erro != 0) return -erro;

	STO=1;
	SI=0;
	while(STO==1);

	while(esc_byte_contr(end_disp,W)!=0);

	return 0;
}

int le_eeprom(unsigned char end_disp, unsigned char end){
	int dado;
	unsigned char erro;

	erro = esc_byte_contr(end_disp, W);

	if(erro != 0) return -erro;

	erro = esc_byte_dado(end);

	if(erro != 0) return -erro;

	erro = esc_byte_contr( end_disp, R );

	if(erro != 0) return -erro;

	AA=0;
	SI=0;
	while(SI==0);

	if(SMB0STA != 0x58)
		return -(int)SMB0STA;

	dado = (int)SMB0DAT;

	STO=1;
	SI=0;
	while(STO==1);

	return dado;
}

void Timer4_ISR(void) __interrupt 16{
	SMB0CN = SMB0CN & ~0x40;
	SMB0CN = SMB0CN | 0x40;
	TF4 = 0;
}


void envia_temperatura_DAC0(float temp_saida) {
	unsigned int codigo=0;
	// 12 bits -1 ==> 2^12 -1 = 4095
	// 4095/51.0 = 80.2941176 (fator de conversao entre temperatura e DAC)
	codigo = (unsigned int)(temp_saida*80.2941176);

	DAC0L = codigo;
	DAC0H = codigo>>8;

}

void le_ADC0( unsigned char canal, unsigned char ganho){
	ADC0CF = (ADC0CF & 0xf8) | ganho;

	AMX0SL = canal;

	AD0BUSY = 1; // inicia conversao AD

	while(AD0BUSY); // Espera conversão terminar

	
}

//armazena na EEPROM o valor de ADC0
void armazenaEEPROM(void){
	int retorno;
	unsigned char valor_eeprom;
	float guarda_adc0;
	le_ADC0(AIN0_0, G1);

	// T = ADC0*(51/4095) = ADC0*0.01245421
	// como cada byte da eeprom guardam 255 valores diferentes, fazemos T*255/51 = T*5
	guarda_adc0 = (float)(ADC0*0.062271);
	valor_eeprom = (unsigned char)(guarda_adc0);
	retorno = esc_eeprom(0xA0, end_para_eeprom, valor_eeprom);

	printf_fast_f("Valor gravado na EEPROM: %u, end: %d\n", valor_eeprom,end_para_eeprom);
}

//cada 20ms gera uma interrupção, para 1s = 50 * 20ms
void leituraPeriodica(void) __interrupt 5{

	if (flag_leitura_periodica == 1){
		contador_int +=1;
		//printf_fast_f("contador %d\n",contador_int);
		if (contador_int == 200*intervalo){
			contador_int = 0;
			armazenaEEPROM();
			//end 0 da EEPROM como ponteiro para ultima temperatura 
			esc_eeprom(0xA0, 0, end_para_eeprom);
			end_para_eeprom+=1;
			if (end_para_eeprom == 128){
				printf_fast_f("Ultimo endereco da EEPROM atingido.\n");
				end_para_eeprom = 1;
				flag_leitura_periodica = 0;
				contador_int = 0;
				intervalo = 0;
			}
		}
	}

	TF2 = 0;
}

//le as temperaturas para a opção (t)
void lerTemperatura(void){
	int max_digitos = 0;
	float fator = 10.0;
	float temperatura_lida = 0.0;
	//formato sempre XX.X
	while(1){
		if(lido_teclado!=TECS_OPEN && lido_teclado<10){
			temperatura_lida = temperatura_lida + lido_teclado*fator;
			fator = fator/10;
			printf_fast_f("Temperatura escolhida: %.1f\n", temperatura_lida);
			if (temperatura_lida > 51.0){
				printf_fast_f("Temperatura invalida, tente outra menor que 51.0 C.\n");
				printf_fast_f("Informe a temperatura no formato XX.X:\n");
				temperatura_lida = 0.0;
				max_digitos = 0;
				lido_teclado = TECS_OPEN;
				fator = 10.0;
				continue;
			}
			lido_teclado = TECS_OPEN;
			max_digitos +=1;
			if (max_digitos>2){
				break;
			}
		}
	}
	//simula sensor pelo DAC0
	envia_temperatura_DAC0(temperatura_lida);
}

//inicia com o processo de leitura periódica
void processoLeitura(void){
	flag_leitura_periodica = 1;
	while(1){
		if (lido_teclado == tecla_P){
			lido_teclado = TECS_OPEN;
			flag_leitura_periodica = 0;
			end_para_eeprom = 1;
			contador_int = 0;
			intervalo = 0;
			printf_fast_f("Fim processo de leitura e armazenamento periodico.\n");
			break;
		}
		if (lido_teclado == tecla_T){
			lido_teclado = TECS_OPEN;
			printf_fast_f("Informe a temperatura no formato XX.X:\n");
			lerTemperatura();
		}
	}
}

// le o intervalo desejado pelo usuario
void lerIntervalo(void){
	printf_fast_f("Digite o intervalo entre as leituras:\n");
	while(1){
		if(lido_teclado!=TECS_OPEN && lido_teclado<10){
			intervalo = intervalo*10 + lido_teclado;
			printf_fast_f("%d segundos\n", intervalo);
			lido_teclado = TECS_OPEN;
		}
		if (lido_teclado == tecla_S){
			lido_teclado = TECS_OPEN;
			printf_fast_f("Iniciado processo de leitura e armazenamento periodico.\n");
			processoLeitura();
			break;
		}		
	}
}

//opção que mostra os valores de temperatura guardados na eeprom até o último endereço utilizado.
void leiturasEEPROM(void){
	int dado_lido;
	int end_eeprom;
	float temperatura_final;
	// endereco 0 como ponteiro
	ultimo_end = le_eeprom(0xA0, 0);
	printf_fast_f("--- Temperaturas da EEPROM ---\n");
	for(end_eeprom=1;end_eeprom<=ultimo_end;end_eeprom++){
		dado_lido = le_eeprom(0xA0, end_eeprom);
		temperatura_final = (float) dado_lido/5;
		printf_fast_f("Endereco: %d, Temperatura: %.1f\n", end_eeprom, temperatura_final);
	}
}

//pode (i) ou (v)
void opcaoSelecionada(void){
	int flag_fim = 0;
	while(1){
		switch(lido_teclado){
			case tecla_I:
			lido_teclado = TECS_OPEN;
			lerIntervalo();
			flag_fim = 1;
			break;
			case tecla_V:
			lido_teclado = TECS_OPEN;			
			leiturasEEPROM();
			flag_fim = 1;
			break;
			default:
			break;		
		}
		if (flag_fim == 1){
			break;
		}

	}
}

void printMenu(void){
	printf_fast_f("----------------------------------------\n");
	printf_fast_f(" (i) - intervalo entre leituras  (s)    \n");
	printf_fast_f(" (t) - entrada de temperatura  (XX.X)   \n");
	printf_fast_f(" (s) - inicia leituras e armazenamentos \n");
	printf_fast_f(" (p) - termina leituras e armazenamentos\n");
	printf_fast_f(" (v) - visualizar dados da EEPROM       \n");
	printf_fast_f("----------------------------------------\n");
}

void main(void) {


	Init_Device();
    SFRPAGE = LEGACY_PAGE;

	SMB0CN = SMB0CN & ~0x40;
	SMB0CN = SMB0CN | 0x40;

	
	while(1){

		printMenu();
		opcaoSelecionada();

	}

}