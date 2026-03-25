> [!IMPORTANT]
> 3/25/2026  
> Deleted the `replace` gsc function, MiscMod users would have to disable it

Based on: https://vcodmods.com/server/iw1x-server-source
# CoD 1.1 server mod
**Focused on improving downloading and security**

Some features are:
- Faster download speed (UDP)
- Download stuck issue fix
- Freeze exploit patch
- CD key validity bypass
- IP ban support

---
---

## Build instructions

Using Linux

- `apt install cmake g++-multilib`
- download the repo, then `cd build`
- `cmake ..`, or if you will use MiscMod, use `cmake -DENABLE_FILE_HANDLING=ON ..` instead
- `make`

Obtain `iw1x.so` from `build/bin`

---
---
---

### References
- [Killtube](https://killtube.org/)
- The [reverse engineering Discord server](https://discord.com/invite/weKN5wb)
- [codextended](https://github.com/xtnded/codextended)
- [zk_libcod](https://github.com/ibuddieat/zk_libcod)
- [CoD2rev_Server](https://github.com/voron00/CoD2rev_Server) and [voron's libcod fork](https://github.com/voron00/libcod)
- [RTCW-MP](https://github.com/id-Software/RTCW-MP)
- [Ghidra](https://en.wikipedia.org/wiki/Ghidra) and [IDA Free](https://hex-rays.com/ida-free/)
- [ChatGPT](https://chatgpt.com/)
