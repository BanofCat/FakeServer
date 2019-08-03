from os import path, system
CurrentPath =  path.split(path.realpath(__file__))[0]

if not path.isfile(path.join(CurrentPath,'Shm.so')):
	system('cd '+CurrentPath+'; make clean -s -f MakefilePy; make -j4 -s -f MakefilePy') 

from Shm import PyShm
__all__=['PyShm']


