# HackCreality
This project allows for the modification of layer data in .obj slice files for resin 3D printers using Chitubox. The system reads and decomposes the file into the individual slices that correspond to each of the masks that the printer uses to expose each layer and allows for the encryption of an individual layer using AES-128 in counter mode. The user can define the AES key, and the nonce and the AES engine use the layer number as part of the counter to ensure that each layer is uniquely encrypted.

## Use
Functionality is currently limited to:
- Open a new .obj file.
- Encrypt using AES a set of layers from the file.
- Unlock and lock the slice file (.obj). This is only necessary if upon slicing the version of Chitubox used is protecting the file.
- Run an offline polarization decryption operation (shows sequence of layers that decrypt the masks presented by the printer).

## Offline polarization decryption
The slice file contains information about the printing speed, including layer exposure time and platform movement speed, and distance. The software uses this information to estimate the time at which a printer will be exposing each layer.
This time however may vary slightly from printer to printer and needs to be carefully adjusted given that there is no feedback from the printer to update the printing status. The user may at any moment change the layer being shown by the polarization decryptor using the arrow keys and finish the operation by hitting 'ESC'.
At this stage, the image output needs to be selected for the keyboard to work.

Because we are using the AES engine in counter mode, the software does not need access to the encrypted image to send the decryption masks (decryption is done directly by the printer), however, the user needs to input correctly the decryption area, voxel size, nonce, and secret key.
