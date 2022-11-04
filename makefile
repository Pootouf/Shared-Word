dist:
	$(MAKE) -C main clean
	tar -zcf "$(CURDIR).tar.gz" main/* holdall/* hashtable/* bunch/* wordstruct/* RapportDeDéveloppement.pdf makefile
