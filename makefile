CC=g++

CFLAGS= -c -std=c++14 -fopenmp -g
LFLAGS= -lm -lsfml-graphics -lsfml-window -lsfml-system -fopenmp
SRCDIR= src
LIBDIR= libspsp
EXPDIR= examples
OBJDIR= obj
INCLUDE= -I./include -I./src/libspsp

################################################################


trace: clean libspsp trace_example
	$(CC) \
		$(OBJDIR)/*.o \
		-o trace_example \
		$(LFLAGS)

trade: clean libspsp trade_example
	$(CC) \
		$(OBJDIR)/*.o \
		-o trade_example \
		$(LFLAGS)

bee: clean libspsp bee_example
	$(CC) \
		$(OBJDIR)/*.o \
		-o bee_example \
		$(LFLAGS)

follow: clean libspsp follow_example
	$(CC) \
		$(OBJDIR)/*.o \
		-o follow_example \
		$(LFLAGS)

gridwar: clean libspsp gridwar_example
	$(CC) \
		$(OBJDIR)/*.o \
		-o gridwar_example \
		$(LFLAGS)

target: clean libspsp target_example
	$(CC) \
		$(OBJDIR)/*.o \
		-o target_example \
		$(LFLAGS)

path: clean libspsp path_example
	$(CC) \
		$(OBJDIR)/*.o \
		-o path_example \
		$(LFLAGS)

target_example: $(OBJDIR)/TargetExample.o

$(OBJDIR)/TargetExample.o: $(SRCDIR)/$(EXPDIR)/TargetExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(EXPDIR)/TargetExample.cpp -o $(OBJDIR)/TargetExample.o

path_example: $(OBJDIR)/PathExample.o

$(OBJDIR)/PathExample.o: $(SRCDIR)/$(EXPDIR)/PathExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(EXPDIR)/PathExample.cpp -o $(OBJDIR)/PathExample.o

trade_example: $(OBJDIR)/TradeExample.o

$(OBJDIR)/TradeExample.o: $(SRCDIR)/$(EXPDIR)/TradeExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(EXPDIR)/TradeExample.cpp -o $(OBJDIR)/TradeExample.o

bee_example: $(OBJDIR)/BeeExample.o

$(OBJDIR)/BeeExample.o: $(SRCDIR)/$(EXPDIR)/BeeExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(EXPDIR)/BeeExample.cpp -o $(OBJDIR)/BeeExample.o

gridwar_example: $(OBJDIR)/GridWarExample.o

$(OBJDIR)/GridWarExample.o: $(SRCDIR)/$(EXPDIR)/GridWarExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(EXPDIR)/GridWarExample.cpp -o $(OBJDIR)/GridWarExample.o

follow_example: $(OBJDIR)/FollowExample.o

$(OBJDIR)/FollowExample.o: $(SRCDIR)/$(EXPDIR)/FollowExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(EXPDIR)/FollowExample.cpp -o $(OBJDIR)/FollowExample.o

trace_example: $(OBJDIR)/TraceExample.o

$(OBJDIR)/TraceExample.o: $(SRCDIR)/$(EXPDIR)/TraceExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(EXPDIR)/TraceExample.cpp -o $(OBJDIR)/TraceExample.o

libspsp: neuron ntemplate synapse 

neuron: $(OBJDIR)/Neuron.o

$(OBJDIR)/Neuron.o: $(SRCDIR)/$(LIBDIR)/Neuron.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(LIBDIR)/Neuron.cpp -o $(OBJDIR)/Neuron.o

ntemplate: $(OBJDIR)/NTemplate.o

$(OBJDIR)/NTemplate.o: $(SRCDIR)/$(LIBDIR)/NTemplate.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(LIBDIR)/NTemplate.cpp -o $(OBJDIR)/NTemplate.o

synapse: $(OBJDIR)/Synapse.o

$(OBJDIR)/Synapse.o: $(SRCDIR)/$(LIBDIR)/Synapse.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(LIBDIR)/Synapse.cpp -o $(OBJDIR)/Synapse.o


clean:
	rm -f $(OBJDIR)/*
	rm -f trace_example follow_example gridwar_example target_example


