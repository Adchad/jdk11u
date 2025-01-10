# Makefile for program source directory in GNU NLS utilities package.
# Copyright (C) 1995, 1996, 1997 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
# Copyright (C) 2001-2017 Free Software Foundation, Inc.
#
# This file may be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.

PACKAGE = bfd
VERSION = 2.28.1

SHELL = /bin/sh


srcdir = /home/adam/binutils-2.28.1/bfd/po
top_srcdir = /home/adam/binutils-2.28.1/bfd
VPATH = /home/adam/binutils-2.28.1/bfd/po
top_builddir = ..

prefix = /usr/local
exec_prefix = ${prefix}
datadir = $(prefix)/
localedir = $(datadir)/locale
gnulocaledir = $(prefix)/share/locale
gettextsrcdir = $(prefix)/share/gettext/po
subdir = po

DESTDIR =

INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
MKINSTALLDIRS = /home/adam/binutils-2.28.1/bfd/../mkinstalldirs

CC = gcc
GENCAT = 
GMSGFMT = PATH=../src:$$PATH :
MSGFMT = :
XGETTEXT = PATH=../src:$$PATH :
MSGMERGE = PATH=../src:$$PATH msgmerge

DEFS = -DHAVE_CONFIG_H
CFLAGS = -fPIC -O
CPPFLAGS = 

INCLUDES = -I.. -I$(top_srcdir)/intl

COMPILE = $(CC) -c $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(XCFLAGS)

SOURCES = cat-id-tbl.c
POFILES =  $(srcdir)/fr.po $(srcdir)/tr.po $(srcdir)/ja.po $(srcdir)/es.po $(srcdir)/sv.po $(srcdir)/da.po $(srcdir)/zh_CN.po $(srcdir)/ro.po $(srcdir)/rw.po $(srcdir)/vi.po $(srcdir)/fi.po $(srcdir)/id.po $(srcdir)/ru.po $(srcdir)/uk.po $(srcdir)/sr.po
GMOFILES =  $(srcdir)/fr.gmo $(srcdir)/tr.gmo $(srcdir)/ja.gmo $(srcdir)/es.gmo $(srcdir)/sv.gmo $(srcdir)/da.gmo $(srcdir)/zh_CN.gmo $(srcdir)/ro.gmo $(srcdir)/rw.gmo $(srcdir)/vi.gmo $(srcdir)/fi.gmo $(srcdir)/id.gmo $(srcdir)/ru.gmo $(srcdir)/uk.gmo $(srcdir)/sr.gmo
DISTFILES = ChangeLog Makefile.in.in SRC-POTFILES.in BLD-POTFILES.in $(PACKAGE).pot \
stamp-cat-id $(POFILES) $(GMOFILES) $(SOURCES)

