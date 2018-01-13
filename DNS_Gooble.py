from socket import *
import time
import json
splunk_host=('163.18.26.108',12345)
google_dns_server='8.8.8.8'

s=socket(AF_INET, SOCK_DGRAM)
s.settimeout(3)

sk=socket(AF_INET, SOCK_DGRAM)
sk.connect(splunk_host)

packet = "24 1a 01 00 00 01 00 00 00 00 00 00 03 77  77 77 06 67 6f 6f 67 6c 65 03 63 6f 6d 00 00 01  00 01"
hex_package = packet.split()

hex_ints = [int(i, 16) for i in hex_package]
hex_bytes = [chr(i) for i in hex_ints]
packet = "".join(hex_bytes)


while True:
	TransmissionTime=time.strftime("%Y-%m-%d_%H:%M:%S",time.localtime())
	start=time.time()
	try:
		s.sendto(packet,(google_dns_server,53))
		s.recvfrom(1024)
		ArrivalTime=time.strftime("%Y-%m-%d_%H:%M:%S",time.localtime())
		end=time.time()-start
		ans={"TransmissionTime":TransmissionTime,"ArrivalTime":ArrivalTime,"DnsIp":google_dns_server,"time":end}
		json_str=json.dumps(ans)
		sk.sendto(json_str,splunk_host)
		time.sleep(10)
	except:
		ans={"TransmissionTime":TransmissionTime,"ArrivalTime":ArrivalTime,"DnsIp":google_dns_server,"error":"timeout"}
		json_str=json.dumps(ans)
		sk.sendto(json_str,splunk_host)
		time.sleep(10)
