/* Copyright (C) 1991,92,93,94,95,96,97,98 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef	_NETINET_IN_H
#define	_NETINET_IN_H	1

#include <features.h>
#include <stdint.h>

#include <sys/types.h>
#include <bits/socket.h>


__BEGIN_DECLS

/* Standard well-defined IP protocols.  */
enum
  {
    IPPROTO_IP = 0,	   /* Dummy protocol for TCP.  */
    IPPROTO_HOPOPTS = 0,   /* IPv6 Hop-by-Hop options.  */
    IPPROTO_ICMP = 1,	   /* Internet Control Message Protocol.  */
    IPPROTO_IGMP = 2,	   /* Internet Group Management Protocol. */
    IPPROTO_IPIP = 4,	   /* IPIP tunnels (older KA9Q tunnels use 94).  */
    IPPROTO_TCP = 6,	   /* Transmission Control Protocol.  */
    IPPROTO_EGP = 8,	   /* Exterior Gateway Protocol.  */
    IPPROTO_PUP = 12,	   /* PUP protocol.  */
    IPPROTO_UDP = 17,	   /* User Datagram Protocol.  */
    IPPROTO_IDP = 22,	   /* XNS IDP protocol.  */
    IPPROTO_IPV6 = 41,     /* IPv6 header.  */
    IPPROTO_ROUTING = 43,  /* IPv6 routing header.  */
    IPPROTO_FRAGMENT = 44, /* IPv6 fragmentation header.  */
    IPPROTO_RSVP = 46,     /* RSVP Protocol.  */
    IPPROTO_GRE = 47,      /* Cisco tunneling protocol.  */
    IPPROTO_ESP = 50,      /* encapsulating security payload.  */
    IPPROTO_AH = 51,       /* authentication header.  */
    IPPROTO_ICMPV6 = 58,   /* ICMPv6.  */
    IPPROTO_NONE = 59,     /* IPv6 no next header.  */
    IPPROTO_DSTOPTS = 60,  /* IPv6 destination options.  */
    IPPROTO_PIM = 103,     /* Protocol Independent Multicast.  */
    IPPROTO_RAW = 255,	   /* Raw IP packets.  */
    IPPROTO_MAX
  };

/* Standard well-known ports.  */
enum
  {
    IPPORT_ECHO = 7,		/* Echo service.  */
    IPPORT_DISCARD = 9,		/* Discard transmissions service.  */
    IPPORT_SYSTAT = 11,		/* System status service.  */
    IPPORT_DAYTIME = 13,	/* Time of day service.  */
    IPPORT_NETSTAT = 15,	/* Network status service.  */
    IPPORT_FTP = 21,		/* File Transfer Protocol.  */
    IPPORT_TELNET = 23,		/* Telnet protocol.  */
    IPPORT_SMTP = 25,		/* Simple Mail Transfer Protocol.  */
    IPPORT_TIMESERVER = 37,	/* Timeserver service.  */
    IPPORT_NAMESERVER = 42,	/* Domain Name Service.  */
    IPPORT_WHOIS = 43,		/* Internet Whois service.  */
    IPPORT_MTP = 57,

    IPPORT_TFTP = 69,		/* Trivial File Transfer Protocol.  */
    IPPORT_RJE = 77,
    IPPORT_FINGER = 79,		/* Finger service.  */
    IPPORT_TTYLINK = 87,
    IPPORT_SUPDUP = 95,		/* SUPDUP protocol.  */


    IPPORT_EXECSERVER = 512,	/* execd service.  */
    IPPORT_LOGINSERVER = 513,	/* rlogind service.  */
    IPPORT_CMDSERVER = 514,
    IPPORT_EFSSERVER = 520,

    /* UDP ports.  */
    IPPORT_BIFFUDP = 512,
    IPPORT_WHOSERVER = 513,
    IPPORT_ROUTESERVER = 520,

    /* Ports less than this value are reserved for privileged processes.  */
    IPPORT_RESERVED = 1024,

    /* Ports greater this value are reserved for (non-privileged) servers.  */
    IPPORT_USERRESERVED = 5000
  };


/* Internet address.  */
struct in_addr
  {
    uint32_t s_addr;
  };


/* Definitions of the bits in an Internet address integer.

   On subnets, host and network parts are found according to
   the subnet mask, not these masks.  */

#define	IN_CLASSA(a)		((((uint32_t) (a)) & 0x80000000) == 0)
#define	IN_CLASSA_NET		0xff000000
#define	IN_CLASSA_NSHIFT	24
#define	IN_CLASSA_HOST		(0xffffffff & ~IN_CLASSA_NET)
#define	IN_CLASSA_MAX		128

