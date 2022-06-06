// HackCreality2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Shobjidl.h> //  COM library, don't put this in the header file
//#include "../include/ctb_file.h"
#include "../include/Hackcreality.h"

#include <chrono>
#include <thread>

#define GENERATE_LOADS_OF_BMPS 0

using namespace cv;
using namespace std;

const std::wstring PATH = std::filesystem::current_path().wstring();
const std::wstring default_im_fn(PATH + L"\\data\\layers\\layer_archive\\layer1scaled.bmp");
const std::wstring DEFAULT_CTB_INP_FILENAME(PATH + L"\\models\\box\\encrypted_box.ctb");
const std::wstring DEFAULT_CTB_OUT_FILENAME(PATH + L"\\models\\box\\output.ctb");

std::wstring in_ctb(DEFAULT_CTB_INP_FILENAME);  //Filenames are in wstrings to support UNICODE
std::wstring out_ctb(DEFAULT_CTB_OUT_FILENAME);

CTB myCTB;
encryption_prop encProp;
printer_prop printProp;

int main()
{
    int option=0;
    while (messageListener(&option)) {
        messageParser(&option);
    }
    return 0;
}



int messageListener(int* option) 
{
    // Print menu according to state
    switch (*option) 
    {
    case 0: 
        cout << STRMAINMENU;
        break;
    default:
        break;
    }

    // Read input and define next state
    char c;
    cin>>c;
    
    switch (c) {
    case '1':
        *option += 1;
        break;
    case '2':
        *option += 2;
        break;
    case '3':
        *option += 3;
        break;
    case '4':
        *option += 4;
        break;
    case '5':
        *option += 5;
        break;
    case '6':
        *option += 6;
        break;
    case '7':
        *option += 7;
        break;
    case '8':
        *option += 8;
        break;
    case 'q'|'Q': {
        *option = 0;
        return 0;
    }
        break;
    default:
        *option = 0;
        break;
    }
    return 1;
}

