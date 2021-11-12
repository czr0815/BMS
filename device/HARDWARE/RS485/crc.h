#include "stm32f10x.h"
#include "usart.h"

uint16_t crc16bitbybit(uint8_t *ptr, uint16_t len);
uint16_t crc16table(uint8_t *ptr, uint16_t len, uint8_t *crcl, uint8_t *crch);
uint16_t crc16tablefast(uint8_t *ptr, uint16_t len);