# Note - the following line gets processed by bfd/configure and amended
# to contain the full list of source dir POTFILES.
SRC-POTFILES = \
	/home/adam/binutils-2.28.1/bfd/aix386-core.c \
	/home/adam/binutils-2.28.1/bfd/aix5ppc-core.c \
	/home/adam/binutils-2.28.1/bfd/aout-adobe.c \
	/home/adam/binutils-2.28.1/bfd/aout-arm.c \
	/home/adam/binutils-2.28.1/bfd/aout-cris.c \
	/home/adam/binutils-2.28.1/bfd/aout-ns32k.c \
	/home/adam/binutils-2.28.1/bfd/aout-sparcle.c \
	/home/adam/binutils-2.28.1/bfd/aout-target.h \
	/home/adam/binutils-2.28.1/bfd/aout-tic30.c \
	/home/adam/binutils-2.28.1/bfd/aout0.c \
	/home/adam/binutils-2.28.1/bfd/aout32.c \
	/home/adam/binutils-2.28.1/bfd/aout64.c \
	/home/adam/binutils-2.28.1/bfd/aoutf1.h \
	/home/adam/binutils-2.28.1/bfd/aoutx.h \
	/home/adam/binutils-2.28.1/bfd/archive.c \
	/home/adam/binutils-2.28.1/bfd/archive64.c \
	/home/adam/binutils-2.28.1/bfd/archures.c \
	/home/adam/binutils-2.28.1/bfd/armnetbsd.c \
	/home/adam/binutils-2.28.1/bfd/bfd.c \
	/home/adam/binutils-2.28.1/bfd/bfdio.c \
	/home/adam/binutils-2.28.1/bfd/bfdwin.c \
	/home/adam/binutils-2.28.1/bfd/binary.c \
	/home/adam/binutils-2.28.1/bfd/bout.c \
	/home/adam/binutils-2.28.1/bfd/cache.c \
	/home/adam/binutils-2.28.1/bfd/cf-i386lynx.c \
	/home/adam/binutils-2.28.1/bfd/cf-sparclynx.c \
	/home/adam/binutils-2.28.1/bfd/cisco-core.c \
	/home/adam/binutils-2.28.1/bfd/coff-alpha.c \
	/home/adam/binutils-2.28.1/bfd/coff-apollo.c \
	/home/adam/binutils-2.28.1/bfd/coff-arm.c \
	/home/adam/binutils-2.28.1/bfd/coff-aux.c \
	/home/adam/binutils-2.28.1/bfd/coff-bfd.c \
	/home/adam/binutils-2.28.1/bfd/coff-go32.c \
	/home/adam/binutils-2.28.1/bfd/coff-h8300.c \
	/home/adam/binutils-2.28.1/bfd/coff-h8500.c \
	/home/adam/binutils-2.28.1/bfd/coff-i386.c \
	/home/adam/binutils-2.28.1/bfd/coff-i860.c \
	/home/adam/binutils-2.28.1/bfd/coff-i960.c \
	/home/adam/binutils-2.28.1/bfd/coff-m68k.c \
	/home/adam/binutils-2.28.1/bfd/coff-m88k.c \
	/home/adam/binutils-2.28.1/bfd/coff-mips.c \
	/home/adam/binutils-2.28.1/bfd/coff-rs6000.c \
	/home/adam/binutils-2.28.1/bfd/coff-sh.c \
	/home/adam/binutils-2.28.1/bfd/coff-sparc.c \
	/home/adam/binutils-2.28.1/bfd/coff-stgo32.c \
	/home/adam/binutils-2.28.1/bfd/coff-svm68k.c \
	/home/adam/binutils-2.28.1/bfd/coff-tic30.c \
	/home/adam/binutils-2.28.1/bfd/coff-tic4x.c \
	/home/adam/binutils-2.28.1/bfd/coff-tic54x.c \
	/home/adam/binutils-2.28.1/bfd/coff-tic80.c \
	/home/adam/binutils-2.28.1/bfd/coff-u68k.c \
	/home/adam/binutils-2.28.1/bfd/coff-w65.c \
	/home/adam/binutils-2.28.1/bfd/coff-we32k.c \
	/home/adam/binutils-2.28.1/bfd/coff-x86_64.c \
	/home/adam/binutils-2.28.1/bfd/coff-z80.c \
	/home/adam/binutils-2.28.1/bfd/coff-z8k.c \
	/home/adam/binutils-2.28.1/bfd/coff64-rs6000.c \
	/home/adam/binutils-2.28.1/bfd/coffcode.h \
	/home/adam/binutils-2.28.1/bfd/coffgen.c \
	/home/adam/binutils-2.28.1/bfd/cofflink.c \
	/home/adam/binutils-2.28.1/bfd/coffswap.h \
	/home/adam/binutils-2.28.1/bfd/compress.c \
	/home/adam/binutils-2.28.1/bfd/corefile.c \
	/home/adam/binutils-2.28.1/bfd/cpu-aarch64.c \
	/home/adam/binutils-2.28.1/bfd/cpu-alpha.c \
	/home/adam/binutils-2.28.1/bfd/cpu-arc.c \
	/home/adam/binutils-2.28.1/bfd/cpu-arm.c \
	/home/adam/binutils-2.28.1/bfd/cpu-avr.c \
	/home/adam/binutils-2.28.1/bfd/cpu-bfin.c \
	/home/adam/binutils-2.28.1/bfd/cpu-cr16.c \
	/home/adam/binutils-2.28.1/bfd/cpu-cr16c.c \
	/home/adam/binutils-2.28.1/bfd/cpu-cris.c \
	/home/adam/binutils-2.28.1/bfd/cpu-crx.c \
	/home/adam/binutils-2.28.1/bfd/cpu-d10v.c \
	/home/adam/binutils-2.28.1/bfd/cpu-d30v.c \
	/home/adam/binutils-2.28.1/bfd/cpu-dlx.c \
	/home/adam/binutils-2.28.1/bfd/cpu-epiphany.c \
	/home/adam/binutils-2.28.1/bfd/cpu-fr30.c \
	/home/adam/binutils-2.28.1/bfd/cpu-frv.c \
	/home/adam/binutils-2.28.1/bfd/cpu-ft32.c \
	/home/adam/binutils-2.28.1/bfd/cpu-h8300.c \
	/home/adam/binutils-2.28.1/bfd/cpu-h8500.c \
	/home/adam/binutils-2.28.1/bfd/cpu-hppa.c \
	/home/adam/binutils-2.28.1/bfd/cpu-i370.c \
	/home/adam/binutils-2.28.1/bfd/cpu-i386.c \
	/home/adam/binutils-2.28.1/bfd/cpu-i860.c \
	/home/adam/binutils-2.28.1/bfd/cpu-i960.c \
	/home/adam/binutils-2.28.1/bfd/cpu-ia64.c \
	/home/adam/binutils-2.28.1/bfd/cpu-iamcu.c \
	/home/adam/binutils-2.28.1/bfd/cpu-ip2k.c \
	/home/adam/binutils-2.28.1/bfd/cpu-iq2000.c \
	/home/adam/binutils-2.28.1/bfd/cpu-k1om.c \
	/home/adam/binutils-2.28.1/bfd/cpu-l1om.c \
	/home/adam/binutils-2.28.1/bfd/cpu-lm32.c \
	/home/adam/binutils-2.28.1/bfd/cpu-m10200.c \
	/home/adam/binutils-2.28.1/bfd/cpu-m10300.c \
	/home/adam/binutils-2.28.1/bfd/cpu-m32c.c \
	/home/adam/binutils-2.28.1/bfd/cpu-m32r.c \
	/home/adam/binutils-2.28.1/bfd/cpu-m68hc11.c \
	/home/adam/binutils-2.28.1/bfd/cpu-m68hc12.c \
	/home/adam/binutils-2.28.1/bfd/cpu-m68k.c \
	/home/adam/binutils-2.28.1/bfd/cpu-m88k.c \
	/home/adam/binutils-2.28.1/bfd/cpu-m9s12x.c \
	/home/adam/binutils-2.28.1/bfd/cpu-m9s12xg.c \
	/home/adam/binutils-2.28.1/bfd/cpu-mcore.c \
	/home/adam/binutils-2.28.1/bfd/cpu-mep.c \
	/home/adam/binutils-2.28.1/bfd/cpu-metag.c \
	/home/adam/binutils-2.28.1/bfd/cpu-microblaze.c \
	/home/adam/binutils-2.28.1/bfd/cpu-mips.c \
	/home/adam/binutils-2.28.1/bfd/cpu-mmix.c \
	/home/adam/binutils-2.28.1/bfd/cpu-moxie.c \
	/home/adam/binutils-2.28.1/bfd/cpu-msp430.c \
	/home/adam/binutils-2.28.1/bfd/cpu-mt.c \
	/home/adam/binutils-2.28.1/bfd/cpu-nds32.c \
	/home/adam/binutils-2.28.1/bfd/cpu-nios2.c \
	/home/adam/binutils-2.28.1/bfd/cpu-ns32k.c \
	/home/adam/binutils-2.28.1/bfd/cpu-or1k.c \
	/home/adam/binutils-2.28.1/bfd/cpu-pdp11.c \
	/home/adam/binutils-2.28.1/bfd/cpu-pj.c \
	/home/adam/binutils-2.28.1/bfd/cpu-plugin.c \
	/home/adam/binutils-2.28.1/bfd/cpu-powerpc.c \
	/home/adam/binutils-2.28.1/bfd/cpu-riscv.c \
	/home/adam/binutils-2.28.1/bfd/cpu-rl78.c \
	/home/adam/binutils-2.28.1/bfd/cpu-rs6000.c \
	/home/adam/binutils-2.28.1/bfd/cpu-rx.c \
	/home/adam/binutils-2.28.1/bfd/cpu-s390.c \
	/home/adam/binutils-2.28.1/bfd/cpu-score.c \
	/home/adam/binutils-2.28.1/bfd/cpu-sh.c \
	/home/adam/binutils-2.28.1/bfd/cpu-sparc.c \
	/home/adam/binutils-2.28.1/bfd/cpu-spu.c \
	/home/adam/binutils-2.28.1/bfd/cpu-tic30.c \
	/home/adam/binutils-2.28.1/bfd/cpu-tic4x.c \
	/home/adam/binutils-2.28.1/bfd/cpu-tic54x.c \
	/home/adam/binutils-2.28.1/bfd/cpu-tic6x.c \
	/home/adam/binutils-2.28.1/bfd/cpu-tic80.c \
	/home/adam/binutils-2.28.1/bfd/cpu-tilegx.c \
	/home/adam/binutils-2.28.1/bfd/cpu-tilepro.c \
	/home/adam/binutils-2.28.1/bfd/cpu-v850.c \
	/home/adam/binutils-2.28.1/bfd/cpu-v850_rh850.c \
	/home/adam/binutils-2.28.1/bfd/cpu-vax.c \
	/home/adam/binutils-2.28.1/bfd/cpu-visium.c \
	/home/adam/binutils-2.28.1/bfd/cpu-w65.c \
	/home/adam/binutils-2.28.1/bfd/cpu-we32k.c \
	/home/adam/binutils-2.28.1/bfd/cpu-xc16x.c \
	/home/adam/binutils-2.28.1/bfd/cpu-xgate.c \
	/home/adam/binutils-2.28.1/bfd/cpu-xstormy16.c \
	/home/adam/binutils-2.28.1/bfd/cpu-xtensa.c \
	/home/adam/binutils-2.28.1/bfd/cpu-z80.c \
	/home/adam/binutils-2.28.1/bfd/cpu-z8k.c \
	/home/adam/binutils-2.28.1/bfd/demo64.c \
	/home/adam/binutils-2.28.1/bfd/dwarf1.c \
	/home/adam/binutils-2.28.1/bfd/dwarf2.c \
	/home/adam/binutils-2.28.1/bfd/ecoff.c \
	/home/adam/binutils-2.28.1/bfd/ecofflink.c \
	/home/adam/binutils-2.28.1/bfd/ecoffswap.h \
	/home/adam/binutils-2.28.1/bfd/elf-attrs.c \
	/home/adam/binutils-2.28.1/bfd/elf-bfd.h \
	/home/adam/binutils-2.28.1/bfd/elf-eh-frame.c \
	/home/adam/binutils-2.28.1/bfd/elf-hppa.h \
	/home/adam/binutils-2.28.1/bfd/elf-ifunc.c \
	/home/adam/binutils-2.28.1/bfd/elf-linux-core.h \
	/home/adam/binutils-2.28.1/bfd/elf-m10200.c \
	/home/adam/binutils-2.28.1/bfd/elf-m10300.c \
	/home/adam/binutils-2.28.1/bfd/elf-nacl.c \
	/home/adam/binutils-2.28.1/bfd/elf-strtab.c \
	/home/adam/binutils-2.28.1/bfd/elf-vxworks.c \
	/home/adam/binutils-2.28.1/bfd/elf.c \
	/home/adam/binutils-2.28.1/bfd/elf32-am33lin.c \
	/home/adam/binutils-2.28.1/bfd/elf32-arc.c \
	/home/adam/binutils-2.28.1/bfd/elf32-arm.c \
	/home/adam/binutils-2.28.1/bfd/elf32-avr.c \
	/home/adam/binutils-2.28.1/bfd/elf32-bfin.c \
	/home/adam/binutils-2.28.1/bfd/elf32-cr16.c \
	/home/adam/binutils-2.28.1/bfd/elf32-cr16c.c \
	/home/adam/binutils-2.28.1/bfd/elf32-cris.c \
	/home/adam/binutils-2.28.1/bfd/elf32-crx.c \
	/home/adam/binutils-2.28.1/bfd/elf32-d10v.c \
	/home/adam/binutils-2.28.1/bfd/elf32-d30v.c \
	/home/adam/binutils-2.28.1/bfd/elf32-dlx.c \
	/home/adam/binutils-2.28.1/bfd/elf32-epiphany.c \
	/home/adam/binutils-2.28.1/bfd/elf32-fr30.c \
	/home/adam/binutils-2.28.1/bfd/elf32-frv.c \
	/home/adam/binutils-2.28.1/bfd/elf32-ft32.c \
	/home/adam/binutils-2.28.1/bfd/elf32-gen.c \
	/home/adam/binutils-2.28.1/bfd/elf32-h8300.c \
	/home/adam/binutils-2.28.1/bfd/elf32-hppa.c \
	/home/adam/binutils-2.28.1/bfd/elf32-hppa.h \
	/home/adam/binutils-2.28.1/bfd/elf32-i370.c \
	/home/adam/binutils-2.28.1/bfd/elf32-i386.c \
	/home/adam/binutils-2.28.1/bfd/elf32-i860.c \
	/home/adam/binutils-2.28.1/bfd/elf32-i960.c \
	/home/adam/binutils-2.28.1/bfd/elf32-ip2k.c \
	/home/adam/binutils-2.28.1/bfd/elf32-iq2000.c \
	/home/adam/binutils-2.28.1/bfd/elf32-lm32.c \
	/home/adam/binutils-2.28.1/bfd/elf32-m32c.c \
	/home/adam/binutils-2.28.1/bfd/elf32-m32r.c \
	/home/adam/binutils-2.28.1/bfd/elf32-m68hc11.c \
	/home/adam/binutils-2.28.1/bfd/elf32-m68hc12.c \
	/home/adam/binutils-2.28.1/bfd/elf32-m68hc1x.c \
	/home/adam/binutils-2.28.1/bfd/elf32-m68k.c \
	/home/adam/binutils-2.28.1/bfd/elf32-m88k.c \
	/home/adam/binutils-2.28.1/bfd/elf32-mcore.c \
	/home/adam/binutils-2.28.1/bfd/elf32-mep.c \
	/home/adam/binutils-2.28.1/bfd/elf32-metag.c \
	/home/adam/binutils-2.28.1/bfd/elf32-microblaze.c \
	/home/adam/binutils-2.28.1/bfd/elf32-mips.c \
	/home/adam/binutils-2.28.1/bfd/elf32-moxie.c \
	/home/adam/binutils-2.28.1/bfd/elf32-msp430.c \
	/home/adam/binutils-2.28.1/bfd/elf32-mt.c \
	/home/adam/binutils-2.28.1/bfd/elf32-nds32.c \
	/home/adam/binutils-2.28.1/bfd/elf32-nios2.c \
	/home/adam/binutils-2.28.1/bfd/elf32-or1k.c \
	/home/adam/binutils-2.28.1/bfd/elf32-pj.c \
	/home/adam/binutils-2.28.1/bfd/elf32-ppc.c \
	/home/adam/binutils-2.28.1/bfd/elf32-rl78.c \
	/home/adam/binutils-2.28.1/bfd/elf32-rx.c \
	/home/adam/binutils-2.28.1/bfd/elf32-s390.c \
	/home/adam/binutils-2.28.1/bfd/elf32-score.c \
	/home/adam/binutils-2.28.1/bfd/elf32-score7.c \
	/home/adam/binutils-2.28.1/bfd/elf32-sh-symbian.c \
	/home/adam/binutils-2.28.1/bfd/elf32-sh.c \
	/home/adam/binutils-2.28.1/bfd/elf32-sh64-com.c \
	/home/adam/binutils-2.28.1/bfd/elf32-sh64.c \
	/home/adam/binutils-2.28.1/bfd/elf32-sparc.c \
	/home/adam/binutils-2.28.1/bfd/elf32-spu.c \
	/home/adam/binutils-2.28.1/bfd/elf32-tic6x.c \
	/home/adam/binutils-2.28.1/bfd/elf32-tilegx.c \
	/home/adam/binutils-2.28.1/bfd/elf32-tilepro.c \
	/home/adam/binutils-2.28.1/bfd/elf32-v850.c \
	/home/adam/binutils-2.28.1/bfd/elf32-vax.c \
	/home/adam/binutils-2.28.1/bfd/elf32-visium.c \
	/home/adam/binutils-2.28.1/bfd/elf32-xc16x.c \
	/home/adam/binutils-2.28.1/bfd/elf32-xgate.c \
	/home/adam/binutils-2.28.1/bfd/elf32-xstormy16.c \
	/home/adam/binutils-2.28.1/bfd/elf32-xtensa.c \
	/home/adam/binutils-2.28.1/bfd/elf32.c \
	/home/adam/binutils-2.28.1/bfd/elf64-alpha.c \
	/home/adam/binutils-2.28.1/bfd/elf64-gen.c \
	/home/adam/binutils-2.28.1/bfd/elf64-hppa.c \
	/home/adam/binutils-2.28.1/bfd/elf64-hppa.h \
	/home/adam/binutils-2.28.1/bfd/elf64-ia64-vms.c \
	/home/adam/binutils-2.28.1/bfd/elf64-mips.c \
	/home/adam/binutils-2.28.1/bfd/elf64-mmix.c \
	/home/adam/binutils-2.28.1/bfd/elf64-ppc.c \
	/home/adam/binutils-2.28.1/bfd/elf64-s390.c \
	/home/adam/binutils-2.28.1/bfd/elf64-sh64.c \
	/home/adam/binutils-2.28.1/bfd/elf64-sparc.c \
	/home/adam/binutils-2.28.1/bfd/elf64-tilegx.c \
	/home/adam/binutils-2.28.1/bfd/elf64-x86-64.c \
	/home/adam/binutils-2.28.1/bfd/elf64.c \
	/home/adam/binutils-2.28.1/bfd/elfcode.h \
	/home/adam/binutils-2.28.1/bfd/elfcore.h \
	/home/adam/binutils-2.28.1/bfd/elflink.c \
	/home/adam/binutils-2.28.1/bfd/elfn32-mips.c \
	/home/adam/binutils-2.28.1/bfd/elfxx-aarch64.c \
	/home/adam/binutils-2.28.1/bfd/elfxx-ia64.c \
	/home/adam/binutils-2.28.1/bfd/elfxx-mips.c \
	/home/adam/binutils-2.28.1/bfd/elfxx-riscv.c \
	/home/adam/binutils-2.28.1/bfd/elfxx-sparc.c \
	/home/adam/binutils-2.28.1/bfd/elfxx-tilegx.c \
	/home/adam/binutils-2.28.1/bfd/epoc-pe-arm.c \
	/home/adam/binutils-2.28.1/bfd/epoc-pei-arm.c \
	/home/adam/binutils-2.28.1/bfd/format.c \
	/home/adam/binutils-2.28.1/bfd/freebsd.h \
	/home/adam/binutils-2.28.1/bfd/genlink.h \
	/home/adam/binutils-2.28.1/bfd/go32stub.h \
	/home/adam/binutils-2.28.1/bfd/hash.c \
	/home/adam/binutils-2.28.1/bfd/hp300bsd.c \
	/home/adam/binutils-2.28.1/bfd/hp300hpux.c \
	/home/adam/binutils-2.28.1/bfd/hpux-core.c \
	/home/adam/binutils-2.28.1/bfd/i386aout.c \
	/home/adam/binutils-2.28.1/bfd/i386bsd.c \
	/home/adam/binutils-2.28.1/bfd/i386dynix.c \
	/home/adam/binutils-2.28.1/bfd/i386freebsd.c \
	/home/adam/binutils-2.28.1/bfd/i386linux.c \
	/home/adam/binutils-2.28.1/bfd/i386lynx.c \
	/home/adam/binutils-2.28.1/bfd/i386mach3.c \
	/home/adam/binutils-2.28.1/bfd/i386msdos.c \
	/home/adam/binutils-2.28.1/bfd/i386netbsd.c \
	/home/adam/binutils-2.28.1/bfd/i386os9k.c \
	/home/adam/binutils-2.28.1/bfd/ieee.c \
	/home/adam/binutils-2.28.1/bfd/ihex.c \
	/home/adam/binutils-2.28.1/bfd/init.c \
	/home/adam/binutils-2.28.1/bfd/irix-core.c \
	/home/adam/binutils-2.28.1/bfd/libaout.h \
	/home/adam/binutils-2.28.1/bfd/libbfd.c \
	/home/adam/binutils-2.28.1/bfd/libbfd.h \
	/home/adam/binutils-2.28.1/bfd/libcoff.h \
	/home/adam/binutils-2.28.1/bfd/libecoff.h \
	/home/adam/binutils-2.28.1/bfd/libhppa.h \
	/home/adam/binutils-2.28.1/bfd/libieee.h \
	/home/adam/binutils-2.28.1/bfd/libnlm.h \
	/home/adam/binutils-2.28.1/bfd/liboasys.h \
	/home/adam/binutils-2.28.1/bfd/libpei.h \
	/home/adam/binutils-2.28.1/bfd/libxcoff.h \
	/home/adam/binutils-2.28.1/bfd/linker.c \
	/home/adam/binutils-2.28.1/bfd/lynx-core.c \
	/home/adam/binutils-2.28.1/bfd/m68k4knetbsd.c \
	/home/adam/binutils-2.28.1/bfd/m68klinux.c \
	/home/adam/binutils-2.28.1/bfd/m68knetbsd.c \
	/home/adam/binutils-2.28.1/bfd/m88kmach3.c \
	/home/adam/binutils-2.28.1/bfd/m88kopenbsd.c \
	/home/adam/binutils-2.28.1/bfd/mach-o-aarch64.c \
	/home/adam/binutils-2.28.1/bfd/mach-o-arm.c \
	/home/adam/binutils-2.28.1/bfd/mach-o-i386.c \
	/home/adam/binutils-2.28.1/bfd/mach-o-x86-64.c \
	/home/adam/binutils-2.28.1/bfd/mach-o.c \
	/home/adam/binutils-2.28.1/bfd/mach-o.h \
	/home/adam/binutils-2.28.1/bfd/merge.c \
	/home/adam/binutils-2.28.1/bfd/mipsbsd.c \
	/home/adam/binutils-2.28.1/bfd/mmo.c \
	/home/adam/binutils-2.28.1/bfd/netbsd-core.c \
	/home/adam/binutils-2.28.1/bfd/netbsd.h \
	/home/adam/binutils-2.28.1/bfd/newsos3.c \
	/home/adam/binutils-2.28.1/bfd/nlm-target.h \
	/home/adam/binutils-2.28.1/bfd/nlm.c \
	/home/adam/binutils-2.28.1/bfd/nlm32-alpha.c \
	/home/adam/binutils-2.28.1/bfd/nlm32-i386.c \
	/home/adam/binutils-2.28.1/bfd/nlm32-ppc.c \
	/home/adam/binutils-2.28.1/bfd/nlm32-sparc.c \
	/home/adam/binutils-2.28.1/bfd/nlm32.c \
	/home/adam/binutils-2.28.1/bfd/nlm64.c \
	/home/adam/binutils-2.28.1/bfd/nlmcode.h \
	/home/adam/binutils-2.28.1/bfd/nlmswap.h \
	/home/adam/binutils-2.28.1/bfd/ns32k.h \
	/home/adam/binutils-2.28.1/bfd/ns32knetbsd.c \
	/home/adam/binutils-2.28.1/bfd/oasys.c \
	/home/adam/binutils-2.28.1/bfd/opncls.c \
	/home/adam/binutils-2.28.1/bfd/osf-core.c \
	/home/adam/binutils-2.28.1/bfd/pc532-mach.c \
	/home/adam/binutils-2.28.1/bfd/pdp11.c \
	/home/adam/binutils-2.28.1/bfd/pe-arm-wince.c \
	/home/adam/binutils-2.28.1/bfd/pe-arm.c \
	/home/adam/binutils-2.28.1/bfd/pe-i386.c \
	/home/adam/binutils-2.28.1/bfd/pe-mcore.c \
	/home/adam/binutils-2.28.1/bfd/pe-mips.c \
	/home/adam/binutils-2.28.1/bfd/pe-ppc.c \
	/home/adam/binutils-2.28.1/bfd/pe-sh.c \
	/home/adam/binutils-2.28.1/bfd/pe-x86_64.c \
	/home/adam/binutils-2.28.1/bfd/pef-traceback.h \
	/home/adam/binutils-2.28.1/bfd/pef.c \
	/home/adam/binutils-2.28.1/bfd/pef.h \
	/home/adam/binutils-2.28.1/bfd/pei-arm-wince.c \
	/home/adam/binutils-2.28.1/bfd/pei-arm.c \
	/home/adam/binutils-2.28.1/bfd/pei-i386.c \
	/home/adam/binutils-2.28.1/bfd/pei-ia64.c \
	/home/adam/binutils-2.28.1/bfd/pei-mcore.c \
	/home/adam/binutils-2.28.1/bfd/pei-mips.c \
	/home/adam/binutils-2.28.1/bfd/pei-ppc.c \
	/home/adam/binutils-2.28.1/bfd/pei-sh.c \
	/home/adam/binutils-2.28.1/bfd/pei-x86_64.c \
	/home/adam/binutils-2.28.1/bfd/peicode.h \
	/home/adam/binutils-2.28.1/bfd/plugin.c \
	/home/adam/binutils-2.28.1/bfd/ppcboot.c \
	/home/adam/binutils-2.28.1/bfd/reloc.c \
	/home/adam/binutils-2.28.1/bfd/reloc16.c \
	/home/adam/binutils-2.28.1/bfd/riscix.c \
	/home/adam/binutils-2.28.1/bfd/rs6000-core.c \
	/home/adam/binutils-2.28.1/bfd/sco5-core.c \
	/home/adam/binutils-2.28.1/bfd/section.c \
	/home/adam/binutils-2.28.1/bfd/simple.c \
	/home/adam/binutils-2.28.1/bfd/som.c \
	/home/adam/binutils-2.28.1/bfd/som.h \
	/home/adam/binutils-2.28.1/bfd/sparclinux.c \
	/home/adam/binutils-2.28.1/bfd/sparclynx.c \
	/home/adam/binutils-2.28.1/bfd/sparcnetbsd.c \
	/home/adam/binutils-2.28.1/bfd/srec.c \
	/home/adam/binutils-2.28.1/bfd/stab-syms.c \
	/home/adam/binutils-2.28.1/bfd/stabs.c \
	/home/adam/binutils-2.28.1/bfd/sunos.c \
	/home/adam/binutils-2.28.1/bfd/syms.c \
	/home/adam/binutils-2.28.1/bfd/targets.c \
	/home/adam/binutils-2.28.1/bfd/tekhex.c \
	/home/adam/binutils-2.28.1/bfd/trad-core.c \
	/home/adam/binutils-2.28.1/bfd/vax1knetbsd.c \
	/home/adam/binutils-2.28.1/bfd/vaxbsd.c \
	/home/adam/binutils-2.28.1/bfd/vaxnetbsd.c \
	/home/adam/binutils-2.28.1/bfd/verilog.c \
	/home/adam/binutils-2.28.1/bfd/versados.c \
	/home/adam/binutils-2.28.1/bfd/version.h \
	/home/adam/binutils-2.28.1/bfd/vms-alpha.c \
	/home/adam/binutils-2.28.1/bfd/vms-lib.c \
	/home/adam/binutils-2.28.1/bfd/vms-misc.c \
	/home/adam/binutils-2.28.1/bfd/vms.h \
	/home/adam/binutils-2.28.1/bfd/xcofflink.c \
	/home/adam/binutils-2.28.1/bfd/xsym.c \
	/home/adam/binutils-2.28.1/bfd/xsym.h \
	/home/adam/binutils-2.28.1/bfd/xtensa-isa.c \
	/home/adam/binutils-2.28.1/bfd/xtensa-modules.c 

