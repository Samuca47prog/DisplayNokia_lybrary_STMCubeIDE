// ============================================================================================================
// === Includes ===

#include "stm32f4xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"

#include "LCD.h"
#include "font.h"
#include "grafic.h"

// ============================================================================================================
// === Defines ===

#define LARGFONTE 	6
#define TAMTELA 	504
#define TAMBUF 		504

// ============================================================================================================
// === Tipos ===

// estados do buffer
typedef enum {B_FREE=0, B_BUSY} BufStatus_t;

/*
 * Estrutura de dados para compartilhar o buffer
 * Realiza o gerenciamento do buffer
 */
typedef struct{
	uint8_t dado[TAMBUF];
	BufStatus_t status;
	uint16_t ocupacao;
} SharedBuffer_t;

// ============================================================================================================
// === Variaveis ===

// manipulador do display
static LCD_HandleTypeDef *lcd;
// declarado como static para garantir que seus dados não serão alterados

// declaração da variável tipo Buffer compartilhado
static SharedBuffer_t buf;

const uint8_t TelaLimpa[TAMBUF]={0};

static HAL_StatusTypeDef (*LCD_write)(uint8_t *data, uint16_t tam, uint8_t mode);




// =============================================================================================================================
// === Funções ===

void LCD5110_init(LCD_HandleTypeDef *hlcd5110)
{
	// phspi irá apontar pro endereço de dados da SPI escolhida
	lcd = hlcd5110;

	//inicialização do buf
	buf.ocupacao=0;
	buf.status=B_FREE;

	// inicialização dos pinos de CS e de DS
	HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 1);
	HAL_GPIO_WritePin(lcd->DC_Port, lcd->DC_Pin, 1);

	// reset do display
	HAL_GPIO_WritePin(lcd->RS_Port, lcd->RS_Pin, 0);
	HAL_Delay(100);
	HAL_GPIO_WritePin(lcd->RS_Port, lcd->RS_Pin, 1);


	switch(lcd->Mode)
	{
		case LCD_BLK:
			LCD_write = LCD_write_BLK;
			break;

		case LCD_IT:
			LCD_write = LCD_write_IT;
			break;

		case LCD_DMA:
			LCD_write = LCD_write_DMA;
			break;
	}

	//LCD_write = LCD_write_IT;


	LCD5110_clear();

	/*
	LCD5110_LCD_write_byte(0x21,0);
	LCD5110_LCD_write_byte(0xc6,0); //ajusta o contraste do display
	LCD5110_LCD_write_byte(0x06,0);
	LCD5110_LCD_write_byte(0x13,0);
	LCD5110_LCD_write_byte(0x20,0);



	// espera transmitir tudo para depois continuar
	while(__HAL_SPI_GET_FLAG(lcd->hspi, SPI_FLAG_BSY)){};
	LCD5110_LCD_write_byte(0x0c,0);
	 */


	buf.dado[0] = 0x21;
	buf.dado[1] = 0xc6;
	buf.dado[2] = 0x06;
	buf.dado[3] = 0x13;
	buf.dado[4] = 0x20;
	buf.dado[5] = 0x0c;
	buf.ocupacao = 6;
	buf.status = B_BUSY;

	//while(buf.status!=B_FREE){};
	while(__HAL_SPI_GET_FLAG(lcd->hspi, SPI_FLAG_BSY)){};

	LCD_write_IT(buf.dado, buf.ocupacao, 0);

	//while(buf.status!=B_FREE){};
	while(__HAL_SPI_GET_FLAG(lcd->hspi, SPI_FLAG_BSY)){};



}

// -----------------------------------------------------------------------------------------------------------------------------
// ---- Função para enviar um bloco de dados ao display ----

HAL_StatusTypeDef LCD5110_write(uint8_t *data, uint16_t tam)
{
	HAL_StatusTypeDef status;

	// Agora será transmitido um bloco de tamanho tam de dados
	status = LCD_write(data, tam, 1);

	return status;
}

HAL_StatusTypeDef LCD_write_BLK(uint8_t *data, uint16_t tam, uint8_t mode)
{
	HAL_StatusTypeDef status;

	// testa se a SPI está livre
	if(__HAL_SPI_GET_FLAG(lcd->hspi, SPI_FLAG_BSY))
		return HAL_BUSY;

	// define se é dado ou comando
	HAL_GPIO_WritePin(lcd->DC_Port, lcd->DC_Pin, mode);

	// habilita o chip select
	HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 0);

	// Agora o bloco será transmitido por interrupção
	status = HAL_SPI_Transmit(lcd->hspi, data, tam, 200);

	// levanta o chip select
	HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 1);

	// libera o buffer
	buf.status = B_FREE;

	return status;
}

