bonito.pt
=========

All the code and configurations behind http://bonito.pt/ mail forwarding system.

---

The initial purpose of this project was to develop en email forwarding system that could run on a cheap VPS. Therefore, the system was made simple and ligth from the start.

This is being published so people using the system can know where their emails are going through.

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

The VPS specifications:
- Virtualization: Xen
- CPU: 2 vCPUs Intel Xeon E5620 @ 2.40GHz
- RAM: 256 MB
- Swap: 512 MB
- HD: 20 GB
- Bandwidth: 100 MBit/s

The operating system:
- GNU/Linux Debian 6 64-bit
- Linux Kernel 2.6.32-5-amd64

The software used:
- Mail Transfer Agent: Postfix
- Web Server: NGINX
- Data Base: PostgreSQL
- Backgound Process Manager: Daemon Tools - Supervise
- FastCGI Manager: spaw-fcgi and multiwatch

---
