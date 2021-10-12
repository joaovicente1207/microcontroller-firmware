#include <stdio.h>
#include "config.c"
#include "def_pinos.h"

#define SEM_TECLA 44
#define tecla_I 21
#define tecla_P 22
#define tecla_R 23
#define tecla_E 24

#define LIGAR 1
#define DESLIGAR 0

#define NOP4()  NOP();NOP();NOP();NOP()
#define NOP8()  NOP4();NOP4()
#define NOP12() NOP4();NOP8()

#define CS1 P2_0
#define CS2 P2_1
#define RS  P2_2
#define RW  P2_3
#define E   P2_4
#define RST P2_5
#define DB  P4
#define ESQ 0
#define DIR 1
#define CO  0
#define DA  1

volatile int flag_ativa = 0;
volatile float contar_tam_pulso = 0;
volatile int flag_monitoramento = 0;
volatile int mensagem_int=0;
volatile int tempo_segundo=0;
volatile int cont_do_putchar;
volatile int flag_do_GLCD;
volatile unsigned char lido_teclado = SEM_TECLA;

volatile unsigned char conta_tempo = 0, mensagem250 = 0; 

const char caracteresTodos[96][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00},// (space)
    {0x00, 0x00, 0x5F, 0x00, 0x00},// !
    {0x00, 0x07, 0x00, 0x07, 0x00},// "
    {0x14, 0x7F, 0x14, 0x7F, 0x14},// #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12},// $
    {0x23, 0x13, 0x08, 0x64, 0x62},// %
    {0x36, 0x49, 0x55, 0x22, 0x50},// &
    {0x00, 0x05, 0x03, 0x00, 0x00},// '
    {0x00, 0x1C, 0x22, 0x41, 0x00},// (
    {0x00, 0x41, 0x22, 0x1C, 0x00},// )
    {0x08, 0x2A, 0x1C, 0x2A, 0x08},// *
    {0x08, 0x08, 0x3E, 0x08, 0x08},// +
    {0x00, 0x50, 0x30, 0x00, 0x00},// ,
    {0x08, 0x08, 0x08, 0x08, 0x08},// -
    {0x00, 0x30, 0x30, 0x00, 0x00},// .
    {0x20, 0x10, 0x08, 0x04, 0x02},// /
    {0x3E, 0x51, 0x49, 0x45, 0x3E},// 0
    {0x00, 0x42, 0x7F, 0x40, 0x00},// 1
    {0x42, 0x61, 0x51, 0x49, 0x46},// 2
    {0x21, 0x41, 0x45, 0x4B, 0x31},// 3
    {0x18, 0x14, 0x12, 0x7F, 0x10},// 4
    {0x27, 0x45, 0x45, 0x45, 0x39},// 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30},// 6
    {0x01, 0x71, 0x09, 0x05, 0x03},// 7
    {0x36, 0x49, 0x49, 0x49, 0x36},// 8
    {0x06, 0x49, 0x49, 0x29, 0x1E},// 9
    {0x00, 0x36, 0x36, 0x00, 0x00},// :
    {0x00, 0x56, 0x36, 0x00, 0x00},// ;
    {0x00, 0x08, 0x14, 0x22, 0x41},// <
    {0x14, 0x14, 0x14, 0x14, 0x14},// =
    {0x41, 0x22, 0x14, 0x08, 0x00},// >
    {0x02, 0x01, 0x51, 0x09, 0x06},// ?
    {0x32, 0x49, 0x79, 0x41, 0x3E},// @
    {0x7E, 0x11, 0x11, 0x11, 0x7E},// A
    {0x7F, 0x49, 0x49, 0x49, 0x36},// B
    {0x3E, 0x41, 0x41, 0x41, 0x22},// C
    {0x7F, 0x41, 0x41, 0x22, 0x1C},// D
    {0x7F, 0x49, 0x49, 0x49, 0x41},// E
    {0x7F, 0x09, 0x09, 0x01, 0x01},// F
    {0x3E, 0x41, 0x41, 0x51, 0x32},// G
    {0x7F, 0x08, 0x08, 0x08, 0x7F},// H
    {0x00, 0x41, 0x7F, 0x41, 0x00},// I
    {0x20, 0x40, 0x41, 0x3F, 0x01},// J
    {0x7F, 0x08, 0x14, 0x22, 0x41},// K
    {0x7F, 0x40, 0x40, 0x40, 0x40},// L
    {0x7F, 0x02, 0x04, 0x02, 0x7F},// M
    {0x7F, 0x04, 0x08, 0x10, 0x7F},// N
    {0x3E, 0x41, 0x41, 0x41, 0x3E},// O
    {0x7F, 0x09, 0x09, 0x09, 0x06},// P
    {0x3E, 0x41, 0x51, 0x21, 0x5E},// Q
    {0x7F, 0x09, 0x19, 0x29, 0x46},// R
    {0x46, 0x49, 0x49, 0x49, 0x31},// S
    {0x01, 0x01, 0x7F, 0x01, 0x01},// T
    {0x3F, 0x40, 0x40, 0x40, 0x3F},// U
    {0x1F, 0x20, 0x40, 0x20, 0x1F},// V
    {0x7F, 0x20, 0x18, 0x20, 0x7F},// W
    {0x63, 0x14, 0x08, 0x14, 0x63},// X
    {0x03, 0x04, 0x78, 0x04, 0x03},// Y
    {0x61, 0x51, 0x49, 0x45, 0x43},// Z
    {0x00, 0x00, 0x7F, 0x41, 0x41},// [
    {0x02, 0x04, 0x08, 0x10, 0x20},// "\"
    {0x41, 0x41, 0x7F, 0x00, 0x00},// ]
    {0x04, 0x02, 0x01, 0x02, 0x04},// ^
    {0x40, 0x40, 0x40, 0x40, 0x40},// _
    {0x00, 0x01, 0x02, 0x04, 0x00},// `
    {0x20, 0x54, 0x54, 0x54, 0x78},// a
    {0x7F, 0x48, 0x44, 0x44, 0x38},// b
    {0x38, 0x44, 0x44, 0x44, 0x20},// c
    {0x38, 0x44, 0x44, 0x48, 0x7F},// d
    {0x38, 0x54, 0x54, 0x54, 0x18},// e
    {0x08, 0x7E, 0x09, 0x01, 0x02},// f
    {0x08, 0x14, 0x54, 0x54, 0x3C},// g
    {0x7F, 0x08, 0x04, 0x04, 0x78},// h
    {0x00, 0x44, 0x7D, 0x40, 0x00},// i
    {0x20, 0x40, 0x44, 0x3D, 0x00},// j
    {0x00, 0x7F, 0x10, 0x28, 0x44},// k
    {0x00, 0x41, 0x7F, 0x40, 0x00},// l
    {0x7C, 0x04, 0x18, 0x04, 0x78},// m
    {0x7C, 0x08, 0x04, 0x04, 0x78},// n
    {0x38, 0x44, 0x44, 0x44, 0x38},// o
    {0x7C, 0x14, 0x14, 0x14, 0x08},// p
    {0x08, 0x14, 0x14, 0x18, 0x7C},// q
    {0x7C, 0x08, 0x04, 0x04, 0x08},// r
    {0x48, 0x54, 0x54, 0x54, 0x20},// s
    {0x04, 0x3F, 0x44, 0x40, 0x20},// t
    {0x3C, 0x40, 0x40, 0x20, 0x7C},// u
    {0x1C, 0x20, 0x40, 0x20, 0x1C},// v
    {0x3C, 0x40, 0x30, 0x40, 0x3C},// w
    {0x44, 0x28, 0x10, 0x28, 0x44},// x
    {0x0C, 0x50, 0x50, 0x50, 0x3C},// y
    {0x44, 0x64, 0x54, 0x4C, 0x44},// z
    {0x00, 0x08, 0x36, 0x41, 0x00},// {
    {0x00, 0x00, 0x7F, 0x00, 0x00},// |
    {0x00, 0x41, 0x36, 0x08, 0x00},// }
    {0x08, 0x08, 0x2A, 0x1C, 0x08},// ->
    {0x08, 0x1C, 0x2A, 0x08, 0x08} // <-
};

