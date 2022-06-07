#pragma once

#include "ctb_file.h" 
#include "aes_ctr.h"

#define _WINNT_WIN32 0600

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h> 
#include <windows.h>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <algorithm>


const std::wstring LNULLSTR = L"";
const std::string  NULLSTR = "";

const std::string STRMAINMENU("\nHack Creality main menu. Select an option. "
    "\n\t1 - Load a ctb file."          "\n\t2 - Unlock ctb file."
    "\n\t3 - Lock ctb file."            "\n\t4 - Generate decryptor images."
    "\n\t5 - Setup printing sequence."  "\n\t6 - Run printer decryptor."
    "\n\t7 - Display test image."       "\n\t8 - Test RLE enconding."
    "\n\tQ - Exit.\n>> ");


// -- AES encryption properties
struct encryption_prop 
{
    uint8_t key[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };
    uint64_t nonce = 0;
    int extract_dim = 400;  // Dimension of the square area encrypted in pixels
    //int i_inilayer = 150, i_endLayer = 350; // First and last layers to encrypt
    int i_inilayer = 0, i_endLayer = 100; // First and last layers to encrypt
    int res = 6;           // 3D resolution of the encryption in pixels (x and y) and layers (in z)
    cv::Rect area;
};


struct screens
{
    std::vector<int>       index;
    std::vector<HMONITOR>  hm;
    std::vector<HDC>       hdc;
    std::vector<RECT>      rc;

    static BOOL CALLBACK MonitorEnum(HMONITOR hm, HDC hdc, LPRECT lprc, LPARAM pData)
    {
        screens* pthis = reinterpret_cast<screens*>(pData);

        pthis->hm.push_back(hm);
        pthis->hdc.push_back(hdc);
        pthis->rc.push_back(*lprc);
        pthis->index.push_back(pthis->hdc.size());
        return TRUE;
    }

    screens()
    {
        EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this);
    }
};

int saveFileDialog(std::wstring* file_name);
int openFileDialog(std::wstring* file_name);
cv::Mat rotateImage(cv::Mat image, float angle);

std::string displayimage(cv::Mat image, std::string window_name);
std::string displayimage_fs(cv::Mat image, std::string window_name);
std::string displayimage_fs(cv::Mat image, std::string window_name, int display_number);
std::string displayimage_fs(std::wstring file_name, std::string window_name);
std::string displayimage_fs(std::wstring file_name, std::string window_name, int display_number);
void plotandwait(cv::Mat image);

int generateDecryptorImages(CTB& myCTB, encryption_prop prop, std::filesystem::path save_path);
std::ofstream newCTB_fstream(CTB refCTB, std::filesystem::path save_path);
void progressBar(int progress, int max);
void progressBar(int progress, int max, std::string add_txt);
int runOfflineDecryptor(encryption_prop eprop, printer_prop pprop);

int messageListener(int* option);
void messageParser(int* option);

cv::Mat     enc2bmp(std::vector<uint8_t> enc, cv::Size area, int res);
layer_bmp   encrypt_area(cv::Mat image, cv::Rect area, uint8_t key[16], uint64_t ictr, int resolution);