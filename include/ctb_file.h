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

#define HEADERCNT   28      // Size of the main header parameters
#define PHEADERCNT  11      // Size of print properties header know parameters (ExtConfig)
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

//vector<uint8_t> with data from a layer image enconded using RLE7  
typedef  std::vector<uint8_t> ctbLayer;

struct layer_bmp 
{
    cv::Mat layer_pt;   //Bitmap of the layer information [plain text]
    cv::Mat layer_ct;   //Bitmap of the encrypted layer (for the new ctb file) [cipher text]
    cv::Mat layer_enc;  //Bitmap of the decrypting layer (for the LCD decryptor)
};

// Printer properties, these can be acquired from the headers of the CTB file
struct printer_prop
{
    //Fields in Main Header
    float layer_height_mm = 0.1f;       // layer height setting used at slicing, in millimeters
    float exposure_s = 15.f;            // exposure time setting used at slicing
    float bot_exposure_s = 50.f;        // bottom exposure time setting used at slicing
    float light_off_time_s = 0.f;       // light off time setting used at slicing
    float bot_light_off_time_s = 0.f;   // bottom light off time setting used at slicing
    uint32_t bot_layer_count = 10;      // number of layers configured as "bottom."
    uint32_t level_set_count = 0;       // number of times each layer image is repeated in the file.

    // Fields in Header ExtConfig
    float bot_lift_dist_mm = 5.f;        // distance to lift the build platform away from the vat after bottom layers
    float bot_lift_speed_mmpm = 60.f;    // speed at which to lift the build platform away from the vat after bottom layers
    float lift_dist_mm = 5.f;            // distance to lift the build platform away from the vat after normal layers
    float lift_speed_mmpm = 60.f;        // speed at which to lift the build platform away from the vat after normal layers
    float retract_speed_mmpm = 150.f;    // speed to use when the build platform re-approaches the vat after lift
    uint32_t width  = 2560;              // layer size in y direction in pixels
    uint32_t height = 1440;              // layer size in x direction in pixels

    // Fields Calculated
    uint16_t nlayers = 480;
};

std::string wstr2str(const std::wstring& wstr);
void print_layer_hex(const std::vector<uint8_t>& layer_data);

class CTB
{
    public:
        // -- Constructors
        CTB();
        CTB(std::wstring fname);
        void read_CTB(std::wstring fname);


        // -- Getters
        bool    ready();
        int     get_width();
        int     get_height();
        int     get_no_layers();
        uint32_t    get_key();
        cv::Mat     get_preview1();
        cv::Mat     get_preview2();
        ctbLayer    get_layer(int i);
        ctbLayer    get_file_header();
        std::vector<ctbLayer> get_all_layers();
        std::vector<uint32_t>    get_layer_len_addrs();

        
        cv::Mat getPreview(std::vector<uint16_t> data, int width, int height);
        cv::Mat getLayerImageRL1(std::vector<uint8_t> data, int width, int height);
        cv::Mat getLayerImageRL7(std::vector<uint8_t> data, int width, int height);

        
        ctbLayer encode_rle7(std::vector<uint8_t>& unencoded);
        ctbLayer decode_rle7(std::vector<uint8_t>& encoded);
        ctbLayer encode_rle7(cv::Mat unencoded);

        ctbLayer encode_rle7_byte(std::vector<uint8_t>& unencoded);
        ctbLayer decode_rle7_byte(std::vector<uint8_t>& unencoded);
  
        inline uint32_t get_runlen(std::vector<uint8_t>::iterator& it);
        uint32_t    decode  (std::vector<uint8_t>::iterator& it, int numbytes);
        
        ctbLayer    encrypt_decrypt_86(std::vector<uint8_t> data, uint32_t iv);
        inline void push_encoded(std::vector<uint8_t>& encoded, std::bitset<8>::reference& c, uint32_t runlen, std::bitset<2>& ref);

        void        decrypt_ctb_file(std::wstring output);
        void        encrypt_ctb_file(uint32_t key, std::wstring output);
        

        std::ofstream   create_ctb(const std::vector<uint8_t>& header, std::string ctbfname);
        void            add_layer_to_ctb(std::ofstream& ctbstrm, const std::vector<uint8_t>& layer_data, const std::uint32_t len_addr);
        printer_prop    getPrinterProperties();
    private:

        bool m_read;
        uint16_t m_no_layers;
        uint16_t m_layer_width;
        uint16_t m_layer_height;

        std::string m_ctb_fname;
        uint32_t m_encrypt_key;

        layer_bmp m_layer_bmps;

        cv::Mat m_preview1;
        cv::Mat m_preview2;

        std::vector<uint8_t>        m_header;
        uint32_t                    main_header[HEADERCNT];
        uint32_t                    print_header[PHEADERCNT];
        std::vector<ctbLayer>       m_layer_data;
        std::vector<std::uint32_t>  m_layer_i_len;
        std::vector<std::uint32_t>  m_layer_i_addr;
        std::vector<std::uint32_t>  m_layer_len_addr;     
};





