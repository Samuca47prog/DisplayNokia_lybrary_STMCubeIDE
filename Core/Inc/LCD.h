typedef struct
{
	SPI_HandleTypeDef *hspi;		// Handler pra SPI usada

	GPIO_TypeDef 	*CS_Port;		// Porta do Chip select
	uint16_t 		CS_Pin;			// Pino do CS

	GPIO_TypeDef 	*DC_Port;		// Porta do Dado ou comando
	uint16_t 		DC_Pin;			// Pino do DC

	GPIO_TypeDef 	*RS_Port;		// Porta do Reset
	uint16_t 		RS_Pin;			// Pino do DC

}LCD_HandleTypeDef;

/*
// estados do buffer
typedef enum {B_FREE=0, B_BUSY} BufStatus_t;

typedef struct{
	uint8_t *dado;
	BufStatus_t status;
	uint16_t ocupacao;
} SharedBuffer_t;
*/

void LCD5110_init(LCD_HandleTypeDef *hlcd5110);

// aula 4
HAL_StatusTypeDef LCD5110_clear(void);

// aula 4
HAL_StatusTypeDef LCD5110_set_XY(uint8_t x, uint8_t y);


// ----------------------------------------------------------------------------------------------------------
// --- Funções adcionadas na aula 3 ---

void LCD_write(uint8_t *data, uint16_t tam, uint8_t mode);

void LCD_draw_char(char c, uint8_t *dat);


uint16_t LCD_draw_string(char *s);

HAL_StatusTypeDef LCD5110_write_str(char *s);

// ----------------------------------------------------------------------------------------------------------
// --- Funções adcionadas na aula 4 ---

// HAL_StatusTypeDef LCD_write_IT(uint8_t *data, uint16_t tam, uint8_t mode);

//HAL_StatusTypeDef LCD_write_IT(SharedBuffer_t *pbuf, uint8_t mode);

// ----------------------------------------------------------------------------------------------------------
