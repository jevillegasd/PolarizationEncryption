#pragma once

#include "../include/decrypt.h"
#include "../include/aes_ctr.h"



int duplicatefile(std::string from, std::string to)
{
    try
    {
        std::ifstream _from(from, std::ios::binary);
        std::ofstream _to(to, std::ios::binary);

        _to << _from.rdbuf();

        return 1;
    }
    catch (const std::exception& e)
    {
        std::cout << "(copyfile) " <<  e.what() << '\n';
        return 0;
    }
}


uint32_t decode(std::stringstream& strm, std::string run_byte, int n)
{
    std::string temp;

    for (int i = 0; i < n; i++) {
        strm >> temp;
        run_byte += temp;
    }
    n = n + 1;
    std::bitset<BITSETLEN> bset(run_byte.substr(n, 7 * n));

    return (uint32_t)bset.to_ulong();
}



uint32_t handle_decode(std::stringstream& strm)
{
    std::string run_byte;
    strm >> run_byte;

    if (run_byte[0] == '0')
    {
        return decode(strm, run_byte, 0);
    }
    else if (run_byte.substr(0, 2) == "10")
    {
        return decode(strm, run_byte, 1);
    }
    else if (run_byte.substr(0, 3) == "110")
    {
        return decode(strm, run_byte, 2);
    }
    else if (run_byte.substr(0, 4) == "1110")
    {
        return decode(strm, run_byte, 3);
    }
    else
    {
        if (VERBOSE) std::cout << "(handle_decode) Unrecognized RLE byte" << std::endl;
        return 0;
    }
}



inline const char* hex2bin_lookup(char c)
{
    // TODO handle default / error
    switch (toupper(c))
    {
    case '0': return "0000";
    case '1': return "0001";
    case '2': return "0010";
    case '3': return "0011";
    case '4': return "0100";
    case '5': return "0101";
    case '6': return "0110";
    case '7': return "0111";
    case '8': return "1000";
    case '9': return "1001";
    case 'A': return "1010";
    case 'B': return "1011";
    case 'C': return "1100";
    case 'D': return "1101";
    case 'E': return "1110";
    case 'F': return "1111";
    default: return " ";
    }
}



inline std::string bin2hex_lookup(std::string str)
{
    if (str == "0000") return "0";
    else if (str == "0001") return "1";
    else if (str == "0010") return "2";
    else if (str == "0011") return "3";
    else if (str == "0100") return "4";
    else if (str == "0101") return "5";
    else if (str == "0110") return "6";
    else if (str == "0111") return "7";
    else if (str == "1000") return "8";
    else if (str == "1001") return "9";
    else if (str == "1010") return "A";
    else if (str == "1011") return "B";
    else if (str == "1100") return "C";
    else if (str == "1101") return "D";
    else if (str == "1110") return "E";
    else if (str == "1111") return "F";
    else return " ";
}



inline std::string bin2hex(std::string str)
{
    return (bin2hex_lookup(str.substr(0, 4)) +
        bin2hex_lookup(str.substr(4, 4)));
}


std::string hex2bin(const std::string& hex)
{
    std::string bin="";
    for (unsigned i = 0; i != hex.length(); ++i)
        bin += hex2bin_lookup(hex[i]);
    return bin;
}




std::string binstr2hexstr(const std::string& bitmap_str)
{
    std::stringstream ss(bitmap_str);
    std::string hex_bitmap_str = "";

    int i = 0;
    int j = bitmap_str.length() / 8;

    try
    {
        while (i < j)
        {
            hex_bitmap_str += bin2hex(bitmap_str.substr(i * 8, 8)) + " ";
            i++;
        }
        if (VERBOSE)
            std::cout << "binstr2hexstr finsished successfully" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << '\n';
    }
    return hex_bitmap_str;
}



std::string bitrun_to_1bit_depth(const std::string& bitmap_str)
{
    std::stringstream ss(bitmap_str);
    std::string hex_bitmap_str = ONEBITDEPTHHEADER;

    int i = 0;
    int j = (bitmap_str.length() / 8);

    try
    {
        while (i < j)
        {
            hex_bitmap_str += bin2hex(bitmap_str.substr(i * 8, 8)) + " ";
            i++;
        }
        if (VERBOSE)
            std::cout << "bitrun_to_1bit_depth finished successfully" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << '\n';
    }

    return hex_bitmap_str;
}



