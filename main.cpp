/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "I2C.h"
#include "ThisThread.h"
#include "mbed.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include <cstring>

#define tiempo_muestreo   1s
// Dirección del TMP102
#define TMP102_ADDRESS 0x90

//Pines y puertos 
BufferedSerial serial(USBTX, USBRX);
I2C i2c(D14, D15);
Adafruit_SSD1306_I2c oled(i2c, D0);
AnalogIn ain(A0);

// Variables globales 
float Vin = 0.0;
int ent = 0;
int dec = 0;
char men[40];

char comando[3] = {0x01, 0x60, 0xA0};
char data[2];

const char *mensaje_inicio = "Arranque del programa\n\r";

// Función para inicializar el OLED
void iniciarOLED() {
    oled.begin();
    oled.setTextSize(1);
    oled.setTextColor(1);
    oled.display();
    ThisThread::sleep_for(3000ms);
    oled.clearDisplay();
    oled.display();
    oled.printf("Test\r\n");
    oled.display();
}

// Función para inicializar el sensor de temperatura TMP102
void iniciarSensorTemperatura() {
    i2c.write(TMP102_ADDRESS, comando, 3);
}

// Función para enviar un mensaje serial
void enviarMensajeSerial(const char *mensaje) {
    serial.write(mensaje, strlen(mensaje));
}

// Función para leer el valor del sensor analógico
void leerSensorAnalogico() {
    Vin = ain * 3.3;
    ent = int(Vin);
    dec = int((Vin - ent) * 10000);
}

// Función para mostrar el voltaje en la pantalla OLED
void mostrarVoltajeOLED() {
    oled.clearDisplay();
    oled.display();
    sprintf(men, "El voltaje es:\n\r %01u.%04u volts \n\r", ent, dec);
    oled.setTextCursor(0, 2);
    oled.printf(men);
    oled.display();
}

// Función para leer la temperatura del sensor TMP102
void leerTemperatura() {
    comando[0] = 0; // Registro de temperatura
    i2c.write(TMP102_ADDRESS, comando, 1); // Enviar el comando para leer
    i2c.read(TMP102_ADDRESS, data, 2); // Leer 2 bytes
    int16_t temp = (data[0] << 4) | (data[1] >> 4);
    float Temperatura = temp * 0.0625;
    ent = int(Temperatura);
    dec = int((Temperatura - ent) * 10000);
}

// Función para mostrar la temperatura en la pantalla OLED
void mostrarTemperaturaOLED() {
    oled.clearDisplay();
    oled.display();
    sprintf(men, "La Temperatura es:\n\r %01u.%04u Celsius\n\r", ent, dec);
    oled.setTextCursor(0, 2);
    oled.printf(men);
    oled.display();
}

int main() {
    // Inicializaciones
    iniciarOLED();
    iniciarSensorTemperatura();
    enviarMensajeSerial(mensaje_inicio);

    while (true) {
        // Leer y mostrar el voltaje del sensor analógico
        leerSensorAnalogico();
        mostrarVoltajeOLED();
        enviarMensajeSerial(men);

        // Leer y mostrar la temperatura del sensor TMP102
        leerTemperatura();
        mostrarTemperaturaOLED();
        enviarMensajeSerial(men);

        // Espera hasta el próximo ciclo de muestreo
        ThisThread::sleep_for(tiempo_muestreo);
    }
}
