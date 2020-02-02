#include "SPI_CRIS_lib.hpp"

void setup() {
  InitCrisSpi();
}

void loop() {
  LoopCrisSpi();
}

ISR(SPI_STC_vect) {
  ISRCrisSpi();
}
