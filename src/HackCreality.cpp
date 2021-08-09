// HackCreality2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Shobjidl.h> //  COM library, don't put this in the header file
#include "../include/ctb_file.h"
#include "../include/Hackcreality.h"

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


int main()
{
    int option=0;

    while (messageListener(&option)) 
    {
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
        case 0: 
        {
            break;
        }
        
        case 1: 
        {
            *option = 0;
            wstring file_name;
            if (openFileDialog(&file_name)) 
            {
                in_ctb.assign(file_name);
                myCTB = CTB(in_ctb);

                if (myCTB.get_key() != 0x0000'0000)
                {
                    std::cout << "File is encrypted..." << std::endl;
                }

                if (myCTB.get_all_layers().size() != 0) 
                {
                    std::cout << "Plotting preview image ...";
                    plotandwait(myCTB.get_preview1());
                    std::cout << "\nCTB file opened. ";
                }
            
            }
            break;
        }

        case 2:
        {
            *option = 0;
            wstring file_name;
            if (openFileDialog(&file_name))
            {
                in_ctb.assign(file_name);
                myCTB.read_CTB(in_ctb);

                wstring output_fname;

                std::cout << "Enter the output file name: ";
                wcin >> output_fname;
                if (output_fname == L"") output_fname = L"default_output.ctb";

                myCTB.decrypt_ctb_file(output_fname);

            }
            break;
        }

        case 3:
        {
            *option = 0;
            wstring file_name;
            if (openFileDialog(&file_name))
            {
                in_ctb.assign(file_name);
                myCTB.read_CTB(in_ctb);

                wstring output_fname;
                uint32_t key = 0;
                

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

                std::cout << "Enter the output file name: ";
                wcin >> output_fname;
                if (output_fname == L"") output_fname = L"default_output.ctb";

                myCTB.encrypt_ctb_file(key, output_fname);

            }
            break;
        }

        case 4: 
        {
            *option = 0;
            generateDecryptorImages(myCTB,  encProp, in_ctb);
            break;
        }
        case 7: 
        {
            *option = 0;
            wstring file_name = PATH + L"\\imgs\\test_img.bmp";
            string window_name = displayimage_fs(file_name, NULLSTR, 0);
            waitKey();
            destroyWindow(window_name);
            break;
        }
            
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



// Rotate an image and return one within a bounding box that fully contains the image.
cv::Mat rotateImage(Mat image, double angle) {
    Point2f image_center((image.cols - 1) / 2., (image.rows - 1) / 2.);
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
    namedWindow(window_name, WINDOW_NORMAL| WINDOW_GUI_EXPANDED);    // Create a window
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
    return(window_name);                        //destroy the created window
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

    int extract_dim = prop.extract_dim;
    int im_width = myCtb.get_width();
    int im_heigth = myCtb.get_height();
    int i_iniLayer = prop.i_inilayer;
    int i_endLayer = prop.i_endLayer;
    int res = prop.res;
    int nonce = prop.nonce;
    uint8_t* key{ prop.key };

    auto all_layers = myCtb.get_all_layers();
    int no_layers   = myCtb.get_no_layers();
    vector<ctbLayer>::iterator it_end     = all_layers.end();
    vector<ctbLayer>::iterator it_begin   = all_layers.begin();

    //Only decrypt the layers between iniLayer and endLayer
    vector<ctbLayer>::iterator it_iniLayer = it_begin + \
                                             min(i_iniLayer, no_layers);

    vector<ctbLayer>::iterator it_endLayer = it_iniLayer + \
                                             min( (i_iniLayer + i_endLayer), no_layers);

    cv::Rect area((im_width - extract_dim) / 2, (im_heigth - extract_dim) / 2, \
        extract_dim, extract_dim);
    encProp.area = area;

    int layer_no = 1;
    for (vector<ctbLayer>::iterator it = it_begin; it != it_end; it++) 
    {
        ctbLayer layer = *it;

        // Build bitmaps from the layer data and their encrypted versions
        Mat imlayer = myCtb.getLayerImageRL7(layer, im_width, im_heigth);
        layer_bmp my_layer_bmp;

        int ctr = nonce + layer_no / res;

        //Only modify the layers between iniLayer and endLayer
        BOOL encrypt = (it >= it_iniLayer && it < it_endLayer);
        if (encrypt)
            my_layer_bmp = myCtb.encrypt_area(imlayer, area, key, ctr, res);
        else 
        {
            my_layer_bmp.layer_ct = imlayer;
            my_layer_bmp.layer_pt = imlayer;
            my_layer_bmp.layer_enc = \
                cv::Mat(imlayer.size(), CV_8UC3, cv::Scalar(0x00, 0x00, 0x00));
        }

        // WORK

        /* Instead of generating sets of bitmaps, we can  just build layer by layer the new ctb file
        without ever needing to have the complete description of  all the layers in memory. 
        */


        //string file = string(filepath.begin(), filepath.end()) + "pt_lay" + to_string(layer_no)+".bmp";
        filesystem::path filepath = p / ("pt_lay" + to_string(layer_no) + ".bmp");
        cv::imwrite(filepath.u8string(), my_layer_bmp.layer_pt);

        filepath = p / ("ct_lay" + to_string(layer_no) + ".bmp");
        cv::imwrite(filepath.u8string(), my_layer_bmp.layer_ct);

        filepath = p / ("enc_lay" + to_string(layer_no) + ".bmp");
        cv::imwrite(filepath.u8string(), my_layer_bmp.layer_enc);


        if (VERBOSE) std::cout << "Writing layer " << layer_no << std::endl;
        layer_no++;

    }
    //destroyWindow("draw");
    cout << "All transformed bmp files were generated. Generating new ctb --- Not really..";  
    return 0;
}