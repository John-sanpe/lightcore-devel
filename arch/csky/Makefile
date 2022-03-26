# SPDX-License-Identifier: GPL-2.0-or-later
obj-y += boot/ lib/ mm/
obj-y += entry.o setup.o proc.o irq.o
obj-y += backtrace.o traps.o

$(obj)/setup.o: $(obj)/asm-offset.h
$(obj)/entry32.o: $(obj)/asm-offset.h
$(obj)/lib: $(obj)/asm-offset.h

extra-y += asm-offset.h kernel.lds
clean-y += include/generated/
