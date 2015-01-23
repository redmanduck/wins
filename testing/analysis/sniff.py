#!/usr/bin/env python

from scapy.all import *

def PacketHandler(pkt) :

  if pkt.haslayer(Dot11) :
    if pkt.type == 0 and pkt.subtype == 8 :
      print "AP MAC: %s with SSID: %s " %(pkt.addr2, pkt.info)


sniff(iface="eth1", prn = PacketHandler)

