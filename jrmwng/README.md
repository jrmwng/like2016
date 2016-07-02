# Branch DCT

Discrete Cosine Transform

C++ Header File: [dct.h](https://github.com/jrmwng/like2016/blob/dct/jrmwng/shared/dct.h)

[dct.h] defines simple overloaded functions (i.e. dct) to perform discrete cosine transform.

    using namespace jrmwng;
    
    double aalrPixel[8][8];
    {
        // initialize aalrPixel
    }
    double aalrData[8][8];
    {
        dct(aalrPixel, aalrData); // transform from aalrPixel to aalrData
    }
