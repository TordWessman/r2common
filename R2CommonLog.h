#ifndef __R2_COMMON_LOG_H__
#define __R2_COMMON_LOG_H__

#ifdef ARDUINO
#include <Arduino.h>
#define R2C_LOG(msg) Serial.println(msg)
#define R2C_LOG2(m1, m2) do { Serial.print(m1); Serial.println(m2); } while(0)
#define R2C_ERR(msg) Serial.println(msg)
#define R2C_ERR2(m1, m2) do { Serial.print(m1); Serial.println(m2); } while(0)
#else
#include <iostream>
#define R2C_LOG(msg) std::cout << msg << std::endl
#define R2C_LOG2(m1, m2) std::cout << m1 << m2 << std::endl
#define R2C_ERR(msg) std::cerr << msg << std::endl
#define R2C_ERR2(m1, m2) std::cerr << m1 << m2 << std::endl
#endif

#endif
