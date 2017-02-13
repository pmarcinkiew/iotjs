## Build IoT.js with Artik 10

IoT.js can be compiled for Tizen Artik 10 using [GBS](https://source.tizen.org/documentation/reference/git-build-system).

It is cross compilation environment designed for all Tizen devices. The gbs.conf file defines environment for compilation of your package.
GBS is based on GIT and RPM packaging.

## GBS installation

[GBS installation](https://source.tizen.org/documentation/developer-guide/getting-started-guide/installing-development-tools) is possible only for Linux.
_Ubuntu_ is supported with _DEB_ packages, there is also guide for _openSUSE_, _Fedora_, _CentOS_.

## GBS configuration

Artik 10 is supported by Tizen Common 3.0 platform. The ~/.gbs.conf file contains links to two repos:
* Tizen Base: http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/
* Tizen Common 3.0 Artik build: http://download.tizen.org/snapshots/tizen/common\_artik/latest/repos/arm-wayland/packages/


These repos are defined in ~/.gbs conf file:
```
cat ~/.gbs.conf
[general]
tmpdir=/var/tmp/
profile = profile.common.tizen3.0.artik
work_dir=.
 
[repo.tizen3.0.artik]
url = http://download.tizen.org/snapshots/tizen/common_artik/latest/repos/arm-wayland/packages/

[repo.tizen.base]
url = http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/
 
[profile.common.tizen3.0.artik]
repos=repo.tizen3.0.artik, repo.tizen.base
```

## GBS build

GBS build command works well only at root. It takes into account only source code committed into GIT repo so you need to commit all changes or use _--include-all_ flag. It downloads all dependencies, prepares _chroot_ environment for compilation:

```bash
# gbs build -A armv7l --debug
```
GBS compilation is very time consuming so please be patient. It downloads all packages from tizen.org and some cross compilation tools run inside qemu.
It can take more than 10 minutes to build IoT.js with very fast Internet connection. It is possible to speed up build with some flags

Please find below example compilation log:

```
yourpc:/root/iotjs# gbs build -A armv7l --debug
gbp:debug: ['git', 'rev-parse', '--is-bare-repository']
gbp:debug: ['git', 'rev-parse', '--git-dir']
gbp:debug: ['git', 'rev-parse', '--show-toplevel']
gbs:info: generate repositories ...
gbs:warning: No local package repository for arch armv7l
gbs:debug: fetching http://download.tizen.org/snapshots/tizen/common_artik/latest/repos/arm-wayland/packages/repodata/repomd.xml => /var/tmp/root-gbs/gbscacheInVj20/repomd.xml
gbs:debug: disable HTTP caching
gbs:debug: fetching http://download.tizen.org/snapshots/tizen/common_artik/latest/repos/arm-wayland/packages/repodata/repomd.xml => /var/tmp/root-gbs/gbscacheInVj20/repomd.xml
gbs:debug: fetching http://download.tizen.org/snapshots/tizen/common_artik/latest/repos/arm-wayland/packages/

[CUT]

info: parsing package data...
info: building repo metadata ...
info: resolving skipped packages ...
info: package dependency resolving ...
debug: Checking dependencies for iotjs
debug: my_system: sudo /bin/echo -n
info: package dependency:
iotjs:
info: *** [1/1] building iotjs-0.0.1-1 armv7l common.tizen3.0.artik (worker: 0) ***
debug: sudo /usr/bin/build --uid 0:0 --target armv7l-tizen-linux --jobs 16 --cachedir /root/GBS-ROOT/local/cache --dist /var/tmp/root-gbs/common.tizen3.0.artik.conf --arch armv7l:armv7el:armv6l:armv5tejl:armv5tel:armv5l:armv4tl:armv4l:armv3l:noarch /root/GBS-ROOT/local/sources/common.tizen3.0.artik/iotjs-0.0.1-1/iotjs.spec --repository /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/RPMS --repository http://download.tizen.org/snapshots/tizen/common_artik/latest/repos/arm-wayland/packages/ --repository http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/ --debug --root /root/GBS-ROOT/local/BUILD-ROOTS/scratch.armv7l.0 
debug: my_system: sudo /usr/bin/build --uid 0:0 --target armv7l-tizen-linux --jobs 16 --cachedir /root/GBS-ROOT/local/cache --dist /var/tmp/root-gbs/common.tizen3.0.artik.conf --arch armv7l:armv7el:armv6l:armv5tejl:armv5tel:armv5l:armv4tl:armv4l:armv3l:noarch /root/GBS-ROOT/local/sources/common.tizen3.0.artik/iotjs-0.0.1-1/iotjs.spec --repository /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/RPMS --repository http://download.tizen.org/snapshots/tizen/common_artik/latest/repos/arm-wayland/packages/ --repository http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/ --debug --root /root/GBS-ROOT/local/BUILD-ROOTS/scratch.armv7l.0 
--repository /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/RPMS --repository http://download.tizen.org/snapshots/tizen/common_artik/latest/repos/arm-wayland/packages/ --repository http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/
logging output to /root/GBS-ROOT/local/BUILD-ROOTS/scratch.armv7l.0/.build.log...
[    0s] Memory limit set to 16005492KB
[    0s] Using BUILD_ROOT=/root/GBS-ROOT/local/BUILD-ROOTS/scratch.armv7l.0
[    0s] Using BUILD_ARCH=armv7l:armv7el:armv6l:armv5tejl:armv5tel:armv5l:armv4tl:armv4l:armv3l:noarch
[    0s] 
[    0s] 
[    0s] yourpc started "build iotjs.spec" at Mon Feb 13 19:20:16 UTC 2017.
[    0s] 
[    0s] 
[    0s] processing recipe /root/GBS-ROOT/local/sources/common.tizen3.0.artik/iotjs-0.0.1-1/iotjs.spec ...
[    0s] init_buildsystem --configdir /usr/lib/build/configs --cachedir /root/GBS-ROOT/local/cache --repository /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/RPMS --repository http://download.tizen.org/snapshots/tizen/common_artik/latest/repos/arm-wayland/packages/ --repository http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/ /root/GBS-ROOT/local/sources/common.tizen3.0.artik/iotjs-0.0.1-1/iotjs.spec ...
[    0s] initializing /root/GBS-ROOT/local/BUILD-ROOTS/scratch.armv7l.0/.srcfiles.cache ...
[    0s] /usr/lib/build/createdirdeps --oldfile /root/GBS-ROOT/local/BUILD-ROOTS/scratch.armv7l.0/.srcfiles.cache /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/RPMS
[    0s] /usr/lib/build/createrepomddeps --cachedir=/root/GBS-ROOT/local/cache http://download.tizen.org/snapshots/tizen/common_artik/latest/repos/arm-wayland/packages/
[    1s] /usr/lib/build/createrepomddeps --cachedir=/root/GBS-ROOT/local/cache http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/
[    1s] Use of uninitialized value $evr in concatenation (.) or string at /usr/lib/build/Build/Rpmmd.pm line 175.
[    1s] expanding package dependencies...
[    1s] Use of uninitialized value in concatenation (.) or string at /usr/lib/build/expanddeps line 281.
[    2s] copying qemu
[    2s]  /usr/bin/qemu-aarch64-static /usr/bin/qemu-alpha-static /usr/bin/qemu-arm-binfmt /usr/bin/qemu-arm-static /usr/bin/qemu-arm64-binfmt /usr/bin/qemu-arm64-static /usr/bin/qemu-armeb-static /usr/bin/qemu-cris-static /usr/bin/qemu-i386-static /usr/bin/qemu-m68k-static /usr/bin/qemu-microblaze-static /usr/bin/qemu-microblazeel-static /usr/bin/qemu-mips-static /usr/bin/qemu-mips64-static /usr/bin/qemu-mips64el-static /usr/bin/qemu-mipsel-static /usr/bin/qemu-mipsn32-static /usr/bin/qemu-mipsn32el-static /usr/bin/qemu-or32-static /usr/bin/qemu-ppc-static /usr/bin/qemu-ppc64-static /usr/bin/qemu-ppc64abi32-static /usr/bin/qemu-ppc64le-static /usr/bin/qemu-s390x-static /usr/bin/qemu-sh4-static /usr/bin/qemu-sh4eb-static /usr/bin/qemu-sparc-static /usr/bin/qemu-sparc32plus-static /usr/bin/qemu-sparc64-static /usr/bin/qemu-tilegx-static /usr/bin/qemu-unicore32-static /usr/bin/qemu-x86_64-static /usr/sbin/qemu-binfmt-conf.sh
[    2s] 
[    2s] registering binfmt handlers for cross build
[    2s] interpreter for 'aarch64' is -1
[    2s] write: Invalid argument
[    2s] /proc/sys/fs/binfmt_misc/register: write failed
[    2s] /usr/lib/build/qemu-reg: line 8: write failed. Content: :aarch64:M::\x7fELF\x02\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\xb7:\xff\xff\xff\xff\xff\xff\xff\x00\xff\xff\xff\xff\xff\xff\xff\xff\xfe\xff\xff:/usr/bin/qemu-arm64-binfmt:P
[    2s] 
[    2s] /.build/qemu-reg: No such file or directory
[    2s] /.build/qemu-reg: failed. Trying alternate binfmt file
[    2s] interpreter for 'aarch64' is -1
[    2s] write: Invalid argument
[    2s] /proc/sys/fs/binfmt_misc/register: write failed
[    2s] /usr/lib/build/qemu-reg: line 8: write failed. Content: :aarch64:M::\x7fELF\x02\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\xb7:\xff\xff\xff\xff\xff\xff\xff\x00\xff\xff\xff\xff\xff\xff\xff\xff\xfe\xff\xff:/usr/bin/qemu-arm64-binfmt:P
[    2s] 
[    2s] /usr/lib/build/qemu-reg: binfmt registration failed
[    2s] [1/101] downloading http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/noarch/autoconf-2.69-1.7.noarch.rpm ... 
[    5s] [2/101] downloading http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/noarch/automake-1.14.1-1.5.noarch.rpm ... 
[    7s] [3/101] downloading http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/armv7l/bash-3.2.57-3.5.armv7l.rpm ... 
[    8s] [4/101] downloading http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/armv7l/binutils-2.25-7.5.armv7l.rpm ... 

[CUT]

[  324s] [97/101] downloading http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/armv7l/util-linux-2.28-8.2.armv7l.rpm ... 
[  326s] [98/101] downloading http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/armv7l/util-linux-su-2.28-8.2.armv7l.rpm ... 
[  326s] [99/101] downloading http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/armv7l/which-2.20-1.9.armv7l.rpm ... 
[  327s] [100/101] downloading http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/armv7l/xz-5.2.2-4.1.armv7l.rpm ... 
[  327s] [101/101] downloading http://download.tizen.org/snapshots/tizen/base/latest/repos/arm/packages/armv7l/zlib-1.2.8-1.18.armv7l.rpm ... 
[  327s] 
[  328s] cycle: libsoftokn3 -> nss
[  328s]   breaking dependency nss -> libsoftokn3
[  328s] [1/40] preinstalling libmagic-data...
[  328s] [2/40] preinstalling setup...
[  328s] [3/40] preinstalling filesystem...
[  328s] [4/40] preinstalling glibc...

[CUT]

[  333s] [36/40] preinstalling util-linux...
[  333s] [37/40] preinstalling util-linux-su...
[  333s] [38/40] preinstalling nss...
[  333s] [39/40] preinstalling libsoftokn3...
[  333s] [40/40] preinstalling rpm...
[  333s] 
[  333s] running qemu-linux-user-x86_64-cross postinstall script
[  334s] cat: /proc/sys/fs/binfmt_misc/armeb: No such file or directory
[  334s] .init_b_cache/scripts/qemu-linux-user-x86_64-cross.post: line 8: /proc/sys/fs/binfmt_misc/armeb: Permission denied
[  334s] running qemu-accel-x86_64-armv7l postinstall script
[  334s] ln: creating symbolic link `/usr/lib/gcc/armv7l-tizen-linux-gnueabi/4.9.2/liblto_plugin.so': No such file or directory
[  334s] running setup postinstall script
[  334s] .init_b_cache/scripts/setup.post: line 1: syntax error near unexpected token `name'
[  334s] .init_b_cache/scripts/setup.post: line 1: `for i, name in ipairs({"passwd", "shadow", "group", "gshadow"}) do'
[  334s] initializing rpm db...
[  334s] reordering...cycle: libsoftokn3 -> nss
[  334s]   breaking dependency nss -> libsoftokn3
[  334s] done
[  334s] [1/101] installing libmagic-data-5.28-5.2
[  334s] [2/101] installing setup-0.9-3.15
[  334s] [3/101] installing terminfo-base-5.9-2.2
[  335s] [4/101] installing filesystem-3.2-12.1
[  335s] [5/101] installing glibc-2.20-13.2

[CUT]

[  446s] [96/101] installing libdlog-devel-0.5.1-10.4
[  447s] [97/101] installing libtool-2.4.2-3.6
[  447s] [98/101] installing nss-3.23-5.19
[  450s] [99/101] installing libsoftokn3-3.23-5.19
[  450s] [100/101] installing rpm-4.11.0.1-11.1
[  452s] [101/101] installing rpm-build-4.11.0.1-11.1
[  452s] removing nis flags from /root/GBS-ROOT/local/BUILD-ROOTS/scratch.armv7l.0/etc/nsswitch.conf...
[  452s] now finalizing build dir...
[  453s] -----------------------------------------------------------------
[  453s] ----- building iotjs.spec (user abuild)
[  453s] -----------------------------------------------------------------
[  453s] -----------------------------------------------------------------
[  453s] + exec rpmbuild --define '_srcdefattr (-,root,root)' --nosignature --target=armv7l-tizen-linux --define '_build_create_debug 1' -ba /usr/src/packages/SOURCES/iotjs.spec
[  453s] Building target platforms: armv7l-tizen-linux
[  453s] Building for target armv7l-tizen-linux
[  453s] Executing(%prep): /bin/sh -e /var/tmp/rpm-tmp.zfHsFZ
[  453s] + umask 022
[  453s] + cd /usr/src/packages/BUILD
[  453s] + cd /usr/src/packages/BUILD
[  453s] + rm -rf iotjs-0.0.1
[  453s] + /bin/gzip -dc /usr/src/packages/SOURCES/iotjs-0.0.1.tar.gz
[  453s] + /bin/tar -xf -
[  453s] + STATUS=0
[  453s] + '[' 0 -ne 0 ']'
[  453s] + cd iotjs-0.0.1
[  453s] + /bin/chmod -Rf a+rX,u+w,g-w,o-w .
[  453s] + cp /usr/src/packages/SOURCES/iotjs.manifest .
[  453s] + exit 0
[  453s] Executing(%build): /bin/sh -e /var/tmp/rpm-tmp.pWvjtq
[  453s] + umask 022
[  453s] + cd /usr/src/packages/BUILD
[  453s] + cd iotjs-0.0.1
[  453s] ++ uname -m
[  453s] + [[ armv7l == \a\a\r\c\h\6\4 ]]
[  453s] + LANG=C
[  453s] + export LANG
[  453s] + unset DISPLAY
[  453s] + CFLAGS='-O2 -g2 -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector -Wformat-security -fmessage-length=0 -Wl,--as-needed --param=ssp-buffer-size=4 -march=armv7-a -mtune=cortex-a8 -mlittle-endian -mfpu=neon -mfloat-abi=softfp -mthumb -Wp,-D__SOFTFP__ -Wl,-O1 -Wl,--hash-style=gnu -Wa,-mimplicit-it=thumb -g'
[  453s] + export CFLAGS
[  453s] + CXXFLAGS='-O2 -g2 -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector -Wformat-security -fmessage-length=0 -Wl,--as-needed --param=ssp-buffer-size=4 -march=armv7-a -mtune=cortex-a8 -mlittle-endian -mfpu=neon -mfloat-abi=softfp -mthumb -Wp,-D__SOFTFP__ -Wl,-O1 -Wl,--hash-style=gnu -Wa,-mimplicit-it=thumb -g'
[  453s] + export CXXFLAGS
[  453s] + FFLAGS='-O2 -g2 -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector -Wformat-security -fmessage-length=0 -Wl,--as-needed --param=ssp-buffer-size=4 -march=armv7-a -mtune=cortex-a8 -mlittle-endian -mfpu=neon -mfloat-abi=softfp -mthumb -Wp,-D__SOFTFP__ -Wl,-O1 -Wl,--hash-style=gnu -Wa,-mimplicit-it=thumb -g -I%_fmoddir'
[  453s] + export FFLAGS
[  453s] + LDFLAGS=
[  453s] + export LDFLAGS
[  453s] + LD_AS_NEEDED=1
[  453s] + export LD_AS_NEEDED
[  453s] + ./tools/build.py --clean --target-board=artik --target-arch=armv7l --target-os=tizen --no-init-submodule --no-snapshot --no-parallel-build
[  454s] -- The C compiler identification is GNU 4.9.2
[  454s] -- Check for working C compiler: /bin/cc

[CUT]

[  489s] make
[  489s] 
[  489s] ==> Build IoT.js
[  489s] 
[  489s] /usr/src/packages/BUILD/iotjs-0.0.1/build/host/arm-linux/debug/bin/jerry
[  489s] cmake /usr/src/packages/BUILD/iotjs-0.0.1 -DCMAKE_TOOLCHAIN_FILE=/usr/src/packages/BUILD/iotjs-0.0.1/cmake/config/armv7l-tizen.cmake -DCMAKE_BUILD_TYPE=Debug -DTARGET_OS=tizen -DPLATFORM_DESCRIPT=armv7l-tizen -DIOTJS_MODULES=fs testdriver httpparser process timer buffer tcp dns console constants -DEXTERNAL_STATIC_LIB= -DEXTERNAL_SHARED_LIB=m rt -DCMAKE_C_FLAGS=-D__LINUX__ -fno-builtin -DDEBUG -DENABLE_DEBUG_LOG -DTARGET_BOARD=RP2 -DCMAKE_EXE_LINKER_FLAGS=-pthread -DEXTERNAL_INCLUDE_DIR=             
[  489s] 
[  489s] make
[  489s] 
[  489s] ==> Check unit tests
[  489s] 
[  489s] Skip unit tests - target is not host
[  489s] 
[  489s] 
[  489s] IoT.js Build Succeeded!!
[  489s] 
[  489s] + exit 0
[  489s] Executing(%install): /bin/sh -e /var/tmp/rpm-tmp.LiWw9Y
[  489s] + umask 022
[  489s] + cd /usr/src/packages/BUILD
[  489s] + cd iotjs-0.0.1
[  489s] + LANG=C
[  489s] + export LANG
[  489s] + unset DISPLAY
[  489s] + rm -rf /usr/src/packages/BUILDROOT/iotjs-0.0.1-1.arm
[  489s] + mkdir -p /usr/src/packages/BUILDROOT/iotjs-0.0.1-1.arm
[  489s] + mkdir -p /usr/src/packages/BUILDROOT/iotjs-0.0.1-1.arm/usr/bin
[  489s] + cp /usr/src/packages/BUILD/iotjs-0.0.1/build/armv7l-tizen/debug/iotjs/iotjs /usr/src/packages/BUILDROOT/iotjs-0.0.1-1.arm/usr/bin
[  489s] + mkdir -p /usr/src/packages/BUILDROOT/iotjs-0.0.1-1.arm/etc/opt/upgrade
[  489s] + mkdir -p /usr/src/packages/BUILDROOT/iotjs-0.0.1-1.arm/usr/share/license
[  489s] + cp LICENSE /usr/src/packages/BUILDROOT/iotjs-0.0.1-1.arm/usr/share/license/iotjs
[  489s] + /usr/lib/rpm/find-debuginfo.sh /usr/src/packages/BUILD/iotjs-0.0.1
[  489s] extracting debug info from /usr/src/packages/BUILDROOT/iotjs-0.0.1-1.arm/usr/bin/iotjs
[  492s] cpio: /home/abuild/rpmbuild/BUILD/glibc-2.20/csu: Cannot stat: No such file or directory
[  492s] 6054 blocks
[  492s] + /usr/lib/rpm/check-buildroot
[  492s] + /usr/lib/rpm//brp-compress
[  492s] + /usr/lib/rpm/brp-strip-static-archive /bin/strip
[  493s] + /usr/lib/rpm/brp-python-hardlink
[  493s] + /usr/lib/rpm/tizen/find-docs.sh /usr/src/packages/BUILDROOT/iotjs-0.0.1-1.arm
[  493s] Processing files: iotjs-0.0.1-1.armv7l
[  493s] Aptk: PG1hbmlmZXN0PgoJPHJlcXVlc3Q+CgkJPGRvbWFpbiBuYW1lPSJfIiAvPgoJPC9y
[  493s] ZXF1ZXN0PgoJPGFzc2lnbj4KCQk8ZmlsZXN5c3RlbSBwYXRoPSIvdXNyL2Jpbi9p
[  493s] bnN0YWxsX3ByZWxvYWRfcGtnIiBsYWJlbD0iU3lzdGVtIiBleGVjX2xhYmVsPSJu
[  493s] b25lIiAvPgoJCTxmaWxlc3lzdGVtIHBhdGg9Ii91c3IvYmluL3BrZ2NtZCIgbGFi
[  493s] ZWw9IlVzZXI6OlNoZWxsIiBleGVjX2xhYmVsPSJub25lIiAvPgoJCTxmaWxlc3lz
[  493s] dGVtIHBhdGg9Ii91c3IvYmluL3BrZ19jbGVhcmRhdGEiIGxhYmVsPSJTeXN0ZW0i
[  493s] IGV4ZWNfbGFiZWw9Im5vbmUiIC8+CgkJPGZpbGVzeXN0ZW0gcGF0aD0iL3Vzci9i
[  493s] aW4vcGtnX2dldHNpemUiIGxhYmVsPSJTeXN0ZW0iIGV4ZWNfbGFiZWw9Im5vbmUi
[  493s] IC8+CgkJPGZpbGVzeXN0ZW0gcGF0aD0iL3Vzci9iaW4vcGtnaW5mbyIgbGFiZWw9
[  493s] IlVzZXI6OlNoZWxsIiBleGVjX2xhYmVsPSJub25lIiAvPgoJPC9hc3NpZ24+Cjwv
[  493s] bWFuaWZlc3Q+Cg==
[  493s] 
[  493s] Provides: iotjs = 0.0.1-1 iotjs(armv7l-32) = 0.0.1-1
[  493s] Requires(interp): /bin/sh /bin/sh
[  493s] Requires(rpmlib): rpmlib(CompressedFileNames) <= 3.0.4-1 rpmlib(PayloadFilesHavePrefix) <= 4.0-1
[  493s] Requires(post): /bin/sh
[  493s] Requires: libc.so.6 libc.so.6(GLIBC_2.4) libgcc_s.so.1 libgcc_s.so.1(GCC_3.5) libm.so.6 libm.so.6(GLIBC_2.4) libpthread.so.0 libpthread.so.0(GLIBC_2.4) librt.so.1 librt.so.1(GLIBC_2.4)
[  493s] Processing files: iotjs-debugsource-0.0.1-1.armv7l
[  493s] Provides: iotjs-debugsource = 0.0.1-1 iotjs-debugsource(armv7l-32) = 0.0.1-1
[  493s] Requires(rpmlib): rpmlib(PayloadFilesHavePrefix) <= 4.0-1 rpmlib(CompressedFileNames) <= 3.0.4-1
[  493s] Processing files: iotjs-debuginfo-0.0.1-1.armv7l
[  493s] Provides: iotjs-debuginfo = 0.0.1-1 iotjs-debuginfo(armv7l-32) = 0.0.1-1
[  493s] Requires(rpmlib): rpmlib(CompressedFileNames) <= 3.0.4-1 rpmlib(PayloadFilesHavePrefix) <= 4.0-1
[  493s] Checking for unpackaged file(s): /usr/lib/rpm/check-files /usr/src/packages/BUILDROOT/iotjs-0.0.1-1.arm
[  493s] warning: Could not canonicalize hostname: yourpc
[  494s] Wrote: /usr/src/packages/SRPMS/iotjs-0.0.1-1.src.rpm
[  494s] Wrote: /usr/src/packages/RPMS/armv7l/iotjs-0.0.1-1.armv7l.rpm
[  495s] Wrote: /usr/src/packages/RPMS/armv7l/iotjs-debugsource-0.0.1-1.armv7l.rpm
[  495s] Wrote: /usr/src/packages/RPMS/armv7l/iotjs-debuginfo-0.0.1-1.armv7l.rpm
[  495s] Executing(%clean): /bin/sh -e /var/tmp/rpm-tmp.hTsrRp
[  495s] + umask 022
[  495s] + cd /usr/src/packages/BUILD
[  495s] + cd iotjs-0.0.1
[  495s] + /bin/rm -rf /usr/src/packages/BUILDROOT/iotjs-0.0.1-1.arm
[  495s] + exit 0
[  495s] ... checking for files with abuild user/group
[  495s] 
[  495s] yourpc finished "build iotjs.spec" at Mon Feb 13 19:28:31 UTC 2017.
[  495s] 
debug: my_system: sudo /bin/cp /var/tmp/root-gbs/common.tizen3.0.artik.conf /root/GBS-ROOT/local/BUILD-ROOTS/scratch.armv7l.0/common.tizen3.0.artik.conf
debug: my_system: sudo /bin/mv /root/GBS-ROOT/local/BUILD-ROOTS/scratch.armv7l.0/.build.log /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/logs/success/iotjs-0.0.1-1/log.txt
debug: my_system: sudo /bin/mv /root/GBS-ROOT/local/BUILD-ROOTS/scratch.armv7l.0//usr/src/packages/SRPMS/*.rpm /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/SRPMS
debug: my_system: sudo /bin/mv  /root/GBS-ROOT/local/BUILD-ROOTS/scratch.armv7l.0//usr/src/packages/RPMS/*/*.rpm /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/RPMS
debug: my_system: /usr/lib/build/createdirdeps /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/RPMS > /root/GBS-ROOT/local/order/.repo.cache.local 
debug: my_system: echo D: >> /root/GBS-ROOT/local/order/.repo.cache.local
debug: my_system: cat /root/GBS-ROOT/local/order/.repo.cache.local /root/GBS-ROOT/local/order/.repo.cache.remote >/root/GBS-ROOT/local/order/.repo.cache
info: finished building iotjs
debug: *** build iotjs exit with status(0), is dirty:1, (worker: 0) ***
info: updating local repo
debug: my_system: touch /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/SRPMS
debug: my_system: touch /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/RPMS
debug: my_system: createrepo --changelog-limit=0 -q /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l > /dev/null 2>&1 
info: *** Build Status Summary ***
=== Total succeeded built packages: (1) ===
info: generated html format report:
     /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/index.html
info: generated RPM packages can be found from local repo:
     /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/RPMS
info: generated source RPM packages can be found from local repo:
     /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/SRPMS
info: build logs can be found in:
     /root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/logs
info: build roots located in:
     /root/GBS-ROOT/local/BUILD-ROOTS/scratch.armv7l.*
gbs:info: Done

```
The output of compilation is set of RPM packages for runtime, debug symbols, debug source code and source code.

### Setting Artik 10

#### microSD card preparation
The configuration procedure for Artik 10 is described in Tizen Wiki article
[Booting Tizen 3.0 from microSD on ARTIK](https://wiki.tizen.org/wiki/Booting_Tizen_3.0_From_microSD_On_ARTIK).

* Repo contains tools for microSD card formatting and pre-build tools like u-boot.
* Latest boot images for Artik 10 are stored at [tizen.org](http://download.tizen.org/snapshots/tizen/common_artik/latest/images/arm-wayland/common-boot-armv7l-artik10/)
* Latest system images for Artik 10 are stored at [tizen.org](http://download.tizen.org/snapshots/tizen/common_artik/latest/images/arm-wayland/common-wayland-3parts-armv7l-artik/)

GIT repo for microSD boot:
```
git clone https://github.com/tizen-artik/sdboot
```

Let's assume you would like to flash microSD card visible in your Linux as /dev/sdc with 2017.02.06 image.
```bash
# cd sdboot
# ./mk_sdboot.sh -m artik10 -d /dev/sdc -b /home/piotr/Downloads/tizen-common-artik_20170206.1_common-boot-armv7l-artik10.tar.gz\
-p /home/piotr/Downloads/tizen-common-artik_20170206.1_common-wayland-3parts-armv7l-artik.tar.gz
```
You need 8GB microSD card.

#### Ubot configuration

Artik 10 provides serial over USB interface. You need minicom or similar software to connect to serial terminal.
If you connect to DEBUG USB port, then Artik will light up blue LEDs next to port.
It is very important to set switch SW2 into correct [position](https://wiki.tizen.org/w/images/f/fa/Artik5_om_sd_1.jpg).

![alt tag](https://raw.githubusercontent.com/pmarcinkiew/iotjs/tizen_docs/docs/build/artik10_diagram_v2.jpg)

You need to connect barrel jack to power supply port and press SW3 POWER switch for 3 seconds to boot Artik.

Micom configuration:

```
lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536                                                                                        
        inet 127.0.0.1+---------[Comm Parameters]----------+                                                                        
        inet6 ::1  pre|                                    |                                                                        
        loop  txqueuel|     Current: 115200 8N1            |                                                                        
        RX packets 4  | Speed            Parity      Data  |                                                                        
        RX errors 0  d| A: <next>        L: None     S: 5  |                                                                        
        TX packets 4  | B: <prev>        M: Even     T: 6  |                                                                        
        TX errors 0  d| C:   9600        N: Odd      U: 7  |ions 0                                                                  
                      | D:  38400        O: Mark     V: 8  |                                                                        
root:~>               | E: 115200        P: Space          |                                                                        
localhost login:      |                                    |                                                                        
localhost login: root | Stopbits                           |                                                                        
Password:             | W: 1             Q: 8-N-1          |                                                                        
2.4+ kernel w/o ELF no| X: 2             R: 7-E-1          |                                                                        
Welcome to Tizen      |                                    |                                                                        
root:~> ifconfig      |                                    |                                                                        
eth0: flags=-28605<UP,| Choice, or <Enter> to exit?        |mtu 1500                                                                
        inet xxx.xxx.1+------------------------------------+st xxx.xxx.155.255                                                      
        inet6 xxxx::xxxx:xxxx:xxxx:xxx  prefixlen 64  scopeid 0x20<link>
        ether xx:xx:xx:xx:xx:xx  txqueuelen 1000  (Ethernet)
        RX packets 975  bytes 50170 (48.9 KiB)
        RX errors 0  dropped 4  overruns 0  frame 0
        TX packets 0  bytes 5221 (5.0 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0
        inet6 ::1  prefixlen 128  scopeid 0x10<host>
        loop  txqueuelen 0  (Local Loopback)
        RX packets 4  bytes 284 (284.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 4  bytes 284 (284.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

root:~>  

```


The default boot device for Artik 10 is eMMC so you need to change configuration of your device using u-boot.
You need to modify [pin configuration](https://wiki.tizen.org/wiki/Booting_Tizen_3.0_From_microSD_On_ARTIK#ARTIK_5_.2F_10) and 
used u-boot shell to reconfigure your boot device. Artik 10 default configuration prevents
it from using Ethernet correctly so it is very helpful to configure MAC address.


```
U-Boot 2012.07-ge10b06ecb-TIZEN.org (Jan 11 2017 - 02:49:29) for ARTIK10

CPU: Exynos5422 Rev0.1 [Samsung SOC on SMP Platform Base on ARM CortexA7]
APLL = 800MHz, KPLL = 800MHz
MPLL = 532MHz, BPLL = 825MHz

Board: ARTIK10
DRAM:  2 GiB
WARNING: Caches not enabled

TrustZone Enabled BSP
BL1 version: �/�⁁�
VDD\_KFC: 0x44
LDO19: 0x28

Checking Boot Mode ... SDMMC
MMC:   S5P_MSHC2: 0, S5P_MSHC0: 1
MMC Device 0: 7.4 GiB
MMC Device 1: 14.6 GiB
MMC Device 2: MMC Device 2 not found
In:    serial
Out:   serial
Err:   serial
rst_stat : 0x800
Net:   No ethernet found.
Hit any key to stop autoboot:  0 #                    <--- INPUT ZERO HERE TO PREVENT AUTO BOOT
ARTIK10 # 
ARTIK10 # run factory_load
ARTIK10 # factory_info write ethaddr XX:XX:XX:XX:XX:XX    # <--- THIS SHOULD SET BY YOU AS MAC
ARTIK10 # run factory_save
ARTIK10 # reset

resetting ...


U-Boot 2012.07-ge10b06ecb-TIZEN.org (Jan 11 2017 - 02:49:29) for ARTIK10

CPU: Exynos5422 Rev0.1 [Samsung SOC on SMP Platform Base on ARM CortexA7]
APLL = 800MHz, KPLL = 800MHz
MPLL = 532MHz, BPLL = 825MHz

Board: ARTIK10
DRAM:  2 GiB
WARNING: Caches not enabled

TrustZone Enabled BSP
BL1 version: 
VDD\_KFC: 0x44
LDO19: 0x28

Checking Boot Mode ... SDMMC
MMC:   S5P_MSHC2: 0, S5P_MSHC0: 1
MMC Device 0: 7.4 GiB
MMC Device 1: 14.6 GiB
MMC Device 2: MMC Device 2 not found
In:    serial
Out:   serial
Err:   serial
rst_stat : 0x800
Net:   No ethernet found.
Hit any key to stop autoboot:  0                # <--- INPUT ZERO HERE TO PREVENT AUTO BOOT
ARTIK10 # env default -f
ARTIK10 # saveenv
Saving Environment to MMC...
Writing to MMC(0)... done

ARTIK10 # env set rootdev 1                     # <--- HERE MICROSD CARD IS SET AS BOOT DEVICE
ARTIK10 # env save
Saving Environment to MMC...
Writing to MMC(0)... done
ARTIK10 # reset

```
After successful configuration you should be able to use
[smart development bridge](https://developer.tizen.org/development/tools/common-tools/smart-development-bridge).

1. Connect to your Artik 10 over USB/serial interface.
2. Login into device with user root and super secret password tizen.
3. Type ifconfig to check your IP networking configuration

You can use this short example of boot log to see how ifconfig can help you:

```
U-Boot 2012.07-ge10b06ecb-TIZEN.org (Jan 11 2017 - 02:49:29) for ARTIK10

CPU: Exynos5422 Rev0.1 [Samsung SOC on SMP Platform Base on ARM CortexA7]
APLL = 800MHz, KPLL = 800MHz
MPLL = 532MHz, BPLL = 825MHz

Board: ARTIK10
DRAM:  2 GiB
WARNING: Caches not enabled

TrustZone Enabled BSP
BL1 version: �/�⁁�
VDD_KFC: 0x44
LDO19: 0x28

Checking Boot Mode ... SDMMC
MMC:   S5P_MSHC2: 0, S5P_MSHC0: 1
MMC Device 0: 7.4 GiB
MMC Device 1: 14.6 GiB
MMC Device 2: MMC Device 2 not found
In:    serial
Out:   serial
Err:   serial
rst_stat : 0x100
Net:   No ethernet found.
Hit any key to stop autoboot:  0 
reading zImage
5406366 bytes read in 26951 ms (195.3 KiB/s)
reading exynos5422-artik10.dtb
70702 bytes read in 29900 ms (2 KiB/s)
reading uInitrd
1420803 bytes read in 237910 ms (4.9 KiB/s)
## Loading init Ramdisk from Legacy Image at 43000000 ...
   Image Name:   uInitrd
   Image Type:   ARM Linux RAMDisk Image (uncompressed)
   Data Size:    1420739 Bytes = 1.4 MiB
   Load Address: 00000000
   Entry Point:  00000000

Starting kernel ...

[    0.093723] [c0] /cpus/cpu@0 missing clock-frequency property
[    0.093750] [c0] /cpus/cpu@1 missing clock-frequency property

[SOME LINES REMOVED]

[    4.408086] [c5] ion_cma ion_mfc_fw: Alread isolated!
[    4.413141] [c5] ion_cma ion_mfc_nfw: Alread isolated!
[    4.421725] [c5] slp_multi slp_multi.18: usb : slp_multi_probe device tree enabled
[    4.432171] [c5] slp_multi slp_multi.18: usb : slp_multi_probe usb_mode driver, version:1.1, init Ok
Loading, please wait...

Welcome to Tizen 3.0.0 (Tizen3/Common)!

[  OK  ] Reached target Swap.
[  OK  ] Created slice Root Slice.

[SOME LINES REMOVED]

[  OK  ] Started Start the Central Key Manager.
[  OK  ] Started Cynara service.
[  OK  ] Started Sensor Daemon.
[  OK  ] Started Cleanup private sharing.
[FAILED] Failed to start nether service.
See "systemctl status nether.service" for details.
[   21.248874] [c6] usb\_mode usb0: slp\_multi\_usb: enable SUCCESS 'mtp'
[   21.254014] usb\_mode usb0: slp\_multi\_usb: Cannot enable ''

localhost login: root
Password:                                 # <- input tizen here and press enter
2.4+ kernel w/o ELF notes? -- report this
Welcome to Tizen
root:~> ifconfig
eth0: flags=-28605<UP,BROADCAST,RUNNING,MULTICAST,DYNAMIC>  mtu 1500
        inet XXX.XXX.XXX.XXX  netmask 255.255.252.0  broadcast XXX.XXX.XXX.255
        inet6 XXXX::XXXX:XXXX:XXXX:XXX  prefixlen 64  scopeid 0x20<link>
        ether XX:XX:XX:XX:XX:XX  txqueuelen 1000  (Ethernet)
        RX packets 1145  bytes 62823 (61.3 KiB)
        RX errors 0  dropped 5  overruns 0  frame 0
        TX packets 0  bytes 5513 (5.3 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0
        inet6 ::1  prefixlen 128  scopeid 0x10<host>
        loop  txqueuelen 0  (Local Loopback)
        RX packets 4  bytes 284 (284.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 4  bytes 284 (284.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

root:~>
```
## SDB

[Smart development bridge](https://developer.tizen.org/development/tools/common-tools/smart-development-bridge)
can be used to connect to device. You need to install it with [Tizen Studio](https://developer.tizen.org/development/tizen-studio).

The sdb command to connect to device:
```
$ sdb connect xxx.xxx.xxx.xxx
```
You need root access to install RPM packages:

```
$ sdb root on
```

You can used sdb push command to copy RPMs to Artik:

```
$ sdb push your_local_GBS_build_folder/*.rpm /root
```

There is shell command to access shell in Artik:
```
$ sdb shell
```

The rpm can be used to install packages to device:

```
sh-3.2# rpm -i your_package.rpm
```

Please find below short session with Artik RPM installation:

```
yourpc# sdb connect XXX.XXX.XXX.XXX
* server not running. starting it now on port 26099 *
* server started successfully *
connecting to XXX.XXX.XXX.XXX:26101 ...
connected to xxx.xxx.xxx.xxx:26101
yourpc# sdb root on
Switched to 'root' account mode
yourpc# sdb push ~/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/RPMS/*.rpm /root
WARNING: Your data is to be sent over an unencrypted connection and could be read by others.
pushed       iotjs-0.0.1-1.armv7l.rpm   100%        209KB           0KB/s
1 file(s) pushed. 0 file(s) skipped.
/root/GBS-ROOT/local/repos/common.tizen3.0.artik/armv7l/RPMS/iotjs-0.0.1-1.armv7l.rpm   1188KB/s (214511 bytes in 0.176s)
yourpc# sdb shell
sh-3.2# rpm -i /root/iotjs-0.0.1-1.armv7l.rpm 
sh-3.2# /usr/bin/iotjs 
usage: iotjs <js> [<iotjs arguments>] [-- <app arguments>]
[ERR] iotjs_environment_parse_command_line_arguments failed
sh-3.2#
```
```
