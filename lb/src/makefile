# 编译器
CXX = g++

# 编译选项
CXXFLAGS = -std=c++11 -Wall

# 头文件目录
INC_DIR = ./util ./

# 库文件目录
LIB_DIR =

# 源文件目录列表
SRC_DIRS = ./util ./

# 源文件列表（通过通配符获取）
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

# 头文件列表
INCS = $(wildcard $(addsuffix /*.h,$(INC_DIR)))

####
# OBJ_DIR = ./obj

# 目标文件列表
OBJS = $(patsubst %.cpp,%.o,$(SRCS))

# 可执行文件名称
TARGET = MainApp

# 链接命令
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread -lrt

# 依赖关系
$(OBJS): %.o: %.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(foreach dir,$(INC_DIR),-I$(dir))

# 清除生成的文件
clean:
	rm -f $(TARGET) $(OBJS)

# 默认目标
all: $(TARGET)
	

print_objs:
	@echo "OBJS = $(OBJS)"
	@echo "SRCS = $(SRCS)"
	@echo "INCS = $(INCS)"

.PHONY: all clean
