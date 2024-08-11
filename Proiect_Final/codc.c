#include <REG51.H>
sfr LCD = 0xA0;
sbit RS = P3^0;
sbit RW = P3^1;
sbit EN = P3^2;
sbit RELAY = P3^7;
sbit BUTTON_PLUS = P0^0;
sbit BUTTON_MINUS = P0^1;

unsigned char temp_ref = 20; // Tensiunea de referinta din incapere
unsigned char temp_actuala = 0; // Variabila pentru temperatura actuala
unsigned char humidity = 0; // Variabila pentru umiditate

void LCD_CMD(unsigned char x);
void LCD_DATA(unsigned char w);
void LCD_INI(void);
void Send_Data(unsigned char *Str);
void msDelay(unsigned int);
void display_actual_temp(unsigned char value);
void display_set_temp(unsigned char value);
void display_humidity(unsigned char value);
void ADC();
void check_buttons();
void timer1_delay_1ms(unsigned int delay);
void heart();
void caracter();
void omulet();

void main(void)
{
    P0 |= 0x03; // Setez butoanele initial ca fiind neapasate
    msDelay(200);  // 0.2 secunde
    LCD_INI(); // Initializarea LCD

    LCD_CMD(0x80);
    Send_Data("Temp.actuala:"); // Scrie mesaj pe LCD
    msDelay(200);  // 0.2 secunde
    LCD_CMD(0xC0);
    Send_Data("Temp.setata:"); // Scrie mesaj pe LCD		
    msDelay(200);  // 0.2 secunde
    LCD_CMD(0x90);
    Send_Data("Umiditatea:"); // Scrie mesaj pe LCD linia 3
    msDelay(200);  // 0.2 secunde
    LCD_CMD(0xD0);
    Send_Data("Caract.spec.:"); // Scrie mesaj pe LCD linia 4
    msDelay(200);  // 0.2 secunde

    heart(); 
    caracter();
    omulet();
    
    while (1) {
        check_buttons();
        ADC();
        display_actual_temp(temp_actuala);
        display_set_temp(temp_ref);
        display_humidity(temp_actuala);
    }
}

void ADC()
{
    temp_actuala = P1; // Simulare citire temperatura actuala de la un ADC
    humidity = P2; // Simulare citire umiditate de la un ADC

    // Control releu
    if (temp_ref > temp_actuala) { // Se verifica daca temperatura setata este mai mare decat cea actuala
        RELAY = 0; // Pornire releu (LED aprins)
    } else {
        RELAY = 1; // Oprire releu (LED stins)
    }
}

void check_buttons() {
    if (BUTTON_PLUS == 0) { // Se verifica daca "+" este apasat
        msDelay(20); // Debouncing
        while (BUTTON_PLUS == 0); // Asteapta eliberarea butonului
        temp_ref++;
    }

    if (BUTTON_MINUS == 0) { // Se verifica daca "-" este apasat
        msDelay(20); // Debouncing
        while (BUTTON_MINUS == 0); // Asteapta eliberarea butonului
        temp_ref--;
    }
}

void display_actual_temp(unsigned char value)
{
    unsigned char tens, units;

    // Set cursor position for actual temperature
    LCD_CMD(0x8D); // Prima linie
    value = P1;
    tens = (value / 0x06) / 10; // Obtinerea cifrei zecilor
    tens = tens + 0x30;
    units = (value / 0x06) % 10; // Obtinerea cifrei unitatilor
    units = units + 0x30;

    LCD_DATA(tens); // Afisarea primei cifre
    LCD_DATA(units); // Afisarea celei de-a doua cifre
    LCD_DATA(0xDF); // Afisarea simbolului gradului
}

void display_set_temp(unsigned char value)
{
    unsigned char tens, units;

    // Set cursor position for set temperature
    LCD_CMD(0xCD); // A doua linie

    tens = value / 10; // Obtinerea cifrei zecilor
    units = value % 10; // Obtinerea cifrei unitatilor

    LCD_DATA(tens + '0'); // Afisarea primei cifre
    LCD_DATA(units + '0'); // Afisarea celei de-a doua cifre
    LCD_DATA(0xDF); // Afisarea simbolului gradului
}