# Note - the following line gets processed by bfd/configure and amended
# to contain the full list of build dir POTFILES.
BLD-POTFILES = \
	../bfd_stdint.h \
	../bfdver.h \
	../elf32-aarch64.c \
	../elf32-ia64.c \
	../elf32-riscv.c \
	../elf32-target.h \
	../elf64-aarch64.c \
	../elf64-ia64.c \
	../elf64-riscv.c \
	../elf64-target.h \
	../peigen.c \
	../pepigen.c \
	../pex64igen.c \
	../targmatch.h 

CATALOGS = 
CATOBJEXT = 
INSTOBJEXT = 

.SUFFIXES:
.SUFFIXES: .c .o .po .pox .gmo .mo .msg .cat

.c.o:
	$(COMPILE) $<

.po.pox:
	$(MAKE) $(PACKAGE).pot
	$(MSGMERGE) $< $(srcdir)/$(PACKAGE).pot -o $*.pox

.po.mo:
	$(MSGFMT) -o $@ $<

.po.gmo:
	file=`echo $* | sed 's,.*/,,'`.gmo \
	  && rm -f $$file && $(GMSGFMT) -o $$file $<

.po.cat:
	sed -f ../intl/po2msg.sed < $< > $*.msg \
	  && rm -f $@ && $(GENCAT) $@ $*.msg


