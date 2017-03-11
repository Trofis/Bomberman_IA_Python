import socket
import time
class MySocket:
    """demonstration class only
      - coded for clarity, not efficiency
    """

    def __init__(self, sock=None):
        if sock is None:
            self.sock = socket.socket(
                            socket.AF_INET, socket.SOCK_STREAM)
        else:
            self.sock = sock

    def connect(self, host, port):
        self.sock.connect((host, port))

    def send(self, msg):
        totalsent = 0
        bmsg=bytes(msg, 'utf-8')
        MSGLEN=len(bmsg)
        while totalsent < MSGLEN:
            sent = self.sock.send(bmsg[totalsent:])
            if sent == 0:
                raise RuntimeError("socket connection broken")
            totalsent = totalsent + sent

    def receive(self,MSGLEN):
        chunks = []
        bytes_recd = 0
        while bytes_recd < MSGLEN:
            chunk = self.sock.recv(min(MSGLEN - bytes_recd, 2048))
            if chunk == b'':
                raise RuntimeError("socket connection broken")
            chunks.append(chunk)
            bytes_recd = bytes_recd + len(chunk)
        return b''.join(chunks)
    
    def getInt(self):
        b=self.receive(1)
        return b[0]
    
    def getChar(self):
        b=self.receive(1)
        return chr(b[0])        
            
    def getInt4(self):
        b=self.receive(4)
        res=int.from_bytes(b,byteorder='big',signed=True)
        return res
        
    
    def getString(self):
        data=b''
        c=self.receive(1)
        while c[0]!=0:
            data+=c
            c=self.receive(1)
        return str(data,'utf-8')
    
    def getTypeMsg(self):
        try:
            data=self.receive(4)
        except:
            data=b""
        return str(data,'utf-8') 
    
    def close(self):
        self.sock.close()
        
