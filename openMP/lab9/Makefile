# Компилятор и флаги
CXX = g++
CXXFLAGS = -fopenmp -O2

# Целевые исполняемые файлы
TARGET = lab9

# Исходные файлы
SOURCE = lab9.cpp

# Количество потоков и размеры векторов
THREADS = 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18
SIZES = 21474830


# Файл для сохранения результатов
LOGFILE = "$(TARGET)_results.log"

# Правило по умолчанию (сборка)
build: $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

# Очистка
clean:
	rm -f $(TARGET) $(LOGFILE)

# Запуск программы с замерами и записью в файл
run: build
	@echo "Результаты запусков:" > $(LOGFILE)
	@for size in $(SIZES); do \
		for threads in $(THREADS); do \
			echo "Threads = $$threads" >> $(LOGFILE); \
			OMP_NUM_THREADS=$$threads /usr/bin/time -f "%E," -a -o $(LOGFILE) ./$(TARGET) $$size 4 >> $(LOGFILE); \
		done; \
	done