std::string bitrun_to_8bit_depth(const std::string& bitmap_str)
{
    std::string hex_bitmap_str = EIGHTBITDEPTHHEADER;

    int i = 0;
    int j = bitmap_str.length();

    try
    {
        while (i < j)
        {
            if (bitmap_str[i] == '0') hex_bitmap_str += "00 ";
            else if (bitmap_str[i] == '1') hex_bitmap_str += "FF ";
            else std::cout << "(bitrun_to_8bit_depth) unrecognized char: " << bitmap_str[i] << std::endl;
            i++;
        }
        if (VERBOSE)
            std::cout << "bitrun_to_8bit_depth finished successfully" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << '\n';
    }

    return hex_bitmap_str;
}



std::string layer_hex_to_bitrun(const std::string& layer)
{

    std::string bin = hex2bin(layer);
    std::stringstream sstr(bin);

    std::string bitmap_str = "";
    uint32_t sum = 0;
    uint32_t runlen;
    char pixel;


    std::string str;
    while (sstr >> str)
    {
        if (str.substr(1) == "1111111")
            pixel = '1';
        else if (str.substr(1) == "0000000")
            pixel = '0';
        else {
            pixel = 'X';
            std::cout << "(layer_hex_to_bitrun) Invalid Pixel Encountered, str = " << str << std::endl;
        }

        if (str[0] == '1') {
            runlen = handle_decode(sstr);
            if (VERBOSE)
                std::cout << pixel << " runlen: " << runlen << std::endl;
            sum += runlen;

            std::string s(runlen, pixel);
            bitmap_str += s;
        }
        else if (str[0] == '0') {
            runlen = 1;
            if (VERBOSE)
                std::cout << pixel << " runlen: " << runlen << std::endl;
            sum += 1;

            std::string s{ pixel };
            bitmap_str += s;
        }
        else
        {
            std::cout << "(layer_hex_to_bitrun) str[0] :" << str[0] << std::endl;
        }

    }

    std::cout << "\n\nsum: " << sum << std::endl;
    return bitmap_str;
}


// convert from bitrun 00001111 to hex 0F and then write file as hex with appropriate headers
// decided by bitdepth.
void write_bitmap(std::string& layer, const char* outfname, int bitdepth, bool is_bitrun = false)
{
    std::string bitmap_data = layer;
    if (is_bitrun == false)
        bitmap_data = layer_hex_to_bitrun(layer);

    if (bitdepth == 1)
        bitmap_data = bitrun_to_1bit_depth(bitmap_data);
    else if (bitdepth == 8)
        bitmap_data = bitrun_to_8bit_depth(bitmap_data);
    else
    {
        std::cout << "unsupported bit depth";
        return;
    }

    std::stringstream sstrm(bitmap_data);

    std::uint32_t a;

    FILE* fptr;
    fopen_s(&fptr, outfname, "wb");

    if (fptr != NULL)
    {
        while (sstrm >> std::hex >> std::setw(2) >> a)
        {
            fwrite(&a, sizeof(uint8_t), 1, fptr);
        }

        fclose(fptr);
    }
}



std::string bitmap_to_bitrun(const std::string& bitmap_str, int depth)
{
    std::stringstream sstrm(bitmap_str);
    std::string str;
    std::string bitmap_run;

    if (depth == 1 || depth == 8)
    {
        int i = 0;
        while (i < BITMAP_HEADER_SIZE_DEPTH_8_1)
        {
            sstrm >> str;
            i++;
        }


        i = 0;

        if (depth == 8)
        {
            while (sstrm >> str)
            {
                if (str == "00") bitmap_run += "0";
                else if (str == "FF") bitmap_run += "1";
                else std::cout << "(bitmap_to_bitrun (1,8)) unrecognized str: " << str << std::endl;
                i++;
            }
        }
        else if (depth == 1)
        {
            while (sstrm >> str)
            {
                std::string a(hex2bin_lookup(str[0]));
                std::string b(hex2bin_lookup(str[1]));
                bitmap_run += a + b;
                i++;
            }
        }

        std::cout << "no of bits in bitmap(1,8): " << i << std::endl;
    }

    else if (depth == 24)
    {
        int i = 0;
        while (i < BITMAP_HEADER_SIZE_DEPTH_24)
        {
            sstrm >> str;
            i++;
        }

        i = 0;

        while (sstrm >> str)
        {
            if (str == "00") bitmap_run += "1";
            else if (str == "FF") bitmap_run += "0";
            else std::cout << "(bitmap_to_bitrun (24)) unrecognized str: " << str << std::endl;
            i++;

            int j = 0;
            while (sstrm >> str)
            {
                if (j == 1) break;
                j++;
            }
        }

        std::cout << "no of bits in bitmap(24): " << i << std::endl;
    }


    return bitmap_run;
}



