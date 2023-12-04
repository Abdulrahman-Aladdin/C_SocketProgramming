output: client.o error_handling.o send_request.o handle_response.o
	gcc client.o error_handling.o send_request.o handle_response.o -o output

client.o: client.c
	gcc -c client.c

error_handling.o: error_handling.c error_handling.h
	gcc -c error_handling.c

send_request.o: send_request.c send_request.h
	gcc -c send_request.c	

handle_response.o: handle_response.c handle_response.h
	gcc -c handle_response.c	

clean:
	rm *.o output

# out:
# 	gcc client.c error_handling.c handle_response.c send_request.c -o out

