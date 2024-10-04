# Ethernet-Parser
This is a readme file for Ethernet packet parser file PackateParser.cpp
that was written in system c
the code is passing compilation

For simplicity and the assumption its not intended for performace modeling
I assumed the followings:
  1. the data is being read byte by byte
  2. in each header first byte is current header length and last byte is next header type
  3. DATA payload is also a header state type

the code contain following main parts:
  1. state machine - for current header state
     here it's possible to extract header fields to dedicated buffers 
  3. 'parse_header' function - general function that is used to pivot inside each of the header types
  4. 'parse_next_header' function - this sets the next header type when reading the next header field
  5. 'reset_parser' function - after finish reading DATA payload, rst parser to receive another pckt

