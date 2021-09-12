/*
 * Copyright (c) 1998-2001
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/*
 *  $Id: igmp.c,v 1.18 2002/09/26 00:59:29 pavlin Exp $
 */
/*
 * Part of this program has been derived from mrouted.
 * The mrouted program is covered by the license in the accompanying file
 * named "LICENSE.mrouted".
 *
 * The mrouted program is COPYRIGHT 1989 by The Board of Trustees of
 * Leland Stanford Junior University.
 *
 */

#include "defs.h"

/*
 * Exported variables.
 */
char     *igmp_recv_buf;	/* input packet buffer               */
char     *igmp_send_buf;  	/* output packet buffer              */
int       igmp_socket;	      	/* socket for all network I/O        */
in_addr_t allhosts_group;	/* allhosts  addr in net order       */
in_addr_t allrouters_group;	/* All-Routers addr in net order     */
in_addr_t allreports_group;	/* All IGMP routers in net order     */

/*
 * Local functions definitions.
 */
static void igmp_read        (int i, fd_set *rfd);
static void accept_igmp      (ssize_t recvlen);


/*
 * Open and initialize the igmp socket, and fill in the non-changing
 * IP header fields in the output packet buffer.
 */
void 
init_igmp()
{
    u_char *ip_opt;
    struct ip *ip;
    
    igmp_recv_buf = calloc(1, RECV_BUF_SIZE);
    if (!igmp_recv_buf)
	    logit(LOG_ERR, 0, "%s(): out of memory", __func__);
    igmp_send_buf = calloc(1, RECV_BUF_SIZE);
    if (!igmp_send_buf)
	    logit(LOG_ERR, 0, "%s(): out of memory", __func__);

    if ((igmp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_IGMP)) < 0) 
	logit(LOG_ERR, errno, "IGMP socket");
    
    k_hdr_include(igmp_socket, TRUE);	/* include IP header when sending */
    k_set_rcvbuf(igmp_socket, SO_RECV_BUF_SIZE_MAX,
		 SO_RECV_BUF_SIZE_MIN); /* lots of input buffering        */
    k_set_ttl(igmp_socket, MINTTL);	/* restrict multicasts to one hop */
    k_set_loop(igmp_socket, FALSE);	/* disable multicast loopback     */

    /* One time setup in the buffers */
    ip         = (struct ip *)igmp_send_buf;
    ip->ip_v   = IPVERSION;
    ip->ip_hl  = (IP_HEADER_RAOPT_LEN >> 2);
    ip->ip_tos = 0xc0;                  /* Internet Control   */
    ip->ip_id  = 0;                     /* let kernel fill in */
    ip->ip_off = 0;
    ip->ip_ttl = MAXTTL;		/* applies to unicasts only */
    ip->ip_p   = IPPROTO_IGMP;
    ip->ip_sum = 0;                     /* let kernel fill in               */

    /*
     * RFC2113 IP Router Alert.  Per spec this is required to
     * force certain routers/switches to inspect this frame.
     */
    ip_opt    = (u_char *)(igmp_send_buf + sizeof(struct ip));
    ip_opt[0] = IPOPT_RA;
    ip_opt[1] = 4;
    ip_opt[2] = 0;
    ip_opt[3] = 0;

    /* Everywhere in the daemon we use network-byte-order */    
    allhosts_group = htonl(INADDR_ALLHOSTS_GROUP);
    allrouters_group = htonl(INADDR_ALLRTRS_GROUP);
    allreports_group = htonl(INADDR_ALLRPTS_GROUP);
    
    if (register_input_handler(igmp_socket, igmp_read) < 0)
	logit(LOG_ERR, 0, "Couldn't register igmp_read as an input handler");
}


void
igmp_clean(void)
{
    if (igmp_socket > 0)
	close(igmp_socket);
    if (igmp_recv_buf)
	free(igmp_recv_buf);
    if (igmp_send_buf)
	free(igmp_send_buf);
    igmp_recv_buf = NULL;
    igmp_send_buf = NULL;
    igmp_socket = 0;
}


/* Read an IGMP message */
static void
igmp_read(i, rfd)
    int i;
    fd_set *rfd;
{
    ssize_t igmp_recvlen;
    socklen_t dummy = 0;
    
    igmp_recvlen = recvfrom(igmp_socket, igmp_recv_buf, RECV_BUF_SIZE,
			    0, NULL, &dummy);
    
    if (igmp_recvlen < 0) {
	if (errno != EINTR)
	    logit(LOG_ERR, errno, "IGMP recvfrom");
	return;
    }
    
    /* TODO: make it as a thread in the future releases */
    accept_igmp(igmp_recvlen);
}


/*
 * Process a newly received IGMP packet that is sitting in the input
 * packet buffer.
 */
