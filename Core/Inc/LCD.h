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

//---

void LCD5110_init(LCD_HandleTypeDef *hlcd5110);

HAL_StatusTypeDef LCD5110_write(uint8_t *data, uint16_t tam);

//---

void LCD_draw_char(char c, uint8_t *dat);

uint16_t LCD_draw_string(char *s);

HAL_StatusTypeDef LCD5110_write_str(char *s);

//---

HAL_StatusTypeDef LCD5110_clear(void);

HAL_StatusTypeDef LCD5110_set_XY(uint8_t x, uint8_t y);
