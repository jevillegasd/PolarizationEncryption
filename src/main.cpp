#include "../include/decrypt.h"
#include "../include/BitmapEx.h"
#include "../include/ctb_file.h"
#include "../include/aes_ctr.h"

//extern "C" void crypt_86(uint32_t key, uint32_t iv, uint8_t data[]);

std::vector<double> v;
double* a = &v[0];

int main()
{
    std::string in_ctb(CURRENT_PATH + "\\models\\box\\dslab.ctb");
    std::string out_ctb( CURRENT_PATH  + "\\models\\box\\dslab_test.ctb");  
    //if (!duplicatefile(in_ctb, out_ctb))
    //{
    //    std::cout << "(main) could not duplicate file " << in_ctb << std::endl;
    //    return -1;
    //}


    auto data               = get_layer_data_from_ctb(out_ctb.c_str());
    auto layers_vec         = data.layer_data;
    auto layeri_addr_vec    = data.layer_i_addr;
    auto layer_len_addr_vec = data.layer_len_addr;
    int no_layers           = layers_vec.size();

    auto test = layers_vec[1];

    uint8_t* test_array = &test[0];

    uint32_t iv = 0;

    //crypt_86(key, iv, test_array);

    
    uint8_t key[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};

    
    std::string encryptd_lyr, original_lyr, generated_lyr;
    std::string layer_str, plain_text, cipher_text;

    int layer_no = 1;
    uint64_t nonce = 0;
    int extract_square_dim = 10; // unit of pixels

    for (auto layer : layers_vec)
    {
        // Build  a bitmap from from the layer data
         original_lyr = get_bitrun(layer);

         // Extract a section from the bitmap 
         plain_text = extract_square(original_lyr, extract_square_dim);
         cipher_text = aes_ctr(plain_text, key, nonce + layer_no);
         encryptd_lyr = original_lyr;
         put_square(encryptd_lyr, cipher_text, extract_square_dim);
         
         generated_lyr = generate_decrypting_layer(key, nonce + layer_no, extract_square_dim);
         //compare_generated_vs_original(original_lyr, encryptd_lyr, generated_lyr, layer_no); 
 
         generate_bitmap_from_layer_str_data(original_lyr, extract_square_dim, "original", layer_no); 
         generate_bitmap_from_layer_str_data(encryptd_lyr, extract_square_dim, "printer", layer_no);
         
         generate_bitmap_from_layer_str_data(generated_lyr, extract_square_dim, "lcd", layer_no);


         layer_no++;
    }


    //{
    //    int scaling_rounds = 2;
    //    int scaling_factor_per_round = 9; // maximum scale per round is 9

    //    scale_all_bitmaps(no_layers, scaling_factor_per_round, scaling_rounds, "printer");

    //    //scale_all_bitmaps(no_layers, scaling_factor_per_round, scaling_rounds, "lcd");
    //}
    

    //{
    //    generate_ctb(layeri_addr_vec, layer_len_addr_vec, out_ctb, no_layers, "printer");
    //}


   /* {
        std::string command = "cargo run --bin catibo - convert-- --key = 500 " + in_ctb + " input - " + out_ctb;
       
        system(command.c_str());
    }*/

}









//int main()
//{
//    
//    //std::string from_file(CURRENT_PATH + "\\models\\benchy\\d_3dbenchy.ctb");
//    std::string to_file( CURRENT_PATH + "\\models\\u_test.ctb");
//
//    /*if (!duplicatefile(from_file, to_file))
//    {
//        std::cout << "(main) could not duplicate file\n";
//        return -1;
//    }*/
//
//    auto data = get_layer_data_from_ctb(to_file.c_str());
//    auto layers_vec = data.layer_data;
//    auto layeri_addr_vec = data.layer_i_addr;
//    auto layer_len_addr_vec = data.layer_len_addr;
//    int no_layers = layers_vec.size();
//
//
//    int square_side = 10;
//    std::string layer_str;
//    std::vector<std::string> encryptd_lyrs_vec;
//    std::vector<std::string> original_lyrs_vec;
//
//    uint8_t key[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };
//    uint64_t nonce = 0;
//    std::string plain_text, cipher_text;
//
//    int cnt = 0;
//    for (auto layer : layers_vec)
//    {
//        layer_str = get_bitrun(layer);
//
//        original_lyrs_vec.push_back(layer_str);
//
//        plain_text = extract_square(layer_str, square_side);
//
//        cipher_text = AES_CTR(plain_text, key, nonce + cnt);
//
//        put_square(layer_str, cipher_text, square_side);
//
//        encryptd_lyrs_vec.push_back(layer_str);
//
//        cnt++;
//    }
//
//    std::vector<std::string> generated_lyr_vec = generate_decrypting_layers(no_layers, key, nonce, square_side);
//
//
//    compare_generated_vs_original(original_lyrs_vec, encryptd_lyrs_vec, generated_lyr_vec, no_layers);
//
//    /*std::string output_ctbfile = CURRENT_PATH + "\\models\\benchy\\u_test_enc.ctb";
//    generate_ctb_from_encrypted_layers(
//        layeri_addr_vec,
//        layer_len_addr_vec,
//        layers_bitrun_vec,
//        output_ctbfile,
//        no_layers
//    );*/
//
//
//    //generate_all_bitmaps_from_layer_str_data(layers_bitrun_vec, square_side);
//
//    //scale_all_bitmaps(no_layers, 5, 2);
//
//    /*
//    std::string output_ctbfile = CURRENT_PATH + "\\models\\dslab\\dslab_enc_scaled.ctb";
//    generate_ctb(layeri_addr_vec, layer_len_addr_vec, output_ctbfile, no_layers);*/
//
//}





































