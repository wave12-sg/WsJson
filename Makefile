CXX = g++
CXXFLAGS = -std=c++11 -Wall

ifeq ($(OS),Windows_NT)
    RM = del /Q
    EXECUTABLE_EXTENSION = .exe
else
    RM = rm -f
    EXECUTABLE_EXTENSION =
endif

# 目标文件和依赖关系
TARGET = main$(EXECUTABLE_EXTENSION)

SRC_DIR = .

# 获取所有的 .cpp 文件列表
SRCS := $(wildcard *.cpp) $(wildcard **/*.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

# 构建规则
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理规则
clean:
	$(RM) $(TARGET)
	$(RM) $(OBJS)
	$(RM) $(DEPS)

# 生成依赖关系文件
%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM -MF $@ -MT "$(@:.d=.o) $@" $<

# 包含依赖关系文件
-include $(DEPS)

