//Class to run an AES 128 encyption scheme. No modes of operations implemented
//Developed by Juan Villegas
//May 2020
//Code hosted in https://github.com/jevillegasd/AES
//Based on library by SergeyBel https://github.com/SergeyBel/AES 

#include "../include/aes.h"

AES::AES() {
    uint8_t nkey[AES_KEYLEN] = { 0 };
    KeyExpansion(this->roundKey, nkey);
}

AES::AES(uint8_t key[]) {
    KeyExpansion(this->roundKey, key);
}

void AES::ECB_encrypt(uint8_t* buf) {
    //TO DO, iterate thorugh the buffer and run Cipher at each iteration. Pad last one.
    Cipher((state_t*)buf, this->roundKey);
}

void AES::ECB_decrypt(uint8_t* buf) { //Pad last one.
    //TO DO, iterate thorugh the buffer and run Invipher at each iteration.
    InvCipher((state_t*)buf, this->roundKey);
}

void AES::setKey(uint8_t key[]) {
    KeyExpansion(this->roundKey, key);
}

/*****************************************************************************/
/*                           Public functions                                */
/*****************************************************************************/

void AES::KeyExpansion(gf28 RoundKey[], uint8_t Key[])
{
    unsigned i, j, k;
    uint8_t tempa[4]; // Used for the column/row operations

    // The first round key is the key itself.
    for (i = 0; i < AES_NK; ++i)
    {
        RoundKey[(i * 4) + 0] = Key[(i * 4) + 0];
        RoundKey[(i * 4) + 1] = Key[(i * 4) + 1];
        RoundKey[(i * 4) + 2] = Key[(i * 4) + 2];
        RoundKey[(i * 4) + 3] = Key[(i * 4) + 3];
    }

    // All other round keys are found from the previous round keys.
    for (i = AES_NK; i < AES_NB * (AES_NR + 1); ++i)
    {
        {
            k = (i - 1) * 4;
            tempa[0] = RoundKey[k + 0];
            tempa[1] = RoundKey[k + 1];
            tempa[2] = RoundKey[k + 2];
            tempa[3] = RoundKey[k + 3];
        }

        if (i % AES_NK == 0)
        {
            // This function shifts the 4 bytes in a word to the left once.
            // [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

            // Function RotWord()
            {
                const uint8_t u8tmp = tempa[0];
                tempa[0] = tempa[1];
                tempa[1] = tempa[2];
                tempa[2] = tempa[3];
                tempa[3] = u8tmp;
            }

            // SubWord() is a function that takes a four-byte input word and 
            // applies the S-box to each of the four bytes to produce an output word.

            // Function Subword()
            {
                tempa[0] = getSBoxValue(tempa[0]);
                tempa[1] = getSBoxValue(tempa[1]);
                tempa[2] = getSBoxValue(tempa[2]);
                tempa[3] = getSBoxValue(tempa[3]);
            }

            tempa[0] = tempa[0] ^ Rcon[i / AES_NK];
        }
#if defined(AES256) && (AES256 == 1)
        if (i % Nk == 4)
        {
            // Function Subword()
            {
                tempa[0] = getSBoxValue(tempa[0]);
                tempa[1] = getSBoxValue(tempa[1]);
                tempa[2] = getSBoxValue(tempa[2]);
                tempa[3] = getSBoxValue(tempa[3]);
            }
        }
#endif
        j = i * 4; k = (i - AES_NK) * 4;
        RoundKey[j + 0] = RoundKey[k + 0] ^ tempa[0];
        RoundKey[j + 1] = RoundKey[k + 1] ^ tempa[1];
        RoundKey[j + 2] = RoundKey[k + 2] ^ tempa[2];
        RoundKey[j + 3] = RoundKey[k + 3] ^ tempa[3];
    }
}

void AES::AddRoundKey(uint8_t round, state_t* state, const gf28* RoundKey) {
    uint8_t i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            (*state)[i][j] += RoundKey[(round * AES_NB * 4) + (i * AES_NB) + j];
        }
    }
}

void AES::InvAddRoundKey(uint8_t round, state_t* state, const gf28* RoundKey) {
    uint8_t i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            int pos = (round * AES_NB * 4) + (i * AES_NB) + j;
            (*state)[i][j] += RoundKey[pos];
        }
    }
}

void AES::SubBytes(state_t* state) {
    uint8_t i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            (*state)[j][i] = getSBoxValue((*state)[j][i]);
        }
    }
}

