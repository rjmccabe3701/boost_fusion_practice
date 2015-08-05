
CFLAGS:= -std=c++1y
CXX:= c++

define therule
$(1): $(1).cpp
	c++ $(CFLAGS) -o $$@ $$<
endef

$(eval $(call therule, $(MAKECMDGOALS)))
