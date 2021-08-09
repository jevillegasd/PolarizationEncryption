// ctb_file.cpp : This file opens a ctb file and reads all its components
// Still it doesnt do anything with the encryption of layer data.
// Juan Villegas and Yusuf Jimoh, 2021
#pragma once
#include "..\include\ctb_file.h"

using namespace std;

// -- For Debugging
void print_layer_hex(const vector<uint8_t>& layer_data)
{
    for (int i = 0; i < layer_data.size(); i++)
    {
        cout << hex << setw(2) << setfill('0') << uppercase << +layer_data[i];
    }
    std::cout << std::endl;
}


std::string wstr2str(const std::wstring& wstr)
{
    std::string str_out(wstr.length(), 0);
    int s_size = WideCharToMultiByte(CP_UTF8,
        0,
        &wstr[0],
        -1,
        &str_out[0],
        wstr.length(), NULL, NULL);

    return str_out;
}


CTB::CTB()
{
    this->m_read = false;

    this->m_no_layers = 0;
    this->m_layer_width = 0;
    this->m_layer_height = 0;
    
    this->m_ctb_fname = "";
}

CTB::CTB(wstring fname)
{
    this->m_no_layers = 0;
    this->m_layer_width = 0;
    this->m_layer_height = 0;

    this->read_CTB(fname);
}