all: all-no

all-yes: $(CATALOGS) # $(PACKAGE).pot
all-no:

$(srcdir)/$(PACKAGE).pot: $(SRC-POTFILES) $(BLD-POTFILES)
	$(XGETTEXT) --default-domain=$(PACKAGE) \
	  --directory=$(top_srcdir) \
	  --add-comments --keyword=_ --keyword=N_ \
	  --msgid-bugs-address=bug-binutils@gnu.org \
	  --files-from=$(srcdir)/SRC-POTFILES.in 
	$(XGETTEXT) --default-domain=$(PACKAGE) \
	  --directory=.. \
	  --directory=. \
	  --add-comments --keyword=_ --keyword=N_ \
	  --join-existing \
	  --msgid-bugs-address=bug-binutils@gnu.org \
	  --files-from=$(srcdir)/BLD-POTFILES.in
	rm -f $(srcdir)/$(PACKAGE).pot
	mv $(PACKAGE).po $(srcdir)/$(PACKAGE).pot

$(srcdir)/cat-id-tbl.c: stamp-cat-id; @:
$(srcdir)/stamp-cat-id: $(PACKAGE).pot
	rm -f cat-id-tbl.tmp
	sed -f ../intl/po2tbl.sed $(srcdir)/$(PACKAGE).pot \
		| sed -e "s/@PACKAGE NAME@/$(PACKAGE)/" > cat-id-tbl.tmp
	if cmp -s cat-id-tbl.tmp $(srcdir)/cat-id-tbl.c; then \
	  rm cat-id-tbl.tmp; \
	else \
	  echo cat-id-tbl.c changed; \
	  rm -f $(srcdir)/cat-id-tbl.c; \
	  mv cat-id-tbl.tmp $(srcdir)/cat-id-tbl.c; \
	fi
	cd $(srcdir) && rm -f stamp-cat-id && echo timestamp > stamp-cat-id