HAL_StatusTypeDef LCD_write_IT(uint8_t *data, uint16_t tam, uint8_t mode)
{
	HAL_StatusTypeDef status;

	// testa se a SPI está livre
	if(__HAL_SPI_GET_FLAG(lcd->hspi, SPI_FLAG_BSY))
		return HAL_BUSY;

	// define se é dado ou comando
	HAL_GPIO_WritePin(lcd->DC_Port, lcd->DC_Pin, mode);

	// habilita o chip select
	HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 0);

	// Agora o bloco será transmitido por interrupção
	status = HAL_SPI_Transmit_IT(lcd->hspi, data, tam);

	return status;
}

HAL_StatusTypeDef LCD_write_DMA(uint8_t *data, uint16_t tam, uint8_t mode)
{
	HAL_StatusTypeDef status;

	// testa se a SPI está livre
	if(__HAL_SPI_GET_FLAG(lcd->hspi, SPI_FLAG_BSY))
		return HAL_BUSY;

	// define se é dado ou comando
	HAL_GPIO_WritePin(lcd->DC_Port, lcd->DC_Pin, mode);

	// habilita o chip select
	HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 0);

	// Agora o bloco será transmitido por interrupção
	status = HAL_SPI_Transmit_DMA(lcd->hspi, data, tam);

	return status;
}


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == lcd->hspi->Instance)
	{
		// levanta o chip select, pois a transmissão acabou
		HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 1);
		// libera o buffer
		buf.status = B_FREE;
	}
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

void LCD_draw_char(char letra, uint8_t *buf)
{
	uint8_t i;
	letra -= ' ';

	// pega o byte da biblioteca fonte6_8 e salva no Buffer dado
	for(i=0;i<LARGFONTE;i++)
	{
		*buf = font6_8[(uint8_t)letra][i];
		buf++;
	}
}

uint16_t LCD_draw_string(char *s)
{
	uint8_t *c;
	uint16_t tamanho=0;


	// aponta para o vetor dos dados do caractere
	c=buf.dado;

	// passa pela string pedida até encontrar o seu final
  	while(*s!='\0')
	{
  		// monta caractere por caractere no Buffer, apontado por c
  		LCD_draw_char(*s, c);
		s++;
		c+=LARGFONTE;
		tamanho+=LARGFONTE; // define quantos caracteres foram passados
	}

  	return tamanho;
}

HAL_StatusTypeDef LCD5110_write_str(char *s)
{
	HAL_StatusTypeDef status;

	// testa o estado do buffer
	if(buf.status==B_BUSY)
		return HAL_BUSY; //retorna que o buffer está ocupado

	// atualiza o status do buffer
	buf.status=B_BUSY;

	// preenche o Buffer com os dados de cada caractere e retorna o tamanho final
	buf.ocupacao=LCD_draw_string(s);

	// manda os dados da string à funçâo de escrever no display
	status = LCD_write(buf.dado, buf.ocupacao, 1);

	// atualiza o status do buffer (para modo pooling)
	//buf.status=B_FREE;

	return status;
}

// --- fim da rotina de desenhar uma string ---
// -----------------------------------------------------------------------------------------------------------------------------



HAL_StatusTypeDef LCD5110_clear()
{
	HAL_StatusTypeDef status;

	// testa o estado do buffer
	if(buf.status==B_BUSY)
		return HAL_BUSY; //retorna que o buffer está ocupado

	buf.status = HAL_BUSY;
	// limpa o display escrevendo 0 em todos os pixels
	status = LCD_write(TelaLimpa, TAMTELA, 1);

	return status;
}


// -----------------------------------------------------------------------------------------------------------------------------
// ---- Função para setar a posição XY no display ----

HAL_StatusTypeDef LCD5110_set_XY(uint8_t x, uint8_t y)
{
	HAL_StatusTypeDef status;

	// testa o estado do buffer
	if(buf.status==B_BUSY)
		return HAL_BUSY; //retorna que o buffer está ocupado

	// atualiza o status
	buf.status = B_BUSY;

	// acerta o endereço em função da largura da fonte
	x *= LARGFONTE;

	// garante que x será de 7bits e que y será de 3bits
	x &= 0x7f;
	y &= 0x07;

	buf.dado[0] = 0x40|y;
	buf.dado[1] = 0x80|x;
	buf.ocupacao = 2;

	// envia o comando de setar a posição ao display
	status = LCD_write(buf.dado, buf.ocupacao, 0);

	return status;
}