//int main()
//{
//    int k = 1;
//
//    std::string filename = "c:\\users\\hp\\documents\\a_projects\\a_a3dpro\\vs_project\\hackcreality\\models\\tests2\\en_test_" + std::to_string(k) + ".ctb";
//    auto data = get_layer_data_from_ctb(filename.c_str());
//    auto layers_vec = data.layer_data;
//    auto layeri_addr_vec = data.layer_i_addr;
//    auto layer_len_addr_vec = data.layer_len_addr;
//    int no_layers = layers_vec.size();
//
//
//    int square_side = (2 ^ k) * 10;
//    std::string layer_str;
//    std::vector<std::string> layers_bitrun_vec;
//
//    uint8_t key[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };
//    uint64_t nonce = 0;
//    std::string plain_text, cipher_text;
//
//    int cnt = 0;
//    for (auto layer : layers_vec)
//    {
//        layer_str = get_bitrun(layer);
//
//        plain_text = extract_square(layer_str, square_side);
//
//        cipher_text = AES_CTR2(plain_text, key, nonce + (cnt / 10));
//
//
//        put_square(layer_str, cipher_text, square_side);
//
// 
//        layers_bitrun_vec.push_back(layer_str);
//
//        cnt++;
//    }
//
//    std::string output_ctbfile = "c:\\users\\hp\\documents\\a_projects\\a_a3dpro\\vs_project\\hackcreality\\models\\tests2\\en_test_" + std::to_string(k) + ".ctb";
//    generate_ctb_from_encrypted_layers(
//        layeri_addr_vec,
//        layer_len_addr_vec,
//        layers_bitrun_vec,
//        output_ctbfile,
//        no_layers
//    );
//
//}





//int main()
//{
//    auto data = get_layer_data_from_ctb("c:\\users\\hp\\documents\\a_projects\\a_a3dpro\\vs_project\\hackcreality\\models\\dslab_enc2.ctb");
//    auto layers_vec = data.layer_data;
//    auto layeri_addr_vec = data.layer_i_addr;
//    auto layer_len_addr_vec = data.layer_len_addr;
//    int no_layers = layers_vec.size();
//
//
//    int square_side = 32;
//    std::string layer_str, all_layer_str = "";
//    std::vector<std::string> layers_bitrun_vec;
//
//    uint8_t key[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };
//    uint64_t nonce = 0;
//    std::string plain_text, cipher_text;
//
//    int cnt = 1;
//    for (auto layer : layers_vec)
//    {
//        layer_str   = get_bitrun(layer);
//
//        plain_text  = extract_square(layer_str, square_side);
//        
//        all_layer_str += plain_text;
//    }
//
//    cipher_text = AES_CTR(all_layer_str, key, nonce);
//
//    int i = 0;
//    int matrix_size = square_side * square_side;
//
//    for (auto layer : layers_vec)
//    {
//        layer_str = get_bitrun(layer);
//
//        put_square(layer_str, cipher_text.substr(i, matrix_size), square_side);
//
//        layers_bitrun_vec.push_back(layer_str);
//        
//        i += matrix_size;
//    }
//    
//
//    std::string output_ctbfile = "c:\\users\\hp\\documents\\a_projects\\a_a3dpro\\vs_project\\hackcreality\\models\\dslab_enc2.ctb";
//    generate_ctb_from_encrypted_layers(
//        layeri_addr_vec,
//        layer_len_addr_vec,
//        layers_bitrun_vec,
//        output_ctbfile,
//        no_layers
//    );
//
//}


