// ctb_file.cpp : This file opens a ctb file and reads all its components
// Still it doesnt do anything with the encryption of layer data.
// Juan Villegas and Yusuf Jimoh, NYUAD, July 2021
#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <windows.h>
#include <iostream>
#include <fstream>
#include <stdio.h>  //For file operations
#include <vector>
#include <cstring>
#include <bitset>
#include <filesystem>
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


std::string wstr2str(const std::wstring& wstr);


struct layer_bmp 
{
    cv::Mat layer_pt;   //Bitmap of the layer information [plain text]
    cv::Mat layer_ct;   //Bitmap of the encrypted layer (for the new ctb file) [cipher text]
    cv::Mat layer_enc;  //Bitmap of the decrypting layer (for the LCD decryptor)
};


class CTB
{
    public:
        // -- Constructors
        CTB();
        CTB(std::wstring fname);
        void read_CTB(std::wstring fname);


        // -- Getters
        bool ready();
        int get_width();
        int get_height();
        int get_no_layers();
        uint32_t    get_key();
        cv::Mat     get_preview1();
        cv::Mat     get_preview2();
        ctbLayer    get_layer(int i);
        std::vector<uint8_t>    get_file_header();
        std::vector<ctbLayer>   get_all_layers();

        
        cv::Mat getPreview(std::vector<uint16_t> data, int width, int height);
        cv::Mat getLayerImageRL1(std::vector<uint8_t> data, int width, int height);
        cv::Mat getLayerImageRL7(std::vector<uint8_t> data, int width, int height);

        
        std::vector<uint8_t> encode_rle7(std::vector<uint8_t>& unencoded);
        std::vector<uint8_t> decode_rle7(std::vector<uint8_t>& encoded);
        inline uint32_t get_runlen(std::vector<uint8_t>::iterator& it);
        uint32_t    decode(std::vector<uint8_t>::iterator& it, int numbytes);
        cv::Mat     enc2bmp(std::vector<uint8_t> enc, cv::Size area, int res);
        std::vector<uint8_t> encrypt_decrypt_86(std::vector<uint8_t> data, uint32_t iv);
        layer_bmp   encrypt_area(cv::Mat image, cv::Rect area, uint8_t key[16], uint64_t ictr, int resolution);
        inline void push_encoded(std::vector<uint8_t>& encoded, std::bitset<8>::reference& c, uint32_t runlen, std::bitset<2>& ref);

        void        decrypt_ctb_file(std::wstring output);
        void        encrypt_ctb_file(uint32_t key, std::wstring output);
        std::ofstream    create_ctb(const std::vector<uint8_t>& header, std::string ctbfname);
        void        add_layer_to_ctb(std::ofstream& ctbstrm, const std::vector<uint8_t>& layer_data, const std::uint32_t len_addr);

    private:

        bool m_read;
        int m_no_layers;
        int m_layer_width;
        int m_layer_height;

        std::string m_ctb_fname;
        uint32_t m_encrypt_key;

        layer_bmp m_layer_bmps;

        cv::Mat m_preview1;
        cv::Mat m_preview2;

        std::vector<uint8_t>        m_header;
        std::vector<ctbLayer>       m_layer_data;
        std::vector<std::uint32_t>  m_layer_i_len;
        std::vector<std::uint32_t>  m_layer_i_addr;
        std::vector<std::uint32_t>  m_layer_len_addr;     
};