static void 
accept_igmp(recvlen)
    ssize_t recvlen;
{
    int ipdatalen, iphdrlen, igmpdatalen;
    u_int32 src, dst, group;
    struct igmp *igmp;
    struct ip *ip;
    int ver = 3;
    
    if (recvlen < MIN_IP_HEADER_LEN) {
	logit(LOG_WARNING, 0, "received packet too short (%zd bytes) for IP header", recvlen);
	return;
    }
    
    ip        = (struct ip *)igmp_recv_buf;
    src       = ip->ip_src.s_addr;
    dst       = ip->ip_dst.s_addr;
    
    /* packets sent up from kernel to daemon have ip->ip_p = 0 */
    if (ip->ip_p == 0) {
	if (src == 0 || dst == 0)
	    logit(LOG_WARNING, 0, "kernel request not accurate, src %s dst %s",
		inet_fmt(src, s1), inet_fmt(dst, s2));
	else
	    process_kernel_call();
	return;
    }
    
    iphdrlen  = ip->ip_hl << 2;
    ipdatalen = recvlen - iphdrlen;

    if (iphdrlen + ipdatalen != recvlen) {
	logit(LOG_WARNING, 0,
	    "received packet from %s shorter (%zd bytes) than hdr+data length (%d+%d)",
	    inet_fmt(src, s1), recvlen, iphdrlen, ipdatalen);
	return;
    }
    
    igmp        = (struct igmp *)(igmp_recv_buf + iphdrlen);
    group       = igmp->igmp_group.s_addr;
    igmpdatalen = ipdatalen - IGMP_MINLEN;
    if (igmpdatalen < 0) {
	logit(LOG_WARNING, 0,
	    "received IP data field too short (%d bytes) for IGMP, from %s",
	    ipdatalen, inet_fmt(src, s1));
	return;
    }

/* TODO: too noisy. Remove it? */
#ifdef NOSUCHDEF
    IF_DEBUG(DEBUG_PKT | debug_kind(IPPROTO_IGMP, igmp->igmp_type,
				    igmp->igmp_code))
	logit(LOG_DEBUG, 0, "RECV %s from %-15s to %s",
	    packet_kind(IPPROTO_IGMP, igmp->igmp_type, igmp->igmp_code),
	    inet_fmt(src, s1), inet_fmt(dst, s2));
#endif /* NOSUCHDEF */
    
    switch (igmp->igmp_type) {
    case IGMP_MEMBERSHIP_QUERY:
	/* RFC 3376:7.1 */
	if (ipdatalen == 8) {
	    if (igmp->igmp_code == 0)
		ver = 1;
	    else
		ver = 2;
	} else if (ipdatalen >= 12) {
	    ver = 3;
	} else {
	    logit(LOG_INFO, 0, "Received invalid IGMP query: Max Resp Code = %d, length = %d",
		  igmp->igmp_code, ipdatalen);
	}
	accept_membership_query(src, dst, group, igmp->igmp_code, ver);
	return;
	
    case IGMP_V1_MEMBERSHIP_REPORT:
    case IGMP_V2_MEMBERSHIP_REPORT:
	accept_group_report(src, dst, group, igmp->igmp_type);
	return;
	
    case IGMP_V2_LEAVE_GROUP:
	accept_leave_message(src, dst, group);
	return;
	
    case IGMP_V3_MEMBERSHIP_REPORT:
	if (igmpdatalen < IGMP_V3_GROUP_RECORD_MIN_SIZE) {
	    logit(LOG_INFO, 0, "Too short IGMP v3 Membership report: igmpdatalen(%d) < MIN(%d)",
		  igmpdatalen, IGMP_V3_GROUP_RECORD_MIN_SIZE);
	    return;
	}
	accept_membership_report(src, dst, (struct igmpv3_report *)igmp, ipdatalen);
	return;

    case IGMP_DVMRP:
	/* XXX: TODO: most of the stuff below is not implemented. We are still
	 * only PIM router.
	 */
	group = ntohl(group);

	switch (igmp->igmp_code) {
	case DVMRP_PROBE:
	    dvmrp_accept_probe(src, dst, (char *)(igmp+1), igmpdatalen, group);
	    return;
	    
	case DVMRP_REPORT:
	    dvmrp_accept_report(src, dst, (char *)(igmp+1), igmpdatalen,
				group);
	    return;

	case DVMRP_ASK_NEIGHBORS:
	    accept_neighbor_request(src, dst);
	    return;

	case DVMRP_ASK_NEIGHBORS2:
	    accept_neighbor_request2(src, dst);
	    return;
	    
	case DVMRP_NEIGHBORS:
	    dvmrp_accept_neighbors(src, dst, (u_char *)(igmp+1), igmpdatalen,
				   group);
	    return;

	case DVMRP_NEIGHBORS2:
	    dvmrp_accept_neighbors2(src, dst, (u_char *)(igmp+1), igmpdatalen,
				    group);
	    return;
	    
	case DVMRP_PRUNE:
	    dvmrp_accept_prune(src, dst, (char *)(igmp+1), igmpdatalen);
	    return;
	    
	case DVMRP_GRAFT:
	    dvmrp_accept_graft(src, dst, (char *)(igmp+1), igmpdatalen);
	    return;
	    
	case DVMRP_GRAFT_ACK:
	    dvmrp_accept_g_ack(src, dst, (char *)(igmp+1), igmpdatalen);
	    return;
	    
	case DVMRP_INFO_REQUEST:
	    dvmrp_accept_info_request(src, dst, (u_char *)(igmp+1), igmpdatalen);
	    return;

	case DVMRP_INFO_REPLY:
	    dvmrp_accept_info_reply(src, dst, (u_char *)(igmp+1), igmpdatalen);
	    return;
	    
	default:
	    logit(LOG_INFO, 0,
		"ignoring unknown DVMRP message code %u from %s to %s",
		igmp->igmp_code, inet_fmt(src, s1), inet_fmt(dst, s2));
	    return;
	}
	
    case IGMP_PIM:
	return;    /* TODO: this is PIM v1 message. Handle it?. */
	
    case IGMP_MTRACE_RESP:
	return;    /* TODO: implement it */
	
    case IGMP_MTRACE:
	accept_mtrace(src, dst, group, (char *)(igmp+1), igmp->igmp_code,
		      igmpdatalen);
	return;
	
    default:
	logit(LOG_INFO, 0,
	    "ignoring unknown IGMP message type %x from %s to %s",
	    igmp->igmp_type, inet_fmt(src, s1), inet_fmt(dst, s2));
	return;
    }
}

