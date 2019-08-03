from Shm import PyShm as Shm
import time
import numpy as np

import posix_ipc as Sem

sem_Route=Sem.Semaphore("Flag_RouteUpdated")
sem_IO = Sem.Semaphore("Flag_IOUpdated")

sem_EmgStop = Sem.Semaphore("Flag_EmgStop")
sem_TargetOk = Sem.Semaphore("Flag_TargetOK")

print '\n>>sem route value: ', sem_Route.value
print 'sem io value: ', sem_IO.value, '<<\n'

shm_w = Shm('APP_route.bin')
print 'create return: ', shm_w.create(200000000)

IO_w = Shm('APP_IO.bin')
print 'create return: ', IO_w.create(64)

shm_r = Shm('APP_pos.bin')
while 0 > shm_r.link():
	print '****'
	time.sleep(0.1)

time.sleep(5)


rxType=np.dtype('u1, u1, u1, (1,6)u4, u4, u8, (25)u1')
txType=np.dtype('u1, u1, u1, (5,6)u4, u4, u8, (25)u1') #160 bytes

data = shm_r.readData(0, 64)
data.dtype = rxType
print data

TxData=np.zeros((1, ), dtype=txType)



temp = np.array([[1073741821,1073741822,1073741823,1073741824,1073741825,1073741826], 
				 [1073741810,1073741820,1073741820,1073741820,1073741820,1073741820],
				 [1073741820,1073741820,1073741820,1073741820,1073741820,1073741820],
				 [1073741830,1073741820,1073741820,1073741820,1073741820,1073741820],
				 [1073741840,1073741820,1073741820,1073741820,1073741820,1073741820],
				 [1073741850,1073741820,1073741820,1073741820,1073741820,1073741820],
				 [1073741860,1073741820,1073741820,1073741820,1073741820,1073741820],
				 [1073741870,1073741820,1073741820,1073741820,1073741820,1073741820],
				 [1073741880,1073741820,1073741820,1073741820,1073741820,1073741820],
				 [1073741890,1073741820,1073741820,1073741820,1073741820,1073741820],
				 [1073741900,1073741820,1073741820,1073741820,1073741820,1073741820]], dtype=np.uint32)
Len = temp.nbytes
print "Len: ", Len
shm_w.writeData(np.array([Len]), 0)
shm_w.writeData(temp, 4)

Dlen = shm_w.readData(0, 4)
Dlen.dtype = np.int32
print '>>>>>> ', Dlen


Data = shm_w.readData(4, 264)
Data.dtype = np.int32

print 'target ok 0 : ', sem_TargetOk.value
print '>>>>>> \n', Data
sem_Route.release()

print "sem_Route 0 : ", sem_Route.value


time.sleep(12)
print 'target ok 1 : ', sem_TargetOk.value
print "sem_Route 1 : ", sem_Route.value

print '++++++++++++'

sem_EmgStop.release()


print '------------'
time.sleep(2)
print 'target ok 2 : ', sem_TargetOk.value
sem_EmgStop.acquire()

time.sleep(2)
print 'target ok 3 : ', sem_TargetOk.value
time.sleep(100)