unsigned char leituraPeloGLCD(__bit cd, __bit cs){
    unsigned char byte_return;
    RW = 1;
    CS1 = cs;
    CS2 = !cs;
    RS = cd; 
    NOP4();
    E = 1;
    NOP8();
    SFRPAGE = CONFIG_PAGE;
    byte_return = DB;
    SFRPAGE = LEGACY_PAGE;
    NOP4();
    E = 0;
    NOP12();
    return byte_return;
}

void escrevePeloGLCD(unsigned char dado, __bit cd, __bit cs){

    while( leituraPeloGLCD( CO, ESQ ) & 0x10 );
    RW = 0;
    CS1 = cs;
    CS2 = !cs;
    RS = cd; 
    SFRPAGE = CONFIG_PAGE;
    DB = dado;
    SFRPAGE = LEGACY_PAGE;
    NOP4();
    E = 1;
    NOP12();
    E = 0;
    SFRPAGE = CONFIG_PAGE;
    DB = 0xFF;
    SFRPAGE = LEGACY_PAGE;
    NOP12();
}


void StartNoGLCD(){
    E = 0; 
    CS1 = 1;
    CS2 = 1; 
    RST = 1;
    SFRPAGE = CONFIG_PAGE;
    DB = 0xFF;
    SFRPAGE = LEGACY_PAGE;

    while( leituraPeloGLCD( CO, ESQ ) & 0x10 );
    while( leituraPeloGLCD( CO, DIR ) & 0x10 );

    escrevePeloGLCD( 0x3F, CO, ESQ );
    escrevePeloGLCD( 0x3F, CO, DIR );

    escrevePeloGLCD( 0x40, CO, ESQ );
    escrevePeloGLCD( 0xB8, CO, ESQ );
    escrevePeloGLCD( 0xC0, CO, ESQ );

    escrevePeloGLCD( 0x40, CO, DIR );
    escrevePeloGLCD( 0xB8, CO, DIR );
    escrevePeloGLCD( 0xC0, CO, DIR );
}

