// IPPROTO_xxx
#define IP		0 /* Dummy protocol for TCP.  */
#define ICMP		1 /* Internet Control Message Protocol.  */
#define IGMP		2 /* Internet Group Management Protocol. */
#define IPIP		4 /* IPIP tunnels (older KA9Q tunnels use 94).  */
#define TCP		6 /* Transmission Control Protocol.  */
#define EGP		8 /* Exterior Gateway Protocol.  */
#define PUP		12 /* PUP protocol.  */
#define UDP		17 /* User Datagram Protocol.  */
#define IDP		22 /* XNS IDP protocol.  */
#define TP		29 /* SO Transport Protocol Class 4.  */
#define DCCP		33 /* Datagram Congestion Control Protocol.  */
#define IPV6		41 /* IPv6 header.  */
#define RSVP		46 /* Reservation Protocol.  */
#define GRE		47 /* General Routing Encapsulation.  */
#define ESP		50 /* encapsulating security payload.  */
#define AH		51 /* authentication header.  */
#define MTP		92 /* Multicast Transport Protocol.  */
#define BEETPH		94 /* IP option pseudo header for BEET.  */
#define ENCAP		98 /* Encapsulation Header.  */
#define PIM		103 /* Protocol Independent Multicast.  */
#define COMP		108 /* Compression Header Protocol.  */
#define SCTP		132 /* Stream Control Transmission Protocol.  */
#define UDPLITE		136 /* UDP-Lite protocol.  */
#define MPLS		137 /* MPLS in IP.  */
#define ETHERNET	143 /* Ethernet-within-IPv6 Encapsulation.  */
#define RAW		255 /* Raw IP packets.  */
#define MPTCP		262 /* Multipath TCP connection.  */


#define	IP		0		/* dummy for IP */
#define HOPOPTS		0		/* IPv6 hop-by-hop options */
#define	ICMP		1		/* control message protocol */
#define	IGMP		2		/* group mgmt protocol */
#define IPV4		4
#define	TCP		6		/* tcp */
#define	EGP		8		/* exterior gateway protocol */
#define PIGP		9
#define	UDP		17		/* user datagram protocol */
#define	DCCP		33		/* datagram congestion control protocol */
#define IPV6		41
#define ROUTING		43		/* IPv6 routing header */
#define FRAGMENT	44		/* IPv6 fragmentation header */
#define RSVP		46 		/* resource reservation */
#define	GRE		47		/* General Routing Encap. */
#define	ESP		50		/* SIPP Encap Sec. Payload */
#define	AH		51		/* SIPP Auth Header */
#define MOBILE		55
#define ICMPV6		58		/* ICMPv6 */
#define NONE		59		/* IPv6 no next header */
#define DSTOPTS		60		/* IPv6 destination options */

#define MOBILITY_OLD	62
#define	ND		77		/* Sun net disk proto (temp.) */
#define	EIGRP		88		/* Cisco/GXS IGRP */
#define OSPF		89
#define PIM		103
#define IPCOMP		108
#define VRRP		112
#define CARP		112
#define PGM             113
#define SCTP		132
#define MOBILITY	135


#define IP 0
#define HOPOPTS 0
#define ICMP 1
#define IGMP 2
#define GGP 3
#define IPV4 4
#define ST 5
#define TCP 6
#define CBT 7
#define EGP 8
#define IGP 9
#define PUP 12
#define UDP 17
#define IDP 22
#define RDP 27
#define IPV6 41
#define ROUTING 43
#define FRAGMENT 44
#define ESP 50
#define AH 51
#define ICMPV6 58
#define NONE 59
#define DSTOPTS 60
#define ND 77
#define ICLFXBM 78
#define PIM 103
#define PGM 113
#define L2TP 115
#define SCTP 132

#define RAW 255
#define MAX 256

