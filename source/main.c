#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <wafel/dynamic.h>
#include <wafel/ios_dynamic.h>
#include <wafel/utils.h>
#include <wafel/patch.h>
#include <wafel/ios/svc.h>
#include <wafel/trampoline.h>

const char *mydev = "/dev/usb01";
const char *mlc_alt_mount = "/vol/storage_mlc02";

void print_state(trampoline_state *state){
    debug_printf("state %p: r0: %p, r1: %p, r2: %p, r3: %p, r4: %p, r5: %p, r6: %p, r7: %p, r8: %p, r9: %p, r10: %p, r11: %p, r12: %p, lr: %p\n", state,
            state->r[0],state->r[1],state->r[2],state->r[3],state->r[4],state->r[5],state->r[6],state->r[7],state->r[8],state->r[9],state->r[10],state->r[11], state->r[12], state->lr);
}

int mlc_mount_hook(const char* dev, const char *dir, const char *mount_point, void *owner, int (*orgfunc)(const char*, const char*, const char*, void*)){
    debug_printf("Calling MCP_MountWithSubdir(%s, %s, %s, %p) at %p\n", dev, dir, mlc_alt_mount, owner, orgfunc);
    int ret = orgfunc(dev, dir, mlc_alt_mount, owner);
    debug_printf("MCP_MountWithSubdir returned %X\n", ret);

    dev = mydev;
    debug_printf("Calling MCP_MountWithSubdir(%s, %s, %s, %p) at %p\n", dev, dir, mount_point, owner, orgfunc);
    ret = orgfunc(dev, dir, mount_point, owner);
    debug_printf("MCP_MountWithSubdir returned %X\n", ret);

    return ret;
}

int wait_for_mlc_hook(const char* dev, u32 timeout, int r2, int r3, int (*orgfunc)(const char*, u32)){
    int res = orgfunc(dev, timeout);
    debug_printf("Waiting for %s returned: %d\n", dev, res);

    dev = "/dev/usb01";
    do{
        debug_printf("Waiting for %s...\n", dev);
        res = orgfunc(dev, timeout);
    } while (res);
    return res;
}

void print_thumb_state(trampoline_t_state *state){
    debug_printf("THUMB state %p: r0: %p, r1: %p, r2: %p, r3: %p, r4: %p, r5: %p, r6: %p, r7: %p, lr: %p\n", state,
            state->r[0],state->r[1],state->r[2],state->r[3],state->r[4],state->r[5],state->r[6],state->r[7], state->lr);
}

void after_mlc_hook(trampoline_t_state *state){
    debug_printf("HOOK: MLC MOUNTED!!!!\n");
}

// This fn runs before everything else in kernel mode.
// It should be used to do extremely early patches
// (ie to BSP and kernel, which launches before MCP)
// It jumps to the real IOS kernel entry on exit.
__attribute__((target("arm")))
void kern_main()
{
    // Make sure relocs worked fine and mappings are good
    debug_printf("we in here trampoline demo plugin kern %p\n", kern_main);

    debug_printf("init_linking symbol at: %08x\n", wafel_find_symbol("init_linking"));

    trampoline_t_blreplace(0x05027d18, wait_for_mlc_hook);
    trampoline_t_blreplace(0x05027d54, mlc_mount_hook);
    //trampoline_t_hook_before(0x05027d58, print_thumb_state);
    //trampoline_t_hook_before(0x05027d58, after_mlc_hook);

}

// This fn runs before MCP's main thread, and can be used
// to perform late patches and spawn threads under MCP.
// It must return.
void mcp_main()
{

}