void display_humidity(unsigned char temp_value)
{
    unsigned char humidity_value;
    unsigned char tens, units;

    if (temp_value < 10) {
        humidity_value = 50 + 2 * (temp_value - 10); 
    } else {
        humidity_value = 50 - (10 - temp_value); 
    }

    if (humidity_value > 50) {
        humidity_value = 50;
    } else if (humidity_value < 0) {
        humidity_value = 0;
    }

    LCD_CMD(0x9D); 

    tens = humidity_value / 10; // Obtinerea cifrei zecilor
    units = humidity_value % 10; // Obtinerea cifrei unitatilor

    LCD_DATA(tens + '0'); // Afisarea primei cifre
    LCD_DATA(units + '0'); // Afisarea celei de-a doua cifre
    LCD_DATA('%'); // Afisarea simbolului procent
}


void LCD_CMD(unsigned char x)
{
    LCD = x;
    RS = 0;
    RW = 0;
    EN = 1;
    msDelay(5);
    EN = 0;
}

void LCD_DATA(unsigned char w)
{
    LCD = w;
    RS = 1;
    RW = 0;
    EN = 1;
    msDelay(5);
    EN = 0;
}

void Send_Data(unsigned char *Str)
{
    while(*Str) // Bucla pana cand s-au terminat datele
        LCD_DATA(*Str++); // Trimiterea datelor la LCD una cate una
}

void LCD_INI(void)
{
    msDelay(250); // Call delay
    LCD_CMD(0x38); // Comanada pe 8 bits a LCD-ului
    LCD_CMD(0x0C); // Display ON
    LCD_CMD(0x01); // Clear LCD
    LCD_CMD(0x06); // Entry mode set
    LCD_CMD(0x80); // Set cursor at beginning of first line
}

void msDelay(unsigned int Time)
{
    while (Time--)
        timer1_delay_1ms(1); // Delay de 1ms folosing Timer1
}

void timer1_delay_1ms(unsigned int delay)
{
    unsigned int i;
    TMOD |= 0x10;  // Timer1 in mode 1 
    TH1 = 0xEE;    // Initial value for 1ms delay at 11.0592 MHz
    TL1 = 0x00;
    TR1 = 1;       // Start Timer1

    for (i = 0; i < delay; i++) {
        while (!TF1); // Wait until Timer1 overflow
        TF1 = 0;      // Clear overflow flag
    }

    TR1 = 0;       // Stop Timer1
}

void heart()
{
    LCD_CMD(64); // Set CGRAM address for the heart character
    LCD_DATA(0x00);
    LCD_DATA(0x0A);
    LCD_DATA(0x1F);
    LCD_DATA(0x1F);
    LCD_DATA(0x0E);
    LCD_DATA(0x04);
    LCD_DATA(0x00);
    LCD_DATA(0x00);

    LCD_CMD(0xDD); // Set cursor position for heart character on line 4
    LCD_DATA(0); // Display heart character
    msDelay(10);
}

void caracter()
{
    LCD_CMD(72); // Set CGRAM address for the custom character
    LCD_DATA(0x00);
    LCD_DATA(0x0A);
    LCD_DATA(0x0A);
    LCD_DATA(0x0A);
    LCD_DATA(0x11);
    LCD_DATA(0x0E);
    LCD_DATA(0x00);
    LCD_DATA(0x00);

    LCD_CMD(0xDE); // Set cursor position for custom character on line 4
    LCD_DATA(1); // Display custom character
    msDelay(10);
}

void omulet()
{
    LCD_CMD(80); // Set CGRAM address for the custom character
    LCD_DATA(0x0E);
    LCD_DATA(0x0E);
    LCD_DATA(0x0E);
    LCD_DATA(0x04);
    LCD_DATA(0x1F);
    LCD_DATA(0x04);
    LCD_DATA(0x1F);
    LCD_DATA(0x11);

    LCD_CMD(0xDF); // Set cursor position for custom character on line 4
    LCD_DATA(2); // Display custom character
    msDelay(10);
}