typedef enum {
	LCD_BLK=0, LCD_IT, LCD_DMA
}LCD_ModeTypeDef;

typedef struct
{
	SPI_HandleTypeDef *hspi;		// Handler pra SPI usada

	LCD_ModeTypeDef Mode;			// Guarda o modo de transmissão da SPI

	GPIO_TypeDef 	*CS_Port;		// Porta do Chip select
	uint16_t 		CS_Pin;			// Pino do CS

	GPIO_TypeDef 	*DC_Port;		// Porta do Dado ou comando
	uint16_t 		DC_Pin;			// Pino do DC

	GPIO_TypeDef 	*RS_Port;		// Porta do Reset
	uint16_t 		RS_Pin;			// Pino do DC

}LCD_HandleTypeDef;

//---

void LCD5110_init(LCD_HandleTypeDef *hlcd5110);

//---

HAL_StatusTypeDef LCD5110_write(uint8_t *data, uint16_t tam);

HAL_StatusTypeDef LCD_write_BLK(uint8_t *data, uint16_t tam, uint8_t mode);

HAL_StatusTypeDef LCD_write_DMA(uint8_t *data, uint16_t tam, uint8_t mode);

HAL_StatusTypeDef LCD_write_IT(uint8_t *data, uint16_t tam, uint8_t mode);

//---

void LCD_draw_char(char c, uint8_t *dat);

uint16_t LCD_draw_string(char *s);

HAL_StatusTypeDef LCD5110_write_str(char *s);

//---

HAL_StatusTypeDef LCD5110_clear(void);

HAL_StatusTypeDef LCD5110_set_XY(uint8_t x, uint8_t y);
