CFLAGS	= --std=c++11 `mysql_config --cflags --libs`
BIN		= beleg.x 


index.html: Doxyfile $(BIN)
	doxygen

$(BIN):	fancyFunction.cpp fancyFunction.h 
	g++ $< -o $@ $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(BIN) html
