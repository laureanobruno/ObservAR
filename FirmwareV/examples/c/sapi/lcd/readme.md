# Ejemplos de display LCD

### Hoja de datos

Se inincluye la hoja de datos del controlador Hitachi
HD44780 HD44780.pdf".

### Crear caracteres personalizados

https://maxpromer.github.io/LCD-Character-Creator/

### Uso de display LCD v�a I2C

Para usar el display por I2C mediante el I/O expander
I2C PCF8574T, se debe a�adir en el config.mk:

``` makefile
DEFINES+=LCD_HD44780_I2C_PCF8574T
```

Si no se a�ade el display funciona atrav�s de GPIOs.

### Tipograf�a

Adem�s la fuente tipogr�fica "HD44780.ttf" para dise�ar
pantallas.
