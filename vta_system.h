#ifndef VTA_SYSTEM_H
#define VTA_SYSTEM_H

#include <systemc.h>
#include "axi_interconnect.h"
#include "axi_lite_slave.h"

// ==========================================
// COLLEAGUE IMPORTS
// ==========================================
// #include "instruction_fetch_module.h" // <-- NEW: Fetcher
// #include "load_module.h"
// #include "compute_module.h"
// #include "store_module.h"
// #include "tt_dispatcher.h" 

SC_MODULE(vta_system) {
    // --- Global Power & Clock ---
    sc_in<bool> ACLK;
    sc_in<bool> ARESETN;

    // ==========================================
    // 1. MOTHERBOARD TRACES (Internal Wires)
    // ==========================================
    
    // Traces between Arbiter and Main Memory
    sc_signal<sc_uint<32>> sys_AWADDR, sys_WDATA, sys_ARADDR, sys_RDATA;
    sc_signal<sc_uint<8>>  sys_AWLEN, sys_ARLEN;
    sc_signal<sc_uint<2>>  sys_BRESP, sys_RRESP;
    sc_signal<bool> sys_AWVALID, sys_AWREADY, sys_WVALID, sys_WREADY, sys_WLAST;
    sc_signal<bool> sys_BVALID, sys_BREADY, sys_ARVALID, sys_ARREADY;
    sc_signal<bool> sys_RVALID, sys_RREADY, sys_RLAST;

    // Traces for Master 0 (Load Module)
    sc_signal<sc_uint<32>> m0_AWADDR, m0_WDATA, m0_ARADDR, m0_RDATA;
    sc_signal<sc_uint<8>>  m0_AWLEN, m0_ARLEN;
    sc_signal<sc_uint<2>>  m0_BRESP, m0_RRESP;
    sc_signal<bool> m0_AWVALID, m0_AWREADY, m0_WVALID, m0_WREADY, m0_WLAST;
    sc_signal<bool> m0_BVALID, m0_BREADY, m0_ARVALID, m0_ARREADY;
    sc_signal<bool> m0_RVALID, m0_RREADY, m0_RLAST;
    sc_signal<bool> m0_AWLOCK, m0_ARLOCK; 

    // Traces for Master 1 (Compute Module)
    sc_signal<sc_uint<32>> m1_AWADDR, m1_WDATA, m1_ARADDR, m1_RDATA;
    sc_signal<sc_uint<8>>  m1_AWLEN, m1_ARLEN;
    sc_signal<sc_uint<2>>  m1_BRESP, m1_RRESP;
    sc_signal<bool> m1_AWVALID, m1_AWREADY, m1_WVALID, m1_WREADY, m1_WLAST;
    sc_signal<bool> m1_BVALID, m1_BREADY, m1_ARVALID, m1_ARREADY;
    sc_signal<bool> m1_RVALID, m1_RREADY, m1_RLAST;
    sc_signal<bool> m1_AWLOCK, m1_ARLOCK; 

    // Traces for Master 2 (Store Module)
    sc_signal<sc_uint<32>> m2_AWADDR, m2_WDATA, m2_ARADDR, m2_RDATA;
    sc_signal<sc_uint<8>>  m2_AWLEN, m2_ARLEN;
    sc_signal<sc_uint<2>>  m2_BRESP, m2_RRESP;
    sc_signal<bool> m2_AWVALID, m2_AWREADY, m2_WVALID, m2_WREADY, m2_WLAST;
    sc_signal<bool> m2_BVALID, m2_BREADY, m2_ARVALID, m2_ARREADY;
    sc_signal<bool> m2_RVALID, m2_RREADY, m2_RLAST;
    sc_signal<bool> m2_AWLOCK, m2_ARLOCK; 

    // Traces for Master 3 (Instruction Fetch Module) <-- NEW
    sc_signal<sc_uint<32>> m3_AWADDR, m3_WDATA, m3_ARADDR, m3_RDATA;
    sc_signal<sc_uint<8>>  m3_AWLEN, m3_ARLEN;
    sc_signal<sc_uint<2>>  m3_BRESP, m3_RRESP;
    sc_signal<bool> m3_AWVALID, m3_AWREADY, m3_WVALID, m3_WREADY, m3_WLAST;
    sc_signal<bool> m3_BVALID, m3_BREADY, m3_ARVALID, m3_ARREADY;
    sc_signal<bool> m3_RVALID, m3_RREADY, m3_RLAST;
    sc_signal<bool> m3_AWLOCK, m3_ARLOCK; 

    // ==========================================
    // 2. HARDWARE COMPONENTS
    // ==========================================
    axi_interconnect* arbiter;
    axi_lite_slave* dram;
    
    // instruction_fetch_module* fetch_inst; // <-- NEW
    // load_module* load_inst;
    // compute_module* compute_inst;
    // store_module* store_inst;
    // tt_dispatcher* dispatcher_inst; 

    SC_CTOR(vta_system) {
        
        // --- INSTANTIATE MAIN MEMORY ---
        dram = new axi_lite_slave("main_memory"); 
        dram->ACLK(ACLK); dram->ARESETN(ARESETN);
        dram->AWADDR(sys_AWADDR); dram->AWVALID(sys_AWVALID); dram->AWREADY(sys_AWREADY); dram->AWLEN(sys_AWLEN);
        dram->WDATA(sys_WDATA);   dram->WVALID(sys_WVALID);   dram->WREADY(sys_WREADY);   dram->WLAST(sys_WLAST);
        dram->BRESP(sys_BRESP);   dram->BVALID(sys_BVALID);   dram->BREADY(sys_BREADY);
        dram->ARADDR(sys_ARADDR); dram->ARVALID(sys_ARVALID); dram->ARREADY(sys_ARREADY); dram->ARLEN(sys_ARLEN);
        dram->RDATA(sys_RDATA);   dram->RRESP(sys_RRESP);     dram->RVALID(sys_RVALID);   dram->RREADY(sys_RREADY); dram->RLAST(sys_RLAST);

        // --- INSTANTIATE ARBITER ---
        arbiter = new axi_interconnect("round_robin_arbiter");
        arbiter->ACLK(ACLK); arbiter->ARESETN(ARESETN);
        
        // Connect Arbiter DOWN to Memory Traces
        arbiter->AWADDR_OUT(sys_AWADDR); arbiter->AWVALID_OUT(sys_AWVALID); arbiter->AWREADY_IN(sys_AWREADY); arbiter->AWLEN_OUT(sys_AWLEN);
        arbiter->WDATA_OUT(sys_WDATA);   arbiter->WVALID_OUT(sys_WVALID);   arbiter->WREADY_IN(sys_WREADY);   arbiter->WLAST_OUT(sys_WLAST);
        arbiter->BRESP_IN(sys_BRESP);    arbiter->BVALID_IN(sys_BVALID);    arbiter->BREADY_OUT(sys_BREADY);
        arbiter->ARADDR_OUT(sys_ARADDR); arbiter->ARVALID_OUT(sys_ARVALID); arbiter->ARREADY_IN(sys_ARREADY); arbiter->ARLEN_OUT(sys_ARLEN);
        arbiter->RDATA_IN(sys_RDATA);    arbiter->RRESP_IN(sys_RRESP);      arbiter->RVALID_IN(sys_RVALID);   arbiter->RREADY_OUT(sys_RREADY); arbiter->RLAST_IN(sys_RLAST);

        // Connect Arbiter UP to Master 0 Wires (Load)
        arbiter->AWADDR_M0(m0_AWADDR); arbiter->AWVALID_M0(m0_AWVALID); arbiter->AWREADY_M0(m0_AWREADY); arbiter->AWLEN_M0(m0_AWLEN);
        arbiter->WDATA_M0(m0_WDATA);   arbiter->WVALID_M0(m0_WVALID);   arbiter->WREADY_M0(m0_WREADY);   arbiter->WLAST_M0(m0_WLAST);
        arbiter->BRESP_M0(m0_BRESP);   arbiter->BVALID_M0(m0_BVALID);   arbiter->BREADY_M0(m0_BREADY);
        arbiter->ARADDR_M0(m0_ARADDR); arbiter->ARVALID_M0(m0_ARVALID); arbiter->ARREADY_M0(m0_ARREADY); arbiter->ARLEN_M0(m0_ARLEN);
        arbiter->RDATA_M0(m0_RDATA);   arbiter->RRESP_M0(m0_RRESP);     arbiter->RVALID_M0(m0_RVALID);   arbiter->RREADY_M0(m0_RREADY); arbiter->RLAST_M0(m0_RLAST);
        arbiter->AWLOCK_M0(m0_AWLOCK); arbiter->ARLOCK_M0(m0_ARLOCK); 

        // Connect Arbiter UP to Master 1 Wires (Compute)
        arbiter->AWADDR_M1(m1_AWADDR); arbiter->AWVALID_M1(m1_AWVALID); arbiter->AWREADY_M1(m1_AWREADY); arbiter->AWLEN_M1(m1_AWLEN);
        arbiter->WDATA_M1(m1_WDATA);   arbiter->WVALID_M1(m1_WVALID);   arbiter->WREADY_M1(m1_WREADY);   arbiter->WLAST_M1(m1_WLAST);
        arbiter->BRESP_M1(m1_BRESP);   arbiter->BVALID_M1(m1_BVALID);   arbiter->BREADY_M1(m1_BREADY);
        arbiter->ARADDR_M1(m1_ARADDR); arbiter->ARVALID_M1(m1_ARVALID); arbiter->ARREADY_M1(m1_ARREADY); arbiter->ARLEN_M1(m1_ARLEN);
        arbiter->RDATA_M1(m1_RDATA);   arbiter->RRESP_M1(m1_RRESP);     arbiter->RVALID_M1(m1_RVALID);   arbiter->RREADY_M1(m1_RREADY); arbiter->RLAST_M1(m1_RLAST);
        arbiter->AWLOCK_M1(m1_AWLOCK); arbiter->ARLOCK_M1(m1_ARLOCK); 

        // Connect Arbiter UP to Master 2 Wires (Store)
        arbiter->AWADDR_M2(m2_AWADDR); arbiter->AWVALID_M2(m2_AWVALID); arbiter->AWREADY_M2(m2_AWREADY); arbiter->AWLEN_M2(m2_AWLEN);
        arbiter->WDATA_M2(m2_WDATA);   arbiter->WVALID_M2(m2_WVALID);   arbiter->WREADY_M2(m2_WREADY);   arbiter->WLAST_M2(m2_WLAST);
        arbiter->BRESP_M2(m2_BRESP);   arbiter->BVALID_M2(m2_BVALID);   arbiter->BREADY_M2(m2_BREADY);
        arbiter->ARADDR_M2(m2_ARADDR); arbiter->ARVALID_M2(m2_ARVALID); arbiter->ARREADY_M2(m2_ARREADY); arbiter->ARLEN_M2(m2_ARLEN);
        arbiter->RDATA_M2(m2_RDATA);   arbiter->RRESP_M2(m2_RRESP);     arbiter->RVALID_M2(m2_RVALID);   arbiter->RREADY_M2(m2_RREADY); arbiter->RLAST_M2(m2_RLAST);
        arbiter->AWLOCK_M2(m2_AWLOCK); arbiter->ARLOCK_M2(m2_ARLOCK); 

        // Connect Arbiter UP to Master 3 Wires (Fetch) <-- NEW
        arbiter->AWADDR_M3(m3_AWADDR); arbiter->AWVALID_M3(m3_AWVALID); arbiter->AWREADY_M3(m3_AWREADY); arbiter->AWLEN_M3(m3_AWLEN);
        arbiter->WDATA_M3(m3_WDATA);   arbiter->WVALID_M3(m3_WVALID);   arbiter->WREADY_M3(m3_WREADY);   arbiter->WLAST_M3(m3_WLAST);
        arbiter->BRESP_M3(m3_BRESP);   arbiter->BVALID_M3(m3_BVALID);   arbiter->BREADY_M3(m3_BREADY);
        arbiter->ARADDR_M3(m3_ARADDR); arbiter->ARVALID_M3(m3_ARVALID); arbiter->ARREADY_M3(m3_ARREADY); arbiter->ARLEN_M3(m3_ARLEN);
        arbiter->RDATA_M3(m3_RDATA);   arbiter->RRESP_M3(m3_RRESP);     arbiter->RVALID_M3(m3_RVALID);   arbiter->RREADY_M3(m3_RREADY); arbiter->RLAST_M3(m3_RLAST);
        arbiter->AWLOCK_M3(m3_AWLOCK); arbiter->ARLOCK_M3(m3_ARLOCK); 
    }

    ~vta_system() {
        delete arbiter;
        delete dram;
    }
};
#endif