#include <SPI.h>
#include <SD.h>
#include "test_file_encoder.h"

void
setup(
)
{
    SPI.begin();
    SD.begin(SD_CS_PIN);
    Serial.begin(BAUD_RATE);
    runalltests_file_encode_id();
}

void
loop(
)
{

}