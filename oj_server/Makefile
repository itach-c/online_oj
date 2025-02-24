oj_server: oj_server.cc
	g++ -o $@ $^ -std=c++11   -ljsoncpp -lctemplate -lmysqlcppconn -g
.PHONY:clean
clean:
	rm -rf oj_server