install: install-exec install-data
install-exec:
install-info:
install-html:
install-pdf:
install-data: install-data-no
install-data-no: all
install-data-yes: all
	if test -r $(MKINSTALLDIRS); then \
	  $(MKINSTALLDIRS) $(DESTDIR)$(datadir); \
	else \
	  $(top_srcdir)/mkinstalldirs $(DESTDIR)$(datadir); \
	fi
	@catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  case "$$cat" in \
	    *.gmo) destdir=$(gnulocaledir);; \
	    *)     destdir=$(localedir);; \
	  esac; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  dir=$(DESTDIR)$$destdir/$$lang/LC_MESSAGES; \
	  if test -r $(MKINSTALLDIRS); then \
	    $(MKINSTALLDIRS) $$dir; \
	  else \
	    $(top_srcdir)/mkinstalldirs $$dir; \
	  fi; \
	  if test -r $$cat; then \
	    $(INSTALL_DATA) $$cat $$dir/$(PACKAGE)$(INSTOBJEXT); \
	    echo "installing $$cat as $$dir/$(PACKAGE)$(INSTOBJEXT)"; \
	  else \
	    $(INSTALL_DATA) $(srcdir)/$$cat $$dir/$(PACKAGE)$(INSTOBJEXT); \
	    echo "installing $(srcdir)/$$cat as" \
		 "$$dir/$(PACKAGE)$(INSTOBJEXT)"; \
	  fi; \
	  if test -r $$cat.m; then \
	    $(INSTALL_DATA) $$cat.m $$dir/$(PACKAGE)$(INSTOBJEXT).m; \
	    echo "installing $$cat.m as $$dir/$(PACKAGE)$(INSTOBJEXT).m"; \
	  else \
	    if test -r $(srcdir)/$$cat.m ; then \
	      $(INSTALL_DATA) $(srcdir)/$$cat.m \
		$$dir/$(PACKAGE)$(INSTOBJEXT).m; \
	      echo "installing $(srcdir)/$$cat as" \
		   "$$dir/$(PACKAGE)$(INSTOBJEXT).m"; \
	    else \
	      true; \
	    fi; \
	  fi; \
	done
	if test "$(PACKAGE)" = "gettext"; then \
	  if test -r $(MKINSTALLDIRS); then \
	    $(MKINSTALLDIRS) $(DESTDIR)$(gettextsrcdir); \
	  else \
	    $(top_srcdir)/mkinstalldirs $(DESTDIR)$(gettextsrcdir); \
	  fi; \
	  $(INSTALL_DATA) $(srcdir)/Makefile.in.in \
			  $(DESTDIR)$(gettextsrcdir)/Makefile.in.in; \
	else \
	  : ; \
	fi

