#include <systemc.h>
#include "load_module.h"
#include "compute_module.h"
#include "store_module.h"
// #include "instruction_fetch_module.h" // Uncomment when ready!
#include "axi_interconnect.h"
#include "axi_lite_slave.h"

int sc_main(int argc, char* argv[]) {
    sc_clock ACLK("ACLK", 10, SC_NS); sc_signal<bool> ARESETN;
    
    // Dynamic Starting Address Wires for the 4 Masters
    sc_signal<sc_uint<32>> sys_start_m0, sys_start_m1, sys_start_m2, sys_start_m3;

    // =========================================================
    // --- 1. SIGNAL DECLARATIONS (The Copper Wires) ---
    // =========================================================
    
    // CPU/Load Wires (M0)
    sc_signal<sc_uint<32>> awaddr_m0, wdata_m0, araddr_m0, rdata_m0; 
    sc_signal<sc_uint<8>> awlen_m0, arlen_m0;
    sc_signal<bool> awvalid_m0, wvalid_m0, wlast_m0, bready_m0, awready_m0, wready_m0, bvalid_m0;
    sc_signal<bool> arvalid_m0, rready_m0, arready_m0, rvalid_m0, rlast_m0;
    sc_signal<sc_uint<2>> bresp_m0, rresp_m0;

    // GPU/Compute Wires (M1)
    sc_signal<sc_uint<32>> awaddr_m1, wdata_m1, araddr_m1, rdata_m1; 
    sc_signal<sc_uint<8>> awlen_m1, arlen_m1;
    sc_signal<bool> awvalid_m1, wvalid_m1, wlast_m1, bready_m1, awready_m1, wready_m1, bvalid_m1;
    sc_signal<bool> arvalid_m1, rready_m1, arready_m1, rvalid_m1, rlast_m1;
    sc_signal<sc_uint<2>> bresp_m1, rresp_m1;

    // Tensor/Store Wires (M2)
    sc_signal<sc_uint<32>> awaddr_m2, wdata_m2, araddr_m2, rdata_m2; 
    sc_signal<sc_uint<8>> awlen_m2, arlen_m2;
    sc_signal<bool> awvalid_m2, wvalid_m2, wlast_m2, bready_m2, awready_m2, wready_m2, bvalid_m2;
    sc_signal<bool> arvalid_m2, rready_m2, arready_m2, rvalid_m2, rlast_m2;
    sc_signal<sc_uint<2>> bresp_m2, rresp_m2;

    // Fetch Wires (M3) <-- NEW
    sc_signal<sc_uint<32>> awaddr_m3, wdata_m3, araddr_m3, rdata_m3; 
    sc_signal<sc_uint<8>> awlen_m3, arlen_m3;
    sc_signal<bool> awvalid_m3, wvalid_m3, wlast_m3, bready_m3, awready_m3, wready_m3, bvalid_m3;
    sc_signal<bool> arvalid_m3, rready_m3, arready_m3, rvalid_m3, rlast_m3;
    sc_signal<sc_uint<2>> bresp_m3, rresp_m3;

    // Arbiter-to-Memory Wires (OUT)
    sc_signal<sc_uint<32>> awaddr_out, wdata_out, araddr_out, rdata_in; 
    sc_signal<sc_uint<8>> awlen_out, arlen_out;
    sc_signal<bool> awvalid_out, wvalid_out, wlast_out, bready_out, awready_in, wready_in, bvalid_in;
    sc_signal<bool> arvalid_out, rready_out, arready_in_read, rvalid_in, rlast_in;
    sc_signal<sc_uint<2>> bresp_in, rresp_in;

    // Dispatcher Lock Wires
    sc_signal<bool> awlock_m0, awlock_m1, awlock_m2, awlock_m3;
    sc_signal<bool> arlock_m0, arlock_m1, arlock_m2, arlock_m3;

    // =========================================================
    // --- 2. MODULE INSTANTIATION ---
    // =========================================================
    load_module cpu("LOAD_MODULE"); compute_module gpu("COMPUTE_MODULE"); store_module tensor("STORE_MODULE");
    // instruction_fetch_module fetch("FETCH_MODULE"); // <-- NEW: Uncomment when ready!
    axi_interconnect arbiter("ARBITER"); axi_lite_slave memory("MEMORY");

    // =========================================================
    // --- 3. SOLDERING THE WIRES ---
    // =========================================================
    
    // Solder CPU (M0)
    cpu.ACLK(ACLK); arbiter.ACLK(ACLK); cpu.ARESETN(ARESETN); arbiter.ARESETN(ARESETN);
    cpu.START_ADDR(sys_start_m0); 
    cpu.AWADDR(awaddr_m0); arbiter.AWADDR_M0(awaddr_m0); cpu.AWLEN(awlen_m0); arbiter.AWLEN_M0(awlen_m0);
    cpu.AWVALID(awvalid_m0); arbiter.AWVALID_M0(awvalid_m0); cpu.AWREADY(awready_m0); arbiter.AWREADY_M0(awready_m0);
    cpu.WDATA(wdata_m0); arbiter.WDATA_M0(wdata_m0); cpu.WLAST(wlast_m0); arbiter.WLAST_M0(wlast_m0);
    cpu.WVALID(wvalid_m0); arbiter.WVALID_M0(wvalid_m0); cpu.WREADY(wready_m0); arbiter.WREADY_M0(wready_m0);
    cpu.BRESP(bresp_m0); arbiter.BRESP_M0(bresp_m0); cpu.BVALID(bvalid_m0); arbiter.BVALID_M0(bvalid_m0);
    cpu.BREADY(bready_m0); arbiter.BREADY_M0(bready_m0);
    arbiter.AWLOCK_M0(awlock_m0); 
    cpu.ARADDR(araddr_m0); arbiter.ARADDR_M0(araddr_m0); cpu.ARLEN(arlen_m0); arbiter.ARLEN_M0(arlen_m0);
    cpu.ARVALID(arvalid_m0); arbiter.ARVALID_M0(arvalid_m0); cpu.ARREADY(arready_m0); arbiter.ARREADY_M0(arready_m0);
    cpu.RDATA(rdata_m0); arbiter.RDATA_M0(rdata_m0); cpu.RLAST(rlast_m0); arbiter.RLAST_M0(rlast_m0);
    cpu.RVALID(rvalid_m0); arbiter.RVALID_M0(rvalid_m0); cpu.RREADY(rready_m0); arbiter.RREADY_M0(rready_m0);
    cpu.RRESP(rresp_m0); arbiter.RRESP_M0(rresp_m0);
    arbiter.ARLOCK_M0(arlock_m0); 

    // Solder GPU (M1)
    gpu.ACLK(ACLK); gpu.ARESETN(ARESETN);
    gpu.START_ADDR(sys_start_m1); 
    gpu.AWADDR(awaddr_m1); arbiter.AWADDR_M1(awaddr_m1); gpu.AWLEN(awlen_m1); arbiter.AWLEN_M1(awlen_m1);
    gpu.AWVALID(awvalid_m1); arbiter.AWVALID_M1(awvalid_m1); gpu.AWREADY(awready_m1); arbiter.AWREADY_M1(awready_m1);
    gpu.WDATA(wdata_m1); arbiter.WDATA_M1(wdata_m1); gpu.WLAST(wlast_m1); arbiter.WLAST_M1(wlast_m1);
    gpu.WVALID(wvalid_m1); arbiter.WVALID_M1(wvalid_m1); gpu.WREADY(wready_m1); arbiter.WREADY_M1(wready_m1);
    gpu.BRESP(bresp_m1); arbiter.BRESP_M1(bresp_m1); gpu.BVALID(bvalid_m1); arbiter.BVALID_M1(bvalid_m1);
    gpu.BREADY(bready_m1); arbiter.BREADY_M1(bready_m1);
    arbiter.AWLOCK_M1(awlock_m1); 
    gpu.ARADDR(araddr_m1); arbiter.ARADDR_M1(araddr_m1); gpu.ARLEN(arlen_m1); arbiter.ARLEN_M1(arlen_m1);
    gpu.ARVALID(arvalid_m1); arbiter.ARVALID_M1(arvalid_m1); gpu.ARREADY(arready_m1); arbiter.ARREADY_M1(arready_m1);
    gpu.RDATA(rdata_m1); arbiter.RDATA_M1(rdata_m1); gpu.RLAST(rlast_m1); arbiter.RLAST_M1(rlast_m1);
    gpu.RVALID(rvalid_m1); arbiter.RVALID_M1(rvalid_m1); gpu.RREADY(rready_m1); arbiter.RREADY_M1(rready_m1);
    gpu.RRESP(rresp_m1); arbiter.RRESP_M1(rresp_m1);
    arbiter.ARLOCK_M1(arlock_m1); 

    // Solder Tensor (M2)
    tensor.ACLK(ACLK); tensor.ARESETN(ARESETN);
    tensor.START_ADDR(sys_start_m2); 
    tensor.AWADDR(awaddr_m2); arbiter.AWADDR_M2(awaddr_m2); tensor.AWLEN(awlen_m2); arbiter.AWLEN_M2(awlen_m2);
    tensor.AWVALID(awvalid_m2); arbiter.AWVALID_M2(awvalid_m2); tensor.AWREADY(awready_m2); arbiter.AWREADY_M2(awready_m2);
    tensor.WDATA(wdata_m2); arbiter.WDATA_M2(wdata_m2); tensor.WLAST(wlast_m2); arbiter.WLAST_M2(wlast_m2);
    tensor.WVALID(wvalid_m2); arbiter.WVALID_M2(wvalid_m2); tensor.WREADY(wready_m2); arbiter.WREADY_M2(wready_m2);
    tensor.BRESP(bresp_m2); arbiter.BRESP_M2(bresp_m2); tensor.BVALID(bvalid_m2); arbiter.BVALID_M2(bvalid_m2);
    tensor.BREADY(bready_m2); arbiter.BREADY_M2(bready_m2);
    arbiter.AWLOCK_M2(awlock_m2); 
    tensor.ARADDR(araddr_m2); arbiter.ARADDR_M2(araddr_m2); tensor.ARLEN(arlen_m2); arbiter.ARLEN_M2(arlen_m2);
    tensor.ARVALID(arvalid_m2); arbiter.ARVALID_M2(arvalid_m2); tensor.ARREADY(arready_m2); arbiter.ARREADY_M2(arready_m2);
    tensor.RDATA(rdata_m2); arbiter.RDATA_M2(rdata_m2); tensor.RLAST(rlast_m2); arbiter.RLAST_M2(rlast_m2);
    tensor.RVALID(rvalid_m2); arbiter.RVALID_M2(rvalid_m2); tensor.RREADY(rready_m2); arbiter.RREADY_M2(rready_m2);
    tensor.RRESP(rresp_m2); arbiter.RRESP_M2(rresp_m2);
    arbiter.ARLOCK_M2(arlock_m2); 

    // Solder Fetch Arbiter Wires (M3) <-- NEW
    arbiter.AWADDR_M3(awaddr_m3); arbiter.AWLEN_M3(awlen_m3);
    arbiter.AWVALID_M3(awvalid_m3); arbiter.AWREADY_M3(awready_m3);
    arbiter.WDATA_M3(wdata_m3); arbiter.WLAST_M3(wlast_m3);
    arbiter.WVALID_M3(wvalid_m3); arbiter.WREADY_M3(wready_m3);
    arbiter.BRESP_M3(bresp_m3); arbiter.BVALID_M3(bvalid_m3);
    arbiter.BREADY_M3(bready_m3);
    arbiter.AWLOCK_M3(awlock_m3); 
    arbiter.ARADDR_M3(araddr_m3); arbiter.ARLEN_M3(arlen_m3);
    arbiter.ARVALID_M3(arvalid_m3); arbiter.ARREADY_M3(arready_m3);
    arbiter.RDATA_M3(rdata_m3); arbiter.RLAST_M3(rlast_m3);
    arbiter.RVALID_M3(rvalid_m3); arbiter.RREADY_M3(rready_m3);
    arbiter.RRESP_M3(rresp_m3);
    arbiter.ARLOCK_M3(arlock_m3); 
    
    /* // SOLDER FETCH MODULE (Uncomment when your team finishes the module)
    fetch.ACLK(ACLK); fetch.ARESETN(ARESETN);
    fetch.START_ADDR(sys_start_m3);
    fetch.AWADDR(awaddr_m3); arbiter.AWADDR_M3(awaddr_m3); fetch.AWLEN(awlen_m3); arbiter.AWLEN_M3(awlen_m3);
    fetch.AWVALID(awvalid_m3); arbiter.AWVALID_M3(awvalid_m3); fetch.AWREADY(awready_m3); arbiter.AWREADY_M3(awready_m3);
    fetch.WDATA(wdata_m3); arbiter.WDATA_M3(wdata_m3); fetch.WLAST(wlast_m3); arbiter.WLAST_M3(wlast_m3);
    fetch.WVALID(wvalid_m3); arbiter.WVALID_M3(wvalid_m3); fetch.WREADY(wready_m3); arbiter.WREADY_M3(wready_m3);
    fetch.BRESP(bresp_m3); arbiter.BRESP_M3(bresp_m3); fetch.BVALID(bvalid_m3); arbiter.BVALID_M3(bvalid_m3);
    fetch.BREADY(bready_m3); arbiter.BREADY_M3(bready_m3);
    fetch.ARADDR(araddr_m3); arbiter.ARADDR_M3(araddr_m3); fetch.ARLEN(arlen_m3); arbiter.ARLEN_M3(arlen_m3);
    fetch.ARVALID(arvalid_m3); arbiter.ARVALID_M3(arvalid_m3); fetch.ARREADY(arready_m3); arbiter.ARREADY_M3(arready_m3);
    fetch.RDATA(rdata_m3); arbiter.RDATA_M3(rdata_m3); fetch.RLAST(rlast_m3); arbiter.RLAST_M3(rlast_m3);
    fetch.RVALID(rvalid_m3); arbiter.RVALID_M3(rvalid_m3); fetch.RREADY(rready_m3); arbiter.RREADY_M3(rready_m3);
    fetch.RRESP(rresp_m3); arbiter.RRESP_M3(rresp_m3);
    */

    // Solder Arbiter Out to Memory
    memory.ACLK(ACLK); memory.ARESETN(ARESETN); 
    arbiter.AWADDR_OUT(awaddr_out); memory.AWADDR(awaddr_out); arbiter.AWLEN_OUT(awlen_out); memory.AWLEN(awlen_out);
    arbiter.AWVALID_OUT(awvalid_out); memory.AWVALID(awvalid_out); arbiter.AWREADY_IN(awready_in); memory.AWREADY(awready_in);
    arbiter.WDATA_OUT(wdata_out); memory.WDATA(wdata_out); arbiter.WLAST_OUT(wlast_out); memory.WLAST(wlast_out);
    arbiter.WVALID_OUT(wvalid_out); memory.WVALID(wvalid_out); arbiter.WREADY_IN(wready_in); memory.WREADY(wready_in);
    arbiter.BRESP_IN(bresp_in); memory.BRESP(bresp_in); arbiter.BVALID_IN(bvalid_in); memory.BVALID(bvalid_in);
    arbiter.BREADY_OUT(bready_out); memory.BREADY(bready_out);
    arbiter.ARADDR_OUT(araddr_out); memory.ARADDR(araddr_out); arbiter.ARLEN_OUT(arlen_out); memory.ARLEN(arlen_out);
    arbiter.ARVALID_OUT(arvalid_out); memory.ARVALID(arvalid_out); arbiter.ARREADY_IN(arready_in_read); memory.ARREADY(arready_in_read);
    arbiter.RDATA_IN(rdata_in); memory.RDATA(rdata_in); arbiter.RLAST_IN(rlast_in); memory.RLAST(rlast_in);
    arbiter.RVALID_IN(rvalid_in); memory.RVALID(rvalid_in); arbiter.RREADY_OUT(rready_out); memory.RREADY(rready_out);
    arbiter.RRESP_IN(rresp_in); memory.RRESP(rresp_in);

    // =========================================================
    // --- 4. WAVEFORM TRACING ---
    // =========================================================
    sc_trace_file *wf = sc_create_vcd_trace_file("soc_trace");
    sc_trace(wf, ACLK, "ACLK"); sc_trace(wf, ARESETN, "ARESETN");
    sc_trace(wf, arbiter.active_master, "active_write_master");
    sc_trace(wf, awvalid_out, "ARBITER_AWVALID_OUT"); sc_trace(wf, awaddr_out,  "ARBITER_AWADDR_OUT");
    sc_trace(wf, wvalid_out,  "ARBITER_WVALID_OUT"); sc_trace(wf, wdata_out,   "ARBITER_WDATA_OUT");
    sc_trace(wf, bvalid_in,   "MEMORY_BVALID");
    sc_trace(wf, arbiter.active_read_master, "active_read_master");
    sc_trace(wf, arvalid_out, "ARBITER_ARVALID_OUT"); sc_trace(wf, araddr_out,  "ARBITER_ARADDR_OUT");
    sc_trace(wf, rvalid_in,   "MEMORY_RVALID"); sc_trace(wf, rdata_in,    "MEMORY_RDATA"); sc_trace(wf, rlast_in,    "MEMORY_RLAST");
    
    // Master Traces
    sc_trace(wf, rdata_m0, "CPU_RDATA"); sc_trace(wf, rdata_m1, "GPU_RDATA"); sc_trace(wf, rdata_m2, "TENSOR_RDATA");
    sc_trace(wf, memory.current_w_addr, "MEM_WRITE_ADDR"); sc_trace(wf, memory.current_r_addr, "MEM_READ_ADDR");

    // =========================================================
    // --- 5. EXECUTE SIMULATION ---
    // =========================================================
    ARESETN.write(0); 

    // Simulate the Dispatcher sending the starting addresses
    sys_start_m0.write(0x0000);
    sys_start_m1.write(0x1000);
    sys_start_m2.write(0x2000);
    sys_start_m3.write(0x3000); // <-- Fetch start address

    // Ensure no Dispatcher Interrupts fire randomly at startup
    awlock_m0.write(0); arlock_m0.write(0);
    awlock_m1.write(0); arlock_m1.write(0);
    awlock_m2.write(0); arlock_m2.write(0);
    awlock_m3.write(0); arlock_m3.write(0); // <-- Fetch locks

    sc_start(20, SC_NS); 
    ARESETN.write(1); 
    sc_start(2000000, SC_NS); 
    
    sc_close_vcd_trace_file(wf);
    return 0;
}