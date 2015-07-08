#define TFT_DC 2
#define TFT_CS 15

#define BUFFPIXEL       60                      // must be a divisor of 240 
#define BUFFPIXEL_X3    BUFFPIXEL*3                     // BUFFPIXELx3

uint16_t idx,row,j,n;
int state;
uint8_t pxbuffer[BUFFPIXEL_X3];                 // 3 * pixels to buffer

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

void lcd_init()
{
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextWrap(true);
 }
 
void bmpPUTinit()
{
  state = 0;
  row = 319;
  idx = 0;
  j=0;
  n=0;
}

bool bmpPUT(HTTPUpload& upload)
{
  if(upload.currentSize == 0)
  {
    debugPrintln("Empty PUT",ILI9341_RED);
    state = 0;
    return false;
  }
  switch(state)
  {
   case 0:
     if(bmpCheckHeader(upload.buf))
     {
       bmpDraw(upload.buf,upload.currentSize);
       state++;
     }
     else
       state = -1;
     break; 
   case -1:
     return false;
   default:
     idx=0;
     bmpDraw(upload.buf,upload.currentSize);
     break;
     
  }
  
  return true;
}

void bmpDraw(uint8_t* data,uint32_t size)
{
    for (; row>= 0; row--)
    {
        for(; j<(240/BUFFPIXEL); j++)
        {
            for(;n<BUFFPIXEL_X3;n++)
            {
              pxbuffer[n] = data[idx++];
              if(idx>size)
                return;
            }
            n=0;
              
            uint8_t buffidx = 0;
            int offset_x = j*BUFFPIXEL;

            for(int k=0; k<BUFFPIXEL; k++)
            {
               tft.drawPixel(j*BUFFPIXEL+k,row,tft.color565(pxbuffer[buffidx+2],pxbuffer[buffidx+1],pxbuffer[buffidx+0]));
               buffidx += 3;
            }
        }
        j=0;
    }
}

void bmpDraw(WiFiClient f)
{
    uint32_t time = millis();

    tft.setAddrWindow(0, 0, 239, 319);
    for (int i=319; i>= 0; i--)
    {
        for(j=0; j<(240/BUFFPIXEL); j++)
        {
            for(n=0;n<BUFFPIXEL_X3;n++)
              pxbuffer[n] = f.read();
              
            uint8_t buffidx = 0;
            int offset_x = j*BUFFPIXEL;
            
            unsigned int __color[BUFFPIXEL];
            
            for(int k=0; k<BUFFPIXEL; k++)
            {
                tft.drawPixel(j*BUFFPIXEL+k,i,tft.color565(pxbuffer[buffidx+2],pxbuffer[buffidx+1],pxbuffer[buffidx+0]));
                buffidx += 3;
            }
        }
    }
    
    Serial.print(millis() - time, DEC);
    Serial.println(" ms");
}

boolean bmpCheckHeader(uint8_t* f) 
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
      Serial.println("Wrong dimensions");
      return false;
    }

    if (read16(f) != 1)
    {
      Serial.println("Wrong # color planes");
      return false;
    }

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

// LITTLE ENDIAN!
uint16_t read16(uint8_t* data)
{
    uint16_t d;
    uint8_t b;
    b = data[idx++];
    d = data[idx++];
    d <<= 8;
    d |= b;
    return d;
}

// LITTLE ENDIAN!
uint32_t read32(uint8_t* data)
{
    uint32_t d;
    uint16_t b;

    b = read16(data);
    d = read16(data);
    d <<= 16;
    d |= b;
    return d;
}
