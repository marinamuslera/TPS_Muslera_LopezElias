local:
	g++ -g -std=c++17 -Wall -Wconversion -Wno-sign-conversion -Werror -o tp4 tp4.cpp
	valgrind -s --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes ./tp4
	rm tp4

resuelto:
	g++ -O2 -std=c++17 -Wall -Wconversion -Wno-sign-conversion -Werror -o tp4_resuelto tp4_resuelto.cpp
	./tp4_resuelto
	rm tp4_resuelto

docker:
	docker build --tag tp4_udesa .
	docker run tp4_udesa

clean_docker:
	 bash docker_clean.sh
