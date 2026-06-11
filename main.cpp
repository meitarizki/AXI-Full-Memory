#include <systemc.h>
#include "load_module.h"
#include "compute_module.h"
#include "store_module.h"
#include "axi_interconnect.h"
#include "axi_lite_slave.h"

int sc_main(int argc, char* argv[]) {
    sc_clock ACLK("ACLK", 10, SC_NS); sc_signal<bool> ARESETN;
    
    // NEW: Dynamic Configuration Wires
    sc_signal<int> sys_cfg_width;
    sc_signal<int> sys_cfg_stride;

    // NEW: Dynamic Starting Address Wires for the 3 Masters
    sc_signal<sc_uint<32>> sys_start_m0, sys_start_m1, sys_start_m2;

    // =========================================================
    // --- 1. SIGNAL DECLARATIONS (The Copper Wires) ---
    // =========================================================
    
    // CPU Wires (M0)
    sc_signal<sc_uint<32>> awaddr_m0, wdata_m0, araddr_m0, rdata_m0; 
    sc_signal<sc_uint<8>> awlen_m0, arlen_m0;
    sc_signal<bool> awvalid_m0, wvalid_m0, wlast_m0, bready_m0, awready_m0, wready_m0, bvalid_m0;
    sc_signal<bool> arvalid_m0, rready_m0, arready_m0, rvalid_m0, rlast_m0;
    sc_signal<sc_uint<2>> bresp_m0, rresp_m0;

    // GPU Wires (M1)
    sc_signal<sc_uint<32>> awaddr_m1, wdata_m1, araddr_m1, rdata_m1; 
    sc_signal<sc_uint<8>> awlen_m1, arlen_m1;
    sc_signal<bool> awvalid_m1, wvalid_m1, wlast_m1, bready_m1, awready_m1, wready_m1, bvalid_m1;
    sc_signal<bool> arvalid_m1, rready_m1, arready_m1, rvalid_m1, rlast_m1;
    sc_signal<sc_uint<2>> bresp_m1, rresp_m1;

    // Tensor Wires (M2)
    sc_signal<sc_uint<32>> awaddr_m2, wdata_m2, araddr_m2, rdata_m2; 
    sc_signal<sc_uint<8>> awlen_m2, arlen_m2;
    sc_signal<bool> awvalid_m2, wvalid_m2, wlast_m2, bready_m2, awready_m2, wready_m2, bvalid_m2;
    sc_signal<bool> arvalid_m2, rready_m2, arready_m2, rvalid_m2, rlast_m2;
    sc_signal<sc_uint<2>> bresp_m2, rresp_m2;

    // Arbiter-to-Memory Wires (OUT)
    sc_signal<sc_uint<32>> awaddr_out, wdata_out, araddr_out, rdata_in; 
    sc_signal<sc_uint<8>> awlen_out, arlen_out;
    sc_signal<bool> awvalid_out, wvalid_out, wlast_out, bready_out, awready_in, wready_in, bvalid_in;
    sc_signal<bool> arvalid_out, rready_out, arready_in_read, rvalid_in, rlast_in;
    sc_signal<sc_uint<2>> bresp_in, rresp_in;

    // NEW: Dispatcher Lock Wires
    sc_signal<bool> awlock_m0, awlock_m1, awlock_m2;
    sc_signal<bool> arlock_m0, arlock_m1, arlock_m2;

    // =========================================================
    // --- 2. MODULE INSTANTIATION ---
    // =========================================================
    load_module cpu("LOAD_MODULE"); compute_module gpu("COMPUTE_MODULE"); store_module tensor("STORE_MODULE");
    axi_interconnect arbiter("ARBITER"); axi_lite_slave memory("MEMORY");

    // =========================================================
    // --- 3. SOLDERING THE WIRES ---
    // =========================================================
    
    // Solder CPU
    cpu.ACLK(ACLK); arbiter.ACLK(ACLK); cpu.ARESETN(ARESETN); arbiter.ARESETN(ARESETN);
    cpu.START_ADDR(sys_start_m0); // <-- Solder Dynamic Address Pin
    
    // CPU Write
    cpu.AWADDR(awaddr_m0); arbiter.AWADDR_M0(awaddr_m0); cpu.AWLEN(awlen_m0); arbiter.AWLEN_M0(awlen_m0);
    cpu.AWVALID(awvalid_m0); arbiter.AWVALID_M0(awvalid_m0); cpu.AWREADY(awready_m0); arbiter.AWREADY_M0(awready_m0);
    cpu.WDATA(wdata_m0); arbiter.WDATA_M0(wdata_m0); cpu.WLAST(wlast_m0); arbiter.WLAST_M0(wlast_m0);
    cpu.WVALID(wvalid_m0); arbiter.WVALID_M0(wvalid_m0); cpu.WREADY(wready_m0); arbiter.WREADY_M0(wready_m0);
    cpu.BRESP(bresp_m0); arbiter.BRESP_M0(bresp_m0); cpu.BVALID(bvalid_m0); arbiter.BVALID_M0(bvalid_m0);
    cpu.BREADY(bready_m0); arbiter.BREADY_M0(bready_m0);
    arbiter.AWLOCK_M0(awlock_m0); // Solder Write Lock
    
    // CPU Read
    cpu.ARADDR(araddr_m0); arbiter.ARADDR_M0(araddr_m0); cpu.ARLEN(arlen_m0); arbiter.ARLEN_M0(arlen_m0);
    cpu.ARVALID(arvalid_m0); arbiter.ARVALID_M0(arvalid_m0); cpu.ARREADY(arready_m0); arbiter.ARREADY_M0(arready_m0);
    cpu.RDATA(rdata_m0); arbiter.RDATA_M0(rdata_m0); cpu.RLAST(rlast_m0); arbiter.RLAST_M0(rlast_m0);
    cpu.RVALID(rvalid_m0); arbiter.RVALID_M0(rvalid_m0); cpu.RREADY(rready_m0); arbiter.RREADY_M0(rready_m0);
    cpu.RRESP(rresp_m0); arbiter.RRESP_M0(rresp_m0);
    arbiter.ARLOCK_M0(arlock_m0); // Solder Read Lock

    // Solder GPU
    gpu.ACLK(ACLK); gpu.ARESETN(ARESETN);
    gpu.START_ADDR(sys_start_m1); // <-- Solder Dynamic Address Pin
    
    // GPU Write
    gpu.AWADDR(awaddr_m1); arbiter.AWADDR_M1(awaddr_m1); gpu.AWLEN(awlen_m1); arbiter.AWLEN_M1(awlen_m1);
    gpu.AWVALID(awvalid_m1); arbiter.AWVALID_M1(awvalid_m1); gpu.AWREADY(awready_m1); arbiter.AWREADY_M1(awready_m1);
    gpu.WDATA(wdata_m1); arbiter.WDATA_M1(wdata_m1); gpu.WLAST(wlast_m1); arbiter.WLAST_M1(wlast_m1);
    gpu.WVALID(wvalid_m1); arbiter.WVALID_M1(wvalid_m1); gpu.WREADY(wready_m1); arbiter.WREADY_M1(wready_m1);
    gpu.BRESP(bresp_m1); arbiter.BRESP_M1(bresp_m1); gpu.BVALID(bvalid_m1); arbiter.BVALID_M1(bvalid_m1);
    gpu.BREADY(bready_m1); arbiter.BREADY_M1(bready_m1);
    arbiter.AWLOCK_M1(awlock_m1); // Solder Write Lock
    
    // GPU Read
    gpu.ARADDR(araddr_m1); arbiter.ARADDR_M1(araddr_m1); gpu.ARLEN(arlen_m1); arbiter.ARLEN_M1(arlen_m1);
    gpu.ARVALID(arvalid_m1); arbiter.ARVALID_M1(arvalid_m1); gpu.ARREADY(arready_m1); arbiter.ARREADY_M1(arready_m1);
    gpu.RDATA(rdata_m1); arbiter.RDATA_M1(rdata_m1); gpu.RLAST(rlast_m1); arbiter.RLAST_M1(rlast_m1);
    gpu.RVALID(rvalid_m1); arbiter.RVALID_M1(rvalid_m1); gpu.RREADY(rready_m1); arbiter.RREADY_M1(rready_m1);
    gpu.RRESP(rresp_m1); arbiter.RRESP_M1(rresp_m1);
    arbiter.ARLOCK_M1(arlock_m1); // Solder Read Lock

    // Solder Tensor
    tensor.ACLK(ACLK); tensor.ARESETN(ARESETN);
    tensor.START_ADDR(sys_start_m2); // <-- Solder Dynamic Address Pin
    
    // Tensor Write
    tensor.AWADDR(awaddr_m2); arbiter.AWADDR_M2(awaddr_m2); tensor.AWLEN(awlen_m2); arbiter.AWLEN_M2(awlen_m2);
    tensor.AWVALID(awvalid_m2); arbiter.AWVALID_M2(awvalid_m2); tensor.AWREADY(awready_m2); arbiter.AWREADY_M2(awready_m2);
    tensor.WDATA(wdata_m2); arbiter.WDATA_M2(wdata_m2); tensor.WLAST(wlast_m2); arbiter.WLAST_M2(wlast_m2);
    tensor.WVALID(wvalid_m2); arbiter.WVALID_M2(wvalid_m2); tensor.WREADY(wready_m2); arbiter.WREADY_M2(wready_m2);
    tensor.BRESP(bresp_m2); arbiter.BRESP_M2(bresp_m2); tensor.BVALID(bvalid_m2); arbiter.BVALID_M2(bvalid_m2);
    tensor.BREADY(bready_m2); arbiter.BREADY_M2(bready_m2);
    arbiter.AWLOCK_M2(awlock_m2); // Solder Write Lock
    
    // Tensor Read
    tensor.ARADDR(araddr_m2); arbiter.ARADDR_M2(araddr_m2); tensor.ARLEN(arlen_m2); arbiter.ARLEN_M2(arlen_m2);
    tensor.ARVALID(arvalid_m2); arbiter.ARVALID_M2(arvalid_m2); tensor.ARREADY(arready_m2); arbiter.ARREADY_M2(arready_m2);
    tensor.RDATA(rdata_m2); arbiter.RDATA_M2(rdata_m2); tensor.RLAST(rlast_m2); arbiter.RLAST_M2(rlast_m2);
    tensor.RVALID(rvalid_m2); arbiter.RVALID_M2(rvalid_m2); tensor.RREADY(rready_m2); arbiter.RREADY_M2(rready_m2);
    tensor.RRESP(rresp_m2); arbiter.RRESP_M2(rresp_m2);
    arbiter.ARLOCK_M2(arlock_m2); // Solder Read Lock

    // Solder Arbiter Out to Memory
    memory.ACLK(ACLK); memory.ARESETN(ARESETN); 
    memory.CFG_WIDTH(sys_cfg_width); memory.CFG_STRIDE(sys_cfg_stride);
    
    // Memory Write
    arbiter.AWADDR_OUT(awaddr_out); memory.AWADDR(awaddr_out); arbiter.AWLEN_OUT(awlen_out); memory.AWLEN(awlen_out);
    arbiter.AWVALID_OUT(awvalid_out); memory.AWVALID(awvalid_out); arbiter.AWREADY_IN(awready_in); memory.AWREADY(awready_in);
    arbiter.WDATA_OUT(wdata_out); memory.WDATA(wdata_out); arbiter.WLAST_OUT(wlast_out); memory.WLAST(wlast_out);
    arbiter.WVALID_OUT(wvalid_out); memory.WVALID(wvalid_out); arbiter.WREADY_IN(wready_in); memory.WREADY(wready_in);
    arbiter.BRESP_IN(bresp_in); memory.BRESP(bresp_in); arbiter.BVALID_IN(bvalid_in); memory.BVALID(bvalid_in);
    arbiter.BREADY_OUT(bready_out); memory.BREADY(bready_out);
    
    // Memory Read
    arbiter.ARADDR_OUT(araddr_out); memory.ARADDR(araddr_out); arbiter.ARLEN_OUT(arlen_out); memory.ARLEN(arlen_out);
    arbiter.ARVALID_OUT(arvalid_out); memory.ARVALID(arvalid_out); arbiter.ARREADY_IN(arready_in_read); memory.ARREADY(arready_in_read);
    arbiter.RDATA_IN(rdata_in); memory.RDATA(rdata_in); arbiter.RLAST_IN(rlast_in); memory.RLAST(rlast_in);
    arbiter.RVALID_IN(rvalid_in); memory.RVALID(rvalid_in); arbiter.RREADY_OUT(rready_out); memory.RREADY(rready_out);
    arbiter.RRESP_IN(rresp_in); memory.RRESP(rresp_in);

    // =========================================================
    // --- 4. WAVEFORM TRACING (The Supervisor Dashboard) ---
    // =========================================================
    sc_trace_file *wf = sc_create_vcd_trace_file("soc_trace");
    sc_trace(wf, ACLK, "ACLK");
    sc_trace(wf, ARESETN, "ARESETN");
    
    // Write Arbitration Traces
    sc_trace(wf, arbiter.active_master, "active_write_master");
    sc_trace(wf, awvalid_out, "ARBITER_AWVALID_OUT");
    sc_trace(wf, awaddr_out,  "ARBITER_AWADDR_OUT");
    sc_trace(wf, wvalid_out,  "ARBITER_WVALID_OUT");
    sc_trace(wf, wdata_out,   "ARBITER_WDATA_OUT");
    sc_trace(wf, bvalid_in,   "MEMORY_BVALID");

    // Read Arbitration Traces
    sc_trace(wf, arbiter.active_read_master, "active_read_master");
    sc_trace(wf, arvalid_out, "ARBITER_ARVALID_OUT");
    sc_trace(wf, araddr_out,  "ARBITER_ARADDR_OUT");
    sc_trace(wf, rvalid_in,   "MEMORY_RVALID");
    sc_trace(wf, rdata_in,    "MEMORY_RDATA");
    sc_trace(wf, rlast_in,    "MEMORY_RLAST");
    
    // Master Specific Data Traces
    sc_trace(wf, rdata_m0, "CPU_RDATA");
    sc_trace(wf, rdata_m1, "GPU_RDATA");
    sc_trace(wf, rdata_m2, "TENSOR_RDATA");
    
    // Base + Stride Write Trackers
    sc_trace(wf, memory.current_w_addr, "MEM_WRITE_ADDR");
    sc_trace(wf, memory.w_base_addr, "MEM_WRITE_BASE");
    sc_trace(wf, memory.w_x_count, "MEM_WRITE_X_COUNT");
    
    // Base + Stride Read Trackers
    sc_trace(wf, memory.current_r_addr, "MEM_READ_ADDR");
    sc_trace(wf, memory.r_base_addr, "MEM_READ_BASE");
    sc_trace(wf, memory.r_x_count, "MEM_READ_X_COUNT");

    // =========================================================
    // --- 5. EXECUTE SIMULATION ---
    // =========================================================
    ARESETN.write(0); 

    // Simulate the Dispatcher sending the matrix dimensions
    sys_cfg_width.write(16);
    sys_cfg_stride.write(32);

    // Simulate the Dispatcher sending the starting addresses
    sys_start_m0.write(0x0000);
    sys_start_m1.write(0x1000);
    sys_start_m2.write(0x2000);

    // Ensure no Dispatcher Interrupts fire randomly at startup
    awlock_m0.write(0); arlock_m0.write(0);
    awlock_m1.write(0); arlock_m1.write(0);
    awlock_m2.write(0); arlock_m2.write(0);

    sc_start(20, SC_NS); 
    ARESETN.write(1); 
    
    // Massive runtime to allow 3 Masters * 32 Bursts * Latency to complete
    sc_start(2000000, SC_NS); 
    
    sc_close_vcd_trace_file(wf);
    return 0;
}