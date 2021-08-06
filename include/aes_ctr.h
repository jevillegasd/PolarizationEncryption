#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include "../include/aes.h"
#include <bitset>


#define NBITS       8
#define AESBITS     128
#define BUFFER_SIZE 16

void string2byte(uint8_t* output, std::string input);
std::string byte2string(uint8_t* input, int len);

void phex(uint8_t str[], int len);
void xnor(uint8_t outArr[], uint8_t byteArr1[], uint8_t byteArr2[], size_t len);
void XOR (uint8_t outArr[], uint8_t* byteArr1, uint8_t* byteArr2, size_t len);

void IntToByteArray(uint8_t* buffer, long long value);
void extractElements(uint8_t srcArray[], uint8_t subArray[], int pos);
void extractElements(std::vector<uint8_t> srcArray, uint8_t subArray[], int pos);
std::string aes_ctr(std::string str_pt, uint8_t key[16], uint64_t ictr);
std::string aes_ctr2(std::string str_pt, uint8_t key[16], uint64_t ictr);

std::vector<uint8_t> aes_ctr(std::vector<uint8_t>  pt, uint8_t* key, uint64_t ictr);