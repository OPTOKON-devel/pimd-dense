PIM-DM Multicast Routing for UNIX
=================================
[![Badge][]][License] [![CIstat][]][Travis]

Introduction
------------

pimdd is the dense-mode cousin to [pimd][], the reference implementation
for [PIM-DM draft 5][draft].  [RFC3973][] later revised the protocol,
pimdd does not (yet) support that revision.

This code is old and was sort of dead and forgotten.  The only project
that kept it running was FreeBSD in their ports collection.  As such it
was included in the BSD Router Project.

This project at GitHub is an attempt at reviving it.  The latest code on
the master branch has been infused with fresh DNA strands from the
[pimd][] project, including a netlink back-end to read the unicast
routing table.


Running pimdd
-------------

Run pimdd as a root.  It is highly recommended to run it in debug mode.
Because there are many debug messages, you can specify only a subset of
the messages to be printed out:

```
Usage: pimdd [-hv] [-f FILE] [-d SYS[,SYS]]

 -f FILE   Configuration file, default: /etc/pimdd.conf
 -d SYS    Enable debug of subsystem(s)
 -h        This help text
 -v        Show program version

Available subystems for debug:
  dvmrp_detail, dvmrp_prunes, dvmrp_pruning, dvmrp_mrt, dvmrp_routes,
  dvmrp_routing, dvmrp_neighbors, dvmrp_peers, dvmrp_hello, dvmrp_timers,
  dvmrp, igmp_proto, igmp_timers, igmp_members, groups, membership, igmp,
  trace, mtrace, traceroute, timeout, callout, pkt, packets, interfaces, vif,
  kernel, cache, mfc, k_cache, k_mfc, rsrr, pim_detail, pim_hello,
  pim_neighbors, pim_register, registers, pim_join_prune, pim_j_p, pim_jp,
  pim_graft, pim_bootstrap, pim_bsr, bsr, bootstrap, pim_asserts, pim_routes,
  pim_routing, pim_mrt, pim_timers, pim_rpf, rpf, pim, routes, routing, mrt,
  routers, mrouters, neighbors, timers, asserts, all, 3

Bug report address: https://github.com/troglobit/pimdd/issues
```

If you want to see all messages, use `pimdd -d all`


Build & Install
---------------

When building from a released tarball, the configure script is bundled
and you do not need to do anything special (see below for building from
GIT).  The Makefile supports de facto standard settings and environment
variables such as `--prefix=PATH` and `DESTDIR=` for the install
process.  E.g., to install pimd to `/usr` instead of the default
`/usr/local`, but redirect install to a package directory in `/tmp`:

    ./configure --prefix=/usr --sysconfdir=/etc --localstatedir=/var
	make
    make DESTDIR=/tmp/pimdd-1.0.0 install-strip


Building from GIT
-----------------

If you want to contribute, or simply just try out the latest but
unreleased features, then you need to know a few things about the
[GNU build system][buildsystem]:

- `configure.ac` and a per-directory `Makefile.am` are key files
- `configure` and `Makefile.in` are generated from `autogen.sh`
- `Makefile` is generated by `configure` script

To build from GIT you first need to clone the repository and run the
`autogen.sh` script.  This requires `automake` and `autoconf` to be
installed on your system.

    git clone https://github.com/troglobit/pimd-dense.git
    cd pimd-dense/
    ./autogen.sh
    ./configure && make

GIT sources are a moving target and are not recommended for production
systems, unless you know what you are doing!


Origin & References
-------------------

pimdd was written by Kurt Windisch when he was at University of Oregon.
It is based on the PIM sparse-mode daemon, pimd, which in turn is based
on the DVMRP daemon mrouted.

pimdd is covered by [LICENSE](LICENSE), Copyright 1998 the University of
Oregon.

pimd is covered by [LICENSE.pimd](doc/LICENSE.pimd), Copyright 1998-2001
University of Southern California.

mrouted is covered by [LICENSE.mrouted](doc/LICENSE.mrouted), Copyright
2002 The Board of Trustees of Leland Stanford Junior University

[pimd]:    https://github.com/troglobit/pimd
[draft]:   https://tools.ietf.org/html/draft-ietf-idmr-pim-dm-spec-05
[RFC3973]: https://tools.ietf.org/html/rfc3973
[License]: https://en.wikipedia.org/wiki/BSD_licenses
[Badge]:   https://img.shields.io/badge/License-BSD%204--Clause-blue.svg
[Travis]:  https://travis-ci.org/troglobit/pimd-dense
[CIstat]:  https://travis-ci.org/troglobit/pimd-dense.png?branch=master
