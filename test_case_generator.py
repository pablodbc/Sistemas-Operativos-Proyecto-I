from random import*
N = int(input())
print(N)
print(N*N)
for i in range(N):
	for j in range(N):
		print(i,j,((-1)**randint(0,1))*randint(0,100000))
B = randint(0,N)
print(B)
for i in range(B):
	print(randint(0,N-1),randint(0,N-1),randint(1,N//4),randint(0,1000))

	