# Define this as empty until I found a useful application.
installcheck:

uninstall:
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT).m; \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT).m; \
	done
	rm -f $(DESTDIR)$(gettextsrcdir)/po-Makefile.in.in

check: all

cat-id-tbl.o: ../intl/libgettext.h

html dvi pdf ps info tags TAGS ID:

mostlyclean:
	rm -f core core.* *.pox $(PACKAGE).po *.old.po cat-id-tbl.tmp
	rm -fr *.o

clean: mostlyclean

distclean: clean
	rm -f Makefile Makefile.in *.mo *.msg *.cat *.cat.m
	rm -f SRC-POTFILES BLD-POTFILES 

maintainer-clean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f $(GMOFILES) SRC-POTFILES.in BLD-POTFILES.in

distdir = ../$(PACKAGE)-$(VERSION)/$(subdir)
dist distdir: update-po $(DISTFILES)
	dists="$(DISTFILES)"; \
	for file in $$dists; do \
	  ln $(srcdir)/$$file $(distdir) 2> /dev/null \
	    || cp -p $(srcdir)/$$file $(distdir); \
	done

update-po: Makefile
	$(MAKE) $(PACKAGE).pot
	PATH=`pwd`/../src:$$PATH; \
	cd $(srcdir); \
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  mv $$lang.po $$lang.old.po; \
	  echo "$$lang:"; \
	  if $(MSGMERGE) $$lang.old.po $(PACKAGE).pot -o $$lang.po; then \
	    rm -f $$lang.old.po; \
	  else \
	    echo "msgmerge for $$cat failed!"; \
	    rm -f $$lang.po; \
	    mv $$lang.old.po $$lang.po; \
	  fi; \
	done