#define	IN_CLASSB(a)		((((uint32_t) (a)) & 0xc0000000) == 0x80000000)
#define	IN_CLASSB_NET		0xffff0000
#define	IN_CLASSB_NSHIFT	16
#define	IN_CLASSB_HOST		(0xffffffff & ~IN_CLASSB_NET)
#define	IN_CLASSB_MAX		65536

#define	IN_CLASSC(a)		((((uint32_t) (a)) & 0xe0000000) == 0xc0000000)
#define	IN_CLASSC_NET		0xffffff00
#define	IN_CLASSC_NSHIFT	8
#define	IN_CLASSC_HOST		(0xffffffff & ~IN_CLASSC_NET)

#define	IN_CLASSD(a)		((((uint32_t) (a)) & 0xf0000000) == 0xe0000000)
#define	IN_MULTICAST(a)		IN_CLASSD(a)

#define	IN_EXPERIMENTAL(a)	((((uint32_t) (a)) & 0xe0000000) == 0xe0000000)
#define	IN_BADCLASS(a)		((((uint32_t) (a)) & 0xf0000000) == 0xf0000000)

/* Address to accept any incoming messages.  */
#define	INADDR_ANY		((uint32_t) 0x00000000)
/* Address to send to all hosts.  */
#define	INADDR_BROADCAST	((uint32_t) 0xffffffff)
/* Address indicating an error return.  */
#define	INADDR_NONE		((uint32_t) 0xffffffff)

/* Network number for local host loopback.  */
#define	IN_LOOPBACKNET		127
/* Address to loopback in software to local host.  */
#ifndef INADDR_LOOPBACK
# define INADDR_LOOPBACK	((uint32_t) 0x7f000001)	/* Inet 127.0.0.1.  */
#endif

/* Defines for Multicast INADDR.  */
#define INADDR_UNSPEC_GROUP	((uint32_t) 0xe0000000)      /* 224.0.0.0 */
#define INADDR_ALLHOSTS_GROUP	((uint32_t) 0xe0000001)      /* 224.0.0.1 */
#define INADDR_ALLRTRS_GROUP    ((uint32_t) 0xe0000002)      /* 224.0.0.2 */
#define INADDR_MAX_LOCAL_GROUP  ((uint32_t) 0xe00000ff)      /* 224.0.0.255 */


/* IPv6 address */
struct in6_addr
  {
    union
      {
	uint8_t		u6_addr8[16];
	uint16_t	u6_addr16[8];
	uint32_t	u6_addr32[4];
#if (~0UL) > 0xffffffff
	uint64_t	u6_addr64[2];
#endif
      } in6_u;
#define s6_addr			in6_u.u6_addr8
#define s6_addr16		in6_u.u6_addr16
#define s6_addr32		in6_u.u6_addr32
#define s6_addr64		in6_u.u6_addr64
  };

extern const struct in6_addr in6addr_any;        /* :: */
extern const struct in6_addr in6addr_loopback;   /* ::1 */
#define IN6ADDR_ANY_INIT { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }
#define IN6ADDR_LOOPBACK_INIT { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 }

#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46

/* Get the definition of the macro to define the common sockaddr members.  */
#include <bits/sockaddr.h>


/* Structure describing an Internet socket address.  */
struct sockaddr_in
  {
    __SOCKADDR_COMMON (sin_);
    uint16_t sin_port;			/* Port number.  */
    struct in_addr sin_addr;		/* Internet address.  */

    /* Pad to size of `struct sockaddr'.  */
    unsigned char sin_zero[sizeof (struct sockaddr) -
			   __SOCKADDR_COMMON_SIZE -
			   sizeof (uint16_t) -
			   sizeof (struct in_addr)];
  };

/* Ditto, for IPv6.  */
struct sockaddr_in6
  {
    __SOCKADDR_COMMON (sin6_);
    uint16_t sin6_port;		/* Transport layer port # */
    uint32_t sin6_flowinfo;	/* IPv6 flow information */
    struct in6_addr sin6_addr;	/* IPv6 address */
  };

/* IPv6 multicast request.  */
struct ipv6_mreq
  {
    /* IPv6 multicast address of group */
    struct in6_addr ipv6mr_multiaddr;

    /* local interface */
    unsigned int ipv6mr_ifindex;
  };

/* Get system-specific definitions.  */
#include <bits/in.h>

/* Functions to convert between host and network byte order.

   Please note that these functions normally take `unsigned long int' or
   `unsigned short int' values as arguments and also return them.  But
   this was a short-sighted decision since on different systems the types
   may have different representations but the values are always the same.  */

