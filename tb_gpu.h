#ifndef TB_GPU_H
#define TB_GPU_H

#include <systemc.h>

SC_MODULE(tb_gpu) {
    sc_in<bool> ACLK, ARESETN;
    sc_out<sc_uint<32>> AWADDR, WDATA; sc_out<sc_uint<8>> AWLEN; sc_out<bool> AWVALID, WVALID, WLAST, BREADY;
    sc_in<bool> AWREADY, WREADY, BVALID; sc_in<sc_uint<2>> BRESP;
    sc_out<sc_uint<32>> ARADDR; sc_out<sc_uint<8>> ARLEN; sc_out<bool> ARVALID, RREADY;
    sc_in<bool> ARREADY, RVALID, RLAST; sc_in<sc_uint<32>> RDATA; sc_in<sc_uint<2>> RRESP;

    void drive_test() {
        AWVALID.write(0); WVALID.write(0); WLAST.write(0); BREADY.write(0); ARVALID.write(0); RREADY.write(0);
        wait(50); // Stagger the start time!

        sc_uint<32> current_address = 0x1000; 
        int rows_written = 0;
        
        while (rows_written < 32) { 
            AWADDR.write(current_address); AWLEN.write(3); AWVALID.write(1);
            do { wait(); } while (AWREADY.read() == 0);
            AWVALID.write(0);

            for (int i = 0; i < 4; i++) {
                WDATA.write(0xBB000000 + rows_written + i); // GPU Signature Data
                WVALID.write(1);
                if (i == 3) WLAST.write(1); else WLAST.write(0);
                do { wait(); } while (WREADY.read() == 0);
            }
            WVALID.write(0); WLAST.write(0); BREADY.write(1);
            do { wait(); } while (BVALID.read() == 0);
            BREADY.write(0);
            current_address += 32; rows_written++;
        }

        wait(50);
        current_address = 0x1000; int rows_read = 0;
        while (rows_read < 32) {
            ARADDR.write(current_address); ARLEN.write(3); ARVALID.write(1);
            do { wait(); } while (ARREADY.read() == 0);
            ARVALID.write(0); RREADY.write(1); 
            
            int read_count = 0; 
            // Safer Read Loop: No breaks, just count to 4 naturally
            while (read_count < 4) {
                wait(); 
                if (RVALID.read() == 1) {
                    cout << "GPU Read Row " << rows_read << " Data: " << hex << RDATA.read() << endl;
                    read_count++;
                }
            }
            RREADY.write(0); // Safely close the door AFTER the 4 beats finish
        }
        while(true) wait();
    }
    SC_CTOR(tb_gpu) { SC_THREAD(drive_test); sensitive << ACLK.pos(); }
};
#endif