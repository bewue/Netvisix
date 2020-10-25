#ifndef TINS_CONFIG_H
#define TINS_CONFIG_H

/* Define if the compiler supports basic C++11 syntax */
#define TINS_HAVE_CXX11

/* Have IEEE 802.11 support */
#define TINS_HAVE_DOT11

/* Have WPA2 decryption library */
/* #undef TINS_HAVE_WPA2_DECRYPTION */

/* Use pcap_sendpacket to send l2 packets */
/* #undef TINS_HAVE_PACKET_SENDER_PCAP_SENDPACKET */

/* Have TCPIP classes */
#define TINS_HAVE_TCPIP

/* Have TCP ACK tracking */
/* #undef TINS_HAVE_ACK_TRACKER */

/* Have TCP stream custom data */
/* #undef TINS_HAVE_TCP_STREAM_CUSTOM_DATA */

/* Have GCC builtin swap */
#define TINS_HAVE_GCC_BUILTIN_SWAP

/* Have WPA2Decrypter callbacks */
/* #undef TINS_HAVE_WPA2_CALLBACKS */

/* Have libpcap */
#define TINS_HAVE_PCAP

#endif // TINS_CONFIG_H