extern uint32_t ntohl (uint32_t __netlong);
extern uint16_t ntohs (uint16_t __netshort);
extern uint32_t htonl (uint32_t __hostlong);
extern uint16_t htons (uint16_t __hostshort);

#include <endian.h>

/* Get machine dependent optimized versions of byte swapping functions.  */
#include <bits/byteswap.h>

#if __BYTE_ORDER == __BIG_ENDIAN && defined __OPTIMIZE__
/* The host byte order is the same as network byte order,
   so these functions are all just identity.  */
# define ntohl(x)	(x)
# define ntohs(x)	(x)
# define htonl(x)	(x)
# define htons(x)	(x)
#else
# if __BYTE_ORDER == __LITTLE_ENDIAN && defined __OPTIMIZE__
#  define ntohl(x)	__bswap_32 (x)
#  define ntohs(x)	__bswap_16 (x)
#  define htonl(x)	__bswap_32 (x)
#  define htons(x)	__bswap_16 (x)
# endif
#endif

#define IN6_IS_ADDR_UNSPECIFIED(a) \
	(((uint32_t *) (a))[0] == 0 && ((uint32_t *) (a))[1] == 0 && \
	 ((uint32_t *) (a))[2] == 0 && ((uint32_t *) (a))[3] == 0)

#define IN6_IS_ADDR_LOOPBACK(a) \
	(((uint32_t *) (a))[0] == 0 && ((uint32_t *) (a))[1] == 0 && \
	 ((uint32_t *) (a))[2] == 0 && ((uint32_t *) (a))[3] == htonl (1))

#define IN6_IS_ADDR_MULTICAST(a) (((u_int8_t *) (a))[0] == 0xff)

#define IN6_IS_ADDR_LINKLOCAL(a) \
	((((uint32_t *) (a))[0] & htonl (0xffc00000)) == htonl (0xfe800000))

#define IN6_IS_ADDR_SITELOCAL(a) \
	((((uint32_t *) (a))[0] & htonl (0xffc00000)) == htonl (0xfec00000))

#define IN6_IS_ADDR_V4MAPPED(a) \
	((((uint32_t *) (a))[0] == 0) && (((uint32_t *) (a))[1] == 0) && \
	 (((uint32_t *) (a))[2] == htonl (0xffff)))

#define IN6_IS_ADDR_V4COMPAT(a) \
	((((uint32_t *) (a))[0] == 0) && (((uint32_t *) (a))[1] == 0) && \
	 (((uint32_t *) (a))[2] == 0) && (ntohl (((uint32_t *) (a))[3]) > 1))

#define IN6_ARE_ADDR_EQUAL(a,b) \
	((((uint32_t *) (a))[0] == ((uint32_t *) (b))[0]) && \
	 (((uint32_t *) (a))[1] == ((uint32_t *) (b))[2]) && \
	 (((uint32_t *) (a))[2] == ((uint32_t *) (b))[1]) && \
	 (((uint32_t *) (a))[3] == ((uint32_t *) (b))[3]))

/* Bind socket to a privileged IP port.  */
extern int bindresvport (int __sockfd, struct sockaddr_in *__sock_in);



#define IN6_IS_ADDR_MC_NODELOCAL(a) \
	(IN6_IS_ADDR_MULTICAST(a) && ((((u_int8_t *) (a))[1] & 0xf) == 0x1))

#define IN6_IS_ADDR_MC_LINKLOCAL(a) \
	(IN6_IS_ADDR_MULTICAST(a) && ((((u_int8_t *) (a))[1] & 0xf) == 0x2))

#define IN6_IS_ADDR_MC_SITELOCAL(a) \
	(IN6_IS_ADDR_MULTICAST(a) && ((((u_int8_t *) (a))[1] & 0xf) == 0x5))

#define IN6_IS_ADDR_MC_ORGLOCAL(a) \
	(IN6_IS_ADDR_MULTICAST(a) && ((((u_int8_t *) (a))[1] & 0xf) == 0x8))

#define IN6_IS_ADDR_MC_GLOBAL(a) \
	(IN6_IS_ADDR_MULTICAST(a) && ((((u_int8_t *) (a))[1] & 0xf) == 0xe))

/* IPv6 packet information.  */
struct in6_pktinfo
  {
    struct in6_addr	ipi6_addr;    /* src/dst IPv6 address */
    unsigned int	ipi6_ifindex; /* send/recv interface index */
  };

__END_DECLS

#endif	/* netinet/in.h */
