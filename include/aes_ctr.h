#pragma once

#include <iostream>
#include <cmath>
#include "../include/aes.h"
#include <bitset>


#define NBITS       8
#define AESBITS     128
#define BUFFER_SIZE 16

void string2byte(uint8_t* output, std::string input);
std::string byte2string(uint8_t* input, int len);

void phex(uint8_t str[], int len);
void XNOR(uint8_t outArr[], uint8_t byteArr1[], uint8_t byteArr2[], int len);
void IntToByteArray(uint8_t* buffer, long long value);
void  extractElements(uint8_t srcArray[], uint8_t subArray[], int pos);
std::string AES_CTR(std::string str_pt, uint8_t key[16], int ictr);
std::string AES_CTR2(std::string str_pt, uint8_t key[16], int ictr);