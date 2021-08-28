PIM-DM Multicast Routing for UNIX
=================================
[![Badge][]][License] [![CIstat][]][GitHub] [![Cstat][]][Scan]

Introduction
------------

pimdd is the dense-mode cousin to [pimd][], the reference implementation
for [PIM-DM draft 5][draft].  Later revised in [RFC3973][], which pimdd
does not (yet) fully support.

PIM-DM is very similar to DVRMP ([mrouted][]), they both follow the
flood-and-prune paradigm, but unlike DVMRP a PIM-DM implementation
relies on the underlying unicast routes to be established.

This GitHub project is an attempt at reviving pimdd.  The latest code on
the master branch has been infused with fresh DNA strands from the
[pimd][] project, including a netlink back-end to read the unicast
routing table, and full IGMPv3 support (ASM).

> **HELP NEEDED:** The project needs more volunteers to test and update
> `pimdd` to RFC3973.


Running pimdd
-------------

If you have more than one router, you need to have unicast routing set
up already between all subnets.  Usually an IGP like OSPF or even RIP
is used for this.  Then start pimdd as root, it backgrounds itself as
a proper UNIX daemon:

    pimdd

Use the `pimctl` tool (shared with pimd) to query status.

To help out with development, or tracking down bugs, it is recommended
to run pimdd in debug mode.  Since there are many debug messages, you
can specify only a subset of the messages to be printed out:

```
Usage: pimdd [-hnpqrsv] [-d SYS[,SYS]] [-f FILE] [-l LVL] [-w SEC]

 -d SYS    Enable debug of subsystem(s)
 -f FILE   Configuration file, default: /etc/pimdd.conf
 -h        This help text
 -i NAME   Identity for config + PID file, and syslog, default: pimdd
 -l LVL    Set log level: none, err, notice (default), info, debug
 -n        Run in foreground, do not detach from calling terminal
 -p FILE   Override PID file, default is based on identity, -i
 -s        Use syslog, default unless running in foreground, -n
 -u FILE   Override UNIX domain socket, default based on identity, -i
 -v        Show program version
 -w SEC    Initial startup delay before probing interfaces

Available subystems for debug:
  all, igmp, groups, igmp_proto, igmp_timers, igmp_members, interfaces, 
  kernel, mfc, neighbors, pkt, rsrr, pim, asserts, bsr, detail, graft, hello, 
  registers, routes, pim_routes, jp, pim_timers, rpf, timers, timeout, trace
```

If you want to see all messages, use `pimdd -d all`.  When debugging
`pimdd`, it is recommended to run in foreground as well.


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
    make DESTDIR=/tmp/pimd-dense-1.0.0 install-strip


Building from GIT
-----------------

If you want to contribute, or simply just try out the latest but
unreleased features, then you need to know a few things about the
[GNU build system][build]:

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

This code is old and was sort of dead and forgotten.  The only project
that kept it running was FreeBSD in their ports collection.  As such it
was included in the BSD Router Project.

pimdd was written by Kurt Windisch when he was at University of Oregon.
It is based on the PIM sparse-mode daemon, pimd, which in turn is based
on the DVMRP daemon mrouted.

pimdd is covered by [LICENSE](LICENSE), Copyright 1998 the University of
Oregon.

pimd is covered by [LICENSE.pimd](doc/LICENSE.pimd), Copyright 1998-2001
University of Southern California.

mrouted is covered by [LICENSE.mrouted](doc/LICENSE.mrouted), Copyright
2002 The Board of Trustees of Leland Stanford Junior University

[mrouted]: https://github.com/troglobit/mrouted
[pimd]:    https://github.com/troglobit/pimd
[draft]:   https://tools.ietf.org/html/draft-ietf-idmr-pim-dm-spec-05
[RFC3973]: https://tools.ietf.org/html/rfc3973
[License]: https://en.wikipedia.org/wiki/BSD_licenses
[Badge]:   https://img.shields.io/badge/License-BSD%203--Clause-blue.svg
[GitHub]:  https://github.com/troglobit/pimd-dense/actions/workflows/build.yml/
[CIstat]:  https://github.com/troglobit/pimd-dense/actions/workflows/build.yml/badge.svg
[build]:   https://autotools.io/
[Scan]:    https://scan.coverity.com/projects/troglobit-pimd-dense
[Cstat]:   https://scan.coverity.com/projects/21569/badge.svg
