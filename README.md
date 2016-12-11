# LogAndPcapSave
Safe Pcap files depending on event in Log or DbgView 

- Continuously safe network dump into Pcap file.
- Command line tool that monitors log files (Win/Posix) or the output of OutputDebugString (Win)
- If a key word is found then the corresponding pcap will be safed, otherwise discarded.

Todo
- better search options, e.g. with asterisk