void AES::ShiftRows(state_t* state) {
    uint8_t temp;

    // Rotate first row 1 columns to left  
    temp = (*state)[0][1];
    (*state)[0][1] = (*state)[1][1];
    (*state)[1][1] = (*state)[2][1];
    (*state)[2][1] = (*state)[3][1];
    (*state)[3][1] = temp;

    // Rotate second row 2 columns to left  
    temp = (*state)[0][2];
    (*state)[0][2] = (*state)[2][2];
    (*state)[2][2] = temp;

    temp = (*state)[1][2];
    (*state)[1][2] = (*state)[3][2];
    (*state)[3][2] = temp;

    // Rotate third row 3 columns to left
    temp = (*state)[0][3];
    (*state)[0][3] = (*state)[3][3];
    (*state)[3][3] = (*state)[2][3];
    (*state)[2][3] = (*state)[1][3];
    (*state)[1][3] = temp;
}

void AES::MixColumns(state_t* state) {
    uint8_t i;
    gf28 tmp1, tmp2, tmp3;
    for (i = 0; i < 4; ++i) {
        tmp1 = (*state)[i][0] * 2 + (*state)[i][1] * 3 + (*state)[i][2] * 1 + (*state)[i][3] * 1;
        tmp2 = (*state)[i][0] * 1 + (*state)[i][1] * 2 + (*state)[i][2] * 3 + (*state)[i][3] * 1;
        tmp3 = (*state)[i][0] * 1 + (*state)[i][1] * 1 + (*state)[i][2] * 2 + (*state)[i][3] * 3;
        (*state)[i][3] = (*state)[i][0] * 3 + (*state)[i][1] * 1 + (*state)[i][2] * 1 + (*state)[i][3] * 2;

        (*state)[i][0] = tmp1; (*state)[i][1] = tmp2; (*state)[i][2] = tmp3;
    }
}

void AES::InvMixColumns(state_t* state) {
    int i;
    gf28 tmp1, tmp2, tmp3;
    for (i = 0; i < 4; ++i) {
        tmp1 = (*state)[i][0] * 14 + (*state)[i][1] * 11 + (*state)[i][2] * 13 + (*state)[i][3] * 9;
        tmp2 = (*state)[i][0] * 9 + (*state)[i][1] * 14 + (*state)[i][2] * 11 + (*state)[i][3] * 13;
        tmp3 = (*state)[i][0] * 13 + (*state)[i][1] * 9 + (*state)[i][2] * 14 + (*state)[i][3] * 11;
        (*state)[i][3] = (*state)[i][0] * 11 + (*state)[i][1] * 13 + (*state)[i][2] * 9 + (*state)[i][3] * 14;

        (*state)[i][0] = tmp1; (*state)[i][1] = tmp2; (*state)[i][2] = tmp3;
    }
}

void AES::InvSubBytes(state_t* state) {
    uint8_t i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            (*state)[j][i] = getSBoxInvert((*state)[j][i]);
        }
    }
}

void AES::InvShiftRows(state_t* state) {
    gf28 temp;

    // Rotate first row 1 columns to right  
    temp = (*state)[3][1];
    (*state)[3][1] = (*state)[2][1];
    (*state)[2][1] = (*state)[1][1];
    (*state)[1][1] = (*state)[0][1];
    (*state)[0][1] = temp;

    // Rotate second row 2 columns to right 
    temp = (*state)[0][2];
    (*state)[0][2] = (*state)[2][2];
    (*state)[2][2] = temp;

    temp = (*state)[1][2];
    (*state)[1][2] = (*state)[3][2];
    (*state)[3][2] = temp;

    // Rotate third row 3 columns to right
    temp = (*state)[0][3];
    (*state)[0][3] = (*state)[1][3];
    (*state)[1][3] = (*state)[2][3];
    (*state)[2][3] = (*state)[3][3];
    (*state)[3][3] = temp;
}

void AES::Cipher(state_t* state, const gf28 RoundKey[AES_keyExpSize])
{
    uint8_t round = 0;

    // Add the First round key to the state before starting the rounds.
    AddRoundKey(0, state, RoundKey);

    // There will be Nr rounds.
    // The first Nr-1 rounds are identical.
    // These Nr-1 rounds are executed in the loop below.
    for (round = 1; round < AES_NR; ++round) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(round, state, RoundKey);
    }

    // The last round is given below.
    // The MixColumns function is not here in the last round.
    //std::cout << "\nCorrect M9:\t";  phex(*state);
    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(AES_NR, state, RoundKey);
}

void AES::InvCipher(state_t* state, const gf28 RoundKey[AES_keyExpSize])
{
    uint8_t round = AES_NR;
    InvAddRoundKey(AES_NR, state, RoundKey);
    for (round = AES_NR - 1; round > 0; --round)
    {
        InvShiftRows(state);
        InvSubBytes(state);
        InvAddRoundKey(round, state, RoundKey);
        InvMixColumns(state);
    }
    InvShiftRows(state);
    InvSubBytes(state);
    InvAddRoundKey(0, state, RoundKey);
}

gf28 AES::byte_sbox(gf28 byte) {
    return sbox[(byte)];
}

gf28 AES::byte_rsbox(gf28 byte) {
    return rsbox[(byte)];
}

gf28 AES::ISB(gf28 byte) {
    return rsbox[(byte)];
}

