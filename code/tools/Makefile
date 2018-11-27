all: package_signer

package_signer: package_signer.o
	gcc -g  package_signer.o -o package_signer

package_signer.o: package_signer.c ../package_signer.h
	gcc -g -c -I.. package_signer.c

clean:
	rm *.o package_signer
