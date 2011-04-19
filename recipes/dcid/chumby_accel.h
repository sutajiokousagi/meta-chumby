struct eeprom_data {
  unsigned int address;  // address of read or write
  unsigned char data;    // data to be written (or read data upon return)
};
