#include <systemc.h>

// PacketParser for detecting various packet formats (Ethernet, IPvN, UDP, VXLAN, GRE, etc.)
SC_MODULE(PacketParser) {
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<sc_uint<8>> packet_in;
    sc_out<sc_uint<8>> next_protocol_out;

    // Internal variables for parsing headers
    sc_uint<8> state;
    sc_uint<8> current_header_type;
    sc_uint<8> current_header_length;
    sc_uint<8> header_bytes_parsed;

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
                case 0: // Start parsing Ethernet header
                    if (header_bytes_parsed < 14) {
                        header_bytes_parsed++; // Count 14 bytes for Ethernet
                    } else {
                        current_header_type = packet_in.read(); // Last byte of Ethernet for next header
                        header_bytes_parsed = 0;
                      	parse_next_header();// Parse next header based on Ethernet next header type
                    }
                    break;
                
               	case 1: // Parse IPvN
                    parse_header();
                    break;
              	
              	case 2: // Parse TCP/UDP
                    parse_header();
                    break;

                case 3: // Parse VXLAN
                    parse_header();
                    break;

                case 4: // Parse GRE Tunnel
                    parse_header();
                    break;

                case 5: // Parse MPLS
                    parse_header();
                    break;

              	case 6: // Parse data
                    parse_header();
                    reset_parser(); //if currently finish read DATA so rst to next ethernet packet
                    break;
              
                default:
                    state = 0; // Reset if state is invalid
                    break;
            }
        }
    }

    // Reset function
    void reset_parser() {
        state = 0;
        current_header_type = 0;
        current_header_length = 0;
        header_bytes_parsed = 0;
        next_protocol_out.write(0);
    }

    // Parse next header based on the type
    void parse_next_header() {
        switch (current_header_type) {
            case 0x08: // IPvN
                state = 1;
                break;
            case 0x11: // UDP (after IP)
                state = 2; // Reset after completion
                break;
            case 0x2F: // GRE Tunnel
                state = 4;
                break;
            case 0x84: // VXLAN
                state = 3;
                break;
            case 0x88: // MPLS in IP
                state = 5;
                break;
            case 0xff: // DATA
                state = 6;
                break;  
            default:
                state = 0; // Reset for unknown header
                break;
        }
    }

    // Parsing header
        void parse_header() {
        if (header_bytes_parsed == 0) {
            current_header_length = packet_in.read(); // First byte is header length
        } else if (header_bytes_parsed < current_header_length - 1) {
            header_bytes_parsed++;
        } else {
            current_header_type = packet_in.read(); // Last byte is the next header
            parse_next_header();
        }
    }
};

int sc_main(int argc, char* argv[]) {
    return 0;
}