void CTB::read_CTB(wstring fname)
{
    FILE* stream;

    std::string filename = wstr2str(fname);
    this->m_ctb_fname = filename;
    
    this->m_read = false; // -- Set to true at the end of this function

    // -- Empty vectors incase they have been used previously
    this->m_header.clear();
    this->m_layer_data.clear();
    this->m_layer_i_len.clear();
    this->m_layer_i_addr.clear();
    this->m_layer_len_addr.clear();
    

    int err;
    size_t numread;
    uint32_t stream_i, indi;
    uint32_t header[HEADERCNT];
    uint32_t buffer[BUFFERSIZE];


    stream = fopen(filename.c_str(), "rb"); //Open the file in binary mode
    if (VERBOSE) 
    {
        if (errno == 0) { printf("The file %s was opened\n", filename.c_str()); }
        else { printf("The file %s was not opened\n", filename.c_str()); }
    }

    for (stream_i = 0; stream_i < HEADERSIZE; stream_i += HDATASIZE) 
    {
        numread = fread(buffer, HDATASIZE, 1, stream);
        header[stream_i / HDATASIZE] = *buffer;
        if (VERBOSE) printf("\nGENERAL HEADER 0x%x \t %x", stream_i, *buffer);
    }

    // -- Get encrypton key
    this->m_encrypt_key = header[25];


    int layer_width = header[CTB_MHEADER_RESX];
    int layer_heigth = header[CTB_MHEADER_RESY];

    // Read large preview image data
    uint32_t previewh_add, previewh_len;
    previewh_add = header[15];
    previewh_len = 8 * HDATASIZE;

    //First read the preview 1 header
    uint32_t prev1_header[8];
    while (stream_i < previewh_add - 1) //Dump any parsing data
    { 
        fread_s(buffer, BUFFERSIZE, HDATASIZE, 1, stream);
        stream_i += HDATASIZE;
    }

    while (stream_i < previewh_add + previewh_len) 
    {
        indi = (stream_i - previewh_add) / HDATASIZE;
        numread = fread_s(buffer, BUFFERSIZE, HDATASIZE, 1, stream);
        prev1_header[indi] = *buffer;
        if (VERBOSE) printf("\nPREVIEW1 HEADER 0x%x \t %x", stream_i, *buffer);
        stream_i += HDATASIZE;
    }

    //Then read the preview 1 data
    int PACKSIZE = 1;
    uint32_t prev1dat_add, prev1dat_len;
    prev1dat_add = prev1_header[2];
    prev1dat_len = prev1_header[3];
    std::vector<uint16_t> preview1;

    while (stream_i < prev1dat_len + prev1dat_add) 
    {
        indi = (stream_i - prev1dat_add) / PRDATASIZE;
        numread = fread_s(buffer, BUFFERSIZE, PRDATASIZE, PACKSIZE, stream);
        if (numread == 0) 
        {
            if (VERBOSE) printf("\nError reading address 0x%x", stream_i);
        }
        else 
        {

            preview1.push_back(*buffer);

            if (VERBOSE) 
            {
                if ((stream_i - prev1dat_add) % 16 == 0) { printf("\nPREVIEW1 ADDRESS 0x%x \t %x", stream_i, (uint16_t)*buffer); }
                else { printf("\t %x", (uint16_t)*buffer); }
            }
            stream_i += (PRDATASIZE * PACKSIZE);
        }
    }

    //Read the preview 2 parameters data
    uint32_t preview2h_add;
    preview2h_add = header[18];

    //First read the preview 2 header
    uint32_t prev2_header[8];
    while (stream_i < preview2h_add - 1) //Dump any parsing data
    { 
        fread_s(buffer, BUFFERSIZE, HDATASIZE, 1, stream);
        stream_i += HDATASIZE;
    }

    while (stream_i < preview2h_add + previewh_len) 
    {
        indi = (stream_i - preview2h_add) / HDATASIZE;
        numread = fread_s(buffer, BUFFERSIZE, HDATASIZE, 1, stream);
        prev2_header[indi] = *buffer;
        if (VERBOSE) printf("\nPREVIEW2 HEADER 0x%x \t %x", stream_i, *buffer);
        stream_i += (HDATASIZE);
    }

    //Then read the preview 2 data
    uint32_t prev2dat_add, prev2dat_len;
    prev2dat_add = prev2_header[2];
    prev2dat_len = prev2_header[3];
    std::vector<uint16_t> preview2;

    while (stream_i < prev2dat_len + prev2dat_add) 
    {
        indi = (stream_i - prev2dat_add) / PRDATASIZE;
        numread = fread_s(buffer, BUFFERSIZE, PRDATASIZE, PACKSIZE, stream);

        if (numread == 0) 
        {
            if (VERBOSE) printf("\nError reading address 0x%x", stream_i);
        }
        else {
            preview2.push_back(*buffer);
            if (VERBOSE) 
            {
                if ((stream_i - prev2dat_add) % 16 == 0) { printf("\nPREVIEW2 ADDRESS 0x%x \t %x", stream_i, (uint16_t)*buffer); }
                else { printf("\t %x", (uint16_t)*buffer); }
            }
            stream_i += (PRDATASIZE * PACKSIZE);
        }
    }

    //Read the print parameters data
    uint32_t pparam_add, pparam_len;
    pparam_add = header[CTB_HEADER_PNTADD];
    pparam_len = header[CTB_HEADER_PNTLEN];

    uint32_t* print_header = new uint32_t[pparam_len / HDATASIZE];
    while (stream_i < pparam_add + pparam_len) 
    {
        indi = (stream_i - pparam_add) / HDATASIZE;
        numread = fread(buffer, HDATASIZE, 1, stream);
        print_header[indi] = *buffer;
        if (VERBOSE) printf("\nPRINT HEADER 0x%x \t %x", stream_i, *buffer);
        stream_i += (HDATASIZE);
    }

    //Read the slice parameters data
    uint32_t sparam_add, sparam_len;
    sparam_add = header[CTB_HEADER_SLCADD];
    sparam_len = header[CTB_HEADER_SLCLEN];

    uint32_t* slice_header = new uint32_t[sparam_len / HDATASIZE];
    while (stream_i < sparam_add + sparam_len) 
    {
        indi = (stream_i - sparam_add) / HDATASIZE;
        numread = fread(buffer, HDATASIZE, 1, stream);
        slice_header[indi] = *buffer;
        if (VERBOSE) printf("\nSLICE HEADER 0x%x \t %x", stream_i, *buffer);
        stream_i += (HDATASIZE);
    }

    //Read machine name
    uint32_t mname_len, mname_add;
    mname_add = slice_header[SLC_HEADER_MCHADD];
    mname_len = slice_header[SLC_HEADER_MCHLEN];
    char* machine_name = new char[mname_len];
    char cbuff[1];

    while (stream_i < mname_add + mname_len) 
    {
        indi = (stream_i - mname_add) / HDATASIZE;
        numread = fread(cbuff, 1, 1, stream);
        machine_name[indi] = *cbuff;
        stream_i += (1);
    }
    if (VERBOSE) printf("\nMACHINE NAME \t \t %.*s\n", (int)sizeof(machine_name), machine_name);

    //Read layer header data
    uint32_t layerh_add0, layerh_len, layerh_addi;
    layerh_add0 = header[CTB_HEADER_LAYADD];
    layerh_len = header[CTB_HEADER_LAYLEN];

    std::vector<std::vector<uint32_t>> layer_headers;
    std::vector<std::vector<uint32_t>> layer_headers_idxs;

    while (stream_i < layerh_add0 + LDATASIZE * layerh_len) 
    {
        layerh_addi = (stream_i);     //Address if the ith layer
        std::vector<uint32_t> layer_header;
        std::vector<uint32_t> layer_header_idx;
        while (stream_i < layerh_addi + LDATASIZE) 
        {
            indi = (stream_i - layerh_addi) / HDATASIZE;
            numread = fread(buffer, HDATASIZE, 1, stream);
            layer_header.push_back(*buffer);
            layer_header_idx.push_back(stream_i);

            if (VERBOSE) 
            {
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

    this->m_layer_width = layer_width;
    this->m_layer_height = layer_heigth;   
    
    int k = 0;
    for (auto it = layer_headers.begin(); it != layer_headers.end(); ++it) 
    {
        std::vector<uint32_t> layer_header = *it;
        layeri_add = layer_header[3];
        layeri_len = layer_header[4];

        this->m_layer_i_len.push_back(layeri_len);                     
        this->m_layer_i_addr.push_back(layeri_add);                    
        this->m_layer_len_addr.push_back(layer_headers_idxs[k][4]);


        if (VERBOSE) printf("\n");

        while (stream_i < layeri_add - 1) //Dump data before the start of the layer data (repeated header)
        { 
            fread(buffer, HDATASIZE, 1, stream);
            stream_i += HDATASIZE;
        }

        int indi = 0;
        uint8_t PIXELDSIZE = 1;
        
        while (stream_i < layeri_add + layeri_len) //Dump any parsing data
        { 
            indi = stream_i - layeri_add;
            fread(buffer, PIXELDSIZE, 1, stream);
            uint8_t pixel = *buffer;

            if (VERBOSE) 
            {
                if (indi % 8 == 0) printf("\nLAYER DATA 0x%x \t" STRB2BIN, stream_i, BYTE_TO_BINARY(pixel));
                else  printf(" " STRB2BIN, BYTE_TO_BINARY(pixel));
            }
            stream_i += PIXELDSIZE;
        }
        k++;
    }

    std::cout << "Finished processing header information ..." << std::endl;

    this->m_no_layers = m_layer_i_addr.size();

    std::cout << "Total no of layers: " << m_no_layers << std::endl;

    fclose(stream);

    this->m_preview1 = this->getPreview(preview1, (int)prev1_header[0], (int)prev1_header[1]);
    this->m_preview2 = this->getPreview(preview2, (int)prev2_header[0], (int)prev2_header[1]);

    std::cout << "Finished getting preview images ..." << std::endl;

    this->m_header      = get_file_header();
    this->m_layer_data  = get_all_layers();


    this->m_read = true;
}



// -- Returns a vector of all the layers
vector<vector<uint8_t>> CTB::get_all_layers()
{
    vector<vector<uint8_t>> layers_data;

    if (m_read)
    {
        return m_layer_data;
    }
    else
    {
        ifstream ctbfstream(this->m_ctb_fname, std::ifstream::binary);
        if (!ctbfstream)
        {
            std::cout << "Cannot open " << this->m_ctb_fname << std::endl;
            std::cout << "Failed to get all layers" << std::endl;
        }

        auto file_vec = std::vector<uint8_t>(std::istreambuf_iterator<char>(ctbfstream), std::istreambuf_iterator<char>());
        ctbfstream.close();


        vector<uint8_t> lyr_data;
        for (int i = 0; i < m_no_layers; i++)
        {
            lyr_data.clear();

            if (VERBOSE)
            {
                if ((i % 10) == 0)
                {

                    std::cout << "Loading layer " << i << " of " << m_no_layers << std::endl;
                }
            }

            uint32_t pos = m_layer_i_addr[i];
            uint32_t len = m_layer_i_len[i];

            for (int j = pos; j < len + pos; j++)
            {
                lyr_data.push_back(file_vec[j]);
            }

            layers_data.push_back(lyr_data);
            
        }
    }

    return layers_data;

}



// -- Returns layer i
vector<uint8_t> CTB::get_layer(int i)
{
    vector<uint8_t> d;
    if (m_read)    // -- Check if the file has been parsed for layer_i_addr and layer_i_len
    {
        return m_layer_data[i];
    }
    else
    {
        std::cout << "Layers have not been prepared yet. Returning empty vector ..." << std::endl;
        return d;
    }
}


int CTB::get_height()
{
    return m_layer_height;
}


int CTB::get_width()
{
    return m_layer_width;
}


int CTB::get_no_layers()
{
    return m_no_layers;
}


bool CTB::ready()
{
    return m_read;
}

uint32_t CTB::get_key()
{
    if (!m_read)
    {
        std::cout << "No file loaded yet. Returning empty uint32_t ..." << std::endl;
    }
    return this->m_encrypt_key;
}


cv::Mat CTB::get_preview1()
{
    if (!m_read)
    {
        std::cout << "No file loaded yet. Returning empty cv::Mat ..." << std::endl;
    }
    return this->m_preview1;
}


cv::Mat CTB::get_preview2()
{
    if (!m_read)
    {
        std::cout << "No file loaded yet. Returning empty cv::Mat ..." << std::endl;
    }
    return this->m_preview2;
}


//This function draws an image following the BMP file specification for a color image.
cv::Mat CTB::getPreview(std::vector<uint16_t> data, int width, int height) 
{
    std::cout << "Getting preview data ..." << std::endl;
    cv::Mat image(height , width, CV_8UC3);
    int x = 0, y = 0;

    for (auto it = data.begin(); it != data.end(); ++it) {
        uint16_t dat = *it;
        uint8_t R, G, B, run;

        B = ((dat) & 0x1f) << 3;
        G = ((dat >> 6) & 0x1f) << 3;
        R = ((dat >> 11) & 0x1f) << 3;
        run = (dat >> 5) & 0x01;
        cv::Vec3b color(R,G,B);

        if (run) {
            it++;
            uint16_t run_length = *it & 0x0FFF;
            for (int i = 0; i < run_length + 1; i++) {
                image.at<cv::Vec3b>(cv::Point(x, y)) = color;
                x++;
                if (x >= width) {
                    y++; x = 0;
                }
            }
        }
        else {
            image.at<cv::Vec3b>(cv::Point(x, y)) = color;
            x++;
            if (x >= width) {
                y++; x = 0;
            }
        }
    }
    return image;
}


//Generates an image following the layer specification with no antialiasing
cv::Mat CTB::getLayerImageRL1(std::vector<uint8_t> data, int width, int height) 
{
    cv::Mat image(width, height,CV_8UC3);
    int x = 0, y = 0;

    for (uint8_t dat : data) {
        uint8_t R, G, B, run;

        B = ((dat) & 0x80) ? 0xFF : 0x00;
        G = ((dat) & 0x80) ? 0xFF : 0x00;
        R = ((dat) & 0x80) ? 0xFF : 0x00;
        uint8_t run_length = dat & 0x7F;
        cv::Vec3b color(R, G, B);

        if (run_length>0) {
                for (int i = 0; i < run_length + 1; i++) {
                    image.at<cv::Vec3b>(cv::Point(x, y)) = color;
                    x++;
                    if (x >= width)
                        y++; x = 0;
                }
        }
    }   

    return image;
}

//Generates an image following the layer specification with antialiasing
cv::Mat CTB::getLayerImageRL7(std::vector<uint8_t> data, int width, int height) 
{
    cv::Mat image(width,height, CV_8UC3);
    int x = 0, y = 0;

   
    for (auto it = data.begin(); it != data.end(); ++it) {
        uint8_t dat = *it;
        uint8_t R, G, B, run;
        long run_length;

        B = (((dat) & 0x7F) << 1) + 1;
        G = (((dat) & 0x7F) << 1) + 1;
        R = (((dat) & 0x7F) << 1) + 1;

        run = dat & 0x80;
        cv::Vec3b color(R, G, B);

        if (run) {
            it++;
            uint8_t dat = *it;

            if (!(dat >> 7))
                run_length = decode(it, 0);
            else if ((dat >> 6) == 0b10)
                run_length = decode(it, 1);
            else if ((dat >> 5) == 0b110)
                run_length = decode(it, 2);
            else if ((dat >> 4) == 0b1110)
                run_length = decode(it, 3);
            else
            {
                std::cout << "Unrecognized RLE7 byte" << std::endl;
                return image;
            }

            for (long i = 0; i < run_length; i++) {
                if (!((x < width) && (y < height))) {
                    printf("\n%i - %i : remain %i", x, y, run_length-i);
                    //assert((x < width& y < heigth));
                    return image;
                }
                    
                image.at<cv::Vec3b>(cv::Point(y,x)) = color;
                x++;
                if (x >= width) {
                    y++; x = 0;
                }
            }
        }
        else {
            image.at<cv::Vec3b>(cv::Point(y, x)) = color;
            x++;
            if (x >= width) {
                y++; x = 0;
            }
        }
    }
    
    return image;
}


//RLE7 decodification for the bmp specification
uint32_t CTB::decode(std::vector<uint8_t>::iterator& it, int numbytes)
{
    uint8_t data = *it;
    uint16_t temp2 = 0x7F;
    long length = data & (temp2 >> numbytes);
    for (int i = 0; i < numbytes; i++) {
        it++;
        length = (length << 8) + *it;
    }
    return length;
}


layer_bmp CTB::encrypt_area(cv::Mat image, cv::Rect area, uint8_t key[16], uint64_t ictr, int res) 
{
    int numx = (int) ceil(area.width  * 1. / res);
    int numy = (int) ceil(area.height * 1. / res);
    int ct_len = numx * numy / NBITS;
    ct_len = (ct_len < AES_BLOCKLEN) ? AES_BLOCKLEN : ct_len;
    layer_bmp out;
    image.copyTo(out.layer_pt);

    cv::Mat pi = image(area); //Plain Text Selected Area

    // We define is just and empty plain text, so that we carry the XOR operation outside of the function
    std::vector<uint8_t> pt(ct_len);  
    std::vector<uint8_t> enc = aes_ctr(pt, key, ictr);

    //Map the encrypted text to a bmp using 1 px = 1 bit 
    cv::Mat enci = enc2bmp(enc,area.size(),res);
    

    //Build encrypting layer image.
    cv::Mat layer_enc(image.size(), CV_8UC3, cv::Scalar(0x00, 0x00, 0x00));
    cv::Mat submat1 = layer_enc(area);
    enci.copyTo(submat1);
    layer_enc.copyTo(out.layer_enc);


    //Encryption of pt and build encrypted image
    cv::Mat layer_ct(image.size(), CV_8UC3, cv::Scalar(0x00, 0x00, 0x00));
    image.copyTo(layer_ct);
    cv::bitwise_xor(image, layer_enc, layer_ct);
    layer_ct.copyTo(out.layer_ct);

     return out;
}


//Maps a every bit of the input to a binary image of size area*res
cv::Mat CTB::enc2bmp(std::vector<uint8_t> enc, cv::Size area, int res) 
{
    cv::Mat enci(area.width, area.height, CV_8UC3);
    int numx = (int)ceil(area.width * 1. / res);

    for (int y = 0; y < area.height; y++) {
        for (int x = 0; x < area.width; x++) {
            uint8_t biti = (y / res) * numx + x / res;
            uint8_t byte_pos = biti / NBITS;
            uint8_t bit_pos = biti % NBITS;
            uint8_t ip = ((enc[byte_pos] >> bit_pos) & 0x01);
            uint8_t op = ((enc[byte_pos] >> bit_pos) & 0x01) ? 0xFF : 0x00;
            cv::Vec3b color(op, op, op);
            enci.at<cv::Vec3b>(cv::Point(x, y)) = color;
        }
    }
    return enci;
}



std::vector<uint8_t> CTB::encrypt_decrypt_86(std::vector<uint8_t> data, uint32_t iv)
{
    /// <summary>
    /// This function encrypts or decrypts a layer using an XOR based stream cipher. 
    /// This means that decryption and encryption uses the same mechanism. 
    /// So if you pass an encrypted layer through this function, you get out a decrypted layer and vice versa.
    /// </summary>
    /// <param name="key"> Key that was used to encrypt/decrypt the data</param>
    /// <param name="data"> Uint8_t vector of the layer data</param>
    /// <param name="iv"> The current layer index i.e 0 for bottom layer, 1 for the next layer and so on</param>
    /// <returns> Function returns a vector of the encrypted/decrypted layer data</returns>

    std::vector<uint8_t> result;

    // Multiplication and Addition is in modulo 2^32. operations * and + are automatically modulo 2^32 for uint32_t.
    uint32_t c = this->m_encrypt_key * 0x2D83'CDAC + 0xD8A8'3423;

    uint32_t X = (iv * 0x1E15'30CD + 0xEC3D'47CD) * c;

    int n = 0;
    while (n < data.size())
    {
        for (int i = 0; i < 4; i++) // The data from the layer data is read in Little Endiannes format
        {
            if (n < data.size())
            {
                result.push_back(data[n] ^ (uint8_t)(X >> (i * 8)));
                n++;
            }
            else break;
        }

        X = X + c;
    }

    return result;
}



inline void CTB::push_encoded(vector<uint8_t>& encoded, bitset<8>::reference& c, uint32_t runlen, bitset<2>& ref)
{
    if (runlen == 1)
    {
        if (c == ref[0]) encoded.push_back(0x00);
        else encoded.push_back(0x7F);
    }

    else
    {
        if (c == ref[0]) encoded.push_back(0x80);
        else encoded.push_back(0xFF);

        if (runlen < 128)
            encoded.push_back(static_cast<uint8_t>(runlen));

        else if (runlen < 268435456) // 2 ^ 28 (max acceptable runlen)
        {
            int n;
            if (runlen < 16384) n = 2;          // 2 ^ 14
            else if (runlen < 2097152) n = 3;   // 2 ^ 21
            else n = 4;                         // 2 ^ 28


            vector<uint8_t> a;

            // -- Create bytes (uint8_t) from runlen
            for (int j = n - 1; j >= 0; j--)
                a.push_back(static_cast<uint8_t>(runlen >> (j * 8)));

            // -- Set bit 7 - i
            for (int k = 0; k < (n - 1); k++)
                a[0] |= ( 1 << (7 - k) );

            // -- Push encoded bytes
            for (int l = 0; l < n; l++)
                encoded.push_back(a[l]);

        }
        else
        {
            std::cout << "Invalid Run Length: " << runlen << std::endl;
        }
    }

    if (VERBOSE)
    {
        std::cout << "(Encode_RLE7) runlen: " << dec << runlen << std::endl;
    }
}


// RLE7 Encoding from Image Mat structrure
ctbLayer CTB::encode_rle7(cv::Mat bitmap)
{
    

    vector < uint8_t> onebyte;

    
    cv::Mat greyMat;
    if (bitmap.channels() > 1)
        cv::cvtColor(bitmap, greyMat, cv::COLOR_BGR2GRAY);
    else
        bitmap.copyTo(greyMat);

    for (int x = 0; x < greyMat.cols; x++) {
        for (int y = 0; y < greyMat.rows; y++) {
            uint8_t pixel = (uint8_t) greyMat.at<uchar>(y, x)>>1;
            onebyte.push_back(pixel);
        }
    }

    vector<uint8_t> encoded = onebyte;
    // call vector<uint8_t> encoded = this->encode_rle7(vector<uint8_t>& onebyte)
    return encoded;
}


vector<uint8_t> CTB::encode_rle7_byte(vector<uint8_t>& unencoded)
{
    vector<uint8_t> encoded;
    auto it = unencoded.begin();

    uint8_t curr = *it;

    uint32_t runlen = 1;


    while (++it != unencoded.end())
    {
        if (*it == curr) runlen++;
        else
        {
            if (runlen == 1)
            {
                encoded.push_back(curr);
            }
            else
            {
                curr |= 1 << 7; // -- Set the 7th bit to indicate there is a run

                encoded.push_back(curr);

                if (runlen < 128)
                {
                    encoded.push_back(static_cast<uint8_t>(runlen));
                }

                else if (runlen < 268435456) // 2 ^ 28 (max acceptable runlen)
                {
                    int n;
                    if (runlen < 16384) n = 2;          // 2 ^ 14
                    else if (runlen < 2097152) n = 3;   // 2 ^ 21
                    else n = 4;                         // 2 ^ 28


                    vector<uint8_t> a;

                    // -- Create bytes (uint8_t) from runlen
                    for (int j = n - 1; j >= 0; j--)
                    {
                        a.push_back(static_cast<uint8_t>(runlen >> (j * 8)));
                    }

                    // -- Set bit 7 - i
                    for (int k = 0; k < (n - 1); k++)
                    {
                        a[0] |= (1 << (7 - k));
                    }

                    // -- Push encoded bytes
                    for (int l = 0; l < n; l++)
                    {
                        encoded.push_back(a[l]);
                    }

                }
                else
                {
                    std::cout << "Invalid Run Length: " << runlen << std::endl;
                }

                runlen = 1;
                curr = *it;
            }
        }
    }


    // -- Push final encoded data
    curr |= 1 << 7; // -- Set the 7th bit to indicate there is a run

    encoded.push_back(curr);

    if (runlen < 128)
    {
        encoded.push_back(static_cast<uint8_t>(runlen));
    }

    else if (runlen < 268435456) // 2 ^ 28 (max acceptable runlen)
    {
        int n;
        if (runlen < 16384) n = 2;          // 2 ^ 14
        else if (runlen < 2097152) n = 3;   // 2 ^ 21
        else n = 4;                         // 2 ^ 28


        vector<uint8_t> a;

        // -- Create bytes (uint8_t) from runlen
        for (int j = n - 1; j >= 0; j--)
        {
            a.push_back(static_cast<uint8_t>(runlen >> (j * 8)));
        }

        // -- Set bit 7 - i
        for (int k = 0; k < (n - 1); k++)
        {
            a[0] |= (1 << (7 - k));
        }

        // -- Push encoded bytes
        for (int l = 0; l < n; l++)
        {
            encoded.push_back(a[l]);
        }

    }
    else
    {
        std::cout << "Invalid Run Length: " << runlen << std::endl;
    }

    return encoded;
}



// RLE7 Encoding scheme -- optimized for memory
vector<uint8_t> CTB::encode_rle7(vector<uint8_t>& unencoded)
{
    vector<uint8_t> encoded;
    auto it = unencoded.begin(); 
    
    bitset<2> ref("10"); // -- bitset::reference to 1 or 0
    bitset<8> initl(*it); 
    bitset<8>::reference c = initl[7]; // -- Starting bit

    int id = 0;
    // -- Choose starting reference bit according to the starting bit
    if (c == ref[0])  c = ref[id];
    else              c = ref[++id % 2];

    bitset<8> curr(*it);
    uint32_t runlen = 0;

    while (it != unencoded.end())
    {
        for (int i = 7; i >= 0; i--)
        {
            if (curr[i] == c)  runlen += 1;
            else
            {
                push_encoded(encoded, c, runlen, ref);

                c = ref[++id % 2]; // - Toggle previous bit
                runlen = 1;        // - Reset runlen
            }     
        }

        it++;
        // -- If end of vector unencoded, then push final encoded data
        if (it != unencoded.end())  curr = *it;
        else push_encoded(encoded, c, runlen, ref);
    }

    if (VERBOSE)
    {
        std::cout << "Encoded length: " << dec << encoded.size() << std::endl;
    }
        
    return encoded;
}


// -- Get current run length
inline uint32_t CTB::get_runlen(vector<uint8_t>::iterator& it)
{
    it++;
    uint8_t dat = *it;
    uint32_t run_length = 0;

    if (!(dat >> 7))
        run_length = decode(it, 0);
    else if ((dat >> 6) == 0b10)
        run_length = decode(it, 1);
    else if ((dat >> 5) == 0b110)
        run_length = decode(it, 2);
    else if ((dat >> 4) == 0b1110)
        run_length = decode(it, 3);
    else
    {
        std::cout << "Unrecognized RLE7 Byte" << std::endl;
    }

    return run_length;
}


vector<uint8_t> CTB::decode_rle7_byte(vector<uint8_t>& encoded)
{
    vector<uint8_t> decoded;

    for (auto it = encoded.begin(); it != encoded.end(); ++it)
    {
        uint8_t run;
        uint8_t dat = *it;
        uint32_t run_length;

        run = dat & 0x80;
        uint8_t pixel = (dat & ~(1 << 7)); // -- Clear the last 7th bit

        if (run)
        {
            run_length = get_runlen(it);
            for (int i = 0; i < run_length; i++)
            {
                decoded.push_back(pixel);
            }

        }
        else
        {
            decoded.push_back(pixel);
        }
    }

    return decoded;
 
}



// - RLE7 Decompressing/Decoding -- implementation for memory optimization
vector<uint8_t> CTB::decode_rle7(vector<uint8_t>& encoded)
{
    vector<uint8_t> decoded;

    bitset<8> buf;
    bitset<2> ref("10");

    int a = 0;
    int n = 0;
    int rem = 0;
    int stop = 0;
    uint32_t sum = 0;

    for (auto it = encoded.begin(); it != encoded.end(); ++it) 
    {
        uint8_t run;
        uint8_t dat = *it;
        uint32_t run_length;

        run = dat & 0x80;
        auto pixel = ((dat & 0x7F) == 0x00) ? ref[0] : ref[1];

        if (run)
        {
            run_length = get_runlen(it);

            if (run_length >= 8)
            {
                a = (8 - stop) % 8;
                n = (run_length - a) / 8;
                rem = (run_length - a) % 8;
            }
            else
            {
                n = 0;
                a = (8 - stop) % 8;
                rem = run_length - a;
                rem = (rem < 0) ? 0 : rem;
            }
            
            if (stop != 0)
            {
                while ((stop < 8) && (a > 0))
                {
                    buf[stop] = pixel;
                    stop++; a--;
                }
                stop = stop % 8;
                if (stop == 0) decoded.push_back(static_cast<uint8_t>(buf.to_ulong()));
            }

            if (n > 0)
            {
                if (pixel == ref[0])
                {

                    for (int i = 0; i < n; i++)
                        decoded.push_back(0x00);
                    if (VERBOSE)
                        std::cout << "pushed 0: runlen " << dec << run_length << "no of elements: " << n << "\n";
                }
                else
                {
                    for (int i = 0; i < n; i++)
                        decoded.push_back(0xFF);
                    if (VERBOSE)
                        std::cout << "pushed 1: runlen " << dec << run_length << "no of elements: " << n << "\n";
                }
            }

            if (rem > 0)
            {
                for (int i = 0; i < rem; i++)
                {
                    buf[i] = pixel;
                    std::cout << "inside rem\n";
                }
                stop = rem;
            }
            

            sum += run_length;
        }
        else
        {
            run_length = 1;

            if (stop != 0)
            {
                buf[stop] = pixel;
                stop++;

                stop = stop % 8;

                if (stop == 0)
                {
                    decoded.push_back(static_cast<uint8_t>(buf.to_ulong()));
                }
            }
            else
            {
                buf[0] = pixel;
                stop++;
            }
            
            sum++;
        }
    }

    if (VERBOSE)
    {
        std::cout << "Total bits: " << std::dec << sum << std::endl;
        std::cout << "Decoded data size (bytes): " << dec << decoded.size() << std::endl;
    }
    
    return decoded;
}


void CTB::decrypt_ctb_file(wstring output)
{
    if (m_encrypt_key == 0x0000'0000)
    {
        std::cout << "CTB file " << m_ctb_fname << " is not encrypted. returning ..." << std::endl;
        return;
    }
    // - get the header (excluding layer data) from the ctb file
    auto header = get_file_header();

    // -- Set the key to 0x00000000
    for (int i = 100; i < 104; i++)
    {
        header[i] = 0x00;
    }
        
    string outfilename = wstr2str(std::filesystem::current_path().wstring()) + "\\models\\outputs\\" + wstr2str(output);
    ofstream ctbfilestrm = create_ctb(header, outfilename);

    // -- Free memory
    header.clear();
    header.shrink_to_fit();

    vector<uint8_t> decrypted;

    std::cout << "Decrypting...";
    for (int j = 0; j < this->m_no_layers; j++)
    {
        decrypted = encrypt_decrypt_86(get_layer(j), j);
        add_layer_to_ctb(ctbfilestrm, decrypted, m_layer_len_addr[j]);
        std::cout <<".";
    }

    std::cout << "\nFinished decrypting file " << this->m_ctb_fname << std::endl << std::endl;
    std::cout << "Generated decrypted CTB file " << outfilename << std::endl;

    ctbfilestrm.close();
}


// Enter key in Little Endiannes Format
void CTB::encrypt_ctb_file(uint32_t key, wstring output)
{
    if (m_encrypt_key != 0x0000'0000)
    {
        std::cout << "CTB file " << this->m_ctb_fname << " is already encrypted. Returning ..." << std::endl;
        std::cout << "Key is: " << this->m_encrypt_key << std::endl;
        std::cout << "Alternatively decrypt and then re-encrypt with another key" << std::endl;
        return;
    }

    m_encrypt_key = key;

    // -- Get the header (excluding layer data) from the ctb file
    auto header = this->get_file_header();

    // -- Set the key
    for (int i = 100; i < 104; i++)
    {
        header[i] = static_cast<uint8_t>(key >> (i - 100) * 8);
    }

    string outfilename = wstr2str(std::filesystem::current_path().wstring()) + "\\models\\outputs\\" + wstr2str(output);
    ofstream ctbfilestrm = create_ctb(header, outfilename);

    // -- Free memory
    header.clear();
    header.shrink_to_fit();

    vector<uint8_t> encrypted;

    std::cout << "Encrypting...";
    for (int j = 0; j < this->m_no_layers; j++)
    {
        encrypted = encrypt_decrypt_86(get_layer(j), j);
        add_layer_to_ctb(ctbfilestrm, encrypted, this->m_layer_len_addr[j]);
        std::cout << ".";
    }

    std::cout << "\nFinished encrypting file " << m_ctb_fname << std::endl;
    std::cout << "Generated encrypted CTB file " << wstr2str(output) << std::endl;

    ctbfilestrm.close();
}



// -- Get header information (i.e file header, preview images, table of layer data) only.
std::vector<uint8_t> CTB::get_file_header()
{
    vector<uint8_t> header;

    if (m_read)
    {
        return m_header;

    }
    else
    {
        ifstream ctbfstream(m_ctb_fname, std::ifstream::binary);
        if (!ctbfstream)
        {
            std::cout << "Cannot open " << m_ctb_fname << std::endl;
            std::cout << "Failed to get all layers" << std::endl;
        }

        header = std::vector<uint8_t>(std::istreambuf_iterator<char>(ctbfstream), std::istreambuf_iterator<char>());
        ctbfstream.close();

        header.resize(m_layer_i_addr[0]);
    }

    return header;

    if (VERBOSE)
    {
        std::cout << "File header extracted " << std::endl;
    }
}


// - Create a bare ctb file with headers only. Layer data will be added later.
ofstream CTB::create_ctb(const vector<uint8_t>& header, string ctbfname)
{
    // -- open file for writing in binary
    ofstream outf;
    outf.open(ctbfname, fstream::out | fstream::binary);
    if (!outf)
    {
        std::cout << std::strerror(errno) << std::endl;
        std::cout << "File " << ctbfname <<" could not be created. Returning empty ofstream ..."  << std::endl;
        return outf;
    }
    else
    {
        if (VERBOSE)
        {
            std::cout << "Created file " << ctbfname << std::endl;
        }
        
    }
    
    // -- Write header to file
    for (const auto& d : header) outf << d;

    return outf;
}


// -- Add one layer to a ctb file
void CTB::add_layer_to_ctb(
        ofstream& ctbstrm,
        const std::vector<uint8_t>& layer_data,
        const std::uint32_t len_addr){
    ctbstrm.seekp(0, ios_base::end);

    // -- Holds the address of the current data to be written to file
    std::uint32_t lyr_addr[1] = { ctbstrm.tellp() };

    // -- For some reasons, lyr_len has to be an array for ofstream.write to work
    std::uint32_t lyr_len[1] = { layer_data.size() };

    // -- Change length of layer i in the ctb file
    ctbstrm.seekp(len_addr * sizeof(std::uint8_t));             // -- move to the location where the length of layer i is stored
    ctbstrm.write((char*)lyr_len, sizeof(std::uint32_t));       // -- overwrite the old length with the new length


    // -- Change address of layer i in the ctb file
    ctbstrm.seekp((len_addr - 4) * sizeof(std::uint8_t));       // -- move to the location where the address of layer i is stored
    ctbstrm.write((char*)lyr_addr, sizeof(std::uint32_t));      // -- overwrite old address with new address

    // -- Write data into file
    ctbstrm.seekp(0, ios_base::end);
    for (int i = 0; i < layer_data.size(); i++)
        ctbstrm << layer_data[i];
     
    if (VERBOSE)
        std::cout << "Successfully writen layer to CTB file" << std::endl;
    
}

