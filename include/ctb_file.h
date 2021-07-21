// ctb_file.cpp : This file opens a ctb file and reads all its components
// Still it doesnt do anything with the encryption of layer data.
// Juan Villegas Apr 6 2021
#pragma once

#include <iostream>
#include <fstream>
#include <stdio.h>  //For file operations
#include <vector>
#include <cstring>

#define STRB2BIN "%c%c%c%c%c%c%c%c"
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


struct Data
{
    std::vector<std::vector<uint8_t>> layer_data;
    std::vector<std::uint32_t> layer_i_addr;
    std::vector<std::uint32_t> layer_len_addr;
};

FILE* stream;

using namespace std;
Data get_layer_data_from_ctb(const char * filename)
{
    uint32_t buffer[BUFFERSIZE];
    uint32_t header[HEADERCNT];
    uint32_t stream_i, indi;
    int numread;
    int err;

    stream = fopen(filename, "rb"); //Open the file in binary mode
    if (VERBOSE) {
        if (errno == 0) { printf("The file %s was opened\n", filename); }
        else { printf("The file %s was not opened\n", filename); }
    }
    
    for (stream_i = 0; stream_i < HEADERSIZE; stream_i += HDATASIZE) {
        numread = fread(buffer, HDATASIZE, 1, stream);
        header[stream_i / HDATASIZE] = *buffer;
        if (VERBOSE) printf("\nGENERAL HEADER 0x%x \t %x", stream_i, *buffer);
    }

    // Read large preview image data
    uint32_t previewh_add, previewh_len;
    previewh_add = header[15];
    previewh_len = 8 * HDATASIZE;

    //First read the preview 1 header
    uint32_t prev1_header[8];
    while (stream_i < previewh_add - 1) { //Dump any parsing data
        fread(buffer, HDATASIZE, 1, stream);
        stream_i += HDATASIZE;
    }

    while (stream_i < previewh_add + previewh_len) {
        indi = (stream_i - previewh_add) / HDATASIZE;
        numread = fread(buffer, HDATASIZE, 1, stream);
        prev1_header[indi] = *buffer;
        if (VERBOSE) printf("\nPREVIEW1 HEADER 0x%x \t %x", stream_i, *buffer);
        stream_i += HDATASIZE;
    }

    //Then read the preview 1 data
    uint32_t prev1dat_add, prev1dat_len;
    prev1dat_add = prev1_header[2];
    prev1dat_len = prev1_header[3];
    int PACKSIZE = 1;

    uint16_t* prev1 = new uint16_t[prev1dat_len / PRDATASIZE];
    while (stream_i < prev1dat_len + prev1dat_add) {
        indi = (stream_i - prev1dat_add) / PRDATASIZE;
        numread = fread(buffer,PRDATASIZE, PACKSIZE, stream);
        prev1[indi] = *buffer;

        if (numread == 0) {
            if (VERBOSE) printf("\nError reading address 0x%x", stream_i);
        }
        else {
            if (VERBOSE) {
                if ((stream_i - prev1dat_add) % 16 == 0) { printf("\nPREVIEW1 ADDRESS 0x%x \t %x", stream_i, prev1[indi]); }
                else { printf("\t %x", prev1[indi]); }
            }
            stream_i += (PRDATASIZE * PACKSIZE);
        }
    }


    //Read the preview 2 parameters data
    uint32_t preview2h_add;
    preview2h_add = header[18];

    //First read the preview 2 header
    uint32_t prev2_header[8];
    while (stream_i < preview2h_add - 1) { //Dump any parsing data
        fread(buffer, HDATASIZE, 1, stream);
        stream_i += HDATASIZE;
    }

    while (stream_i < preview2h_add + previewh_len) {
        indi = (stream_i - preview2h_add) / HDATASIZE;
        numread = fread(buffer, HDATASIZE, 1, stream);
        prev2_header[indi] = *buffer;
        if (VERBOSE) printf("\nPREVIEW2 HEADER 0x%x \t %x", stream_i, *buffer);
        stream_i += (HDATASIZE);
    }

    //Then read the preview 2 data
    uint32_t prev2dat_add, prev2dat_len;
    prev2dat_add = prev2_header[2];
    prev2dat_len = prev2_header[3];

    uint16_t* prev2 = new uint16_t[prev2dat_len / PRDATASIZE];
    while (stream_i < prev2dat_len + prev2dat_add) {
        indi = (stream_i - prev2dat_add) / PRDATASIZE;
        numread = fread(buffer,  PRDATASIZE, PACKSIZE, stream);
        prev1[indi] = *buffer;

        if (numread == 0) {
            if (VERBOSE) printf("\nError reading address 0x%x", stream_i);
        }
        else {
            if (VERBOSE) {
                if ((stream_i - prev2dat_add) % 16 == 0) { printf("\nPREVIEW2 ADDRESS 0x%x \t %x", stream_i, prev1[indi]); }
                else { printf("\t %x", prev1[indi]); }
            }
            stream_i += (PRDATASIZE * PACKSIZE);
        }
    }



    //Read the print parameters data
    uint32_t pparam_add, pparam_len;
    pparam_add = header[21];
    pparam_len = header[22];

    uint32_t* print_header = new uint32_t[pparam_len / HDATASIZE];
    while (stream_i < pparam_add + pparam_len) {
        indi = (stream_i - pparam_add) / HDATASIZE;
        numread = fread(buffer, HDATASIZE, 1, stream);
        print_header[indi] = *buffer;
        if (VERBOSE) printf("\nPRINT HEADER 0x%x \t %x", stream_i, *buffer);
        stream_i += (HDATASIZE);
    } 

    //Read the slice parameters data
    uint32_t sparam_add, sparam_len;
    sparam_add = header[26];
    sparam_len = header[27];

    uint32_t* slice_header = new uint32_t[sparam_len / HDATASIZE];
    while (stream_i < sparam_add + sparam_len) {
        indi = (stream_i - sparam_add) / HDATASIZE;
        numread = fread(buffer, HDATASIZE, 1, stream);
        slice_header[indi] = *buffer;
        if (VERBOSE) printf("\nSLICE HEADER 0x%x \t %x", stream_i, *buffer);
        stream_i += (HDATASIZE);
    }

    //Read machine name
    uint32_t mname_len, mname_add;
    mname_add = slice_header[7];
    mname_len = slice_header[8];
    char* machine_name = new char[mname_len];
    char cbuff[1];

    while (stream_i < mname_add + mname_len) {
        indi = (stream_i - mname_add) / HDATASIZE;
        numread = fread(cbuff, 1, 1, stream);
        machine_name[indi] = *cbuff;
        stream_i += (1);
    }
    if (VERBOSE) printf("\nMACHINE NAME \t \t %.*s\n", (int)sizeof(machine_name), machine_name);

    //Read layer header data
    uint32_t layerh_add0, layerh_len, layerh_addi;
    layerh_add0 = header[16];
    layerh_len = header[17];
  
    std::vector<std::vector<uint32_t>> layer_headers;
    std::vector<std::vector<uint32_t>> layer_headers_idxs;

    while (stream_i < layerh_add0 + LDATASIZE * layerh_len) {
        layerh_addi = (stream_i);     //Address if the ith layer
        std::vector<uint32_t> layer_header;
        std::vector<uint32_t> layer_header_idx;
        while (stream_i < layerh_addi + LDATASIZE) {
            indi = (stream_i - layerh_addi) / HDATASIZE;
            numread = fread(buffer, HDATASIZE, 1, stream);
            layer_header.push_back(*buffer);
            layer_header_idx.push_back(stream_i);

            if (VERBOSE) {
                float tmp; //Temporary variable to store the printed float interpretation.
                std::memcpy(&tmp, &buffer, sizeof(float));
                if (indi < 3) {
                    printf("\nLAY%2i HEADER 0x%x 0x%x \t %x %f", (layerh_addi - layerh_add0) / LDATASIZE,
                        stream_i, indi, *buffer, tmp);
                }
                else {
                    printf("\nLAY%2i HEADER 0x%x 0x%x \t %x", (layerh_addi - layerh_add0) / LDATASIZE,
                        stream_i, indi, *buffer);
                }
            }
            stream_i += HDATASIZE;
        }
        layer_headers.push_back(layer_header);
        layer_headers_idxs.push_back(layer_header_idx);
    }

    //Read the layers information

    /* Exactly 84 bytes before the start of every layer data, the header is repeated (36 bytes) then some extra
    * information is given containing Lift Distance and Lift Speed, and perhaps Light PWM.
    */
    uint32_t layeri_add, layeri_len;

    Data D;
    
    //std::vector<std::vector<uint8_t>> layer_data;
    int k = 0;
    for (auto it = layer_headers.begin(); it != layer_headers.end(); ++it) {
        std::vector<uint32_t> layer_header = *it;
        layeri_add = layer_header[3];
        layeri_len = layer_header[4];
        D.layer_i_addr.push_back(layeri_add);
        D.layer_len_addr.push_back(layer_headers_idxs[k][4]);
        

        if(VERBOSE)
            printf("\n");

        while (stream_i < layeri_add - 1) { //Dump data before the start of the layer data (repeated header)
            fread(buffer, HDATASIZE, 1, stream);
            //printf("\nDUMP DATA 0x%x \t %x", stream_i, *buffer);
            stream_i += HDATASIZE;
        }

        uint8_t PIXELDSIZE = 1;
        int indi = 0;
        
        std::vector<uint8_t> single_layer_data;
        while (stream_i < layeri_add + layeri_len) { //Dump any parsing data
            indi = stream_i - layeri_add;
            fread(buffer, PIXELDSIZE, 1, stream);
            uint8_t pixel = *buffer;
            single_layer_data.push_back(pixel);

            if (VERBOSE) {
                if (indi % 8 == 0) printf("\nLAYER DATA 0x%x \t" STRB2BIN, stream_i, BYTE_TO_BINARY(pixel));
                else  printf(" " STRB2BIN, BYTE_TO_BINARY(pixel));
            }

            stream_i += PIXELDSIZE;
        }
        D.layer_data.push_back(single_layer_data);
        k++;
    }

    fclose(stream);

    return D;
}