SRC-POTFILES: SRC-POTFILES.in
	( if test 'x$(srcdir)' != 'x.'; then \
	    posrcprefix='$(top_srcdir)/'; \
	  else \
	    posrcprefix="../"; \
	  fi; \
	  rm -f $@-t $@ \
	    && (sed -e '/^#/d' \
	            -e '/^[ 	]*$$/d' \
		    -e "s@.*@	$$posrcprefix& \\\\@" < $(srcdir)/$@.in \
		| sed -e '$$s/\\$$//') > $@-t \
	    && chmod a-w $@-t \
	    && mv $@-t $@ )

BLD-POTFILES: BLD-POTFILES.in
	  ( rm -f $@-t $@ \
	    && (sed -e '/^#/d' \
	            -e '/^[ 	]*$$/d' \
		    -e "s@.*@	../& \\\\@" < $(srcdir)/$@.in \
		| sed -e '$$s/\\$$//') > $@-t \
	    && chmod a-w $@-t \
	    && mv $@-t $@ )

SRC-POTFILES.in: # ../Makefile
	cd .. && $(MAKE) po/SRC-POTFILES.in

BLD-POTFILES.in: # ../Makefile
	cd .. && $(MAKE) po/BLD-POTFILES.in

# Note - The presence of SRC-POTFILES and BLD-POTFILES as dependencies
# here breaks the implementation of the 'distclean' rule for maintainers.
# This is because if 'make distclean' is run in the BFD directory, the
# Makefile there will be deleted before 'distclean' is made here, and so
# the dependency SRC-POTFILES -> SRC-POTFILES.in -> ../Makefile cannot
# be satisfied.
#
# The SRC-POTFILES and BLD-POTFILES dependencies cannot be removed,
# however since it is necessary that these files be built during
# *configure* time, so that configure can insert them into the
# po/Makefile that it is creating, so that the Makefile will have
# the correct dependencies.
Makefile: Make-in ../config.status SRC-POTFILES BLD-POTFILES
	cd .. \
	  && CONFIG_FILES=$(subdir)/Makefile.in:$(subdir)/Make-in \
	     CONFIG_HEADERS= $(SHELL) ./config.status

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
