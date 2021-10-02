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

void LCD5110_init(LCD_HandleTypeDef *hlcd5110);

void LCD5110_write_char(unsigned char c);

void LCD5110_write_char_reg(unsigned char c);

// aula 4
HAL_StatusTypeDef LCD5110_clear(void);

// aula 4
HAL_StatusTypeDef LCD5110_set_XY(uint8_t x, uint8_t y);

void LCD5110_write_string(char *s);

void LCD5110_write_Dec(unsigned int buffer);

// ----------------------------------------------------------------------------------------------------------
// --- Funções adcionadas na aula 3 ---

void LCD_write(uint8_t *data, uint16_t tam, uint8_t mode);

void LCD_draw_char(char c, uint8_t *dat);

void LCD_write_char(unsigned char c);

uint16_t LCD_draw_string(char *s);

HAL_StatusTypeDef LCD5110_write_str(char *s);

// ----------------------------------------------------------------------------------------------------------
// --- Funções adcionadas na aula 4 ---

HAL_StatusTypeDef LCD_write_IT(uint8_t *data, uint16_t tam, uint8_t mode);

// ----------------------------------------------------------------------------------------------------------

void enableSleep();
		void disableSleep();
		void update();
		void clrScr();
		void fillScr();
		void invert(int mode);
		void setPixel(uint16_t x, uint16_t y);
		void clrPixel(uint16_t x, uint16_t y);
		void invPixel(uint16_t x, uint16_t y);
		void invertText(int mode);
		void print(char *st, int x, int y);
		//void print(String st, int x, int y);
		//void printNumI(long num, int x, int y, int length=0, char filler=' ');
		//void printNumF(double num, byte dec, int x, int y, char divider='.', int length=0, char filler=' ');
		void setFont(uint8_t* font);
		void drawBitmap(int x, int y, uint8_t* bitmap, int sx, int sy);
		void drawLine(int x1, int y1, int x2, int y2);
		void clrLine(int x1, int y1, int x2, int y2);
		void drawRect(int x1, int y1, int x2, int y2);
		void clrRect(int x1, int y1, int x2, int y2);
		void drawRoundRect(int x1, int y1, int x2, int y2);
		void clrRoundRect(int x1, int y1, int x2, int y2);
		void drawCircle(int x, int y, int radius);
		void clrCircle(int x, int y, int radius);
		void LCD5110_drawLine(int x1, int y1, int x2, int y2);
