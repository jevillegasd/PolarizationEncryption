# HackCreality
This project allows for the modification of layer data in .obj slice files for resin 3D printers using Chitubox. The system reads and decomposes the file into the individual slices that correspond to each of the masks that the printer uses to expose each layer and allows for the encryption of an individual layer using AES-128 in counter mode. The user can define the AES key, and the nonce and the AES engine use the layer number as part of the counter to ensure that each layer is uniquely encrypted.

## USE
Functionality is currently limited to:
- Open a new .obj file.
- Encrypt using AES a set of layers from the file.
- Unlock and lock the slice file (.obj). This is only necessary if upon slicing the version of Chitubox used is protecting the file.
- Run an offline polarization decryption operation (shows sequence of layers that decrypt the masks presented by the printer).

## OFFLINE POLARIZATION DECRYPTION
The slice file contains information about the printing speed, including layer exposure time and platform movement speed, and distance. The software uses this information to estimate the time at which a printer will be exposing each layer.
This time however may vary slightly from printer to printer and needs to be carefully adjusted given that there is no feedback from the printer to update the printing status. The user may at any moment change the layer being shown by the polarization decryptor using the arrow keys and finish the operation by hitting 'ESC'.
At this stage, the image output needs to be selected for the keyboard to work.

Because we are using the AES engine in counter mode, the software does not need access to the encrypted image to send the decryption masks (decryption is done directly by the printer), however, the user needs to input correctly the decryption area, voxel size, nonce, and secret key.


<!-----

Yay, no errors, warnings, or alerts!

Conversion time: 0.928 seconds.


Using this Markdown file:

1. Paste this output into your source file.
2. See the notes and action items below regarding this conversion run.
3. Check the rendered output (headings, lists, code blocks, tables) for proper
   formatting and use a linkchecker before you publish this page.

Conversion notes:

* Docs to Markdown version 1.0β33
* Tue Jun 07 2022 09:33:49 GMT-0700 (PDT)
* Source doc: randz
* Tables are currently converted to HTML tables.
----->


## FILE PROCESSING STEPS