//int main()
//{
//    auto data = get_layer_data_from_ctb("c:\\users\\hp\\documents\\a_projects\\a_a3dpro\\vs_project\\hackcreality\\models\\dslab\\dslab.ctb");
//    auto layers_vec = data.layer_data;
//    auto layeri_addr_vec = data.layer_i_addr;
//    auto layer_len_addr_vec = data.layer_len_addr;
//    int no_layers = layers_vec.size();
//
//
//    int square_side = 32;
//    std::string layer_str, all_layer_str = "";
//    std::vector<std::string> layers_bitrun_vec;
//
//    uint8_t key[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };
//    uint64_t nonce = 0;
//    std::string plain_text, cipher_text;
//
//
//    
//    for (auto layer : layers_vec)
//    {
//        layer_str   = get_bitrun(layer);
//
//        plain_text  = extract_square(layer_str, square_side);
//        
//        all_layer_str += plain_text;
//    }
//
//    cipher_text = AES_CTR(all_layer_str, key, nonce);
//
//    int i = 0;
//    int matrix_size = square_side * square_side;
//
//    for (auto layer : layers_vec)
//    {
//        layer_str = get_bitrun(layer);
//
//        put_square(layer_str, cipher_text.substr(i, matrix_size), square_side);
//
//        layers_bitrun_vec.push_back(layer_str);
//        
//        i += matrix_size;
//    }
//
//    
//    generate_all_bitmaps_from_layer_str_data(layers_bitrun_vec);
//
//    scale_all_bitmaps(no_layers, 5);
//
//
//
//    std::string output_ctbfile = "c:\\users\\hp\\documents\\a_projects\\a_a3dpro\\vs_project\\hackcreality\\models\\dslab\\dslab_enc_scaled.ctb";
//    generate_ctb(layeri_addr_vec, layer_len_addr_vec, output_ctbfile, no_layers);
//
//}


//int main()
//{
//    //draw_bitmap(Hwnd);
//    CBitmapEx bitmapEx;
//    std::string name = "C:\\Users\\hp\\Documents\\A_PROJECTS\\A_a3DPro\\VS_project\\HackCreality\\data\\layers\\layer1scaled.bmp";
//    bitmapEx.Load(_T(name.c_str()));
//    bitmapEx.Draw(GetDC(Hwnd));
//    std::cin.get();
//}


//int main()
//{
//    std::string s = "123456123456123456123456123456123456123456123456";
//
//    for (int i = 0; i < s.length(); i++)
//    {
//        if (i % 6 == 0)
//            std::cout << "\n" << s[i] << " ";
//        else
//            std::cout << s[i] << " ";
//    }
//
//    std::string s2 = "ABCDEFGHIJKLMNOP";
//    put_square(s, s2, 4);
//
//    std::cout << "\n\n\n";
//    for (int i = 0; i < s.length(); i++)
//    {
//        if (i % 6 == 0)
//            std::cout << "\n" << s[i] << " ";
//        else
//            std::cout << s[i] << " ";
//    }
//}


//int main()
//{
//    auto data = get_layer_data_from_ctb("c:\\users\\hp\\documents\\a_projects\\a_a3dpro\\vs_project\\hackcreality\\models\\dslab.ctb");
//    auto layers_vec = data.layer_data;
//    auto layeri_addr_vec = data.layer_i_addr;
//    auto layer_len_addr_vec = data.layer_len_addr;
//    int no_layers = layers_vec.size();
//
//    std::string layer_0 = get_bitrun(layers_vec[0]);
//
//    std::string square = extract_square(layer_0, 32);
//
//    std::cout << square.length() << std::endl << std::endl;
//    
//    print_square(square, 32);
//
//}


//int main()
//{
//    auto data = get_layer_data_from_ctb("c:\\users\\hp\\documents\\a_projects\\a_a3dpro\\vs_project\\hackcreality\\models\\cup.ctb");
//
//    auto layers_vec = data.layer_data;
//    auto layeri_addr_vec = data.layer_i_addr;
//    auto layer_len_addr_vec = data.layer_len_addr;
//    int no_layers = layers_vec.size();
//
//    std::cout << "Read all layers data successfully" << std::endl;
//
//    for (int i = 0; i < no_layers; i++)
//    {
//        std::cout << "Layer " << i << " old addr: " << layeri_addr_vec[i] << std::endl;
//    }
//
//    generate_all_bitmaps_from_layer_data(layers_vec);
//
//    scale_all_bitmaps(no_layers, 5);
//
//    std::string ctbfile = "C:\\Users\\hp\\Documents\\A_PROJECTS\\A_a3DPro\\VS_project\\HackCreality\\models\\cup_test.ctb";
//    generate_ctb(layeri_addr_vec, layer_len_addr_vec, ctbfile, no_layers);
//
//}
   




































// const unsigned g_unMaxBits = 32;
// std::string bin2hex(const std::string& s)
// {
    
//     std::bitset<g_unMaxBits> bs(s);
//     unsigned n = bs.to_ulong();
//     std::stringstream ss;
//     ss << std::hex << n;

//     std::string str = ss.str();
//     char char_array[3];
//     strcpy(char_array, str.c_str());

//     int i = 0;
//     char c;
//     while (char_array[i])
//     {
//         c=char_array[i];
//         putchar (toupper(c));
//         i++;
//     }

//     return char_array;
// }