void
send_igmp(buf, src, dst, type, code, group, datalen)
    char *buf;
    u_int32 src, dst;
    int type, code;
    u_int32 group;
    int datalen;
{
    struct sockaddr_in sdst;
    struct ip *ip;
    struct igmp *igmp;
    int sendlen;
#ifdef RAW_OUTPUT_IS_RAW
    extern int curttl;
#endif /* RAW_OUTPUT_IS_RAW */
    int setloop = 0;

    /* Prepare the IP header */
    ip 			    = (struct ip *)buf;
    ip->ip_len              = IP_HEADER_RAOPT_LEN + IGMP_MINLEN + datalen;
    ip->ip_src.s_addr       = src; 
    ip->ip_dst.s_addr       = dst;
    sendlen                 = ip->ip_len;
#ifdef RAW_OUTPUT_IS_RAW
    ip->ip_len              = htons(ip->ip_len);
#endif /* RAW_OUTPUT_IS_RAW */

    igmp                    = (struct igmp *)(buf + IP_HEADER_RAOPT_LEN);
    igmp->igmp_type         = type;
    igmp->igmp_code         = code;
    igmp->igmp_group.s_addr = group;
    igmp->igmp_cksum        = 0;
    igmp->igmp_cksum        = inet_cksum((u_int16 *)igmp,
					 IGMP_MINLEN + datalen);
    
    if (IN_MULTICAST(ntohl(dst))){
	k_set_if(igmp_socket, src);
	if (type != IGMP_DVMRP || dst == allhosts_group) {
	    setloop = 1;
	    k_set_loop(igmp_socket, TRUE);
	}
#ifdef RAW_OUTPUT_IS_RAW
	ip->ip_ttl = curttl;
    } else {
	ip->ip_ttl = MAXTTL;
#endif /* RAW_OUTPUT_IS_RAW */
    }
    
    bzero((void *)&sdst, sizeof(sdst));
    sdst.sin_family = AF_INET;
#ifdef HAVE_SA_LEN
    sdst.sin_len = sizeof(sdst);
#endif
    sdst.sin_addr.s_addr = dst;
    if (sendto(igmp_socket, igmp_send_buf, sendlen, 0,
	       (struct sockaddr *)&sdst, sizeof(sdst)) < 0) {
	if (errno == ENETDOWN)
	    check_vif_state();
	else
	    logit(log_severity(IPPROTO_IGMP, type, code), errno,
		"sendto to %s on %s", inet_fmt(dst, s1), inet_fmt(src, s2));
	if (setloop)
	    k_set_loop(igmp_socket, FALSE);
	return;
    }
    
    if (setloop)
	k_set_loop(igmp_socket, FALSE);
    
    IF_DEBUG(DEBUG_PKT|debug_kind(IPPROTO_IGMP, type, code))
	logit(LOG_DEBUG, 0, "SENT %s from %-15s to %s",
	    packet_kind(IPPROTO_IGMP, type, code),
	    src == INADDR_ANY_N ? "INADDR_ANY" :
	    inet_fmt(src, s1), inet_fmt(dst, s2));
}
