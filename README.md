# wafel_payloader

This plugin installs a few different trampolines to call C functions to influence function calls inside IOSU, to print and influence the register state at almost arbitrary locations and to just run something at a specific point int MCP startup

## How to use

- Copy the `wafel_trampoline.ipx` to `/wiiu/ios_plugins`

## Building

```bash
export STROOPWAFEL_ROOT=/path/too/stroopwafel-repo
make
```

## Thanks

- [shinyquagsire23](https://github.com/shinyquagsire23)
