// ctb_file.cpp : This file opens a ctb file and reads all its components
// Still it doesnt do anything with the encryption of layer data.
// Juan Villegas and Yusuf Jimoh, NYUAD, July 2021
#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <fstream>
#include <stdio.h>  //For file operations
#include <vector>
#include <cstring>
#include "aes_ctr.h"

#define STRB2BIN "%c%c%c%c%c%c%c%c"
#define BITSETLEN 32

#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), (byte & 0x40 ? '1' : '0'),  (byte & 0x20 ? '1' : '0'),  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), (byte & 0x04 ? '1' : '0'),  (byte & 0x02 ? '1' : '0'),  (byte & 0x01 ? '1' : '0')

#define HDATASIZE  4        // Size of header data in bytes
#define PRDATASIZE 2        // Size of preview data in bytes
#define LDATASIZE  36       // 

#define HEADERCNT   28
#define HEADERSIZE (HDATASIZE*HEADERCNT)
#define BUFFERSIZE  36

#define VERBOSE 0

#define SLC_HEADER_MCHADD 7
#define SLC_HEADER_MCHLEN 8
#define CTB_HEADER_LAYADD 16
#define CTB_HEADER_LAYLEN 17
#define CTB_HEADER_PNTADD 21
#define CTB_HEADER_PNTLEN 22
#define CTB_HEADER_SLCADD 26
#define CTB_HEADER_SLCLEN 27
#define CTB_MHEADER_RESX 13
#define CTB_MHEADER_RESY 14

constexpr int BITMAP_HEADER_SIZE_DEPTH_8_1 = 62;
constexpr int BITMAP_HEADER_SIZE_DEPTH_24 = 54;

#include <opencv2/opencv.hpp>
typedef  std::vector<uint8_t> ctbLayer;

struct layer_bmp {
    cv::Mat layer_pt;   //Bitmap of the layer information [plain text]
    cv::Mat layer_ct;   //Bitmap of the encrypted layer (for the new ctb file) [cipher text]
    cv::Mat layer_enc;  //Bitmap of the decrypting layer (for the LCD decryptor)
};

struct ctbData
{
    std::vector<ctbLayer>       layer_data;
    std::vector<std::uint32_t>  layer_i_addr;
    std::vector<std::uint32_t>  layer_len_addr;

    cv::Mat preview1;
    cv::Mat preview2;

    int layer_width;
    int layer_heigth;
};


using namespace std;
ctbData get_layer_data_from_ctb(const char* filename);
cv::Mat getPreview(std::vector<uint16_t> data, int width, int height);
cv::Mat getLayerImageRL1(std::vector<uint8_t> data, int width, int height);
cv::Mat getLayerImageRL7(std::vector<uint8_t> data, int width, int height);
uint32_t decode(std::vector<uint8_t>::iterator& it, int numbytes);
layer_bmp encrypt_area(cv::Mat image, cv::Rect area, uint8_t key[16], uint64_t ictr, int resolution);
cv::Mat enc2bmp(std::vector<uint8_t> enc, cv::Size area, int res);
