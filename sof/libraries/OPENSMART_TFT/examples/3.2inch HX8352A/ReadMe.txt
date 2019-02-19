Note1: You should make sure you are using OPEN-SMART 3.2 INCH TFT LCD shield whose driver IC is HX8352A.
Note2: Plese make sure uncomment the code (#define SUPPORT_8352A ) in the head of the file MCUFRIEND_kbv.cpp.
Note3: Because the resolution of the shield is 240*400, the code in the MCUFRIEND_kbv.cpp should also be like that:
MCUFRIEND_kbv::MCUFRIEND_kbv(int CS, int RS, int WR, int RD, int RST):Adafruit_GFX(240, 400)
{
    // we can not access GPIO pins until AHB has been enabled.
}