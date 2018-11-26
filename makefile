CC=g++

CFLAGS= -c -std=c++14 -fopenmp -g
LFLAGS= -lm -lsfml-graphics -lsfml-window -lsfml-system -fopenmp
SRCDIR= src
OBJDIR= obj
INCLUDE= -Iinclude

################################################################


trace: neuron ntemplate synapse trace_example
	$(CC) \
		$(OBJDIR)/Neuron.o $(OBJDIR)/NTemplate.o $(OBJDIR)/Synapse.o \
		$(OBJDIR)/TraceExample.o \
		-o trace_example \
		$(LFLAGS) 

follow: neuron ntemplate synapse follow_example
	$(CC) \
		$(OBJDIR)/Neuron.o $(OBJDIR)/NTemplate.o $(OBJDIR)/Synapse.o \
		$(OBJDIR)/FollowExample.o \
		-o follow_example \
		$(LFLAGS)

gridwar: neuron ntemplate synapse gridwar_example
	$(CC) \
		$(OBJDIR)/Neuron.o $(OBJDIR)/NTemplate.o $(OBJDIR)/Synapse.o \
		$(OBJDIR)/GridWarExample.o \
		-o gridwar_example \
		$(LFLAGS)

target: neuron ntemplate synapse target_example
	$(CC) \
		$(OBJDIR)/Neuron.o $(OBJDIR)/NTemplate.o $(OBJDIR)/Synapse.o \
		$(OBJDIR)/TargetExample.o \
		-o target_example \
		$(LFLAGS)

target_example: $(OBJDIR)/TargetExample.o

$(OBJDIR)/TargetExample.o: $(SRCDIR)/TargetExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/TargetExample.cpp -o $(OBJDIR)/TargetExample.o

gridwar_example: $(OBJDIR)/GridWarExample.o

$(OBJDIR)/GridWarExample.o: $(SRCDIR)/GridWarExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/GridWarExample.cpp -o $(OBJDIR)/GridWarExample.o

follow_example: $(OBJDIR)/FollowExample.o

$(OBJDIR)/FollowExample.o: $(SRCDIR)/FollowExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/FollowExample.cpp -o $(OBJDIR)/FollowExample.o

trace_example: $(OBJDIR)/TraceExample.o

$(OBJDIR)/TraceExample.o: $(SRCDIR)/TraceExample.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/TraceExample.cpp -o $(OBJDIR)/TraceExample.o

neuron: $(OBJDIR)/Neuron.o

$(OBJDIR)/Neuron.o: $(SRCDIR)/Neuron.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/Neuron.cpp -o $(OBJDIR)/Neuron.o

ntemplate: $(OBJDIR)/NTemplate.o

$(OBJDIR)/NTemplate.o: $(SRCDIR)/NTemplate.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/NTemplate.cpp -o $(OBJDIR)/NTemplate.o

synapse: $(OBJDIR)/Synapse.o

$(OBJDIR)/Synapse.o: $(SRCDIR)/Synapse.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCDIR)/Synapse.cpp -o $(OBJDIR)/Synapse.o


clean:
	rm -f $(OBJDIR)/*
	rm -f trace_example follow_example gridwar_example target_example