std::string space(const std::string& str, int size)
{
    int i = 1;
    std::string s = str;

    while (((size * i) - 1) < s.length())
    {
        s.insert(((size * i) - 1), " ");
        i++;
    }

    return s;
}




Encode_RLE7_Data encode_rle7(const std::string& bitmap_str)
{
    std::string str = "";

    int i = 0;
    char ch;

    while (i < bitmap_str.length())
    {
        ch = bitmap_str[i];

        int runlen = 1;

        while (bitmap_str[++i] == ch)
            runlen++;

        if (runlen == 1)
        {
            str += "0" + std::string(7, ch);
        }
        else
        {
            str += "1" + std::string(7, ch);

            if (runlen < 128) // 2 ^ 8
                str += "0" + std::bitset<7>(runlen).to_string();
            else if (runlen < 16384) // 2 ^ 14
                str += "10" + std::bitset<14>(runlen).to_string();
            else if (runlen < 2097152) // 2 ^ 21
                str += "110" + std::bitset<21>(runlen).to_string();
            else if (runlen < 268435456) // 2 ^ 28
                str += "1110" + std::bitset<28>(runlen).to_string();
            else
                std::cout << "invalid run length: " << runlen << std::endl;
        }
    }

    std::cout << "\n(encode_rle7) scanned bit length: " << i << std::endl;
    std::cout << std::endl;


    int lay_hex_len = str.length() / 8;
    str = binstr2hexstr(str);

    Encode_RLE7_Data R;
    R.lay_hex_data = str;
    R.lay_hex_len = lay_hex_len;
    return R;
}



void scale_all_bitmaps(int no_layers, int scale, int no_rounds, std::string type)
{   
    int h_adjust = 105;
    int w_adjust = h_adjust * 1.77;

    std::cout << "h_adjust: " << h_adjust << std::endl;
    std::cout << "w_adjust: " << w_adjust << std::endl;

    for (int r = 0; r < no_rounds; r++)
    {

        for (int i = 0; i < no_layers; i++)
        {
            CBitmapEx bitmapEx;
            std::string name;
            name = CURRENT_PATH + "\\data\\layers\\layer" + std::to_string(i + 1) + "_" + type + ".bmp";

            bitmapEx.Load(_T(name.c_str()));

            int n = (scale - 1) / 2;
            bitmapEx.Scale(scale * 100, scale * 100);
            bitmapEx.Crop(n * 1440 + h_adjust, n * 2560 - w_adjust, 1440, 2560);


            name = CURRENT_PATH + "\\data\\layers\\layer" + std::to_string(i + 1) + "_" + type + ".bmp";
            bitmapEx.Save(_T(name.c_str()));
        }
    }
    

    std::cout << no_layers << " bitmap scaling successful" << std::endl;
}



// void draw_bitmap(HWND Hwnd)
// {
//     CBitmapEx bitmapEx;
//     std::string name = CURRENT_PATH + "\\data\\layers\\layer1scaled.bmp";
//     bitmapEx.Load(_T(name.c_str()));
//     bitmapEx.Draw(GetDC(Hwnd));

//     std::cin.get();

// }



void generate_all_bitmaps_from_layer_vec_data(const std::vector<std::vector<uint8_t>>& all_layers_vec)
{
    std::string layer_str;
    int i = 1;
    for (auto layer : all_layers_vec)
    {
        std::stringstream sstrm;
        for (auto j : layer)
        {
            sstrm << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (uint32_t)j << " ";
        }
        layer_str = sstrm.str();
        std::string name = CURRENT_PATH + "\\data\\layers\\layer" + std::to_string(i) + ".bmp";
        write_bitmap(layer_str, name.c_str(), 8);
        i++;
    }
}




void generate_bitmap_from_layer_str_data(std::string& layer_str, int square_side, std::string type, int layer_no)
{
    

    paint_surrounding(layer_str, square_side, type);

    std::string name = CURRENT_PATH + "\\data\\layers\\layer" + std::to_string(layer_no) + "_" + type + ".bmp";

    write_bitmap(layer_str, name.c_str(), BITMAP_BIT_DEPTH , true);
}