void messageParser(int* option) 
{
    switch (*option) 
    {
        case 0: {} break;
        
        case 1:{
            *option = 0;
            wstring file_name;
            if (openFileDialog(&file_name)) {
                in_ctb.assign(file_name);
                myCTB = CTB(in_ctb);
                printProp = myCTB.getPrinterProperties();

                if (myCTB.get_all_layers().size() != 0) {
                    if (myCTB.get_key() != 0x0000'0000) {
                        std::cout << std::hex << std::setiosflags(std::ios::showbase);
                        std::cout << "File is encrypted with key " << (myCTB.get_key())  << std::endl;
                        std::cout << std::resetiosflags(std::ios::showbase) << std::dec;
                    }            
                    std::cout << "Plotting preview image...";
                    plotandwait(myCTB.get_preview1());
                    std::cout << "\nCTB file opened. ";
                }
            }
        } break;

        case 2:{
            *option = 0;
            wstring file_name;
            if (!myCTB.ready()) {
                if (openFileDialog(&file_name)) {
                    in_ctb.assign(file_name);
                    myCTB.read_CTB(in_ctb);
                }     
            }
            
            if (myCTB.ready()){
                wstring output_fname;
                if (saveFileDialog(&output_fname)) {
                    std::cout << "Enter the output file name: ";
                    wcin >> output_fname;
                    if (output_fname == L"") output_fname = L"default_output.ctb";

                    myCTB.decrypt_ctb_file(output_fname);
                }
            } 
        }break;

        case 3:{
            *option = 0;
            
            wstring file_name;
            if (!myCTB.ready()) {
                if (openFileDialog(&file_name)) {
                    in_ctb.assign(file_name);
                    myCTB.read_CTB(in_ctb);
                }
            }

            if (myCTB.ready()) {
                wstring output_fname;
                if (saveFileDialog(&output_fname)) {
                    uint32_t key = 0;
                    //std::cout << "Enter the output file name: ";
                    //wcin >> output_fname;
                    //if (output_fname == L"") output_fname = L"default_output.ctb";

                    unsigned long number;
                    std::string numbuf;
                    std::cout << "Enter 32 bit key as block of unspaced hexadecimal(e.g B30911B8): ";
                    cin >> numbuf;
                    number = strtoul(numbuf.c_str(), NULL, 16);
                    if (ULONG_MAX == number && ERANGE == errno)
                    {
                        std::cerr << "Number too big!" << std::endl;
                        break;
                    }
                    key = static_cast<uint32_t>(number);
                    std::cout << "Your entered key is: " << hex << key << std::endl;
                    std::cout << dec;

                    myCTB.encrypt_ctb_file(key, output_fname);
                }
            }
        } break;

        case 4:{
            *option = 0;
            encProp.area = cv::Rect(1080,520, encProp.extract_dim, encProp.extract_dim);
            generateDecryptorImages(myCTB, encProp, in_ctb);
        }break;
        case 5: { // Adjust polariziation ti best XOR operation. 
            *option = 0;
            cout << "Run the Calibrate tool on your printer and adjust the LCD decryptor output contrast using arrows: \n Left and Right:\t White pixel. \n Up and Down: \t Dark pixel.\n";
            printer_prop pprop;
            cv::Size size(pprop.width, pprop.height);

            uint8_t pxBlack = 0x00;
            uint8_t pxWhite = 0xFF;
            

            cv::Mat checkerboard(size,CV_8UC3, cv::Scalar(0x00, 0x00, 0x00));
            
            int sqSize = 40;  
            int c= '1';
            string wn = "Hack  Creality";
            while (c != 27) {
                cv::Vec3b wcolor(pxWhite, pxWhite, pxWhite);
                cv::Vec3b bcolor(pxBlack, pxBlack, pxBlack);

                for (int x = 0; x < checkerboard.cols; x++) {
                    for (int y = 0; y < checkerboard.rows; y++) {
                        int check = ((x / sqSize) % 2 + (y / sqSize) % 2) % 2;
                        if (check)
                            checkerboard.at<cv::Vec3b>(cv::Point(x, y)) = wcolor;
                        else
                            checkerboard.at<cv::Vec3b>(cv::Point(x, y)) = bcolor;
                    }
                }
                wn = displayimage_fs(checkerboard, wn, 1);
                c = waitKeyEx();
                

                switch (c) {
                    case 2490368:
                        pxBlack++;
                        break;
                    case 2621440:
                        pxBlack--;
                        break;
                    case 2424832:
                        pxWhite--;
                        break;
                    case 2555904:
                        pxWhite++;
                        break;
                    default:
                        continue;
                }
                cout << "\rWhite Pixel: " << (int) pxWhite << "\t Black Pixel: " << (int) pxBlack << std::dec << "    ";

            }
            
            try {
                destroyWindow(wn);
            }
            catch (Exception e) {}
        }break;
        case 6:{
                *option = 0;
                printer_prop pprop;
                
                encProp.area = cv::Rect(1080, 520, encProp.extract_dim, encProp.extract_dim);
                if (myCTB.ready())
                    pprop = myCTB.getPrinterProperties();

                runOfflineDecryptor(encProp, pprop);
         } break;   
        
        case 7:{
            *option = 0;
            wstring file_name = PATH + L"\\imgs\\test_img.bmp";
            string window_name = displayimage_fs(file_name, NULLSTR, 0);
            waitKey();
            destroyWindow(window_name); 
        } break;
       
        default: 
        {
            cout << "Nothing here. This probably needs to be implemented.";
            option = 0; 
        }
    }
    return;
}