void setando_Y( unsigned char y, __bit cs ){
    y = y & 0x3F;
    escrevePeloGLCD( (0x40 | y) , CO, cs );
}

void setando_X( unsigned char x, __bit cs ){
    x = x & 0x07;
    escrevePeloGLCD( (0xB8 | x) , CO, cs );
}

void limparTudoGLCD( __bit cs ){
    unsigned char pg, pos;

    for( pg = 0; pg < 8; pg++ ){
        setando_X(pg,cs); 
        setando_Y(0,cs); 
        for( pos = 0; pos < 64; pos++ )
            escrevePeloGLCD( 0x00 , DA, cs);
    }

}

void putchar(unsigned char c){
    __bit side;

    if( c > 0 && c < 9){
        setando_X(c-1, ESQ);
        setando_X(c-1, DIR);
        setando_Y(0, ESQ);
        setando_Y(0, DIR);
        flag_do_GLCD = c-1;
        cont_do_putchar = 0;
    }
    else{ 
        if(cont_do_putchar < 8) side = ESQ;
        else side = DIR; 

        c = c - 32; 

        escrevePeloGLCD(caracteresTodos[c][0],DA, side);
        escrevePeloGLCD(caracteresTodos[c][1],DA, side);
        escrevePeloGLCD(caracteresTodos[c][2],DA, side);
        escrevePeloGLCD(caracteresTodos[c][3],DA, side);
        escrevePeloGLCD(caracteresTodos[c][4],DA, side);

        escrevePeloGLCD(0,DA,side);
        escrevePeloGLCD(0,DA,side);
        escrevePeloGLCD(0,DA,side);

        cont_do_putchar++;

        if(cont_do_putchar > 15){
            cont_do_putchar = 0;
            if(flag_do_GLCD < 8){
                flag_do_GLCD++;
                setando_X(flag_do_GLCD, ESQ);
                setando_X(flag_do_GLCD, DIR);
                setando_Y(0, ESQ);
                setando_Y(0, DIR);  
            }
            else flag_do_GLCD = 0;
        }
    }
}

void delay_ms(unsigned int t)
{
    TMOD = TMOD | 0x01;
    TMOD = TMOD & ~0x02;
    
    for(; t>0; t--){
        TR0 = 0;
        TF0 = 0;
        TH0 = 0x9E;
        TL0 = 0x58;
        TR0 = 1;
        while( TF0 == 0 ){}
    }
}


// funcao que retorna o pulso
float leituraDoPulso(){
    unsigned int th_tl;
    float ret;

    while(P1_0 == 1);
    
    contar_tam_pulso = 0;
    IE1 = 0;
    TR1 = 1;
    while(IE1 == 0);
    TR1 = 0;
    TF1 = 0;
    th_tl = (unsigned int) TH1*256 + (unsigned int) TL1;
    TL1 = 0;
    TH1 = 0; 
    contar_tam_pulso += (float)th_tl/25000000;

    ret = 0.015/contar_tam_pulso;

    return ret;
}

// INT do timer 1
void TIMER1_INT(void) interrupt 3
{
    TF1 = 0;
    contar_tam_pulso += 0.00262144;
    if(contar_tam_pulso >= 1){ // mede ate 1s
        IE1 = 1;
    }
}


// funcao para emular o encoder 
void emula_encoder(unsigned int rotacao){
    float enviar;
    unsigned int aux;
    //ajusta os limites
    if (rotacao > 902){
        rotacao = 902;
    }
    if (rotacao < 18){
        rotacao = 18;
    }

    enviar = (375000.0/rotacao);
    aux = 65536 - enviar;

    SFRPAGE = TMR4_PAGE;

    RCAP4L = aux; 
    RCAP4H = aux >> 8; 

    SFRPAGE = LEGACY_PAGE;
}

