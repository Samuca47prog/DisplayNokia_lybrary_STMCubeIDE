
#include "stm32f4xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"

#include "LCD.h"
#include "font.h"
#include "grafic.h"


#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

#define false 0
#define true 1


// defines da aula 3
#define LARGFONTE 	6
#define TAMTELA 	504
#define TAMBUF 		504



// manipulador do display
static LCD_HandleTypeDef *lcd;
// declarado como static para garantir que seus dados não serão alterados

//Buffer que armazena os dados que são enviados ao display
static uint8_t Buffer[TAMBUF];

const uint8_t TelaLimpa[TAMBUF]={0};

//Define the LCD Operation function
void LCD5110_LCD_write_byte(unsigned char dat,unsigned char mode);



// =============================================================================================================================
//
void LCD5110_init(LCD_HandleTypeDef *hlcd5110)
{
	// phspi irá apontar pro endereço de dados da SPI escolhida
	lcd = hlcd5110;

	// inicialização dos pinos de CS e de DS
	HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 1);
	HAL_GPIO_WritePin(lcd->DC_Port, lcd->DC_Pin, 1);

	HAL_GPIO_WritePin(lcd->RS_Port, lcd->RS_Pin, 0);
	HAL_Delay(10);
	HAL_GPIO_WritePin(lcd->RS_Port, lcd->RS_Pin, 1);

	LCD5110_LCD_write_byte(0x21,0);
	LCD5110_LCD_write_byte(0xc6,0); //ajusta o contraste do display
	LCD5110_LCD_write_byte(0x06,0);
	LCD5110_LCD_write_byte(0x13,0);
	LCD5110_LCD_write_byte(0x20,0);
	LCD5110_clear();
	LCD5110_LCD_write_byte(0x0c,0);
}


// -----------------------------------------------------------------------------------------------------------------------------
// ---- Função para escrever um byte no display ----
void LCD5110_LCD_write_byte(unsigned char dat,unsigned char mode)
{
	// define se é dado ou comando
	HAL_GPIO_WritePin(lcd->DC_Port, lcd->DC_Pin, mode);

	// habilita o chip select
	HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 0);

	// Transmissão dos dados feitos pela SPI do HAL
	HAL_SPI_Transmit(lcd->hspi, &dat, 1, 200);

	// desabilita o chip select
	HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 1);
}
// ainda necessária para fazer a configuração inicial

// -----------------------------------------------------------------------------------------------------------------------------
// ---- Função para enviar um bloco de dados ao display ----
/**
 * @brief Realiza a transmissão de dados por blocos
 *
 * @param data dados já formatados da informação que se quer escrever
 * @param tam tamanho dos dados
 * @param mode modo: (0)comando, (1)escrita
 */
void LCD_write(uint8_t *data, uint16_t tam, uint8_t mode)
{
	// define se é dado ou comando
	HAL_GPIO_WritePin(lcd->DC_Port, lcd->DC_Pin, mode);

	// habilita o chip select
	HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 0);

	// Agora será transmitido um bloco de tamanho tam de dados
	HAL_SPI_Transmit(lcd->hspi, data, tam, 200);

	// desabilita o chip select
	HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
// ---- Desenhar uma string no display ----
/*
 * Rotina de desenhar uma string no display é feita com 3 funções;
 *
 * 		# void LCD5110_write_str(char *s)
 * 			- recebe a string a ser escrita
 * 			- chama a draw string para construir a string
 * 			- chama a LCD_write para enviar os dados ao display
 *
 * 		# uint16_t LCD_draw_string(char *s)
 * 			- recebe a mesma string da anterior
 * 			- chama a draw char para montar os dados dos caracteres
 * 			- organiza os dados no Buffer
 *
 * 		# void LCD_draw_char(char letra, uint8_t *buf)
 * 			- constroi os dados de cada caractere
 *
 */

/**
 * 	monta os dados do caractere
 */
void LCD_draw_char(char letra, uint8_t *buf)
{
	uint8_t i;
	letra -= ' ';

	// pega o byte da biblioteca fonte6_8 e salva no Buffer dado
	for(i=0;i<LARGFONTE;i++)
	{
		*buf = font6_8[letra][i];
		buf++;
	}
}


/**
 * 	monta os dados da string completa
 */
uint16_t LCD_draw_string(char *s)
{
	uint8_t *c;
	uint16_t tamanho=0;


	// aponta para o vetor dos dados do caractere
	c=Buffer;

	// passa pela string pedida até encontrar o seu final
  	while(*s!='\0')
	{
  		// monta caractere por caractere no Buffer, apontado por c
  		LCD_draw_char(*s, c);
		s++;
		c+=LARGFONTE;
		tamanho+=LARGFONTE; // define quantos caracteres foram passados !!!!!!!!!!!!!!!!!!!!!!!!!
	}

  	return tamanho;
}

/**
 * 	escreve a string recebida no display
 */
void LCD5110_write_str(char *s)
{
	// tamnho da string a ser escrita
	uint16_t tam;
	// preenche o Buffer com os dados de cada caractere e retorna o tamanho final
	tam = LCD_draw_string(s);
	// manda os dados da string à funçâo de escrever no display
	LCD_write(Buffer, tam, 1);
}

// --- fim da rotina de desenhar uma string ---
// -----------------------------------------------------------------------------------------------------------------------------

/**
 * 	monta os dados da string completa
 */
void LCD5110_clear()
{
	// limpa o display escrevendo 0 em todos os pixels
	LCD_write(TelaLimpa, TAMTELA, 1);
}



// -----------------------------------------------------------------------------------------------------------------------------
// ---- Função para setar a posição XY no display ----
void LCD5110_set_XY(unsigned char X,unsigned char Y)
{
	unsigned char x;
	x = 6*X;

	LCD5110_LCD_write_byte(0x40|Y,0);
	LCD5110_LCD_write_byte(0x80|x,0);
}



// =============================================================================================================================
// ---- Funções para configurar os pinos ----



/* Para usar a função printf */
int _write(int file, char *ptr, int len)
{
  int i=0;
  for(i=0 ; i<len ; i++)
	  LCD5110_write_char(*ptr++);
  return len;
}

