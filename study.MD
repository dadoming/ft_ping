#### Why does it need to be a raw socket?
- Because raw sockets provide the neccessary access to the network layer,
	which is required to directly manipulate the packets.
	TCP and UDP sockets do not allow this
- These special packets are called ICMP packets, which are used to send
	error messages and operational information indicating, for example,
	that a requested service is not available or that a host or router
	could not be reached.
- Network Layer Access: ICMP operates at the network layer (Layer 3 of the
	OSI model), and raw sockets provide access to this layer, unlike
	standard sockets that operate at the transport layer.
- Raw sockets require root privileges and this is why this exercise needs
	to be done in a VM at 42.

##### ICMP stands for Internet Control Message Protocol.

Executing the command:
```bash
$ cat /etc/protocols | grep icmp
```
We'll get a table with all the protocols

| Protocol      | Number    | Name           | Description  |
|---------------|-----------|----------------|--------------|
| icmp          | 1         | ICMP           | internet control message protocol|
| tcp           | 6         | TCP            | transmission control protocol |
| udp           | 17        | UDP            | user datagram protocol |
| icmp6         | 58        | ICMPv6         | ICMP for IPv6 |
| ...           | ...       | ...            | ... |


------------------------------------------

##### To find information about raw sockets check:
```
man 7 raw
```
Important things to note from this page:
- **Address Format:** For sending and receiving datagrams (sendto(2), recvfrom(2), and similar), raw sockets  use  the  standard sockaddr_in address structure defined in ip(7). The sin_port field could be used to specify the IP protocol number, but  it  is  ignored  for  sending  in  Linux 2.2 and later, and should be always set to 0 (see BUGS).  For incoming packets, sin_port is set to zero.
- **Socket options:** Raw socket options can be set with setsockopt(2) and read with getsockopt(2) by passing the IPPROTO_RAW family flag.
    - ICMP_FILTER Enable a special filter for raw sockets bound to the IPPROTO_ICMP protocol. The value has a bit set for each ICMP message type which should be filtered out.  The default is to filter no ICMP messages. In addition, all ip(7) IPPROTO_IP socket options valid for datagram sockets are supported.
- ... and more

##### ICMP Packet Format

In the ICMP packet format, the first 8 bytes are the ICMP header, followed by the IP header, and then the data. The ICMP header consists of the following fields:
- Type (8 bits): Type 0 is an echo reply, type 8 is an echo request.
- Code (8 bits): This field carries some additional information about the error message and type.
- Checksum (16 bits): The checksum is used to check the number of bits of the complete message and enable the ICMP tool to ensure that complete data is delivered.
- Rest of the header (32 bits): This is the Extended Header, which has the work of pointing out the problem in IP Message. Byte locations are identified by the pointer which causes the problem message and receiving device looks here for pointing to the problem.

```c

struct icmphdr
{
  uint8_t type;		/* message type */
  uint8_t code;		/* type sub-code */
  uint16_t checksum;
  union
  {
    struct
    {
      uint16_t	id;
      uint16_t	sequence;
    } echo;			/* echo datagram */
    uint32_t	gateway;	/* gateway address */
    struct
    {
      uint16_t	__glibc_reserved;
      uint16_t	mtu;
    } frag;			/* path mtu discovery */
  } un;
};
```