std::string get_bitrun(const std::vector<uint8_t>& layer_vec)
{
    std::stringstream sstrm;
    for (auto i : layer_vec)
    {
        sstrm << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (uint32_t)i << " ";
    }

    std::string layer_hex_str = sstrm.str();

    std::string bitrun = layer_hex_to_bitrun(layer_hex_str);

    return bitrun;
}



int generate_ctb(   const std::vector<std::uint32_t>& layeri_addr_vec,
                    const std::vector<std::uint32_t>& layeri_len_addr,
                    std::string ctbfile, 
                    int no_layers,
                    std::string type)
{
    std::uint32_t c[1];
    c[0] = layeri_addr_vec[0];

    std::ofstream outf;
    outf.open(ctbfile, std::ios::out | std::ios::binary | std::ios::in);
    if (!outf)
    {
        std::cout << "Cannot open " << ctbfile << std::endl;
        return 1;
    }


    for (int i = 0; i < no_layers; i++)
    {
        std::ifstream ifstrm;
        std::string name = CURRENT_PATH + "\\data\\layers\\layer" + std::to_string(i+1) + "_" + type + ".bmp";

        ifstrm.open(name, std::ifstream::binary);
        if (!ifstrm)
        {
            std::cout << "Cannot open " << name << std::endl;
            return 2;
        }

        std::vector<uint8_t> data = std::vector<uint8_t>(std::istreambuf_iterator<char>(ifstrm), std::istreambuf_iterator<char>());

        std::stringstream sstrm1;

        for (auto i : data)
        {
            sstrm1 << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (uint32_t)i << " ";
        }
        std::string bitmap_layer = sstrm1.str();


        std::string layer_bitrun_recovered = bitmap_to_bitrun(bitmap_layer, 24); // 24 because the bit-depth of the generated image after scaling is 24
        Encode_RLE7_Data R = encode_rle7(layer_bitrun_recovered);                // If the bitmap is not scaled, the change 24 to BITMAP_BIT_DEPTH
        std::string re_encoded_layer_hex = R.lay_hex_data;


        std::uint32_t layer_hex_len[1];
        layer_hex_len[0] = R.lay_hex_len;

        std::cout << "Layer " << i << " len: " << layer_hex_len[0] << std::endl;

        std::stringstream sstrm2(re_encoded_layer_hex);

        std::uint32_t a[1];

        std::cout << "Layer " << i << " len addr: " << layeri_len_addr[i] << std::endl;


        //Change length of layer i in the ctb file
        outf.seekp(layeri_len_addr[i] * sizeof(std::uint8_t));
        outf.write((char*)layer_hex_len, sizeof(std::uint32_t));


        //Change address of layer i in the ctb file
        outf.seekp((layeri_len_addr[i] - 4) * sizeof(std::uint8_t));
        outf.write((char*)c, sizeof(std::uint32_t));

        std::cout << "Layer " << i << " new addr: " << c[0] << std::endl;

        //write new (encrypted and decoded) layer to ctb file
        while (sstrm2 >> std::hex >> std::setw(2) >> a[0])
        {
            outf.seekp(c[0]++ * sizeof(std::uint8_t));
            outf.write((char*)a, sizeof(std::uint8_t));
        }

        ifstrm.close();
    }

    outf.close();

    std::cout << "CTB file generated successfully at: \n" << ctbfile << std::endl;

    return 0;

}



