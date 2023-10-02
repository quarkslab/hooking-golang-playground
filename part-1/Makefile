C-CODE=c-code
GO-CODE=go-code

all:
	cd $(GO-CODE) && $(MAKE) TARGETS
	cd $(C-CODE)  && $(MAKE) TARGET

clean:
	cd $(GO-CODE) && $(MAKE) clean
	cd $(C-CODE) && $(MAKE) clean