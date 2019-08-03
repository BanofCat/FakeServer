__author__='zhz'
# -*- coding:utf-8 -*-
  
import numpy as np
cimport numpy as np
import sys

"""
declare the c++ sharedMem class
"""
cdef extern from "shm_module.h":
    cdef cppclass sharedMem:
        sharedMem(char *filename) except +
        int create(unsigned int shm_size, unsigned int timeout_ns)
        int create(unsigned int shm_size)
        int link()
        int writeData(unsigned char* wbuf, int start, int length)
        int flushData() 
        int readData(unsigned char* rbuf, int start, int length)
        int destroy()
        int destroy(char* filepath)
        int unlink()

"""
redefine c++ sharedMem class to the python PyShm class
"""
cdef class PyShm: 
    cdef sharedMem *ShmPtr
    cdef int ShmSize

    def __cinit__(self, char* fileName):
        self.ShmPtr = new sharedMem(fileName)

    def __dealloc__(self):
        del self.ShmPtr

    def createWithTime(self, unsigned int shm_size, unsigned int timeout_ns):
        assert(shm_size >= 0) 
        self.ShmSize = shm_size
        return self.ShmPtr.create(shm_size, timeout_ns)

    def create(self, unsigned int shm_size):
        assert(shm_size >= 0)
        self.ShmSize = shm_size 
        return self.ShmPtr.create(shm_size)

    def link(self):
        self.ShmSize = self.ShmPtr.link()
        return self.ShmSize

    def unlink(self):
        return self.ShmPtr.unlink()

    """
    np.ndarray Data : the Data for writing to shm, the data length is the Data.nbytes
    int start : the start position of shm for writing

    return :  1 , write successfully
             -1 , failed to write
    """
    def writeData(self, np.ndarray Data, int start):
        assert (start + Data.nbytes <= self.ShmSize)
        cdef unsigned char* charArr = <unsigned char*> Data.data        
        return self.ShmPtr.writeData(charArr, start, Data.nbytes)

    def flushData(self):
        self.ShmPtr.flushData()

    """
    int start : the start position of shm for reading.
    int lengthBytes : the number bytes for reading from shm.
    type dtype : specify the return ndarray dtype, defaults: np.uint8

    return : None , Failed to read 
             np.ndarray, the data of reading .
    """
    def readData(self, int start, int lengthBytes, type dtype=np.uint8):        
        assert(start+lengthBytes <= self.ShmSize)
        cdef np.ndarray[np.uint8_t] Data = np.zeros((lengthBytes,), dtype=np.uint8)
        Data.dtype = dtype
        retVal = self.ShmPtr.readData(<unsigned char*>Data.data, start, lengthBytes)
        if retVal < 0:
            return None        
        return Data

    def destroy(self):
        return self.ShmPtr.destroy()

    def destroyWithPath(self, char* filepath):
        return self.ShmPtr.destroy(filepath)

        


    

 


