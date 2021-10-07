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
	uint8_t *dado;
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

uint8_t buffer[TAMBUF];

const uint8_t TelaLimpa[TAMBUF]={0};




//Define the LCD Operation function
void LCD5110_LCD_write_byte(unsigned char dat,unsigned char mode);

HAL_StatusTypeDef LCD_write_IT(SharedBuffer_t *pbuf, uint8_t mode);

HAL_StatusTypeDef LCD5110_write(uint8_t *data, uint16_t tam);

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
	HAL_Delay(10);
	HAL_GPIO_WritePin(lcd->RS_Port, lcd->RS_Pin, 1);

	//LCD5110_clear();
	//LCD5110_clear();
	//while(buf.status!=B_FREE){};

	/*
	LCD5110_LCD_write_byte(0x21,0);
	LCD5110_LCD_write_byte(0xc6,0); //ajusta o contraste do display
	LCD5110_LCD_write_byte(0x06,0);
	LCD5110_LCD_write_byte(0x13,0);
	LCD5110_LCD_write_byte(0x20,0);
	LCD5110_LCD_write_byte(0x0c,0);
	*/

	buffer[0] = 0x21;
	buffer[1] = 0xc6;
	buffer[2] = 0x06;
	buffer[3] = 0x13;
	buffer[4] = 0x20;
	buffer[5] = 0x0c;

	while(buf.status==B_BUSY){};

	buf.ocupacao = 6;
	buf.status = B_BUSY;
	buf.dado=buffer;
	LCD_write_IT(&buf, 1);

	while(buf.status==B_BUSY){};

}


// ainda necessária para fazer a configuração inicial
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


// -----------------------------------------------------------------------------------------------------------------------------
// ---- Função para enviar um bloco de dados ao display ----
/**
 * @brief Realiza a transmissão de dados por blocos
 *
 * @param data dados já formatados da informação que se quer escrever
 * @param tam tamanho dos dados
 * @param mode modo: (0)comando, (1)escrita
 */
HAL_StatusTypeDef LCD5110_write(uint8_t *data, uint16_t tam)
{
	HAL_StatusTypeDef status;

	if(buf.status == B_BUSY)
		return HAL_BUSY;

	// separa o buffer
	buf.status=B_BUSY;

	// preenche o buffer com os dados
	buf.dado=data;
	buf.ocupacao=tam;

	status = LCD_write_IT(&buf, 1);

	return status;
}//ok


//--------------------------------------------------------------------------------------
// --- função de transmissão no modo não bloqueante
HAL_StatusTypeDef LCD_write_IT(SharedBuffer_t *pbuf, uint8_t mode)
{
	HAL_StatusTypeDef status;

	// testa se a SPI está livre
	if(lcd->hspi->State!=HAL_SPI_STATE_READY)
		return HAL_BUSY;

	// define se é dado ou comando
	HAL_GPIO_WritePin(lcd->DC_Port, lcd->DC_Pin, mode);

	// habilita o chip select
	HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 0);

	// Agora o bloco será transmitido por interrupção
	status = HAL_SPI_Transmit_IT(lcd->hspi, pbuf->dado, pbuf->ocupacao);

	return status;

	// Só pode ser levantado depois do final da transmissão
	//HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 1);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == lcd->hspi->Instance)
	{
		// levanta o chip select, pois a transmissão acabou
		HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 1);
		// libera o buffer
		buf.status = B_FREE;

		/*	buf.dado = buf_2nd;
		 *
		 * 	Permite organizar uma fila de buffers.
		 * 	Quando terminar de transmitir, aponta pro próximo buffer de dados.
		 * 	Assim, é possível escrever os dados nos demais bufers,
		 * 	garantindo que eles sempre serão escritos.
		 */
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
	status = LCD_write_IT(&buf, 1);

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

	// escreve os dados no buffer
	buf.status=B_BUSY;
	buf.dado=(uint8_t *)TelaLimpa;
	buf.ocupacao=TAMBUF;

	// limpa o display escrevendo 0 em todos os pixels
	status = LCD_write_IT(&buf, 1);

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

	// escreve os comandos no buffer
	buf.dado[0] = 0x40|y;
	buf.dado[1] = 0x80|x;
	buf.ocupacao = 2;

	// envia o comando de setar a posição ao display
	status = LCD_write_IT(&buf, 0);

	return status;
}//ok


