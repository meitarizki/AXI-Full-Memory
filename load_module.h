#ifndef LOAD_MODULE_H
#define LOAD_MODULE_H

#include <systemc.h>

SC_MODULE(load_module) {
    sc_in<bool> ACLK;
    sc_in<bool> ARESETN;

    // NEW: The dynamic starting address pin driven by the top-level system
    sc_in<sc_uint<32>> START_ADDR; 

    // --- WRITE CHANNELS ---
    sc_out<sc_uint<32>> AWADDR, WDATA;
    sc_out<sc_uint<8>>  AWLEN;
    sc_out<bool>        AWVALID, WVALID, WLAST, BREADY;
    sc_in<bool>         AWREADY, WREADY, BVALID;
    sc_in<sc_uint<2>>   BRESP;

    // --- READ CHANNELS ---
    sc_out<sc_uint<32>> ARADDR; 
    sc_out<sc_uint<8>>  ARLEN;
    sc_out<bool>        ARVALID, RREADY;
    sc_in<bool>         ARREADY, RVALID, RLAST;
    sc_in<sc_uint<32>>  RDATA;
    sc_in<sc_uint<2>>   RRESP;

    void drive_test() {
        // 0. INITIALIZE
        AWVALID.write(0); WVALID.write(0); WLAST.write(0); BREADY.write(0);
        ARVALID.write(0); RREADY.write(0);
        wait(); // CPU starts immediately (no stagger)

        // 1. FILL THE 1KB MEMORY (Write 32 Rows)
        cout << "--- STARTING MAXIMUM WRITE BURSTS ---" << endl;
        
        // NO MORE HARDCODING: Capture the starting address from the new pin
        sc_uint<32> current_address = START_ADDR.read(); 
        int rows_written = 0;
        
        while (rows_written < 32) { 
            // Address Phase
            AWADDR.write(current_address); AWLEN.write(3); AWVALID.write(1);
            do { wait(); } while (AWREADY.read() == 0);
            AWVALID.write(0);

            // Data Phase (4 beats = 16 bytes = 1 full width)
            for (int i = 0; i < 4; i++) {
                WDATA.write(0xAA000000 + rows_written + i); // CPU Signature Data
                WVALID.write(1);
                if (i == 3) WLAST.write(1); else WLAST.write(0);
                do { wait(); } while (WREADY.read() == 0);
            }
            WVALID.write(0); WLAST.write(0);

            // Response Phase
            BREADY.write(1);
            do { wait(); } while (BVALID.read() == 0);
            BREADY.write(0);

            // Move Address down by exactly 1 STRIDE (32 bytes) for the next burst
            current_address += 32; 
            rows_written++;
        }

        // 2. READ THE 1KB MEMORY BACK
        wait(20);
        cout << "--- STARTING MAXIMUM READ BURSTS ---" << endl;
        
        // NO MORE HARDCODING: Capture the starting address again for the read phase
        current_address = START_ADDR.read(); 
        int rows_read = 0;

        while (rows_read < 32) {
            ARADDR.write(current_address); ARLEN.write(3); ARVALID.write(1);
            do { wait(); } while (ARREADY.read() == 0);
            ARVALID.write(0);

            RREADY.write(1); 
            int read_count = 0;
            while (read_count < 4) {
                wait();
                if (RVALID.read() == 1) {
                    cout << "CPU Read Row " << rows_read << " Data: " << hex << RDATA.read() << endl;
                    read_count++;
                }
            }
            RREADY.write(0);
            
            current_address += 32;
            rows_read++;
        }

        while(true) wait();
    }

    SC_CTOR(load_module) {
        SC_THREAD(drive_test);
        sensitive << ACLK.pos();
    }
};

#endif