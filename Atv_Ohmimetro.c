// Adição das bibliotecas necessárias
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "hardware/pio.h"
#include "Atv_Ohmimetro.pio.h"

//Definição dos endereços e portas necessários ao uso do I2C
#define I2C_PORT i2c1       
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define ADC_PIN 28          // GPIO para o voltímetro
#define NUM_PIXELS 25       //Definição do número de LEDs da matriz 5X5
#define LEDS_PIN 7          //Definição da porta GPIO da matriz de LEDs 5X5

// Definição das variáveis e demais valores utilizados de forma global
int R_conhecido = 10000;   
float R_x = 0.0;           
float ADC_VREF = 3.31;     
int ADC_RESOLUTION = 4095; 
static ssd1306_t ssd;
static volatile double r = 0.0, b = 0.0, g = 0.0;
static volatile uint32_t valor_led;
static volatile PIO pio = pio0;
static volatile uint offset;
static volatile uint sm;
static volatile int contador = 1;

// Vetores usados para criar os desenhos na matriz de LEDs
double faixa1[NUM_PIXELS] = {0.0, 0.5, 0.0, 0.0, 0.0,
                            0.0, 0.0, 0.0, 0.5, 0.0,
                            0.0, 0.5, 0.0, 0.0, 0.0,
                            0.0, 0.0, 0.0, 0.5, 0.0,
                            0.0, 0.5, 0.0, 0.0, 0.0}; 

double faixa2[NUM_PIXELS] = {0.0, 0.0, 0.5, 0.0, 0.0,
                            0.0, 0.0, 0.5, 0.0, 0.0,
                            0.0, 0.0, 0.5, 0.0, 0.0,
                            0.0, 0.0, 0.5, 0.0, 0.0,
                            0.0, 0.0, 0.5, 0.0, 0.0};
                            
double faixa3[NUM_PIXELS] = {0.0, 0.0, 0.0, 0.5, 0.0,
                            0.0, 0.5, 0.0, 0.0, 0.0,
                            0.0, 0.0, 0.0, 0.5, 0.0,
                            0.0, 0.5, 0.0, 0.0, 0.0,
                            0.0, 0.0, 0.0, 0.5, 0.0}; 

double faixapreta[NUM_PIXELS] = {0.0, 0.0, 0.5, 0.0, 0.0,
                            0.0, 0.0, 0.0, 0.0, 0.0,
                            0.0, 0.0, 0.5, 0.0, 0.0,
                            0.0, 0.0, 0.0, 0.0, 0.0,
                            0.0, 0.0, 0.5, 0.0, 0.0}; 
                          
// Função para converter os parâmetros r, g, b em um valor de 32 bits
uint32_t matrix_rgb(double b, double r, double g){
unsigned char R, G, B;
R = r * 255;
G = g * 255;
B = b * 255;
return (G << 24) | (R << 16) | (B << 8);
}

// Função para formar os desenhos na matriz de LEDs 5x5
void desenhos(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b){
// O loop aciona cada LED da matriz com base em um valor de cor 
for (int i = 0; i < NUM_PIXELS; i++){
  // Determinação da cor de cada LED
  uint32_t valor_led = matrix_rgb(desenho[24 - i]*r, desenho[24 - i]*g, desenho[24 - i]*b);
  // O valor é enviado ao LED para ser exibido
  pio_sm_put_blocking(pio, sm, valor_led);
  }
}

// Função que determina qual cor será exibida na matriz de LEDs de acordo com o valor lido
static void colorirmatriz(int faixa, int contador){
  switch (faixa){
  case 0:
  //No caso da faixa ser igual a 0, é chamado um desenho diferente para impressão na matriz 
  r = 1.0;
  g = 1.0;
  b = 1.0;
  desenhos(faixapreta, valor_led, pio, sm, b, r ,g);
  break;

  case 1: //Marrom
  //Em cada um dos demais casos, tenta-se realizar uma aproximação do código de cores usando os LEDs 
  r = 0.6;
  g = 0.2;
  b = 0.0;
  break;

  case 2: //Vermelho
  r = 1.0;
  g = 0.0;
  b = 0.0;
  break;

  case 3: //Laranja
  r = 1.0;
  g = 0.5;
  b = 0.0;
  break;

  case 4: //Amarelo
  r = 1.0;
  g = 1.0;
  b = 0.0;
  break;

  case 5: //Verde
  r = 0.0;
  g = 1.0;
  b = 0.0;
  break;

  case 6: //Azul
  r = 0.0;
  g = 0.0;
  b = 1.0;
  break;

  case 7: //Violeta
  r = 0.5;
  g = 0.0;
  b = 0.5;
  break;

  case 8: //Cinza
  r = 0.3;
  g = 0.3;
  b = 0.3;
  break;

  case 9: //Branco
  r = 1.0;
  g = 1.0;
  b = 1.0;
  break;

  default: //Desligado
  r = 0.0;
  g = 0.0;
  b = 0.0;
  break;
  }

  // A depender do valor do contador, é chamada a função que realiza o desenho na matriz
  if(contador==1){
    desenhos(faixa1, valor_led, pio, sm, b, r ,g);
  } else if(contador==2){
    desenhos(faixa2, valor_led, pio, sm, b, r ,g);
  } else if(contador==3){
    desenhos(faixa3, valor_led, pio, sm, b, r ,g);
  }
}

