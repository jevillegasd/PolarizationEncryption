#pragma once
#include <iostream>
#include <fstream>
#include <bitset>
#include <sstream>
#include <cstring>
#include <fstream>
#include <streambuf>
#include <iomanip>
#include <stdio.h>
#include <vector>
#include "BitmapEx.h"
#include <filesystem>
#include <algorithm>
#include <cerrno>


const std::string CURRENT_PATH = std::filesystem::current_path().string();


#define BYTESIZE 8
#define BITSETLEN 32
#define VERBOSE 0


constexpr int BITMAP_HEADER_SIZE_DEPTH_8_1 = 62;
constexpr int BITMAP_HEADER_SIZE_DEPTH_24 = 54;

constexpr int SCREEN_WIDTH  = 2560;
constexpr int SCREEN_HEIGHT = 1440;

const int BITMAP_BIT_DEPTH = 8;

#define ONEBITDEPTHHEADER   \
R""""(42 4D 
00 00 00 00
00 00
00 00
3E 00 00 00

28 00 00 00
A0 05 00 00
00 0A 00 00
01 00
01 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00

FF FF FF 00
00 00 00 00

)""""

#define EIGHTBITDEPTHHEADER \
R""""(42 4D
00 00 00 00
00 00
00 00
3E 00 00 00

28 00 00 00
A0 05 00 00
00 0A 00 00
01 00
08 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
02 00 00 00
00 00 00 00

FF FF FF FF
00 00 00 00

)""""

int duplicatefile(std::string from, std::string to);



uint32_t decode(std::stringstream& strm, std::string run_byte, int n);



uint32_t handle_decode(std::stringstream& strm);



inline const char* hex2bin_lookup(char c);



inline std::string bin2hex_lookup(std::string str);



std::string hex2bin(const std::string& hex);



inline std::string bin2hex(std::string str);



std::string binstr2hexstr(const std::string& bitmap_str);


std::string bitrun_to_1bit_depth(const std::string& bitmap_str);



std::string bitrun_to_8bit_depth(const std::string& bitmap_str);



std::string layer_hex_to_bitrun(const std::string& layer);



void write_bitmap(std::string& layer, const char* outfname, int bitdepth, bool is_bitrun);




std::string bitmap_to_bitrun(const std::string& bitmap_str, int depth);



std::string space(const std::string& str, int size);



struct Encode_RLE7_Data
{
    int lay_hex_len;
    std::string lay_hex_data;
};
Encode_RLE7_Data encode_rle7(const std::string& bitmap_str);



void scale_all_bitmaps(int no_layers, int scale, int no_rounds, std::string type);



//void draw_bitmap(HWND Hwnd);



void generate_all_bitmaps_from_layer_vec_data(const std::vector<std::vector<uint8_t>>& all_layers_vec);



void generate_bitmap_from_layer_str_data(std::string& layer_str, int square_side, std::string type, int layer_no);



std::string get_bitrun(const std::vector<uint8_t>& layer_vec);



int generate_ctb(const std::vector<std::uint32_t>& layeri_addr_vec,
    const std::vector<std::uint32_t>& layeri_len_addr,
    std::string ctbfile, int no_layers, std::string type);



int generate_ctb_from_encrypted_layers(const std::vector<std::uint32_t>& layeri_addr_vec,
    const std::vector<std::uint32_t>& layeri_len_addr,
    const std::vector<std::string> encryptd_layers,
    std::string ctbfile, int no_layers);



std::string extract_square(const std::string& layer, int side);



void put_square(std::string& layer, const std::string& square, int side);



void print_square(const std::string& square, int side);



void paint_surrounding(std::string& layer_str, int square_side, std::string type);



std::string generate_decrypting_square(uint8_t* key, int nonce);



std::string generate_decrypting_layer(uint8_t* key, int nonce, int square_side);



std::string string_bitwise_xnor(std::string a, std::string b);



void compare_generated_vs_original( const std::string& original_lyr_vec,
                                    const std::string& encryptd_lyr_vec,
                                    const std::string& generated_lyr_vec,
                                    int layer_no);