// File Open dialog window
int openFileDialog(wstring* file_name)
{
    CoInitialize(NULL);
    IFileDialog *pfd = nullptr;     //File Dialog
    IID         id = CLSID_FileOpenDialog;
    IShellItem  *psi = NULL;         // Shell Item
    wchar_t*    wfile_name = NULL;
    const COMDLG_FILTERSPEC c_rgSaveTypes[] =
    {
        {L"Chitubox Slicer File (*.ctb)",       L"*.ctb"},
    };

    HRESULT hr = CoCreateInstance(id, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

    if (SUCCEEDED(hr))
    {
        hr = pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
        hr = pfd->SetTitle(L"Open sliced file.");
        if (SUCCEEDED(hr))
        {
            hr = pfd->SetFileTypeIndex(1);
            if (SUCCEEDED(hr))
            {      
                pfd->SetFileName(L"filename.ctb");
                pfd->Show(::GetActiveWindow());
                pfd->GetResult(&psi);
                psi->GetDisplayName(SIGDN_FILESYSPATH, &wfile_name);
                if (wfile_name!= 0)
                    *file_name= wstring(wfile_name);
                pfd->Release();
                return 1;
            }
        }
        pfd->Release();  
    }
    return 0;
}

// File Save dialog window
int saveFileDialog(wstring* file_name)
{
    CoInitialize(NULL);
    IFileDialog* pfd = nullptr;     //File Dialog
    IID         id = CLSID_FileSaveDialog;
    IShellItem* psi = NULL;         // Shell Item
    wchar_t* wfile_name = NULL;
    const COMDLG_FILTERSPEC c_rgSaveTypes[] =
    {
        {L"Chitubox Slicer File (*.ctb)",       L"*.ctb"},
    };

    HRESULT hr = CoCreateInstance(id, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

    if (SUCCEEDED(hr))
    {
        hr = pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
        hr = pfd->SetTitle(L"Save encrypted ctb file.");
        if (SUCCEEDED(hr))
        {
            hr = pfd->SetFileTypeIndex(1);
            if (SUCCEEDED(hr))
            {
                pfd->SetFileName(L"filename.ctb");
                pfd->Show(::GetActiveWindow());
                pfd->GetResult(&psi);
                psi->GetDisplayName(SIGDN_FILESYSPATH, &wfile_name);
                if (wfile_name != 0)
                    *file_name = wstring(wfile_name);
                pfd->Release();
                return 1;
            }
        }
        pfd->Release();
    }
    return 0;
}

// Rotate an image and return one within a bounding box that fully contains the image.
cv::Mat rotateImage(Mat image, float angle) {
    Point2f image_center((image.cols - 1) / 2.f, (image.rows - 1) / 2.f);
    Mat rot = getRotationMatrix2D(image_center, angle, 1.0);
    Rect2f bounding_box = RotatedRect(Point2f(), image.size(), angle).boundingRect2f();
    rot.at<double>(0, 2) += bounding_box.width / 2. - image.cols / 2.;
    rot.at<double>(1, 2) += bounding_box.height / 2. - image.rows / 2.;
    Mat res;
    warpAffine(image, res, rot, bounding_box.size());
    return res;
}


//Display an image in the main screen not full_screen
std::string displayimage(Mat image, std::string window_name) {
    if (!window_name.compare(NULLSTR))
        window_name = "Hack Creality";    //Name of the window
    namedWindow(window_name, 
WINDOW_NORMAL| WINDOW_GUI_EXPANDED);    // Create a window
    setWindowProperty(window_name, WND_PROP_ASPECT_RATIO, WINDOW_KEEPRATIO); // Makesit Full Screen
    imshow(window_name, image);                 // Show our image inside the created window.
    return(window_name);
}



//Display an image in the main screen and full_screen
std::string displayimage_fs(Mat image, std::string window_name) {
    if (!window_name.compare(NULLSTR))
        window_name = "Hack Creality";    //Name of the window
    namedWindow(window_name, WINDOW_NORMAL);    // Create a window
    setWindowProperty(window_name, WND_PROP_ASPECT_RATIO, WINDOW_KEEPRATIO); // Makesit Full Screen
    setWindowProperty(window_name, WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);  // Full Screen
    imshow(window_name, image);                 // Show our image inside the created window.
    return(window_name);
}

//Display an image in referenced monitor and full_screen
std::string displayimage_fs(Mat image, std::string window_name, int display_number) {
    screens displays;
    assert(image.dims != 0);
    if (!window_name.compare(NULLSTR))
        window_name = "Hack Creality";    //Name of the window

    if (image.size().width < image.size().height) {
        image = rotateImage(image, 90.);
    }
    
    namedWindow(window_name, WINDOW_NORMAL);    // Create a window

    if (display_number < displays.rc.size()) {
        int left = displays.rc[display_number].left;
        int top = displays.rc[display_number].top;
        moveWindow(window_name, left, top);
    }
        
    setWindowProperty(window_name, WND_PROP_ASPECT_RATIO, WINDOW_KEEPRATIO); // Makesit Full Screen
    setWindowProperty(window_name, WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);  // Full Screen
    imshow(window_name, image);
    return window_name;
}

//Display stored image in a file in the main screen and full_screen
std::string displayimage_fs(std::wstring file_name, std::string window_name) {
   
    if (!file_name.compare(LNULLSTR))
        return NULLSTR;
    Mat image = imread(wstr2str(file_name));
    assert(image.dims != 0);

    if (image.size().width < image.size().height) {
        image = rotateImage(image, 90.);
    }
    window_name = displayimage_fs(image, window_name);
    setWindowProperty(window_name, WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
    return(window_name);                        //to later destroy the created window
}

//Display an image in referenced monitor and full_screen
std::string displayimage_fs(std::wstring file_name, std::string window_name, int display_number) {
    screens displays;
    window_name = displayimage_fs(file_name, window_name);
    int left = displays.rc[display_number].left;
    int top = displays.rc[display_number].top;
    moveWindow(window_name, left, top);
    return window_name;
}



//Display an image an wait for an ESC command
void plotandwait(Mat image) {
    cv::startWindowThread();
    string window_name = displayimage(image, NULLSTR);
    HWND hwnd = (HWND)cvGetWindowHandle(window_name.c_str());
    cout << "\nPress ESC to continue" << std::endl;
    while (IsWindowVisible(hwnd)) {
        if (waitKey(100) == 27) break;
    }

    try {
        if (IsWindowVisible(hwnd))
            destroyWindow(window_name);
    }
    catch (Exception e) {}

}



int generateDecryptorImages(CTB& myCtb , encryption_prop prop, filesystem::path save_path)
{
    if (!myCtb.ready())
    {
        std::cout << "No ctb file loaded yet. Returning ..." << std::endl;
        return -1;
    }

    filesystem::path p(save_path);
    p.remove_filename();
    p += L"data\\layers\\";

    int im_width = myCtb.get_width();
    int im_heigth = myCtb.get_height();
    int i_iniLayer = prop.i_inilayer;
    int i_endLayer = prop.i_endLayer;
    int res = prop.res;
    uint64_t nonce = prop.nonce;
    cv::Rect area = prop.area;
    uint8_t* key{ prop.key };

    auto all_layers  = myCtb.get_all_layers();
    int  no_layers   = (int) myCtb.get_no_layers();
    vector<uint32_t> layer_len_addrs        = myCtb.get_layer_len_addrs();
    vector<ctbLayer>::iterator it_end       = all_layers.end();
    vector<ctbLayer>::iterator it_begin     = all_layers.begin();


    //Only decrypt the layers between i_iniLayer and i_endLayer
    vector<ctbLayer>::iterator it_iniLayer = it_begin + \
                                             min(i_iniLayer, no_layers);

    vector<ctbLayer>::iterator it_endLayer = it_begin + \
                                             min(i_endLayer, no_layers);

    //This is (rows, cols) so (y,x) in an image


   
    ofstream newCTB = newCTB_fstream(myCtb, save_path);

    string window_name = "draw";
    
    cv::startWindowThread();

    int layer_no = 1;
    for (vector<ctbLayer>::iterator it = it_begin; it != it_end; it++) 
    {
        ctbLayer layer = *it;
        
        size_t ctr = nonce + layer_no / res; // A different encryption every res layers
        //size_t ctr = nonce + layer_no;
        ctbLayer layer2file;

        //Only modify the layers between iniLayer and endLayer
        BOOL encrypt = (it >= it_iniLayer && it < it_endLayer);
        if (encrypt) 
        {
            // Build bitmaps from the layer data and their encrypted versions
            Mat imlayer = myCtb.getLayerImageRL7(layer, im_width, im_heigth);
            layer_bmp my_layer_bmp = encrypt_area(imlayer, area, key, ctr, res); 
            displayimage(my_layer_bmp.layer_ct, window_name);
            waitKey(10);
            layer2file      = myCtb.encode_rle7(my_layer_bmp.layer_ct);

#if (GENERATE_LOADS_OF_BMPS) 
            filesystem::path filepath = p / ("pt_lay" + to_string(layer_no) + ".bmp");
            cv::imwrite(filepath.u8string(), my_layer_bmp.layer_pt);

            filepath = p / ("ct_lay" + to_string(layer_no) + ".bmp");
            cv::imwrite(filepath.u8string(), my_layer_bmp.layer_ct);

            filepath = p / ("enc_lay" + to_string(layer_no) + ".bmp");
            cv::imwrite(filepath.u8string(), my_layer_bmp.layer_enc);

#endif      
        }
        else 
        {
            layer2file = layer;//myCtb.encode_rle7(my_layer_bmp.layer_ct);
        }        
        myCtb.add_layer_to_ctb(newCTB, layer2file, layer_len_addrs[layer_no - 1]);
        progressBar(layer_no, no_layers);
        layer_no++;
    }
    
    try {
            destroyWindow(window_name);
    }
    catch (Exception e) {}
    return 0;
}

ofstream newCTB_fstream (CTB refCTB, filesystem::path save_path) 
{
    // Create a new CTB file to store the encrypted version.
   auto header = refCTB.get_file_header();

   //Set the new header ctbencryption as none
   for (int i = 100; i < 104; i++)
        header[i] = 0x00;

   //Instead of this lets make a Savefile dialog
   filesystem::path filepath = save_path.remove_filename()/ "processedCTB.ctb";
   string outfilename = filepath.string();
   ofstream ctbfilestrm = refCTB.create_ctb(header, outfilename);

   header.clear();
   header.shrink_to_fit();
   return ctbfilestrm;
}

//This function runs an printer decryptor that with no feedback from the printer itself.
//The process needs only a key and nonce to run AES and the description of the area that is being encrypted.
int runOfflineDecryptor(encryption_prop eprop, printer_prop pprop){
    // Printing properties
    double layer_height_mm = pprop.layer_height_mm;  // layer height setting used at slicing, in millimeters
    double exposure_s = pprop.exposure_s;            // exposure time setting used at slicing
    double bot_exposure_s = pprop.bot_exposure_s;    // bottom exposure time setting used at slicing
    double light_off_time_s = pprop.light_off_time_s; //light off time setting used at slicing
    double bot_light_off_time_s = pprop.bot_light_off_time_s; // bottom light off time setting used at slicing
    uint32_t bot_layer_count = pprop.bot_layer_count; //number of layers configured as "bottom."
    uint32_t level_set_count = pprop.level_set_count; //number of times each layer image is repeated in the file.
    double bot_lift_dist_mm = pprop.bot_lift_dist_mm;        // distance to lift the build platform away from the vat after bottom layers
    double bot_lift_speed_mmpm = pprop.bot_lift_speed_mmpm;    // speed at which to lift the build platform away from the vat after bottom layers
    double lift_dist_mm = pprop.lift_dist_mm;            // distance to lift the build platform away from the vat after normal layers
    double lift_speed_mmpm = pprop.lift_speed_mmpm;        // speed at which to lift the build platform away from the vat after normal layers
    double retract_speed_mmpm = pprop.retract_speed_mmpm;    // speed to use when the build platform re-approaches the vat after lift
    int nlayers = pprop.nlayers;

    double lift_waitms = (lift_dist_mm+ layer_height_mm) *1000* 60 / lift_speed_mmpm;
    double bot_lift_waitms = (bot_lift_dist_mm+ layer_height_mm) * 1000 * 60  / bot_lift_speed_mmpm;
    double retract_waitms = lift_dist_mm * 1000 * 60 / retract_speed_mmpm;
    double bot_retract_waitms = bot_lift_dist_mm * 1000 * 60 / retract_speed_mmpm;
   
    //bot_exposure_s = 3; //Just to test 
    //exposure_s = 2;
    double fix_delay = 0; // 4000;
    double fix_bot_delay = 0; // 2000;
    double delayms = exposure_s * 1000 + fix_delay;
    double bot_delayms = bot_exposure_s * 1000 + fix_bot_delay;
    double fix_off_delay = 1950;

    //Decryptor properties
    int i_iniLayer = eprop.i_inilayer;
    int i_endLayer = eprop.i_endLayer;
    int res = eprop.res;
    uint64_t nonce = eprop.nonce;
    cv::Rect area = eprop.area;
    uint8_t* key{ eprop.key };
    cv::Size size(pprop.width, pprop.height);
    string window_name{"Hack Creality"};
    
    

    //Preprocess the first layer
    size_t ctr = nonce; // A different encryption every res layers
    Mat display_image = Mat(size, CV_8UC3, cv::Scalar(0x00, 0x00, 0x00));
    Mat blank(size, CV_8UC3, cv::Scalar(0x00, 0x00, 0x00));

    //Flag to only generate decryptor layers between iniLayer and endLayer
    BOOL encrypt = (0 >= i_iniLayer && 0 < i_endLayer);
    if (encrypt)
    { 
        layer_bmp my_layer_bmp = encrypt_area(blank, area, key, ctr, res);
        flip(my_layer_bmp.layer_enc, display_image, 0); // Flip image to display (same trasnformation done by the printer)
        //my_layer_bmp.layer_enc.copyTo(display_image);
    }
    displayimage_fs(display_image, window_name, 1);
    cout << "\nInstructions: Press any key to jump to the next layer, or press ESC to finish. \n Press any key to start...";
    waitKey();

    auto start = std::chrono::high_resolution_clock::now();
    double waitms = bot_delayms;
    for (int layer_no = 1; layer_no <= nlayers; layer_no++)
    {
        string add_txt{ "Delay set to: " + to_string(waitms) + " ms." };
        progressBar(layer_no, nlayers, add_txt);
        ctr = nonce + layer_no / res; // A different encryption every res layers

        //Flag to only generate decryptor layers between iniLayer and endLayer
        BOOL encrypt = (layer_no >= i_iniLayer && layer_no < i_endLayer);
        if (encrypt) {
            layer_bmp my_layer_bmp = encrypt_area(blank, area, key, ctr, res);
            flip(my_layer_bmp.layer_enc, display_image, 0); // Flip image to display (same trasnformation done by the printer)
            //my_layer_bmp.layer_enc.copyTo(display_image);
        }
        else {
            blank.copyTo(display_image);
        }


        double waitms_off;
        if (layer_no < bot_layer_count * level_set_count){
            waitms = bot_delayms; //Bottom layer exposure time
            waitms_off = light_off_time_s * 1000.0+ (lift_waitms + retract_waitms)+ fix_off_delay;
        }
        else {
            waitms = delayms;       //Normal layer exposure time
            waitms_off = bot_light_off_time_s * 1000.0 + (bot_lift_waitms + bot_retract_waitms) + fix_off_delay;
        }
            



        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        waitms = waitms - elapsed.count();
        waitms = waitms <= 0 ? 1 : waitms;
        
        int k(0);
        displayimage_fs(display_image, window_name, 1);
        if (k=waitKey((int) waitms) == 27)
            break;
       
        blank.copyTo(display_image);
        displayimage_fs(display_image, window_name, 1);
        k = 0;
        if (k = waitKey((int)waitms_off) == 27)
            break;
        
        
        start = std::chrono::high_resolution_clock::now();
    }

    cv::destroyAllWindows();
    return 0;
}

//Shows a progress bar in the Command Line
void progressBar(int progress, int max) {
    std::cout.flush();
    int _progress = (progress * 40 / max);
    std::cout << "\rProcessing layer " << progress << ". Progress: [";
    for (int i = 0; i < _progress; i++)
        std::cout << "#";

    for (int i = 0; i < 40 - _progress; i++)
        std::cout << " ";
    std::cout << "] " << (progress * 100 / max) << "%";
}

void progressBar(int progress, int max, string add_txt) {
    std::cout.flush();
    int _progress = (progress * 40 / max);
    std::cout << "\rProcessing layer " << progress << ". Progress: [";
    for (int i = 0; i < _progress; i++)
        std::cout << "#";

    for (int i = 0; i < 40 - _progress; i++)
        std::cout << " ";
    std::cout << "] " << (progress * 100 / max) << "%\t" << add_txt;
}


layer_bmp encrypt_area(cv::Mat image, cv::Rect area, uint8_t key[16], uint64_t ictr, int res)
{
    int numx = (int)ceil(area.width * 1. / res);
    int numy = (int)ceil(area.height * 1. / res);
    int ct_len = numx * numy / NBITS;
    ct_len = (ct_len < AES_BLOCKLEN) ? AES_BLOCKLEN : ct_len;
    layer_bmp out;
    image.copyTo(out.layer_pt);

    // We define is just and empty plain text, so that we carry the XOR operation outside of the function
    std::vector<uint8_t> pt(ct_len);
    std::vector<uint8_t> enc = aes_ctr(pt, key, ictr);

    //Map the encrypted text to a bmp using 1 px = 1 bit 
    cv::Mat enci = enc2bmp(enc, area.size(), res);


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

// Maps a every bit of the input to a binary image of size area * res
cv::Mat enc2bmp(std::vector<uint8_t> enc, cv::Size area, int res)
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