_Load File:_ the file is parsed according to the format described in [this GitHub Repository](https://github.com/cbiffle/catibo/blob/master/doc/cbddlp-ctb.adoc). The linked repository provides extensive information about the file format.

_Create new file_: here, only the header information, preview images, and table of layer data (table describing the address and run length of masks for each layer) are read and written into a new file.

_Load all mask data into array:_ the mask data containing the values of the pixel are loaded into an array for processing.

_Load decryption key from header:_ as described [here](https://github.com/cbiffle/catibo/blob/master/doc/cbddlp-ctb.adoc), the 32-bit key used for the XOR-based Stream Cipher (XSC) is stored in the byte 25 of the file header. If the key is 0x00000000 then the file is unencrypted. Otherwise, the file is encrypted and can be decypted using the procedure described below which can also be found [here](https://github.com/cbiffle/catibo/blob/master/doc/cbddlp-ctb.adoc).

_RLE decode all masks:_ the data encoding and decoding scheme is described [here](https://github.com/cbiffle/catibo/blob/master/doc/cbddlp-ctb.adoc).

_Extract portions to be encrypted from the decoded masks:_ as specified in the _processing properties_ for the processing operation, the square portion to be encrypted is extracted from each mask.

_AES-encrypt extracted portions:_ the extracted portions are encrypted using the AES secret key provided by the user and the nonce which is also provided by the user. Currently, the nonce used is the layer number. Setting the nonce to the layer number exposes the system to _plaintext attacks_. Thwarting this attack can be achieved by changing the nonce (can be done through randomization) for different processing runs.

Write back encrypted portions to their position in the masks from which they were extracted.

_RLE encode all masks:_ during this RLE encoding, each layer’s length may change due to the change in the sequence of bits as a result of the AES-encryption operation.

_XSC-encrypt encoded masks:_ the encoded masks are encrypted using the 32-bit key read from the header.

_Write encrypted masks to new file:_ the XSC-encypted masks from the previous step are then written back to the newly created ctb file which only contains the file header, preview images, and the table of layer data (table describing the address and run length of masks for each layer). Since the run length of each mask may change after RLE encoding of the AES-encrypted mask, the information in the table of layer data is updated as necessary.

## FILE PROCESSING PROPERTIES

The following are the properties that determine how the file is processed:

* 16-byte AES encryption key chosen by user.
* Nonce for AES-CTR mode encryption.
* Side dimension of square to be encrypted (the software currently supports only encryption of squares centered in the middle of the mask. Improvements will allow encryption of different shapes at different locations on the mask.)
* Encryption resolution: this determines the number of pixels (N) to form the side of a square used to collectively decrypt NxN pixels from the original printer screen. By so doing, even if the pixels on both screens are slightly misaligned with their corresponding pixels on the other screen, they can still be decrypted by the surrounding pixels which they may be aligned with. This encryption resolution is further explained and demonstrated the section below.
* Index of first and last masks to be processed. Only mask with index within the specified top and bottom masks are necessarily processed. Improvements will allow arbitrary mask indexes to be processed.

## ENCRYPTION RESOLUTION

The need for an encryption resolution arises as a result of the possibilities of misalignment errors between the upper and lower LCD screens due to the very small sizes of the pixel. Slight misalignments would lead to incorrect decryption of the data and the subsequent printing of an incorrect part. To minimize the effects of misalignments, we use the same encryption bit to encrypt all pixels within a certain bounding square, thereby allowing for slight misalignments without catastrophic impacts on the part printed. This is demonstrated by Tables 1-4.

**Table 1:** Aligned screen case with 1x1 resolution


<table>
  <tr>
   <td>

<table>
  <tr>
   <td>1
   </td>
   <td>0
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>0
   </td>
   <td>1
   </td>
   <td>0
   </td>
   <td>0
   </td>
  </tr>
</table>


Bottom LCD

   </td>
   <td>

<table>
  <tr>
   <td>0
   </td>
   <td>1
   </td>
   <td>0
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>1
   </td>
   <td>0
   </td>
   <td>0
   </td>
   <td>0
   </td>
  </tr>
</table>


Top LCD

   </td>
   <td>

<table>
  <tr>
   <td>1
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>0
   </td>
  </tr>
  <tr>
   <td><strong>1</strong>
   </td>
   <td><strong>1</strong>
   </td>
   <td><strong>0</strong>
   </td>
   <td><strong>0</strong>
   </td>
  </tr>
</table>


Output

   </td>
  </tr>
</table>


**Table 2:** Unaligned screen case with 1x1 resolution


<table>
  <tr>
   <td>

<table>
  <tr>
   <td>1
   </td>
   <td>0
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>0
   </td>
   <td>1
   </td>
   <td>0
   </td>
   <td>0
   </td>
  </tr>
  <tr>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
  </tr>
</table>


Bottom LCD

   </td>
   <td>

<table>
  <tr>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
  </tr>
  <tr>
   <td>0
   </td>
   <td>1
   </td>
   <td>0
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>1
   </td>
   <td>0
   </td>
   <td>0
   </td>
   <td>0
   </td>
  </tr>
</table>


Top LCD

   </td>
   <td>

<table>
  <tr>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
  </tr>
  <tr>
   <td><strong>0</strong>
   </td>
   <td><strong>0</strong>
   </td>
   <td><strong>0</strong>
   </td>
   <td><strong>1</strong>
   </td>
  </tr>
  <tr>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
  </tr>
</table>


Output

   </td>
  </tr>
</table>


**Table 3:** Aligned screen case with 2x2 resolution


<table>
  <tr>
   <td>

<table>
  <tr>
   <td>1
   </td>
   <td>0
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>0
   </td>
   <td>1
   </td>
   <td>0
   </td>
   <td>0
   </td>
  </tr>
</table>


Bottom LCD

   </td>
   <td>

<table>
  <tr>
   <td>1
   </td>
   <td>1
   </td>
   <td>0
   </td>
   <td>0
   </td>
  </tr>
  <tr>
   <td>1
   </td>
   <td>1
   </td>
   <td>0
   </td>
   <td>0
   </td>
  </tr>
</table>


Top LCD

   </td>
   <td>

<table>
  <tr>
   <td>0
   </td>
   <td>1
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td><strong>1</strong>
   </td>
   <td><strong>0</strong>
   </td>
   <td><strong>0</strong>
   </td>
   <td><strong>0</strong>
   </td>
  </tr>
</table>


Output

   </td>
  </tr>
</table>


**Table 4:** Unaligned screen case with 2x2 resolution


<table>
  <tr>
   <td>

<table>
  <tr>
   <td>1
   </td>
   <td>0
   </td>
   <td>1
   </td>
   <td>1
   </td>
  </tr>
  <tr>
   <td>0
   </td>
   <td>1
   </td>
   <td>0
   </td>
   <td>0
   </td>
  </tr>
  <tr>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
  </tr>
</table>


Bottom LCD

   </td>
   <td>

<table>
  <tr>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
  </tr>
  <tr>
   <td>1
   </td>
   <td>1
   </td>
   <td>0
   </td>
   <td>0
   </td>
  </tr>
  <tr>
   <td>1
   </td>
   <td>1
   </td>
   <td>0
   </td>
   <td>0
   </td>
  </tr>
</table>


Top LCD

   </td>
   <td>

<table>
  <tr>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
  </tr>
  <tr>
   <td><strong>1</strong>
   </td>
   <td><strong>0</strong>
   </td>
   <td><strong>0</strong>
   </td>
   <td><strong>0</strong>
   </td>
  </tr>
  <tr>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
   <td><strong>-</strong>
   </td>
  </tr>
</table>


Output

   </td>
  </tr>
</table>





## Limitations and Future Work

Currently, synchronizing the add-on LCD with the original printer's LCD is done by reading the estimated screen time of each mask from the slicer. This introduces slight discrepancies in the timing of the mask display on both LCDs. Eliminating this timing discrepancy may be achieved by using the mask-change signal from the printer to control the add-on LCD.