static void setup() {

  // Determinação de variáveis usadas pela matriz de LEDs
  offset = pio_add_program(pio, &leds_matrix_program);
  sm = pio_claim_unused_sm(pio, true);
  leds_matrix_program_init(pio, sm, offset, LEDS_PIN);

  // Inicialização e configuração do I2C
  i2c_init(I2C_PORT, 400 * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    
  gpio_pull_up(I2C_SDA);                                       
  gpio_pull_up(I2C_SCL);    
  
  // Inicialização e configuração do Display
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); 
  ssd1306_config(&ssd);                                         
  ssd1306_send_data(&ssd);                                      
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  // Inicialização do ADC
  adc_init();
  adc_gpio_init(ADC_PIN); // GPIO 28 como entrada analógica
}

int main() {

  // Função de configuração de entradas e periféricos
  setup();

  // Determinação de strings para impressão no display 
  char medicao[5]; 
  char Faixa1[10];
  char Faixa2[10];
  char Faixa3[10];

  bool cor = true;
  while (true)
  {
    // O ADC é lido repetidamente por 500 vezes e calcula-se uma média do valor
    adc_select_input(2); 
    float soma = 0.0;
    for (int i = 0; i < 500; i++)
    {
      soma += adc_read();
      sleep_ms(1);
    }
    float media = soma/500.0;

    // O valor do resistor desconhecido é calculado
    R_x = (R_conhecido * media) / (ADC_RESOLUTION - media);

    // Os valores das 3 primeiras faixas de cores do resistor são determinadas
    float resistor = R_x; 
    float mult; 
    for(mult = -1; resistor>=10.0; mult++){
      resistor = resistor/10.0;
    }
    double faixa1 = 0.0;
    double faixa2 = modf(resistor, &faixa1);

    // Os valores são convertidos em strings
    sprintf(Faixa1, "%1.0f", faixa1);
    sprintf(Faixa2, "%1.0f", faixa2*10.0);
    sprintf(Faixa3, "%1.0f", mult);
    sprintf(medicao, "%1.0f", R_x);   

    int listra1 = (int)faixa1;
    int listra2 = (int)(faixa2*10.0);
    int listra3 = (int)mult;

    // O conteúdo do Display é atualizado com os novos valores obtidos
    ssd1306_fill(&ssd, !cor);                          
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);      
    ssd1306_draw_string(&ssd, "Resist. ", 6, 8);          
    ssd1306_draw_string(&ssd, medicao, 70, 8);    
    ssd1306_draw_string(&ssd, "Faixa 1 = ", 20, 22); 
    ssd1306_draw_string(&ssd, Faixa1, 100, 22); 
    ssd1306_draw_string(&ssd, "Faixa 2 = ", 20, 36); 
    ssd1306_draw_string(&ssd, Faixa2, 100, 36);          
    ssd1306_draw_string(&ssd, "Faixa 3 = ", 20, 50);   
    ssd1306_draw_string(&ssd, Faixa3, 100, 50);        
    ssd1306_send_data(&ssd);                        
    sleep_ms(700);

    // As faixas de cores são mostradas em ordem na matriz de LEDs em intervalos de 0,8 segundos
    for(contador=1; contador<=3; contador++){
    if(contador==1) colorirmatriz(listra1, contador);
    else if(contador==2) colorirmatriz(listra2, contador);
    else if(contador==3) colorirmatriz(mult, contador);
    sleep_ms(800);
    }
  }
}