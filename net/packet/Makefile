#
# Makefile for the packet AF.
#

obj-$(CONFIG_PACKET) += af_packet.o
obj-$(CONFIG_PACKET_DIAG) += af_packet_diag.o
af_packet_diag-y += diag.o

CFLAGS_af_packet.o += $(MPXK_CFLAGS)
CFLAGS_af_packet_diag.o += $(MPXK_CFLAGS)
