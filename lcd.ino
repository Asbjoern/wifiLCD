 void lcd_init()
{
    Tft.TFTinit();
    Tft.sendCMD(0x36);            //Flip display
    Tft.WRITE_DATA(0xC8);
    
    TFT_BL_ON;
 }
 
#define BUFFPIXEL       60                      // must be a divisor of 240 
#define BUFFPIXEL_X3    BUFFPIXEL*3                     // BUFFPIXELx3

void bmpDraw(WiFiClient f)
{
    uint32_t time = millis();
    uint8_t pxbuffer[BUFFPIXEL_X3];                 // 3 * pixels to buffer

    for (int i=319; i>= 0; i--)
    {
        for(int j=0; j<(240/BUFFPIXEL); j++)
        {
            for(int n=0;n<BUFFPIXEL_X3;n++)
              pxbuffer[n] = f.read();
              
            uint8_t buffidx = 0;
            int offset_x = j*BUFFPIXEL;
            
            unsigned int __color[BUFFPIXEL];
            
            for(int k=0; k<BUFFPIXEL; k++)
            {
                __color[k] = pxbuffer[buffidx+2]>>3;                        // read
                __color[k] = __color[k]<<6 | (pxbuffer[buffidx+1]>>2);      // green
                __color[k] = __color[k]<<5 | (pxbuffer[buffidx+0]>>3);      // blue
                
                buffidx += 3;
            }

            Tft.setCol(offset_x, offset_x+BUFFPIXEL);
            Tft.setPage(i, i);
          
            Tft.sendCMD(0x2c);
            
            for(int m=0; m < BUFFPIXEL; m++)
              Tft.sendData(__color[m]);
        }
    }
    
    Serial.print(millis() - time, DEC);
    Serial.println(" ms");
}

boolean bmpReadHeader(WiFiClient f) 
{
    // read header
    uint32_t tmp;
    uint8_t bmpDepth;
    
    if (read16(f) != 0x4D42) {
        Serial.println("Magic bytes missing");
        return false;
    }

    // read file size
    tmp = read32(f);
    Serial.print("size 0x");
    Serial.println(tmp, HEX);

    // read and ignore creator bytes
    read32(f);

    uint32_t offset = read32(f);
    Serial.print("offset ");
    Serial.println(offset, DEC);

    // read DIB header
    tmp = read32(f);
    Serial.print("header size ");
    Serial.println(tmp, DEC);
    
    
    int bmp_width = read32(f);
    int bmp_height = read32(f);
    
    if(bmp_width != 240 || bmp_height != 320)      // if image is not 320x240, return false
    {
        return false;
    }

    if (read16(f) != 1)
    return false;

    bmpDepth = read16(f);
    Serial.print("bitdepth ");
    Serial.println(bmpDepth, DEC);

    if (read32(f) != 0) {
        Serial.println("Compression not supported!");
        return false;
    }

    read32(f); //imagesizes
    read32(f);//horizontal resolution
    read32(f);//vertical resolution
    read32(f);//number of colors
    read32(f);//number of important colors
    return true;
}

// LITTLE ENDIAN!
uint16_t read16(WiFiClient f)
{
    uint16_t d;
    uint8_t b;
    b = f.read();
    d = f.read();
    d <<= 8;
    d |= b;
    return d;
}

// LITTLE ENDIAN!
uint32_t read32(WiFiClient f)
{
    uint32_t d;
    uint16_t b;

    b = read16(f);
    d = read16(f);
    d <<= 16;
    d |= b;
    return d;
}
