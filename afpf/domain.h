#define PF_LOCAL	1	/* Local to host (pipes and file-domain).  */
#define PF_INET		2	/* IP protocol family.  */
#define PF_AX25		3	/* Amateur Radio AX.25.  */
#define PF_IPX		4	/* Novell Internet Protocol.  */
#define PF_APPLETALK	5	/* Appletalk DDP.  */
#define PF_NETROM	6	/* Amateur radio NetROM.  */
#define PF_BRIDGE	7	/* Multiprotocol bridge.  */
#define PF_ATMPVC	8	/* ATM PVCs.  */
#define PF_X25		9	/* Reserved for X.25 project.  */
#define PF_INET6	10	/* IP version 6.  */
#define PF_ROSE		11	/* Amateur Radio X.25 PLP.  */
#define PF_DECnet	12	/* Reserved for DECnet project.  */
#define PF_NETBEUI	13	/* Reserved for 802.2LLC project.  */
#define PF_SECURITY	14	/* Security callback pseudo AF.  */
#define PF_KEY		15	/* PF_KEY key management API.  */
#define PF_PACKET	17	/* Packet family.  */
#define PF_ASH		18	/* Ash.  */
#define PF_ECONET	19	/* Acorn Econet.  */
#define PF_ATMSVC	20	/* ATM SVCs.  */
#define PF_RDS		21	/* RDS sockets.  */
#define PF_SNA		22	/* IBM SNA */
#define PF_IRDA		23	/* IRDA sockets.  */
#define PF_PPPOX	24	/* PPPoX sockets.  */
#define PF_WANPIPE	25	/* Wanpipe API sockets.  */
#define PF_LLC		26	/* Linux LLC.  */
#define PF_IB		27	/* Native InfiniBand address.  */
#define PF_MPLS		28	/* MPLS.  */
#define PF_CAN		29	/* Controller Area Network.  */
#define PF_TIPC		30	/* TIPC sockets.  */
#define PF_BLUETOOTH	31	/* Bluetooth sockets.  */
#define PF_IUCV		32	/* IUCV sockets.  */
#define PF_RXRPC	33	/* RxRPC sockets.  */
#define PF_ISDN		34	/* mISDN sockets.  */
#define PF_PHONET	35	/* Phonet sockets.  */
#define PF_IEEE802154	36	/* IEEE 802.15.4 sockets.  */
#define PF_CAIF		37	/* CAIF sockets.  */
#define PF_ALG		38	/* Algorithm sockets.  */
#define PF_NFC		39	/* NFC sockets.  */
#define PF_VSOCK	40	/* vSockets.  */
#define PF_KCM		41	/* Kernel Connection Multiplexor.  */
#define PF_QIPCRTR	42	/* Qualcomm IPC Router.  */
#define PF_SMC		43	/* SMC sockets.  */
#define PF_XDP		44	/* XDP sockets.  */

#define	AF_LOCAL	AF_UNIX		/* local to host (pipes, portals) */
#define	AF_INET		2		/* internetwork: UDP, TCP, etc. */
#define	AF_IMPLINK	3		/* arpanet imp addresses */
#define	AF_PUP		4		/* pup protocols: e.g. BSP */
#define	AF_CHAOS	5		/* mit CHAOS protocols */
#define	AF_NETBIOS	6		/* SMB protocols */
#define	AF_ISO		7		/* ISO protocols */
#define	AF_OSI		AF_ISO
#define	AF_ECMA		8		/* European computer manufacturers */
#define	AF_DATAKIT	9		/* datakit protocols */
#define	AF_CCITT	10		/* CCITT protocols, X.25 etc */
#define	AF_SNA		11		/* IBM SNA */
#define AF_DECnet	12		/* DECnet */
#define AF_DLI		13		/* DEC Direct data link interface */
#define AF_LAT		14		/* LAT */
#define	AF_HYLINK	15		/* NSC Hyperchannel */
#define	AF_APPLETALK	16		/* Apple Talk */
#define	AF_ROUTE	17		/* Internal Routing Protocol */
#define	AF_LINK		18		/* Link layer interface */
#define	AF_pseudo_XTP	19		/* eXpress Transfer Protocol (no AF) */
#define	AF_COIP		20		/* connection-oriented IP, aka ST II */
#define	AF_CNT		21		/* Computer Network Technology */
#define AF_pseudo_RTIP	22		/* Help Identify RTIP packets */
#define	AF_IPX		23		/* Novell Internet Protocol */
#define	AF_SIP		24		/* Simple Internet Protocol */
#define	AF_pseudo_PIP	25		/* Help Identify PIP packets */
#define	AF_ISDN		26		/* Integrated Services Digital Network*/
#define	AF_E164		AF_ISDN		/* CCITT E.164 recommendation */
#define	AF_pseudo_KEY	27		/* Internal key-management function */
#define	AF_INET6	28		/* IPv6 */
#define	AF_NATM		29		/* native ATM access */
#define	AF_ATM		30		/* ATM */
#define AF_pseudo_HDRCMPLT 31		/* Used by BPF to not rewrite headers * in interface output routine */
#define	AF_NETGRAPH	32		/* Netgraph sockets */
#define	AF_SLOW		33		/* 802.3ad slow protocol */
#define	AF_SCLUSTER	34		/* Sitara cluster protocol */
#define	AF_ARP		35		/* Address Resolution Protocol */
#define	AF_BLUETOOTH	36		/* Bluetooth sockets */
#define	AF_IEEE80211	37		/* IEEE 802.11 protocol */
#define	AF_INET_SDP	40		/* OFED Socket Direct Protocol ipv4 */
#define	AF_INET6_SDP	42		/* OFED Socket Direct Protocol ipv6 */
#define	AF_HYPERV	43		/* HyperV sockets */

#define AF_INET 2
#define AF_IMPLINK 3
#define AF_PUP 4
#define AF_CHAOS 5
#define AF_IPX 6
#define AF_ISO 7
#define AF_OSI AF_ISO
#define AF_ECMA 8
#define AF_DATAKIT 9
#define AF_CCITT 10
#define AF_SNA 11
#define AF_DECnet 12
#define AF_DLI 13
#define AF_LAT 14
#define AF_HYLINK 15
#define AF_APPLETALK 16
#define AF_NETBIOS 17
#define AF_VOICEVIEW 18		/* VoiceView address. */
#define AF_FIREFOX 19		/* FireFox address. */
#define AF_BAN 21		/* Banyan address. */
#define AF_ATM 22		/* Native ATM services address. */
#define AF_Cluster 23		/* Address for Microsoft cluster products. */
#define AF_IEEE12844 25		/* IEEE 1284.4 workgroup address. */
#define AF_IRDA 26		/* IrDA address. */
#define AF_NETDES 28		/* Address for Network Designers OSI gateway-enabled protocols. */
#define AF_TCNPROCESS 29
#define AF_TCNMESSAGE 30
#define AF_ICLFXBM 31
#define AF_X25 9999 /* AX.25 packet layer protocol. */
