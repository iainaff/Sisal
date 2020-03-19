C    Test program for cfft.

C    Compute the FFT of a random sequence of
C    complex numbers, using the direct
C    definition of the Fourier Transform, and
C    using cfft.  Compare the results.  Then
C    compute the inverse transform (dividing
C    by N), and compare with the original
C    data.

	       integer log2n
               INTEGER N
               PARAMETER (NN = 2**19)
               COMPLEX X(0:NN-1), Y(0:NN-1)
               INTEGER NTABLE, NWORK
               PARAMETER (NTABLE = 100 + 8*NN, NWORK = 12*NN)
               REAL TABLE(NTABLE), WORK(NWORK)
	       real t1,t2,tsecnd

               read *, log2n
	       N = 2**log2n

C              PI = ATAN2(0.0, -1.0)
C --------------------------------------------------
C    Initialize input array, X, with random
C    complex numbers.

	       t1 = tsecnd()
               DO 10, J = 0, N-1
                  X(J) = CMPLX(real(j), 0.0)
   10   CONTINUE
	       t2 = tsecnd()-t1
	       print *, 'gen = ', t2

C --------------------------------------------------
C    init the Fourier Transform using cfft
        t1=tsecnd()
        CALL CFFT(0, N,1.0,X,1,Y,1,TABLE, NTABLE,WORK,NWORK)
        t2=tsecnd()-t1
	print *, 'init = ', t2

C --------------------------------------------------
C    Compute Y = the Forward Fourier Transform of X,
C    using cfft.

	t1=tsecnd()
        CALL CFFT(+1, N,1.0,X,1,Y,1,TABLE, NTABLE,WORK,NWORK)
        t2=tsecnd()-t1
	print *, 'cfft = ', t2

C       do 400 i = 0,N
Cprint *, Y(i)
C400    continue
               STOP
               END