int generate_ctb_from_encrypted_layers(const std::vector<std::uint32_t>& layeri_addr_vec,
    const std::vector<std::uint32_t>& layeri_len_addr,
    const std::vector<std::string> encryptd_layers,
    std::string ctbfile, int no_layers)
{
    std::uint32_t c[1];
    c[0] = layeri_addr_vec[0];

    std::ofstream outf;
    outf.open(ctbfile, std::ios::out | std::ios::binary | std::ios::in);
    if (!outf)
    {
        std::cout << std::strerror(errno) << std::endl;
        std::cout << "Cannot open " << ctbfile << std::endl;
        return 1;
    }

    Encode_RLE7_Data R;
    std::string layer_bitrun, re_encoded_layer_hex;

    for (int i = 0; i < no_layers; i++)
    {
        layer_bitrun = encryptd_layers[i];
        R = encode_rle7(layer_bitrun);
        re_encoded_layer_hex = R.lay_hex_data;


        std::uint32_t layer_hex_len[1];
        layer_hex_len[0] = R.lay_hex_len;

        std::cout << "Layer " << i << " len: " << layer_hex_len[0] << std::endl;

        std::stringstream sstrm2(re_encoded_layer_hex);

        std::uint32_t a[1];


        std::cout << "Layer " << i << " len addr: " << layeri_len_addr[i] << std::endl;


        //Change length of layer i in the ctb file
        outf.seekp(layeri_len_addr[i] * sizeof(std::uint8_t));
        outf.write((char*)layer_hex_len, sizeof(std::uint32_t));


        //Change address of layer i in the ctb file
        outf.seekp((layeri_len_addr[i] - 4) * sizeof(std::uint8_t));
        outf.write((char*)c, sizeof(std::uint32_t));

        std::cout << "Layer " << i << " new addr: " << c[0] << std::endl;

        while (sstrm2 >> std::hex >> std::setw(2) >> a[0])
        {
            outf.seekp(c[0]++ * sizeof(std::uint8_t));
            outf.write((char*)a, sizeof(std::uint8_t));
        }
    }

    outf.close();

    std::cout << "ctb file generated successfully" << std::endl;

    return 0;

}



std::string extract_square(const std::string& layer, int side)
{
    int x = (SCREEN_WIDTH - side) / 2;  // -- square padding
    int y = (SCREEN_HEIGHT - side) / 2; // -- square padding

    std::string str = "";

    for (int i = x; i < (x + side); i++)
    {
        int idx_start = (i * SCREEN_HEIGHT) + y; // -- move to appropriate line and then add y to move to start of pixel
        str += layer.substr(idx_start, side);    // -- add set of pixels
    }

    return str;
}



void put_square(std::string& layer, const std::string& square, int side)
{
    int x = (SCREEN_WIDTH - side) / 2;
    int y = (SCREEN_HEIGHT - side) / 2;

    int j = 0;
    for (int i = x; i < (x + side); i++)
    {
        int idx_start = (i * SCREEN_HEIGHT) + y;
        layer.replace(idx_start, side, square.substr((j++ * side), side));
    }
}



void print_square(const std::string& square, int side)
{
    for (int i = 0; i < square.length(); i++)
    {
        if (i % side == 0)
            std::cout << "\n" << square[i] << " ";
        else
            std::cout << square[i] << " ";
    }
}



void paint_surrounding(std::string& layer_str, int square_side, std::string type)
{   
    std::string target_area = extract_square(layer_str, square_side);

    if (type == "printer")
    {
        std::string black_layer(SCREEN_HEIGHT * SCREEN_WIDTH, '0'); // -- Pixel value of 1 is actually black (tricky right?)
        layer_str = black_layer;
    }
       
    else if (type == "lcd")
    {
        std::string black_layer(SCREEN_HEIGHT * SCREEN_WIDTH, '1'); // -- Pixel value of 1 is actually black (tricky right?)
        layer_str = black_layer;
    }

    else if (type == "original") {} // - leave original image as it is

    else {}

    put_square(layer_str, target_area, square_side);
}


std::string generate_decrypting_square(uint8_t* key, int nonce)
{
    std::string pt(SCREEN_HEIGHT * SCREEN_WIDTH, '1'); // Change to '0' for XOR. Currently XNOR
    return AES_CTR(pt, key, nonce); 
}


std::string generate_decrypting_layer(uint8_t* key, int nonce, int square_side)
{

    std::string intermediate_string = generate_decrypting_square(key, nonce);

    std::string black_layer(SCREEN_HEIGHT * SCREEN_WIDTH, '1');

    put_square(black_layer, intermediate_string, square_side);

    return black_layer;
}



std::string string_bitwise_xnor(std::string a, std::string b) 
{
    std::string ans = "";

    for (int i = 0; i < a.size(); i++)
    {
        if (a[i] == b[i])
            ans += "1";
        else
            ans += "0";
    }
    return ans;
}



void compare_generated_vs_original( const std::string & original_lyr,
                                    const std::string & encryptd_lyr,
                                    const std::string & generated_lyr,
                                    int layer_no)
{

    std::string temp = string_bitwise_xnor(encryptd_lyr, generated_lyr);

    if (temp == original_lyr)
        std::cout << "same layer, layer " << layer_no << std::endl;
    else
        std::cout << "not same layer, layer " << layer_no << std::endl;

}


std::vector<std::vector<uint32_t>> printer_encrypt()
{

}