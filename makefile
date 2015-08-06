
CFLAGS:= -std=c++1y
CXX:= c++

clean:
	rm -rf *.exe

define therule
.PHONY: $(1)

$(1): $(1).exe

$(1).exe: $(1).cpp fusion_includes.h
	c++ $(CFLAGS) -o $$@ $$<


endef

ifneq ($(MAKECMDGOALS),clean)
$(eval $(call therule, $(MAKECMDGOALS)))
endif


