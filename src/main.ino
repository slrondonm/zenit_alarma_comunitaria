/** ************************************************************ *
 *                                                               *
 *                   ZENIT ALARMA COMUNITARIA                    *
 *          Sistema de Alarma comunitaria con Perifoneo          *
 *         Creado y Diseñado por Ing. Sergio L. Rondón M.        *
 *                                                               *
 * ************************************************************* */

/**
 * Archivo de Programación para la Alarma Comunitaria Zenit
 *
 * Zenit Alarma Comunitaria, Archivo principal de Programación
 * inclución de librerias y de archivos necesarios
 * para el correcto funcionamiento de la Alarma Zenit
 *
 * Copyright (C) 2021 Ing. Sergio L. Rondon M.
 *
 * LICENSE: This file is part of Zenit.
 *
 * Zenit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Zenit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @category   Principal
 * @package    AlarmaComunitaria
 * @author     Ing. Sergio L. Rondón M. <sl.rondon.m@gmail.com>
 * @copyright  2021 Ing. Sergio L. Rondon M.
 * @license    https://www.gnu.org/licenses/gpl-3.0.txt  GPL-3.0-or-later
 * @version    SVN: $Id$
 * @link       http://pear.php.net/package/PackageName
 * @see        NetOther, Net_Sample::Net_Sample()
 * @since      File available since Release 1.2.0
 * @deprecated File deprecated in Release 2.0.0
 */

#define SIM800L_IP5306_VERSION_20190610

// Define the serial console for debug prints, if needed
#define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG SerialMon
#include <string.h>
#include "utilities.h"

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800   // Modem is SIM800
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to the module)
#define SerialAT Serial1

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

bool res = "";
char ctrz = 26;
String sms = "";

// Contacts List
int usersCount = 1; // Total Users
// Numbers Contacts users
String users[] = {
    "3212510238",
};

// Define relays pins
#define buzzer 15
#define relay_pin 18
#define relay_alarma 19

void setup()
{
    // Set console baud rate
    SerialMon.begin(115200);

    // Set pin OUTPUT
    pinMode(relay_pin, OUTPUT);
    pinMode(relay_alarma, OUTPUT);
    pinMode(buzzer, OUTPUT);

    delay(10);

    // Start power management
    if (setupPMU() == false)
    {
        Serial.println("Setting power error");
    }

    // Some start operations
    setupModem();

    // Set GSM module baud rate and UART pins
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

    delay(6000);

    SerialMon.println("Initializing modem...");
    modem.restart();

    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem Info: ");
    SerialMon.println(modemInfo);
    delay(1000);

    SerialAT.println("AT"); // Once the handshake test is successful, it will back to OK
    // updateSerial();
    delay(2);

    SerialAT.println("AT+CMGF=1\r"); // Configuring TEXT mode
    // updateSerial();
    delay(2);

    SerialAT.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
    // updateSerial();
    delay(2);

    SerialAT.println("AT+CMGD=1,4"); // Delete all SMS
    delay(2);

    SerialAT.println("AT+CMGL=\"ALL\",0 ");
    delay(2);

    SerialAT.println("ATS0=2"); // Set numbers of rings before Automatically Answering the Call
    // updateSerial();
    delay(2);

    SerialAT.println("AT+CHFA=1\r\n"); // Swap the audio channels
    // updateSerial();
    delay(2);

    SerialAT.println("AT+CRSL=100\r\n"); // Set ringer sound level
    // updateSerial();
    delay(2);

    SerialAT.println("AT+CLVL=100\r\n"); // Set loud speaker volume level
    // updateSerial();
    delay(2);

    SerialAT.println("AT+CLIP=1\r\n"); // Calling line identification presentation
    // updateSerial();
    delay(1000);

    for (int i = 0; i < usersCount; i++)
    {
        res = modem.sendSMS(users[i], String("La Alarma de incendio ha sido activada " + ctrz));
        DBG("SMS:", res ? "OK" : "fail");
        delay(100);

        // relay = 1;
        // return;
    }
}

void loop()
{
}
