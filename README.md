bonito.pt
=========

All the code and configurations behind [bonito.pt](http://bonito.pt/) mail forwarding system.

---

The initial purpose of this project was to develop en email forwarding system that would:
- Run on a cheap VPS.
 Therefore the system was made simple and ligth from the start.
- Be completely based on Open Source software.
 So a Linux/Xen VPS was chosen.

This is being published so that:
- Someone else can used the code and configurations as examples for similar projects.
- People using the system can know where their emails are going through.

---

So this is how it works:

- The MTA reads forwarding rules from the DB.
 - On message reception, the real target address is fetched from the database.
 - The SMTP level sender is changed to a locally generated address to pass SPF checks.
 - The return path for bounces is ensured, as it possible to retrieve the original SMTP level sender from the locally generated address.

- The web interface allows configuration of forwarding rules on the DB.
 - There is no login, session, user/password nor authentication of any kind.
 - To configure a forwarding rule to target a given email, the client must have access to its inbox.
 - Email validation is performed by sending a URL with a token to the email targeted by the desired configuration.

---

[Chimehost](http://www.chimehost.com/) VPS specifications:
- Virtualization: [Xen](http://www.xenproject.org/)
- CPU: 2 vCPUs Intel Xeon E5620 @ 2.40GHz
- RAM: 256 MB
- Swap: 512 MB
- HD: 20 GB
- Bandwidth: 100 MBit/s

The operating system:
- [GNU](http://www.gnu.org/)/[Linux](https://www.kernel.org/) [Debian](http://www.debian.org/) 6 64-bit
- Linux Kernel 2.6.32-5-amd64

The software used in the system:
- Mail Transfer Agent: [Postfix](http://www.postfix.org/)
- Web Server: [NGINX](http://nginx.org/)
- Data Base: [PostgreSQL](http://www.postgresql.org/)
- Backgound Process Manager: [daemontools](http://cr.yp.to/daemontools.html)
- FastCGI Manager: [spaw-fcgi](http://redmine.lighttpd.net/projects/spawn-fcgi/wiki) and [multiwatch](http://redmine.lighttpd.net/projects/multiwatch/wiki)
- CSS Package: [SAPO INK](http://ink.sapo.pt/)

---

Missing features:
- Change/Disable forwarding rules.
- Domain key signature for forwarded messages.

