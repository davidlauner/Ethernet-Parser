#include <systemc.h>

// Enum for header types
enum packet_type {
    ETHERNET,
    IPVN,
    TCP_UDP,
    VXLAN,
    GRE,
    MPLS,
    DATA
};

// PacketParser for detecting various packet formats (Ethernet, IPvN, UDP, VXLAN, GRE, etc.)
SC_MODULE(PacketParser) {
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<sc_uint<8>> packet_in;  // 8-bit packet input (byte by byte)
    sc_out<packet_type> parsed_type;

    // Internal variables for parsing headers
    sc_uint<8> current_header_type;
    sc_uint<8> current_header_length;
    sc_uint<8> header_bytes_parsed;
    
    // Internal state for parsing
    packet_type state;
    
    // Constructor
    SC_CTOR(PacketParser) {
        SC_METHOD(parse_packet);
        sensitive << clk.pos();
    }

    // Parsing logic for Ethernet, IPvN, GRE, MPLS, etc.
    void parse_packet() {
        if (reset.read() == true) {
            reset_parser();
        } else {
            switch (state) {
                case ETHERNET: // Start parsing Ethernet header
                    parsed_type.write(ETHERNET);
                    ///////////////////////////////////
                    // place holder for taking Eth necessary fields into a buffer like: source and destination MAC
                    ///////////////////////////////////
                    if (header_bytes_parsed < 14) {
                        header_bytes_parsed++; // Count 14 bytes for Ethernet
                    } else {
                        current_header_type = packet_in.read(); // Last byte of Ethernet for next header
                        header_bytes_parsed = 0;
                      	parse_next_header();// Parse next header based on Ethernet next header type
                    }
                    break;
               	case IPVN: // Parse IPVN
                    parsed_type.write(IPVN);
                    //////////////////////////////////////////
                    // place holder for taking IPVN necessary fields into a buffer like: source and destination IP
                    //////////////////////////////////////////
                    parse_header();
                    break;
              	case TCP_UDP: // Parse TCP/UDP
                    parsed_type.write(TCP_UDP);
                    //////////////////////////////////////////
                    // place holder for taking UDP necessary fields into a buffer like: source and destination PORT
                    //////////////////////////////////////////
                    parse_header();
                    break;

                case VXLAN: // Parse VXLAN
                    parsed_type.write(VXLAN);
                    //////////////////////////////////////////
                    // place holder for taking VXLAN necessary fields into a buffer
                    //////////////////////////////////////////
                    parse_header();
                    break;

                case GRE: // Parse GRE Tunnel
                    parsed_type.write(GRE);
                    //////////////////////////////////////////
                    // place holder for taking VXLAN necessary fields into a buffer
                    //////////////////////////////////////////
                    parse_header();
                    break;

                case MPLS: // Parse MPLS
                    parsed_type.write(MPLS);
                    //////////////////////////////////////////
                    // place holder for taking VXLAN necessary fields into a buffer
                    //////////////////////////////////////////
                    parse_header();
                    break;

              	case DATA: // Parse DATA
                    parsed_type.write(DATA);
                    //////////////////////////////////////////
                    // movinf payload data to next block in pipe
                    //////////////////////////////////////////
                    parse_header();
                    reset_parser(); //if currently finish read DATA so rst to next ethernet packet
                    break;
              
                default:
                    state = ETHERNET; // Reset if state is invalid
                    break;
            }
        }
    }

    // Reset function
    void reset_parser() {
        state = ETHERNET;
        current_header_type = 0;
        current_header_length = 0;
        header_bytes_parsed = 0;
        parsed_type.write(ETHERNET);
    }

    // Parse next header based on the type
    void parse_next_header() {
        switch (current_header_type) {
            case 0x00: // Ethernet
                state = ETHERNET;
                break;
            case 0x08: // IPvN
                state = IPVN;
                break;
            case 0x11: // UDP (after IP)
                state = TCP_UDP; 
                break;
            case 0x2F: // GRE Tunnel
                state = GRE;
                break;
            case 0x84: // VXLAN
                state = VXLAN;
                break;
            case 0x88: // MPLS in IP
                state = MPLS;
                break;
            case 0xff: // DATA
                state = DATA;
                break;  
            default:
                state = ETHERNET; // Reset for unknown header
                break;
        }
    }

    // Parsing header - function for increment parser reading   
        void parse_header() {
        if (header_bytes_parsed == 0) {
            current_header_length = packet_in.read(); // First byte is header length
        } else if (header_bytes_parsed < current_header_length - 1) {
            header_bytes_parsed++;
        } else {
            current_header_type = packet_in.read(); // Last byte is the next header
            header_bytes_parsed = 0; //rst header bytes cntr for next header type
            parse_next_header();
        }
    }
};

int sc_main(int argc, char* argv[]) {
    return 0;
}
