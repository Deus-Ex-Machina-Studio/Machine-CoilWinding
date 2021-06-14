#ifndef _COILWINDING_H_
#define _COILWINDING_H_

#include "Libraries/AccelStepper/src/AccelStepper.h"
#include "Libraries/AccelStepper/src/MultiStepper.h"
// #include "Libraries/CustomStepper/CustomStepper.hpp"

#define ENA 7 // Пин первой обмотки
#define ENB 2 // Пин второй обмотки

#define COOLER_PIN 9

#define STEPS 100 // Количество шагов на один оборот
#define MSPEED 100 // Масимальная скорость
#define SSPEED 100 // Базовая скорость

/// Намотка катушки
namespace CoilWinding {
    /// Педаль
    static Pedal pedal = Pedal();

    /// Обдув
    static CoolerPWM cooler = {COOLER_PIN};

    /// Может вращаться
    static bool b_canMove = false;
    /// Режим работы
    static bool b_mode = true;

    /// Скорость вращения
    static float VSpeed = 1.0f; // [0; 1]
    /// Первый ли запуск после включения питания
    static bool VFirstLoad = true;
    /// Максимальное кол-во оборотов INT
    static long int VMaxINT = 0; // size = 4 byte

    /// Максимальное кол-во оборотов ARRAY
    static int VMax[6] = { 0, 0, 0, 0, 0, 0, };

    /// Кол-во оборотов
    long countAxis = 0;
    /// Предыдущее кол-во оборотов
    long last_countAxis = 0;
    /// Последняя позиция
    long lastPosition = 0;
    /// Направление
    bool b_direction = true;

    /// Шаговый двигатель
    AccelStepper stepperMotor(AccelStepper::FULL4WIRE, 6, 5, 4, 3);
    /// Для вращения двигателей
    long positions[1] = { STEPS };

    /// Создание
    void Init() {
        pinMode(PEDAL_PIN, INPUT);

        pinMode(ENA, OUTPUT);
        pinMode(ENB, OUTPUT);
        pinMode(COOLER_PIN, OUTPUT);

        stepperMotor.setMaxSpeed(MSPEED); // Устанавливаем скорость вращения об./мин.
        stepperMotor.setSpeed(-SSPEED); // Устанавливаем скорость вращения об./мин.
    }

    /// Установить блок катушек
    void SetBlock(bool value) {
        digitalWrite(ENA, value);
        digitalWrite(ENB, value);
    }

    /// Перемещение
    void Move() {
        // степенная функция
        VSpeed = map(pow(pedal.currentValue, 5/4), 0, PEDAL_MAX - PEDAL_MIN, 0, 255) / 255.0f;
        //VSpeed = 0.25f;

        if (VSpeed != 0) {
            SetBlock(true);

            // countAxis = roundf(double(-stepperMotor.currentPosition()) / STEPS * 10) / 10;
            countAxis = -stepperMotor.currentPosition() / STEPS;

            stepperMotor.setSpeed(SSPEED * (b_direction ? -1 : 1) * VSpeed);
            stepperMotor.runSpeed();
        } else {
            SetBlock(false);
        }
    }
    
    /// Обновление
    void Update() {
        if (b_canMove) {
            pedal.Update();
            Move();
        } else {
            SetBlock(false);
        }
    }
};

#endif