// INT do timer 3 a cada 4ms
void TIMER3_INT(void) __interrupt 14  
{
    if(flag_monitoramento == 1)
    {
        mensagem250++;
        if(mensagem250==250)
        {
            mensagem250=0;
            conta_tempo++;
            if(conta_tempo==60)
            {
                conta_tempo=0;
            }
            
        }
        TF3=0;        
    }
}

// INT para as teclas
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
            case 'p':
                lido_teclado = tecla_P;
                break;
            case 'r':
                lido_teclado = tecla_R;
                break;
            case 'e':
                lido_teclado = tecla_E;
                break;          
            default:
                break;
    }
    RI0=0;
    }
}



void LigaMenu(void)
{
    printf_fast_f("\x02(i)- MONITORIA");
    printf_fast_f("\x03(p)- PARAR MON");
    printf_fast_f("\x04(r)- ALTER ROT");
    printf_fast_f("\x05(e)- ENTER    ");
}

void ajustaRotacao(void){
    int algarismos = 1;
    int rotacao_ajuste = 0;

    while(1)
    {
        
        if(algarismos < 4 && lido_teclado < 10)
        {
            rotacao_ajuste = rotacao_ajuste*10 + lido_teclado;
            algarismos++;
            lido_teclado = SEM_TECLA;
            printf_fast_f("\x02               ");
            printf_fast_f("\x02 AJU. RPM = %d",rotacao_ajuste);
        }
        if(lido_teclado == tecla_E && algarismos > 1)
        {
            lido_teclado = SEM_TECLA;
            // enter rotação
            emula_encoder(rotacao_ajuste);
            limparTudoGLCD(ESQ);
            limparTudoGLCD(DIR);
            printf_fast_f("\x01               ");
            printf_fast_f("\x01 ROT. ENVIADA");
            if (rotacao_ajuste > 900){
                printf_fast_f("\x03               ");
                printf_fast_f("\x03 => 900.0 RPM");
            }
            if (rotacao_ajuste < 20){
                printf_fast_f("\x03               ");
                printf_fast_f("\x03 =< 20.0 RPM");
            }
            delay_ms(1000);
            break;             
        }            
    }

}


void main (void) {
    unsigned char radar=DESLIGAR, int_de_temp=1;
    unsigned char segundos_de_antes = 0;
    float pulso_em_RPM = 0;
    Init_Device();
    SFRPAGE = LEGACY_PAGE;

    // inicializando 
    StartNoGLCD();

    // Limpando LCD
    limparTudoGLCD(ESQ);
    limparTudoGLCD(DIR);

    // impressao do menu
    LigaMenu();
    emula_encoder(20);
    
    while(1) 
    {
        if(lido_teclado == tecla_I){
            flag_monitoramento = 1;
            radar=LIGAR;
        }

        if(radar == LIGAR && conta_tempo != segundos_de_antes && conta_tempo%int_de_temp == 0){
            lido_teclado = SEM_TECLA;
            limparTudoGLCD(ESQ);
            limparTudoGLCD(DIR);
            printf_fast_f("\x01               ");
            printf_fast_f("\x01 MONITOR de VEL");
            pulso_em_RPM = leituraDoPulso();
            printf_fast_f("\x03               ");
            printf_fast_f("\x03 Tempo: %ds", conta_tempo);
            printf_fast_f("\x04 VEL: %.2fRPM", pulso_em_RPM);
            segundos_de_antes= conta_tempo;

        }

        if(lido_teclado == tecla_R)
        {
            if (flag_monitoramento == 1)
            {
                flag_ativa = 1;
            }
            flag_monitoramento = 0;
            conta_tempo = 0;
            // mudar a rotacao
            lido_teclado = SEM_TECLA; 
            limparTudoGLCD(ESQ);
            limparTudoGLCD(DIR);
            printf_fast_f("\x01               ");
            printf_fast_f("\x01 AJUSTE ROTACAO");
            ajustaRotacao();
            if (flag_ativa == 1)
            {
                flag_monitoramento = 1;
                flag_ativa = 0;
            }                      
        }
        if(lido_teclado == tecla_P && flag_monitoramento == 1)
        {
            flag_monitoramento = 0;
            radar=DESLIGAR;
            conta_tempo = 0;   
            // para leitura
            lido_teclado = SEM_TECLA;
            limparTudoGLCD(ESQ);
            limparTudoGLCD(DIR);
            LigaMenu();
            printf_fast_f("\x07               ");
            printf_fast_f("\x07 FIM MONITOR.");             
        }    
    }
}

