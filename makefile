CC=g++

# DEBUG
#CFLAGS= -c -std=c++14 -fopenmp -g3
# RELEASE
CFLAGS= -c -std=c++14 -fopenmp -O3 -Wall

LFLAGS= -lm -lsfml-graphics -lsfml-window -lsfml-system -fopenmp
SRCDIR= src
LIBDIR= libspsp
EXPDIR= examples
OBJDIR= obj
INCLUDE= -I./include -I./src/libspsp

################################################################


trace: clean_examples libspsp trace_example
	$(CC) \
		$(OBJDIR)/*/*.o \
		-o trace_example \
		$(LFLAGS)

trade: clean_examples libspsp trade_example
	$(CC) \
		$(OBJDIR)/*/*.o \
		-o trade_example \
		$(LFLAGS)

bee: clean_examples libspsp bee_example
	$(CC) \
		$(OBJDIR)/*/*.o \
		-o bee_example \
		$(LFLAGS)

follow: clean_examples libspsp follow_example
	$(CC) \
		$(OBJDIR)/*/*.o \
		-o follow_example \
		$(LFLAGS)

gridwar: clean_examples libspsp gridwar_example
	$(CC) \
		$(OBJDIR)/*/*.o \
		-o gridwar_example \
		$(LFLAGS)

target: clean_examples libspsp target_example
	$(CC) \
		$(OBJDIR)/*/*.o \
		-o target_example \
		$(LFLAGS)

path: clean_examples libspsp path_example point
	$(CC) \
		$(OBJDIR)/*/*.o \
		-o path_example \
		$(LFLAGS)

target_example: $(OBJDIR)/$(EXPDIR)/TargetExample.o
$(OBJDIR)/$(EXPDIR)/TargetExample.o: $(SRCDIR)/$(EXPDIR)/TargetExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(EXPDIR)/TargetExample.cpp -o $(OBJDIR)/$(EXPDIR)/TargetExample.o

path_example: $(OBJDIR)/$(EXPDIR)/PathExample.o
$(OBJDIR)/$(EXPDIR)/PathExample.o: $(SRCDIR)/$(EXPDIR)/PathExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(EXPDIR)/PathExample.cpp -o $(OBJDIR)/$(EXPDIR)/PathExample.o

trade_example: $(OBJDIR)/$(EXPDIR)/TradeExample.o
$(OBJDIR)$(EXPDIR)/TradeExample.o: $(SRCDIR)/$(EXPDIR)/TradeExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(EXPDIR)/TradeExample.cpp -o $(OBJDIR)/$(EXPDIR)/TradeExample.o

bee_example: $(OBJDIR)/$(EXPDIR)/BeeExample.o
$(OBJDIR)/$(EXPDIR)/BeeExample.o: $(SRCDIR)/$(EXPDIR)/BeeExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(EXPDIR)/BeeExample.cpp -o $(OBJDIR)/$(EXPDIR)/BeeExample.o

gridwar_example: $(OBJDIR)/$(EXPDIR)/GridWarExample.o
$(OBJDIR)/$(EXPDIR)/GridWarExample.o: $(SRCDIR)/$(EXPDIR)/GridWarExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(EXPDIR)/GridWarExample.cpp -o $(OBJDIR)/$(EXPDIR)/GridWarExample.o

follow_example: $(OBJDIR)/$(EXPDIR)/FollowExample.o
$(OBJDIR)/$(EXPDIR)/FollowExample.o: $(SRCDIR)/$(EXPDIR)/FollowExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(EXPDIR)/FollowExample.cpp -o $(OBJDIR)/$(EXPDIR)/FollowExample.o

trace_example: $(OBJDIR)/$(EXPDIR)/TraceExample.o
$(OBJDIR)/$(EXPDIR)/TraceExample.o: $(SRCDIR)/$(EXPDIR)/TraceExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(EXPDIR)/TraceExample.cpp -o $(OBJDIR)/$(EXPDIR)/TraceExample.o

point: $(OBJDIR)/$(EXPDIR)/Point.o
$(OBJDIR)/$(EXPDIR)/Point.o: $(SRCDIR)/$(EXPDIR)/Point.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(EXPDIR)/Point.cpp -o $(OBJDIR)/$(EXPDIR)/Point.o

libspsp: neuron ntemplate synapse

neuron: $(OBJDIR)/$(LIBDIR)/Neuron.o
$(OBJDIR)/$(LIBDIR)/Neuron.o: $(SRCDIR)/$(LIBDIR)/Neuron.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(LIBDIR)/Neuron.cpp -o $(OBJDIR)/$(LIBDIR)/Neuron.o

ntemplate: $(OBJDIR)/$(LIBDIR)/NTemplate.o
$(OBJDIR)/$(LIBDIR)/NTemplate.o: $(SRCDIR)/$(LIBDIR)/NTemplate.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(LIBDIR)/NTemplate.cpp -o $(OBJDIR)/$(LIBDIR)/NTemplate.o

synapse: $(OBJDIR)/$(LIBDIR)/Synapse.o
$(OBJDIR)/$(LIBDIR)/Synapse.o: $(SRCDIR)/$(LIBDIR)/Synapse.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/$(LIBDIR)/Synapse.cpp -o $(OBJDIR)/$(LIBDIR)/Synapse.o

clean_examples:
	rm -f trace_example bee_example follow_example gridwar_example target_example
	rm -f $(OBJDIR)/$(EXPDIR)/*

clean:
	rm -f $(OBJDIR)/*/*
	rm -f trace_example follow_example gridwar_example target_example bee_example


