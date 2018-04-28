all: 
	-cd Coordinador && $(MAKE) all
	-cd Planificador && $(MAKE) all
	-cd Esi && $(MAKE) all

clean:
	-cd Coordinador && $(MAKE) clean
	-cd Esi && $(MAKE) clean
	-cd Planificador && $(MAKE) clean